/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file cfe_psp_memscrub.c
*
*   \brief Implementation of PSP memory-scrub functions
*
*   \brief Implementation of Memory Scrubbing task using Aitech internal functions
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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
void CFE_PSP_MemScrubTask(void);

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
osal_id_t g_semUpdateMemAddr_id;

/**
 ** \brief Boolean flag to indicate scrub addresses have been updated
 ** \par Description:
 ** See above g_semUpdateMemAddr_id for more information.
 ** We use this flag for optimization sake; use this flag to check
 ** if scrub addresses need to be updated. See ...SCRUB_Task
 */
bool g_bScrubAddrUpdates_flag = false;

/** \brief Contains the boolean value if Mem Scrub should start at startup
 **
 */
bool g_bMemScrubStartOnStartup = CFE_PSP_MEMSCRUB_TASK_START_ON_STARTUP;

/**
 ** \brief Mem Scrub configuration and status structure
 */
CFE_PSP_MemScrubStatus_t g_MemScrub_Status = 
{
    .RunMode = CFE_PSP_MEMSCRUB_RUN_MODE,
    .uiBlockSize_Pages = CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES,
    .uiTaskDelay_msec = CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC,
    .uiTimedEndAddr = 0,
    .uiTimedStartAddr = 0,
    .uiMemScrubStartAddr = CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR,
    .uiMemScrubEndAddr = CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR,
    .uiMemScrubTotalPages = 0,
    .uiMemScrubCurrentPage = 0,
    .opMemScrubTaskPriority = CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY,
    .uiMemScrub_MaxPriority = CFE_PSP_MEMSCRUB_PRIORITY_UP_RANGE,
    .uiMemScrub_MinPriority = CFE_PSP_MEMSCRUB_PRIORITY_DOWN_RANGE
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
int32 CFE_PSP_MemScrubValidate(CFE_PSP_MemScrubStatus_t *pNewValues)
{
    int32  iRetCode = CFE_PSP_SUCCESS;
    uint32 uiMaxBlocksize = 0;

    /* check address range */
    if (pNewValues->uiMemScrubEndAddr < pNewValues->uiMemScrubStartAddr)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Start address after end address\n");
        iRetCode = CFE_PSP_ERROR;
    }

    /* If top of memory has not been initialized, then initialize it */
    if (g_uiEndOfRam == 0)
    {
        g_uiEndOfRam = (cpuaddr) sysPhysMemTop();
    }

    if (pNewValues->uiMemScrubEndAddr > g_uiEndOfRam)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Invalid new end address\n");
        iRetCode = CFE_PSP_ERROR;
    }

    /* check task priority is within range */
    if ((pNewValues->opMemScrubTaskPriority < pNewValues->uiMemScrub_MinPriority) ||
        (pNewValues->opMemScrubTaskPriority > pNewValues->uiMemScrub_MaxPriority))
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Priority is outside range\n");
        iRetCode = CFE_PSP_ERROR;
    }

    /* check that block size is within range (1, RAM_SIZE/PAGE_SIZE ) */
    uiMaxBlocksize = g_uiEndOfRam / MEMSCRUB_PAGE_SIZE;
    if ((pNewValues->uiBlockSize_Pages > uiMaxBlocksize) | (pNewValues->uiBlockSize_Pages < 1))
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
            if (pNewConfiguration->opMemScrubTaskPriority == g_MemScrub_Status.opMemScrubTaskPriority)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            else
            {
                /* Apply new priority to variable */
                g_MemScrub_Status.opMemScrubTaskPriority = pNewConfiguration->opMemScrubTaskPriority;

                /* Apply new priority to running task */
                iReturnCode = CFE_PSP_SetTaskPrio(CFE_PSP_MEMSCRUB_TASK_NAME, g_MemScrub_Status.opMemScrubTaskPriority);
            }
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
        g_MemScrub_Status.RunMode = CFE_PSP_MEMSCRUB_RUN_MODE,
        g_MemScrub_Status.uiBlockSize_Pages = CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES,
        g_MemScrub_Status.uiTaskDelay_msec = CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC,
        g_MemScrub_Status.uiTimedEndAddr = g_MemScrub_Status.uiBlockSize_Pages * MEMSCRUB_PAGE_SIZE,
        g_MemScrub_Status.uiTimedStartAddr = 0,
        g_MemScrub_Status.uiMemScrubTaskId = OS_OBJECT_ID_UNDEFINED;
        g_MemScrub_Status.uiMemScrubCurrentPage = 0;
        g_MemScrub_Status.uiMemScrubTotalPages = 0;
        g_MemScrub_Status.uiMemScrubStartAddr = CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR;
        g_MemScrub_Status.uiMemScrubEndAddr = CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR;
        g_MemScrub_Status.opMemScrubTaskPriority = CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY;
        g_MemScrub_Status.uiMemScrub_MaxPriority = CFE_PSP_MEMSCRUB_PRIORITY_UP_RANGE;
        g_MemScrub_Status.uiMemScrub_MinPriority = CFE_PSP_MEMSCRUB_PRIORITY_DOWN_RANGE;

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
    }
    else
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to disable mem scrub task\n");
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
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pConfig, size_t iConfigSize, bool talk)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (sizeof(g_MemScrub_Status) > iConfigSize)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    if (iReturnCode == CFE_PSP_SUCCESS)
    {
        memcpy(pConfig, &g_MemScrub_Status, sizeof(g_MemScrub_Status));

        if (talk == true)
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Mode %u\n" \
                                            "Address Range [0x%08X-0x%08X]\n" \
                                            "Timed [0x%08X-0x%08X] - Delay %u msec - Blocks %u bytes\n" \
                                            "Cur Pages: %u - Total Pages: %u\n" \
                                            "Priority %u\n",
                    pConfig->RunMode,
                    pConfig->uiMemScrubStartAddr,
                    pConfig->uiMemScrubEndAddr,
                    pConfig->uiTimedStartAddr,
                    pConfig->uiTimedEndAddr,
                    pConfig->uiTaskDelay_msec,
                    pConfig->uiBlockSize_Pages,
                    pConfig->uiMemScrubCurrentPage,
                    pConfig->uiMemScrubTotalPages,
                    pConfig->opMemScrubTaskPriority
                    );
        }
    }

    return iReturnCode;
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
void CFE_PSP_MemScrubTask(void)
{
    STATUS status = CFE_PSP_ERROR;

    uint32 uiLocalMemScrubStartAddr = 0;
    uint32 uiLocalMemScrubEndAddr = 0;
    uint32 uiBlockSize_Bytes = 0;

    while(1)
    {
        if (g_MemScrub_Status.RunMode == MEMSCRUB_IDLE_MODE)
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
                    uiLocalMemScrubStartAddr = g_MemScrub_Status.uiMemScrubStartAddr;
                    uiLocalMemScrubEndAddr = g_MemScrub_Status.uiMemScrubEndAddr;
                    g_bScrubAddrUpdates_flag = false;
                    /* If cannot give back the semaphore, exit while loop */
                    if (OS_BinSemGive(g_semUpdateMemAddr_id) != OS_SUCCESS)
                    {
                        OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to give semaphore\n");
                        break;
                    }
                }
                /* If cannot get the semaphore, exit while loop */
                else
                {
                    OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to take semaphore\n");
                    break;
                }
            }
        }
        /* This covers both Run Mode 2 and 3 */
        else
        {
            uiLocalMemScrubStartAddr = g_MemScrub_Status.uiTimedStartAddr;
            uiLocalMemScrubEndAddr = g_MemScrub_Status.uiTimedEndAddr;
        }

        /* Call the active memory scrubbing function */
        status = scrubMemory(uiLocalMemScrubStartAddr,
                             uiLocalMemScrubEndAddr,
                             &g_MemScrub_Status.uiMemScrubCurrentPage
        );

        /* If scrubMemory returns an error, exit while loop */
        if (status != OK)
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Unexpected ERROR during scrubMemory call\n");
            break;
        }
        else
        {
            g_MemScrub_Status.uiMemScrubTotalPages += g_MemScrub_Status.uiMemScrubCurrentPage;

            /* If we are not in Run Mode 1, prepare Start and End addresses for next run */
            if (g_MemScrub_Status.RunMode != MEMSCRUB_IDLE_MODE)
            {
                uiBlockSize_Bytes = g_MemScrub_Status.uiBlockSize_Pages * MEMSCRUB_PAGE_SIZE;

                /* Advance Timed Mode Start and End Addresses by the Block Size */
                g_MemScrub_Status.uiTimedStartAddr = g_MemScrub_Status.uiTimedEndAddr;
                g_MemScrub_Status.uiTimedEndAddr += uiBlockSize_Bytes;

                /* if the End Address folded back, reinitialize Start and End */
                if (g_MemScrub_Status.uiTimedEndAddr > g_MemScrub_Status.uiMemScrubEndAddr)
                {
                    if (g_MemScrub_Status.uiTimedStartAddr >= g_MemScrub_Status.uiMemScrubEndAddr)
                    {
                        g_MemScrub_Status.uiTimedEndAddr = uiBlockSize_Bytes;
                        g_MemScrub_Status.uiTimedStartAddr = 0;
                    }
                    else
                    {
                        g_MemScrub_Status.uiTimedEndAddr = g_MemScrub_Status.uiTimedStartAddr + 
                                                          (g_MemScrub_Status.uiMemScrubEndAddr % uiBlockSize_Bytes);
                    }
                }
            }
        }

        /* If we are in Run Mode 2 (Timed Mode), delay task */
        if (g_MemScrub_Status.RunMode == MEMSCRUB_TIMED_MODE)
        {
            status = OS_TaskDelay(g_MemScrub_Status.uiTaskDelay_msec);
            if (status == OS_ERROR)
            {
                OS_printf(MEMSCRUB_PRINT_SCOPE "Could not Delay Task for Timed Mode\n");
                break;
            }
        }

        /* If we are in Run Mode 3 (Manual Mode), exit loop */
        if (g_MemScrub_Status.RunMode == MEMSCRUB_MANUAL_MODE)
        {
            break;
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
    int32 iRetCodeSem = OS_SUCCESS;

    /*
    ** Create binary semaphore
    **
    ** Note that OS_BinSemCreate will set g_semUpdateMemAddr_id
    ** to a non-zero value
    */
    iRetCodeSem = OS_BinSemCreate(&g_semUpdateMemAddr_id, CFE_PSP_MEMSCRUB_BSEM_NAME, OS_SEM_FULL, 0);
    /*
    If the semaphore was successfully created, set all variables and check
    the task needs to start.
     */
    if (iRetCodeSem == OS_SUCCESS)
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

        /* Set memory scrub end address */
        if (g_MemScrub_Status.uiMemScrubEndAddr == 0)
        {
            /* User indicates to use end of RAM */
            g_MemScrub_Status.uiMemScrubEndAddr = g_uiEndOfRam;
        }
        g_MemScrub_Status.uiMemScrubTotalPages = 0;
        g_MemScrub_Status.uiMemScrubCurrentPage = 0;
        g_MemScrub_Status.uiTimedEndAddr = g_MemScrub_Status.uiBlockSize_Pages * MEMSCRUB_PAGE_SIZE;
        g_MemScrub_Status.uiTimedStartAddr = 0;

        iReturnCode = CFE_PSP_SUCCESS;

        /*
        Start the Mem Scrub task if Mem Scrub task is set to run on startup 
        and it is not in Run Mode 3
        */
        if ((g_bMemScrubStartOnStartup == true) && (g_MemScrub_Status.RunMode != MEMSCRUB_MANUAL_MODE))
        {
            /* Attempt to enable memory scrub task */
            if (CFE_PSP_MemScrubEnable() == CFE_PSP_SUCCESS)
            {
                OS_printf(MEMSCRUB_PRINT_SCOPE "Running in Mode %u\n", g_MemScrub_Status.RunMode);
            }
            else
            {
                OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to enable mem scrub task\n");
                iReturnCode = CFE_PSP_ERROR;
            }
        }
    }
    /*
    If the Semaphore cannot be taken because the name has already being taken,
    then the CFE_PSP_MemScrubInit has already been called once.
     */
    else if (iRetCodeSem == OS_ERR_NAME_TAKEN)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Already Initialized\n");
    }
    /* Catch all other errors */
    else
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to create semaphore, OSAL ERROR %d\n", iRetCodeSem);
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

    iStatus = OS_TaskGetIdByName(&mem_scrub_id, CFE_PSP_MEMSCRUB_TASK_NAME);

    if (iStatus == OS_ERR_NAME_NOT_FOUND)
    {
        bReturnValue = false;
    }

    return bReturnValue;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubTrigger
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubTrigger(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /* Check if memory scrub task is already running */
    if (CFE_PSP_MemScrubIsRunning() == true)
    {
        /* Memory scrub task already running */
        OS_printf(MEMSCRUB_PRINT_SCOPE "Active Memory Scrubbing task is already running\n");
    }
    /* Check if semaphore has been created */
    else if (OS_ObjectIdEqual(g_semUpdateMemAddr_id, OS_OBJECT_ID_UNDEFINED))
    {
        /* Semaphore has not been created */
        OS_printf(MEMSCRUB_PRINT_SCOPE "Mem Scrub has not been initialized\n");
    }
    else
    {
        if (g_MemScrub_Status.RunMode == MEMSCRUB_MANUAL_MODE)
        {
            /* Run memory scrubbing task */
            CFE_PSP_MemScrubTask();
            iReturnCode = CFE_PSP_SUCCESS;
        }
        else
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Trigger only works in Run Mode Idle and Timed\n");
        }
    }

    return iReturnCode;
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
        OS_printf(MEMSCRUB_PRINT_SCOPE "Active Memory Scrubbing task is already running\n");
    }
    /* Check if semaphore has been created */
    else if (OS_ObjectIdEqual(g_semUpdateMemAddr_id, OS_OBJECT_ID_UNDEFINED))
    {
        /* Semaphore has not been created */
        iReturnCode = CFE_PSP_ERROR;
        OS_printf(MEMSCRUB_PRINT_SCOPE "Mem Scrub has not been initialized\n");
    }
    else
    {
        /* Attempt to start memory scrubbing task */
        iReturnCode = OS_TaskCreate(&g_MemScrub_Status.uiMemScrubTaskId, 
                                    CFE_PSP_MEMSCRUB_TASK_NAME, 
                                    CFE_PSP_MemScrubTask,
                                    OSAL_TASK_STACK_ALLOCATE, 
                                    OSAL_SIZE_C(1024), 
                                    g_MemScrub_Status.opMemScrubTaskPriority, 
                                    0);

        /* Check that task was successfully created */
        if (iReturnCode != OS_SUCCESS)
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Error creating memory scrub task\n");
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
            if (OS_TaskDelete(g_MemScrub_Status.uiMemScrubTaskId) == OS_SUCCESS)
            {
                if (OS_BinSemGive(g_semUpdateMemAddr_id) == OS_SUCCESS)
                {
                    /*
                    ** No matter what happens, reset values
                    */
                    g_MemScrub_Status.uiMemScrubTotalPages = 0;
                    g_MemScrub_Status.uiMemScrubCurrentPage = 0;
                    g_MemScrub_Status.uiTimedEndAddr = g_MemScrub_Status.uiBlockSize_Pages * MEMSCRUB_PAGE_SIZE;
                    g_MemScrub_Status.uiTimedStartAddr = 0;
                    g_MemScrub_Status.uiMemScrubTaskId = OS_OBJECT_ID_UNDEFINED;

                    iReturnCode = CFE_PSP_SUCCESS;
                }
                else
                {
                    OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to release semaphore\n");
                }
            }
            else
            {
                OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to delete memory scrub task\n");
            }
        }
        else
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Unable to take semaphore\n");
        }
    }
    else
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Memory scrub task is not running\n");
        iReturnCode = CFE_PSP_SUCCESS;
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
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *pErrStats, size_t iErrSize, bool talkative)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (sizeof(CFE_PSP_MemScrubErrStats_t) > iErrSize)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    if (iReturnCode == CFE_PSP_SUCCESS)
    {
        if (talkative == true)
        {
            ckCtrs();
        }

        pErrStats->uil2errTotal = l2errTotal;
        pErrStats->uil2errMult = l2errMult;
        pErrStats->uil2errTagPar = l2errTagPar;
        pErrStats->uil2errMBECC = l2errMBECC;
        pErrStats->uil2errSBECC = l2errSBECC;
        pErrStats->uil2errCfg = l2errCfg;
        pErrStats->uimchCause = mchCause;
        pErrStats->uimchkHook = mchkHook;
    }
    return iReturnCode;
}
