/************************************************************************************************
**
** File: cfe_psp_timer.c
**
** Copyright (c) 2019-2021 United States Government as represented by
** the Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package (BSP).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
*************************************************************************************************/


/* For supporting REALTIME clock */
#include <time.h>

/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"

#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

#include "psp_time_sync.h"

/** \name NTP Sync Configuration - Linux */
/** \{ */
/**
 *  \brief Default NTP Sync Start/Stop on Startup
 * 
 *  \par Description:
 *      Enable or disable the Automatic time sync with the OS
 */
#ifndef CFE_MISSION_TIME_SYNC_OS_ENABLE
#define CFE_MISSION_TIME_SYNC_OS_ENABLE true
#endif

/**
 *  \brief Default Synchronization Frequency
 * 
 *  \par Description:
 *      Default number of seconds between time synchronizations.
 *      CFE Time Service updates MET and STCF from linux OS.
 *      When set to zero, CFE Time will be synchronized only once during start.
 * 
 *  \par Limits
 *      Positive integer up to 255.
 *      If this value is too low, it will starve the other processes.
 */
#ifndef CFE_MISSION_TIME_SYNC_OS_SEC
#define CFE_MISSION_TIME_SYNC_OS_SEC 30
#endif

/** \brief Default NTP Sync Task Name */
#ifndef NTPSYNC_TASK_NAME
#define NTPSYNC_TASK_NAME           "PSPNTPSync"
#endif

/** \brief Default NTP Sync Task Priority */
#ifndef NTPSYNC_DEFAULT_PRIORITY
#define NTPSYNC_DEFAULT_PRIORITY    60
#endif

/** \brief Default NTP Sync Task Priority Upper range */
#ifndef NTPSYNC_PRIORITY_UP_RANGE
#define NTPSYNC_PRIORITY_UP_RANGE    255
#endif

/** \brief Default NTP Sync Task Priority Lower range */
#ifndef NTPSYNC_PRIORITY_DOWN_RANGE
#define NTPSYNC_PRIORITY_DOWN_RANGE 60
#endif
/** \} */

/**
 ** \brief Default NTP Sync pre-print string 
 ** \par Description:
 ** This string is printed before every print related to NTP Sync API.
*/
#define NTPSYNC_PRINT_SCOPE         "PSP NTP SYNC: "


/**** External Function Prototypes ****/

/* Below are defined in CFE module time cfe_time.h and cfe_time_utils.h */
extern uint32 CFE_TIME_Micro2SubSecs(uint32);

/* 
SetTime may not be necessary since the function CFE_TIME_SetTimeCmd in CFE module time
can take the ground command directly
*/
extern void CFE_TIME_SetTime(CFE_TIME_SysTime_t);

/*
** Static function declarations
**
** See function definitions for more information
*/
static int32 CFE_PSP_TIME_NTPSync_Task_Init(void);
static void CFE_PSP_TIME_NTPSync_Task(void);

/**** Global variables ****/

/**
 * \brief Contains the NTP Sync Task ID
 *        If 0, task is not running
 */
static uint32 g_uiPSPNTPTask_id = 0;

/**
 * \brief Current value of NTP Sync priority task.
 */
static osal_priority_t g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY;

/**
 * \brief Boolean variable to control if to synchronize CFE Time Service with OS
 * local time. True, synch will occur. False, timer will not be disabled, but 
 * sync will not execute.
 */
static bool g_bEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

/**
 * \brief Change how often to sync CFE Time Service with OS Local Time. OS local
 * time is synchronized to NTP server(s) automatically from within OS if 
 * enabled.
 */
static uint16 g_usOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;

/* Declare this file a PSP Module */
CFE_PSP_MODULE_DECLARE_SIMPLE(ntp_clock_linux);

/* Declare ntp_clock_linux_Destroy to avoid re-ordering */
int32 ntp_clock_linux_Destroy(void);


