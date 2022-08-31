/**
 ** \file
 **
 ** \brief API for Memory Scrubbing on GR740
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** Implementation of Memory Scrubbing task
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** The memory scrubber reads a fixed eight 32-bit words at a time, then sleeps
 ** for a adjustable number of cycles, and restart from where it left off.
 ** When a correctable error is detected, the memory scrubber will perform a 
 ** correction and then continue with scrubbing.
 ** When a uncorrectable error is detected, the memory area is left untouched.
 ** If the 
 */

#include <vxWorks.h>

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_gr740.h"
#include "cfe_psp_memscrub.h"

/* Defined in cfe_psp_memory.c */
extern uint32 g_uiEndOfRam;

/**
 ** \brief Binary semaphore id used for mem scrub addresses changes
 ** \par Description:
 ** It is possible for multiple threads to be altering global mem scrub start
 ** address variable and global mem scrub end address variable. In order
 ** to protect against disjoint mem address setting due to context switching 
 ** between threads, we implement a semaphore to check before assigning new
 ** mem scrub start/end address values.
 **
 ** \par Assumptions, External Events, and Notes:
 ** Cannot initialize static variable to non-constant expression. I believe
 ** compiler is interpreting OS_OBJECT_ID_UNDEFINED as non constant expression
 ** because of type cast in preprocessor macro. Instead, assign variable to
 ** OS_OBJECT_ID_UNDEFINED in CFE_PSP_MemScrubInit. Since CFE_PSP_MemScrubInit
 ** will be the first mem scrub-related function to be called and below variable
 ** is static, this will be safe. 
 */
static osal_id_t g_semUpdateMemAddr_id;


/**
 ** \brief Boolean flag to indicate scrub addresses have been updated
 ** \par Description:
 ** See above g_semUpdateMemAddr_id for more information.
 ** We use this flag for optimization sake; use this flag to check
 ** if scrub addresses need to be updated. See ...SCRUB_Task
 */
static bool g_bScrubAddrUpdates_flag = false;

/** \brief Contains the boolean value if Mem Scrub should start at startup
 **
 */
static bool g_bMemScrubStartOnStartup = MEMSCRUB_TASK_START_ON_STARTUP;

/**
 ** \brief Mem Scrub configuration and status structure
 */
static CFE_PSP_MemScrubStatus_t g_MemScrub_Status = 
{
    .RunMode = MEMSCRUB_RUN_MODE,
    .uiBlockSize_Pages = MEMSCRUB_BLOCKSIZE_PAGES,
    .uiTaskDelay_msec = MEMSCRUB_TASK_DELAY_MSEC,
    .uiTimedEndAddr = 0,
    .uiTimedStartAddr = 0,
    .uiMemScrubStartAddr = MEMSCRUB_DEFAULT_START_ADDR,
    .uiMemScrubEndAddr = MEMSCRUB_DEFAULT_END_ADDR,
    .uiMemScrubTotalPages = 0,
    .uiMemScrubCurrentPage = 0,
    .opMemScrubTaskPriority = 0,
    .uiMemScrub_MaxPriority = 0,
    .uiMemScrub_MinPriority = 0
};

/**
 ** \brief Checks Mem Scrub configuration structure for errors
 **
 ** \par Description
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param pNewValues - Mem Scrub Status structure
 **
 ** \return #CFE_PSP_SUCCESS if all the variables pass checks
 ** \return #CFE_PSP_ERROR if any check fails
 */
