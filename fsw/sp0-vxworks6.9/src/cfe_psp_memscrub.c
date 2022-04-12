/**
 ** \file
 **
 ** \brief API for Memory Scrubbing on SP0
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
 ** Implementation of Memory Scrubbing task using Aitech internal functions
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

#include <vxWorks.h>

/* Memory Scrubbing */
#include <mem_scrub.h>

/* Include PSP API */
#include "cfe_psp.h"

/* For CFE_PSP_SetTaskPrio() */
#include "cfe_psp_start.h"
#include "cfe_psp_memscrub.h"
#include "cfe_psp_config.h"

/*
** Static function declarations
**
** See function definition for details
*/
static void CFE_PSP_MemScrubTask(void);

/* Defined in sysLib.c */
/** \name External definition of variables containing error statistics */
/** \{ */
//UndCC_Begin(SSET056)
extern uint32_t    l2errTotal;
extern uint32_t    l2errMult;
extern uint32_t    l2errTagPar;
extern uint32_t    l2errMBECC;
extern uint32_t    l2errSBECC;
extern uint32_t    l2errCfg;
extern uint32_t    mchCause;
extern uint32_t    mchkHook;
//UndCC_End(SSET056)
/** \} */
/** \brief External definition of function to print memory scrubbing statistics */
extern void ckCtrs(void);

/* Defined in cfe_psp_memory.c */
extern uint32 g_uiEndOfRam;

/** \brief Task Priority of Memory Scrubbing Task */
static osal_priority_t g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;

/**
 ** \brief Contains the Active Memory Scrubbing Task ID
 ** \par Description:
 ** If 0, task is not running
 */
static osal_id_t g_uiMemScrubTaskId;

/**
 ** \brief Contains the Active Memory Scrubbing Start Address
 ** \par Description:
 ** The start address can be anything in the address space.
 */
static uint32 g_uiMemScrubStartAddr = 0;

/**
 ** \brief Contains the Active Memory Scrubbing End Address
 ** \par Description:
 ** End Address cannot be larger than the maximum RAM
 */
static uint32 g_uiMemScrubEndAddr = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Current Page
 ** \par Description:
 ** Current page that the task is working on. This value gets
 ** reset whenever task restart.
 */
static uint32 g_uiMemScrubCurrentPage = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Total Pages
 ** \par Description:
 ** Total number of pages processed since the start of the task. This value gets
 ** reset whenever task restart.
 */
static uint32 g_uiMemScrubTotalPages = 0;

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
 ** \brief Boolean flag to indicate scrub addresses have been udpated
 ** \par Description:
 ** See above g_semUpdateMemAddr_id for more information.
 ** We use this flag for optimization sake; use this flag to check
 ** if scrub addresses need to be updated. See ...SCRUB_Task
 */