/*
** Purpose: Initialize the CFE PSP Time Task synchronizing with the NTP server
**
** Description:
** This function intializes the cFE PSP Time sync task with the NTP server.
**
** Assumptions, External Events, and Notes:
** None
**
** Param: None
** 
** Return: OS_SUCCESS @copybrief OS_SUCCESS
**         OS_INVALID_POINTER if any of the necessary pointers are NULL
**         OS_ERR_INVALID_SIZE if the stack_size argument is zero
**         OS_ERR_NAME_TOO_LONG name length including null terminator greater than #OS_MAX_API_NAME
**         OS_ERR_INVALID_PRIORITY if the priority is bad @covtest
**         OS_ERR_NO_FREE_IDS if there can be no more tasks created
**         OS_ERR_NAME_TAKEN if the name specified is already used by a task
**         OS_ERROR if an unspecified/other error occurs @covtest
*/
static int32 CFE_PSP_TIME_NTPSync_Task_Init(void)
{
    /* Initialize */
    int32       iStatus = OS_ERROR;
    
    /*Create the 1Hz task for synchronizing with OS time*/
    iStatus = OS_TaskCreate(&g_uiPSPNTPTask_id,
                            NTPSYNC_TASK_NAME,
                            CFE_PSP_TIME_NTPSync_Task,
                            OSAL_TASK_STACK_ALLOCATE, 
                            OSAL_SIZE_C(1024),
                            g_ucNTPSyncTaskPriority,
                            0
                            );

    if (iStatus != OS_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Failed to create NTP Sync task\n");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync task Initialized\n");
    }

    return iStatus;
}

/*
 ** Purpose: Check if the NTP Sync task is running
 **
 ** Description:
 ** This function checks on whether or not
 ** the NTP Sync task is running
 **
 ** Assumptions, External Events, and Notes:
 ** Will check via OS_TaskGetIdByName using NTPSYNC_TASK_NAME
 ** as the name of NTP sync task
 **
 ** Param: None
 **
 ** Return: true - If NTP Sync Task is running
 **         false - If NTP Sync Task is not running
 */
bool CFE_PSP_TIME_NTPSync_Task_isRunning(void)
{
    /* Initialize */
    bool        bReturnValue = true;
    int32       iStatus = OS_SUCCESS;
    osal_id_t   osidUpdateId = 0;

    iStatus = OS_TaskGetIdByName(&osidUpdateId, NTPSYNC_TASK_NAME);

    if (iStatus == OS_ERR_NAME_NOT_FOUND)
    {
        bReturnValue = false;
    }

    return bReturnValue;
}

/*
 ** Purpose: Set the NTP Sync task priority
 **
 ** Description:
 ** This function sets the NTP Sync task priority
 **
 ** Assumptions, External Events, and Notes:
 ** New priority must be between NTPSYNC_PRIORITY_DOWN_RANGE
 ** and NTPSYNC_PRIORITY_UP_RANGE. If the new priority is not
 ** within this range, the default priority will be assigned.
 **
 ** Param - [in] newTaskPriority - The new task priority
 **
 ** Return: CFE_PSP_SUCCESS - If successfully set new priority
 **         CFE_PSP_ERROR - If unsuccessfully set new priority
 */
int32  CFE_PSP_TIME_NTPSync_Task_Priority_Set(osal_priority_t opPriority)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (opPriority != g_ucNTPSyncTaskPriority)
    {
        if ((opPriority <= NTPSYNC_PRIORITY_UP_RANGE) && 
            (opPriority >= NTPSYNC_PRIORITY_DOWN_RANGE))
        {
            g_ucNTPSyncTaskPriority = opPriority;
        }
        else
        {
            iReturnCode = CFE_PSP_ERROR;
            g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY;
            OS_printf(NTPSYNC_PRINT_SCOPE "Task Priority outside allowed range\n");
        }
    }
    
    return iReturnCode;
}

/*
 ** Purpse: Enable/disable time sync
 **
 ** Description:
 ** This function sets the enabling/disabling of time sync.
 **
 ** Assumptions, External Events, and Notes:
 **
 ** Param: [in] bStatus - Boolean flag for sync or not sync
 ** 
 ** Return: CFE_PSP_SUCCESS - If successfully set status
 **         CFE_PSP_ERROR - If unsuccessfully set status
 */