static int32 CFE_PSP_MemScrubValidate(CFE_PSP_MemScrubStatus_t *pNewValues)
{
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* check address range */
    if (pNewValues->uiMemScrubEndAddr < pNewValues->uiMemScrubStartAddr)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Start address after end address\n");
        iRetCode = CFE_PSP_ERROR;
    }

    /* If top of memory has not been initialized, then initialize it */
    if (g_uiEndOfRam == 0)
    {
        g_uiEndOfRam = (uint32) sysPhysMemTop();
    }

    if (pNewValues->uiMemScrubEndAddr > g_uiEndOfRam)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Invalid new end address\n");
        iRetCode = CFE_PSP_ERROR;
    }

    /* check task priority is within range */
    // if ((pNewValues->opMemScrubTaskPriority < pNewValues->uiMemScrub_MinPriority) ||
    //     (pNewValues->opMemScrubTaskPriority > pNewValues->uiMemScrub_MaxPriority))
    // {
    //     OS_printf(MEMSCRUB_PRINT_SCOPE "Priority is outside range\n");
    //     iRetCode = CFE_PSP_ERROR;
    // }

    /* check that block size is within range */
    if ((pNewValues->uiBlockSize_Pages > 259765) | (pNewValues->uiBlockSize_Pages < 1))
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Incorrect block size\n");
        iRetCode = CFE_PSP_ERROR;
    }

    return iRetCode;
} /* CFE_PSP_MemScrubValidate */

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubSet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /* If Mem Scrub did not initialize exit */
    if (OS_ObjectIdEqual(g_semUpdateMemAddr_id, OS_OBJECT_ID_UNDEFINED))
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Binary semaphore not created\n");
    }
    /* Try taking previously created binary semaphore */
    else if (OS_BinSemTake(g_semUpdateMemAddr_id) == OS_SUCCESS)
    {
        if (CFE_PSP_MemScrubValidate(pNewConfiguration) == CFE_PSP_SUCCESS)
        {
            /* Indicate that address values are being updated */
            g_bScrubAddrUpdates_flag = true;

            g_MemScrub_Status.uiMemScrubStartAddr = pNewConfiguration->uiMemScrubStartAddr;

            g_MemScrub_Status.uiMemScrubEndAddr = pNewConfiguration->uiMemScrubEndAddr;
                
            g_MemScrub_Status.uiMemScrubTotalPages = 0;
            g_MemScrub_Status.uiMemScrubCurrentPage = 0;
            g_MemScrub_Status.uiTimedEndAddr = pNewConfiguration->uiBlockSize_Pages * MEMSCRUB_PAGE_SIZE;
            g_MemScrub_Status.uiTimedStartAddr = 0;
            g_MemScrub_Status.uiTaskDelay_msec = pNewConfiguration->uiTaskDelay_msec;

            /* Return SUCCESS if task priority does not need to be changed */
            // if (pNewConfiguration->opMemScrubTaskPriority == g_MemScrub_Status.opMemScrubTaskPriority)
            // {
            //     iReturnCode = CFE_PSP_SUCCESS;
            // }
            // else
            // {
            //     /* Apply new priority to variable */
            //     g_MemScrub_Status.opMemScrubTaskPriority = pNewConfiguration->opMemScrubTaskPriority;

            //     /* Apply new priority to running task */
            //     iReturnCode = CFE_PSP_SetTaskPrio(MEMSCRUB_TASK_NAME, g_MemScrub_Status.opMemScrubTaskPriority);
            // }
        }
        else
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "New configuration did not pass validation\n");
        }

        /* Give back the semaphore */
        if(OS_BinSemGive(g_semUpdateMemAddr_id) != OS_SUCCESS)
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to give semaphore\n");
            iReturnCode = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to take binary semaphore\n");
    }

    return iReturnCode;
} /* CFE_PSP_MemScrubSet */

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubStatus
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pStatus, bool talk)
{
    memcpy(pStatus, &g_MemScrub_Status, sizeof(g_MemScrub_Status));

    if (talk == true)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Mode %u\n" \
                                        "Address Range [0x%08X-0x%08X]\n" \
                                        "Timed [0x%08X-0x%08X] - Delay %u msec - Blocks %u bytes\n" \
                                        "Cur Pages: %u - Total Pages: %u\n" \
                                        "Priority %u\n",
                pStatus->RunMode,
                pStatus->uiMemScrubStartAddr,
                pStatus->uiMemScrubEndAddr,
                pStatus->uiTimedStartAddr,
                pStatus->uiTimedEndAddr,
                pStatus->uiTaskDelay_msec,
                pStatus->uiBlockSize_Pages,
                pStatus->uiMemScrubCurrentPage,
                pStatus->uiMemScrubTotalPages,
                pStatus->opMemScrubTaskPriority
                );
    }
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubInit
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubInit(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    int32 iRetCodeSem = OS_ERROR;

    /*
    ** Create binary semaphore
    **
    ** Note that OS_BinSemCreate will set g_semUpdateMemAddr_id
    ** to a non-zero value
    */
    iRetCodeSem = OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_FULL, 0);
    /*
    If the semaphore was successfully created, set all variables and check
    the task needs to start.
     */
    if (iRetCodeSem == OS_SUCCESS)
    {
        /* Set memory scrub end address */
        if (g_MemScrub_Status.uiMemScrubEndAddr == 0)
        {
            /* User indicates to use end of RAM */
            g_MemScrub_Status.uiMemScrubEndAddr = g_uiEndOfRam;
        }

        /* Set the low and high address range */
        MEMSCRUB_REG->range_low_addr = MEMSCRUB_DEFAULT_START_ADDR;
        MEMSCRUB_REG->range_high_addr = MEMSCRUB_DEFAULT_END_ADDR;

        /* Restart automatically once scrubber reach the end, the range high address */
        SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_LOOP_MODE, 1);

        /* Set the sleep cycles between burst */
        SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_DELAY_BETWEEN_BLOCKS, g_MemScrub_Status.uiTaskDelay_msec);

        /* Set operating mode to running */
        SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_OPERATION_MODE, 0x00);

        /*
        Start the Mem Scrub task if Mem Scrub task is set to run on startup 
        and it is not in Run Mode 3
        */
        if ((g_bMemScrubStartOnStartup == true) && (g_MemScrub_Status.RunMode != MEMSCRUB_MANUAL_MODE))
        {
            SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SCRUBBER_ENABLE, 1);
        }

        iReturnCode = CFE_PSP_SUCCESS;
    }
    return iReturnCode;
}