static bool g_bScrubAddrUpdates_flag = false;

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubSet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubSet(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /*
    ** Take binary semaphore
    */
    if (OS_ObjectIdEqual(g_semUpdateMemAddr_id, OS_OBJECT_ID_UNDEFINED))
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Binary semaphore not created\n");
    }
    else if (OS_BinSemTake(g_semUpdateMemAddr_id) == OS_SUCCESS)
    {
        /* Verify start and end address */
        if (newStartAddr < newEndAddr)
        {
            /* Indicate that address values are being updated */
            g_bScrubAddrUpdates_flag = true;

            g_uiMemScrubStartAddr = newStartAddr;

            /* If top of memory has not been initialized, then initialize it */
            if (g_uiEndOfRam == 0)
            {
                g_uiEndOfRam = (uint32) sysPhysMemTop();
            }

            if (newEndAddr > g_uiEndOfRam)
            {
                g_uiMemScrubEndAddr = g_uiEndOfRam;
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Invalid newEndAddr. Range set to end of RAM\n");
            }
            else
            {
                g_uiMemScrubEndAddr = newEndAddr;
            }
            
            /*
            * Task Priority does not need to be changed
            */
            if (task_priority == g_uiMemScrubTaskPriority)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            /*
            * If the Task Priority is different from current value and is within
            * the allowed range, change it
            */
            else
            {
                /*
                * If the Task Priority is set outside the range, use default priority and
                * report error
                */
                if ((task_priority > MEMSCRUB_PRIORITY_UP_RANGE) || (task_priority < MEMSCRUB_PRIORITY_DOWN_RANGE))
                {
                    /* Apply default priority */
                    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
                    /* Report error */
                    OS_printf(MEM_SCRUB_PRINT_SCOPE "Priority is outside range, using default `%u`\n",
                            g_uiMemScrubTaskPriority);
                }
                else
                {
                    /* Apply new priority */
                    g_uiMemScrubTaskPriority = task_priority;
                }

                iReturnCode = CFE_PSP_SetTaskPrio(MEMSCRUB_TASK_NAME, g_uiMemScrubTaskPriority);
            }
        }
        else
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "[ERR] Start address after end address\n");
        }
        if(OS_BinSemGive(g_semUpdateMemAddr_id) != OS_SUCCESS)
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to give semaphore\n");
            iReturnCode = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to take binary semaphore\n");
    }

    return iReturnCode;
} /* end CFE_PSP_MemScrubSet */


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

    if (CFE_PSP_MemScrubDisable() == CFE_PSP_SUCCESS)
    {
        /* Reset all memory scrub related values to default */
        g_uiMemScrubTaskId = OS_OBJECT_ID_UNDEFINED;
        g_uiMemScrubCurrentPage = 0;
        g_uiMemScrubTotalPages = 0;
        g_uiMemScrubStartAddr = 0;
        g_uiMemScrubEndAddr = g_uiEndOfRam;
        g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;

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
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to delete semaphore\n");
            }
        }
        else
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to take semaphore\n");
        }
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to disable mem scrub task\n");
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubStatus
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_MemScrubStatus(CFE_PSP_MemScrubStatus_t *mss_Status, bool talk)
{
    mss_Status->uiMemScrubStartAddr = g_uiMemScrubStartAddr;
    mss_Status->uiMemScrubEndAddr = g_uiMemScrubEndAddr;
    mss_Status->uiMemScrubCurrentPage = g_uiMemScrubCurrentPage;
    mss_Status->uiMemScrubTotalPages = g_uiMemScrubTotalPages;
    mss_Status->opMemScrubTaskPriority = g_uiMemScrubTaskPriority;

    if (talk == true)
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "StartAdr: 0x%08X - EndAdr: 0x%08X\n" \
                                        "CurrentPages: %u - TotalPages: %u\n" \
                                        "Scrub Task Priority: %u\n",
                mss_Status->uiMemScrubStartAddr,
                mss_Status->uiMemScrubEndAddr,
                mss_Status->uiMemScrubCurrentPage,
                mss_Status->uiMemScrubTotalPages,
                mss_Status->opMemScrubTaskPriority
                );
    }
}