int32 CFE_PSP_TIME_NTPSync_Set_Status(bool bStatus)
{
    int32 iReturnValue = CFE_PSP_SUCCESS;

    g_bEnableGetTimeFromOS_flag = bStatus;

    if (bStatus == true)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync with OS enabled\n");
        ntp_clock_linux_Init((uint32)0);
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync with OS disabled\n");
        ntp_clock_linux_Destroy();
    }

    return (int32) g_bEnableGetTimeFromOS_flag;
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_NTP_Daemon_isRunning(void)
**
**  Purpose:
**    Get status of NTP daemon
**
**  Arguments:
**    none
**
**  Return:
**    bool - True if NTP Daemon is running, False if it is not running
******************************************************************************/
bool CFE_PSP_TIME_NTP_Daemon_isRunning(void)
{
    bool    return_code = true;
    
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_TIME_NTP_Daemon_isRunning not implemented in Linux OS");

    return return_code;
}

/*
 ** Purpose: Gracefully shutdown NTP Sync Module 
 ** 
 ** Description:
 ** Function will attempt to delete the NTP Sync task. Usually this function
 ** will be called called when exiting cFS.
 ** 
 ** Assumptions, External Events, and Notes:
 ** When this function is called, no matter what it's return status is,
 ** the g_bEnableGetTimeFromOS_flag will be set to false.
 ** 
 ** Return: CFE_PSP_SUCCESS 
 **         CFE_PSP_ERROR
 */
int32 ntp_clock_linux_Destroy()
{
    int32      iReturnValue = CFE_PSP_SUCCESS;
    g_bEnableGetTimeFromOS_flag = false;

    if (CFE_PSP_TIME_NTPSync_Task_isRunning() == true)
    {
        if (OS_TaskDelete(g_uiPSPNTPTask_id) == OS_SUCCESS)
        {
            g_uiPSPNTPTask_id = 0;
        }
        else
        {
            iReturnValue = CFE_PSP_ERROR;
            OS_printf(NTPSYNC_PRINT_SCOPE "Could not kill the NTP Sync task\n");
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP sync task is not running\n");
    }

    return iReturnValue;
}

/*
 ** Purpose: Entry point for the module
 ** 
 ** Description:
 ** None
 **
 ** Assumptions, External Events, and Notes:
 ** Will initialize regardless of g_bEnableGetTimeFromOS_flag value.
 ** PSP Module ID is not used in the SP0 implementation
 **
 ** Param: [in] PspModuleId - Unused
 **
 ** Return: None
 */
void ntp_clock_linux_Init(uint32 PspModuleId) //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    if (g_bEnableGetTimeFromOS_flag == true)
    {
        if (CFE_PSP_TIME_NTPSync_Task_isRunning() == true)
        {
            /* If NTP Sync Task is running */
            OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync task already running\n");
        }
        else
        {
            /* Attempt to start NTP Sync Task */
            if (CFE_PSP_TIME_NTPSync_Task_Init() != OS_SUCCESS)
            {
                g_bEnableGetTimeFromOS_flag = false;
                OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync task initialization failed");
            }
        }
    }
}

/*
** Purpose: Get the currently set sync frequency
**
** Description:
** This function returns the NTP time synchronization frequency, in seconds.
**
** Assumptions, External Events, and Notes:
** None
**
** Param: None
** 
** Return: Current frequency
*/
uint16 CFE_PSP_TIME_NTPSync_GetFreq(void)
{
    /* Return the value of g_usOSTimeSync_Sec */
    return g_usOSTimeSync_Sec;
}

/*
 ** Purpose: Change the sync frequency
 **
 ** Description:
 ** This function updates the NTP time synchronization frequency, in seconds.
 **
 ** Assumptions, External Events, and Notes:
 ** None
 **
 ** Param: [in] uiNewFreqSec - The new frequency, in seconds
 */
void CFE_PSP_TIME_NTPSync_SetFreq(uint16 uiNewFreqSec)
{
    g_usOSTimeSync_Sec = uiNewFreqSec;
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_Set_OS_Time()
**
**  Purpose:
**    Set the OS CLOCK_REALTIME to a specified timestamp
**    IMPORTANT: If NTP daemon is enabled and it is capable of contacting an
**               NTP server, setting time does not work.
**
**  Arguments:
**    uint32 ts_sec - Seconds since Epoch 1/1/1970
**    uint32 ts_nsec - NanoSecond timestamp
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_TIME_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec)
{
    int32               return_status = CFE_PSP_SUCCESS;

    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_Set_OS_Time not implemented in Linux OS");

    return return_status;
}