void MemScrubPrintReg(void)
{
    printf("AHB Status register = %08X\n", MEMSCRUB_REG->ahb_status);
    printf("AHB Failing address register = %08X\n", MEMSCRUB_REG->ahb_failing_address);
    printf("AHB Error configuration register = %08X\n", MEMSCRUB_REG->ahb_error_configuration);
    printf("Status register = %08X\n", MEMSCRUB_REG->ahb_error_configuration);
    printf("Configuration register = %08X\n", MEMSCRUB_REG->configuration);
    printf("Range low address register = %08X\n", MEMSCRUB_REG->range_low_addr);
    printf("Range high address register = %08X\n", MEMSCRUB_REG->range_high_addr);
    printf("Position register = %08X\n", MEMSCRUB_REG->position);
    printf("Error threshold register = %08X\n", MEMSCRUB_REG->error_threshold);
    printf("Initialization data register = %08X\n", MEMSCRUB_REG->initialization_data);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubDelete
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubDelete(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /* Add code to stop MEM Scrubber */
    /* TODO */

    /* Reset all memory scrub related values to default */
    g_MemScrub_Status.RunMode = MEMSCRUB_RUN_MODE,
    g_MemScrub_Status.uiBlockSize_Pages = MEMSCRUB_BLOCKSIZE_PAGES,
    g_MemScrub_Status.uiTaskDelay_msec = MEMSCRUB_TASK_DELAY_MSEC,
    g_MemScrub_Status.uiTimedEndAddr = g_MemScrub_Status.uiBlockSize_Pages * MEMSCRUB_PAGE_SIZE,
    g_MemScrub_Status.uiTimedStartAddr = 0,
    g_MemScrub_Status.uiMemScrubTaskId = OS_OBJECT_ID_UNDEFINED;
    g_MemScrub_Status.uiMemScrubCurrentPage = 0;
    g_MemScrub_Status.uiMemScrubTotalPages = 0;
    g_MemScrub_Status.uiMemScrubStartAddr = MEMSCRUB_DEFAULT_START_ADDR;
    g_MemScrub_Status.uiMemScrubEndAddr = MEMSCRUB_DEFAULT_END_ADDR;
    // g_MemScrub_Status.opMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    // g_MemScrub_Status.uiMemScrub_MaxPriority = MEMSCRUB_PRIORITY_UP_RANGE;
    // g_MemScrub_Status.uiMemScrub_MinPriority = MEMSCRUB_PRIORITY_DOWN_RANGE;

    /* Attempt to delete semaphore */
    if (OS_BinSemTake(g_semUpdateMemAddr_id) == OS_SUCCESS)
    {
        if (OS_BinSemDelete(g_semUpdateMemAddr_id) == OS_SUCCESS)
        {
            g_semUpdateMemAddr_id = OS_OBJECT_ID_UNDEFINED;
            iReturnCode = CFE_PSP_SUCCESS;
        }
        else
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to delete semaphore\n");
        }
    }
    else
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to take semaphore\n");
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubErrStats
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, bool talkative)
{
    if (talkative == true)
    {
        OS_printf("Mem Scrub Statistics\n");
    }

    /* Gather mem scrub statistics and saved them in CFE_PSP_MemScrubErrStats_t structure */
    

    return (CFE_PSP_SUCCESS);
}