/**
** \func Main function for the Memory Scrubbing task
**
** \par Description:
** This is the main function for the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** The scrubMemory function implemented by AiTech may never return an error.
** The function may never exit, the task is meant to be deleted using 
** CFE_PSP_MemScrubDelete. Additionally, changes to global Mem Scrub
** start and end address (by some other thread/task) will not have an affect
** on the thread running this scrub task.
** Global scrub memory start/end addresses should only be set via ...SCRUB_Set
** This function is function should not be part of mem scrub public API
** 
** \param None
**
** \return None
*/
static void CFE_PSP_MemScrubTask(void)
{
    STATUS status = CFE_PSP_ERROR;

    uint32 uiLocalMemScrubStartAddr = 0;
    uint32 uiLocalMemScrubEndAddr = 0;

    while(1)
    {
        /* 
        ** We do this boolean comparison over just semaphore + assignment
        ** under the assumption that these values will updates
        ** infrequently relative to scrubMemory function call
        */
        if (g_bScrubAddrUpdates_flag == true)
        {
            if (OS_BinSemTake(g_semUpdateMemAddr_id) == OS_SUCCESS)
            {
                uiLocalMemScrubStartAddr = g_uiMemScrubStartAddr;
                uiLocalMemScrubEndAddr = g_uiMemScrubEndAddr;
                g_bScrubAddrUpdates_flag = false;
                if (OS_BinSemGive(g_semUpdateMemAddr_id) != OS_SUCCESS)
                {
                    OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to give semaphore\n");
                    break;
                }
            }
            else
            {
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to take semaphore\n");
                break;
            }
        }
        /* Call the active memory scrubbing function */
        status = scrubMemory(uiLocalMemScrubStartAddr, uiLocalMemScrubEndAddr, &g_uiMemScrubCurrentPage);

        if (status == CFE_PSP_ERROR)
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Unexpected ERROR during scrubMemory call\n");
            break;
        }
        else
        {
            g_uiMemScrubTotalPages += g_uiMemScrubCurrentPage;
        }
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
    /* Initialize */
    int32 iReturnCode = CFE_PSP_ERROR;
    g_semUpdateMemAddr_id = OS_OBJECT_ID_UNDEFINED;

    /*
    ** Create binary semaphore
    **
    ** Note that OS_BinSemCreate will set g_semUpdateMemAddr_id
    ** to a non-zero value
    */
    if (OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_FULL, 0) == OS_SUCCESS)
    {
        if (MEM_SCRUB_TASK_START_ON_STARTUP == true)
        {
            /*
            ** Manually set memory scrub start/end addresses
            **
            ** We manually set these in initialization. We do not call
            ** CFE_PSP_MemScrubSet because that function will attempt
            ** to set task priority when mem scrub task hasn't been started
            ** yet, resulting in an error. We can ignore the error, but there are
            ** other statements in CFE_PSP_MemScrubSet that may elicit errors
            **
            ** When startup process reaches this point, g_uiEndOfRam should already
            ** be initialized to physical end of RAM via CFE_PSP_SetupReservedMemoryMap()
            */

            /* Set memory scrub start address */
            if (MEM_SCRUB_DEFAULT_START_ADDR == -1)
            {
                /* User indicates to use general case */
                g_uiMemScrubStartAddr = 0;
            }
            else
            {
                /* User indicates to use configured start addr */
                g_uiMemScrubStartAddr = (uint32)MEM_SCRUB_DEFAULT_START_ADDR;
            }

            /* Set memory scrub end address */
            if (MEM_SCRUB_DEFAULT_END_ADDR == -1)
            {
                /* User indicates to use end of RAM */
                g_uiMemScrubEndAddr = g_uiEndOfRam;
            }
            else
            {
                /* User indicates to use configured address */
                g_uiMemScrubEndAddr = (uint32)MEM_SCRUB_DEFAULT_END_ADDR;
            }

            /* Attempt to enable memory scrub task */
            if (CFE_PSP_MemScrubEnable() == CFE_PSP_SUCCESS)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            else
            {
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to enable mem scrub task\n");
            }
        }
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to create semaphore\n");
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubIsRunning
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
bool CFE_PSP_MemScrubIsRunning(void)
{
    /* Initialize */
    bool        bReturnValue = true;
    int32       iStatus = OS_SUCCESS;
    osal_id_t   mem_scrub_id = OS_OBJECT_ID_UNDEFINED;

    iStatus = OS_TaskGetIdByName(&mem_scrub_id, MEMSCRUB_TASK_NAME);

    if (iStatus == OS_ERR_NAME_NOT_FOUND)
    {
        bReturnValue = false;
    }

    return bReturnValue;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubEnable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubEnable(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* Check if memory scrub task is already running */
    if (CFE_PSP_MemScrubIsRunning() == true)
    {
        /* Memory scrub task already running */
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task is already running\n");
    }
    /* Check if semaphore has been created */
    else if (OS_ObjectIdEqual(g_semUpdateMemAddr_id, OS_OBJECT_ID_UNDEFINED))
    {
        /* Semaphore has not been created */
        iReturnCode = CFE_PSP_ERROR;
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Must create binary semaphore before starting mem scrub task\n");
    }
    else
    {
        /* Attempt to start memory scrubbing task */
        iReturnCode = OS_TaskCreate(&g_uiMemScrubTaskId, 
                                    MEMSCRUB_TASK_NAME, 
                                    CFE_PSP_MemScrubTask,
                                    OSAL_TASK_STACK_ALLOCATE, 
                                    OSAL_SIZE_C(1024), 
                                    g_uiMemScrubTaskPriority, 
                                    0);
        
        /* Check that task was successfully created */
        if (iReturnCode != OS_SUCCESS)
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Error creating memory scrub task\n");
            iReturnCode = CFE_PSP_ERROR;
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubDisable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubDisable(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /* Check if memory scrub task is running */
    if (CFE_PSP_MemScrubIsRunning() == true)
    {
        /* Attempt to take semaphore */
        if (OS_BinSemTake(g_semUpdateMemAddr_id) == OS_SUCCESS)
        {
            /* Attempt to delete memory scrub task */
            if (OS_TaskDelete(g_uiMemScrubTaskId) == OS_SUCCESS)
            {
                if (OS_BinSemGive(g_semUpdateMemAddr_id) == OS_SUCCESS)
                {
                    iReturnCode = CFE_PSP_SUCCESS;
                }
                else
                {
                    OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to release semaphore\n");
                }
            }
            else
            {
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to delete memory scrub task\n");
            }
        }
        else
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Unable to take semaphore\n");
        }
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Memory scrub task is not running\n");
        iReturnCode = CFE_PSP_SUCCESS;
    }

    /*
    ** No matter what happens, reset values
    */
    g_uiMemScrubTotalPages = 0;
    g_uiMemScrubTaskId = OS_OBJECT_ID_UNDEFINED;

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubErrStats
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, bool talkative)
{
    if (talkative == true)
    {
        ckCtrs();
    }

    errStats->uil2errTotal = l2errTotal;
    errStats->uil2errMult = l2errMult;
    errStats->uil2errTagPar = l2errTagPar;
    errStats->uil2errMBECC = l2errMBECC;
    errStats->uil2errSBECC = l2errSBECC;
    errStats->uil2errCfg = l2errCfg;
    errStats->uimchCause = mchCause;
    errStats->uimchkHook = mchkHook;
}