/*
 * Purpose: Get the current time from the linux OS
 * 
 * Param: [inout] myT Reference to CFE_TIME_SysTime_t time structure
 *
 * Return: CFE_PSP_SUCCESS
 *         CFE_PSP_ERROR
 */
int32 CFE_PSP_TIME_Get_OS_Time(CFE_TIME_SysTime_t *myT)
{
    struct timespec     unixTime;
    uint32              tv_sec = 0;
    uint32              tv_msec = 0;
    int                 ret;
    int32               return_code = CFE_PSP_SUCCESS;

    if (myT != NULL)
    {
        /* Get real time clock from OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == CFE_PSP_SUCCESS)
        {
            /* If the unix time has synchronzed with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT->Seconds = (uint32) unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32) unixTime.tv_nsec / 1000;
                myT->Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
            }
            else
            {
                /* OS has not sync with NTP server yet. */
                return_code = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
            return_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_TIME_Get_OS_Time called without a proper argument\n");
        return_code = CFE_PSP_ERROR;
    }

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_Get_OS_Time()
**
**  Purpose:
**    Syncronize CFE Time Service with OS local time. OS time is 
**    automatically syncronized to NTP server
**    Declaration is dictated by OSAL, see osapi-os-timer.h OS_TimerCreate info
**
**    IMPORTANT: Function declaration is dictated by OS_TimerCreate function.
**
**  Arguments:
**    void
**
**  Return:
**    void
******************************************************************************/
void CFE_PSP_TIME_NTPSync_Task(void)
{
    CFE_TIME_SysTime_t  myT;
    int32               ret;
    uint32              sleep_time;

    while (1)
    {
        sleep_time = g_usOSTimeSync_Sec * 1000U;
        ret = OS_TaskDelay(sleep_time);

        if (ret == OS_SUCCESS)
        {
            /* If the flag is enabled */
            if (g_bEnableGetTimeFromOS_flag)
            {
                /* Get real time clock from OS */
                ret = CFE_PSP_TIME_Get_OS_Time(&myT);
                
                /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
                if (ret == CFE_PSP_SUCCESS)
                {
                    CFE_TIME_SetTime(myT);
                }
                else
                {
                    /* OS has not sync with NTP server yet. */
                    OS_printf(NTPSYNC_PRINT_SCOPE "OS has not sync with NTP server yet, trying again later.\n");
                }
            }
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "OS_TaskDelay error\n");
        }
    }
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_StartNTPDaemon()
**
**  Purpose:
**    Start the NTP daemon on OS that sync with NTP server
**
**  Arguments:
**    none
**
**  Return:
**    int32 - Task ID or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_TIME_StartNTPDaemon(void)
{
    int32       return_code = 0;
    
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_StartNTPDaemon not implemented in Linux OS");

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_StopNTPDaemon()
**
**  Purpose:
**    Stop the NTP daemon on OS that sync with NTP server
**
**  Arguments:
**    none
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
**            Note: NTP task already stopped, return CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_TIME_StopNTPDaemon(void)
{
    int32       return_code = CFE_PSP_SUCCESS;

    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_StopNTPDaemon not implemented in Linux OS");

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_NTP_Daemon_Set_Status(bool)
**
**  Purpose:
**    Enable or Disable OS NTP dameon process
**
**  Arguments:
**    bool - When True starts NTP daemon, else stop process
**
**  Return:
**    int32 - Task ID or CFE_PSP_SUCCESS or CFE_PSP_ERROR
**            Task ID is returned when successfully starting NTP process
**            CFE_PSP_SUCCESS is returned when successfully stopping NTP process
******************************************************************************/
int32 CFE_PSP_TIME_NTP_Daemon_Set_Status(bool bStatus)
{
    int32   return_code = CFE_PSP_SUCCESS;

    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_NTP_Daemon_Enable not implemented in Linux OS");

    return return_code;
}
