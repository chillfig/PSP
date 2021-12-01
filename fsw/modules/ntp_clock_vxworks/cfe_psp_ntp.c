/**
 **
 ** \file cfe_psp_ntp.c
 **
 ** \brief API to control NTP Sync
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
 ** \par Purpose:
 ** This file contains the function declaration that synchronize the cFE Time
 ** services to the NTP server. Note that the NTP server must be built into the
 ** kernel.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** The way this module updates the local time is by calling the CFE Time Service
 ** function CFE_TIME_SetTime(). The function changes the STCF value.\n
 **
 ** GSFC developers do not recommend to use this method of updating CFE time, 
 ** but rather to use the function CFE_TIME_ExternalTime(). The only way to use
 ** this function is by building an app that will periodically (1Hz) get NTP
 ** time and publish it via Software Bus.
 */


#include <vxWorks.h>
/* Endian information is requried by VxWorks IPCOM for Starting/Stopping NTP Daemon */
#if (_BYTE_ORDER == _LITTLE_ENDIAN )
#define IP_LITTLE_ENDIAN
#elif (_BYTE_ORDER == _BIG_ENDIAN )
#define IP_BIG_ENDIAN
#endif
#include <ipcom_err.h>
#include <taskLib.h>

/* For supporting REALTIME clock */
#include <timers.h>

/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"

#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

#include "psp_time_sync.h"


/** \name NTP Sync Configuration */
/** \{ */
/**
 *  \brief Default NTP Sync Start/Stop on Startup
 * 
 *  \par Description:
 *      Enable or disable the Automatic time sync with the OS
 */
#ifndef CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP
#define CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP true
#endif

/**
 *  \brief Default Synchronization Frequency
 * 
 *  \par Description:
 *      Default number of seconds between time synchronizations.
 *      CFE Time Service updates MET and STCF from VxWorks OS.
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
/** \} */


/**
 ** \brief Default NTP Sync pre-print string 
 ** \par Description:
 ** This string is printed before every print related to NTP Sync API.
*/
#define NTPSYNC_PRINT_SCOPE         "PSP NTP SYNC: "


/* Functions provided by Kernel */
/** \brief VxWorks function to get ID of running task */
extern TASK_ID taskNameToId (char *name);
/** \brief VxWorks function to kill a running daemon */
extern IP_PUBLIC Ip_err ipcom_ipd_kill (const char *name );
/** \brief VxWorks function to start a daemon */
extern IP_PUBLIC Ip_err ipcom_ipd_start (const char *name ); 

/** \brief Convert micro seconds in subseconds
 ** \par Description:
 ** Defined in CFE module time cfe_time.h
 */
extern uint32 CFE_TIME_Micro2SubSecs(uint32);

/** \brief Adjust CFE Time STCF so that local time match the new time.
 ** \par Description:
 ** Defined in CFE module time cfe_time_utils.h
 */
extern void CFE_TIME_SetTime(CFE_TIME_SysTime_t);

/*
** Static function declarations
**
** See function definitions for more information
*/
static void CFE_PSP_TIME_NTPSync_Task(void);

/**** Global variables ****/

/**
 * \brief Contains the NTP Sync Task ID
 * If 0, task is not running
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
static bool g_bEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP;

/**
 * \brief Change how often to sync CFE Time Service with OS Local Time. OS local
 * time is synchronized to NTP server(s) automatically from within OS if 
 * enabled.
 */
static uint16 g_usOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;


/* Declare this file a PSP Module */
/** \brief Macro to define this file a PSP Module */
CFE_PSP_MODULE_DECLARE_SIMPLE(ntp_clock_vxworks); //UndCC_Begin(SSET056) Name format required by PSP API


/**
** \func Initialize the CFE PSP Time Task synchronizing with the NTP server
**
** \par Description:
** This function intializes the cFE PSP Time sync task with the NTP server.
**
** \par Assumptions, External Events, and Notes:
** Function will return CFE_PSP_SUCCESS if there is already an NTP Sync
** task running and will NOT attempt to start another
**
** \param None
** 
** \return #CFE_PSP_SUCCESS - If successfully started NTP Sync task
** \return #CFE_PSP_ERROR - If unsuccessfully started NTP Sync task
*/
int32 CFE_PSP_TIME_NTPSync_Task_Enable(void)
{
    int32       iReturnCode = CFE_PSP_SUCCESS;

    /* Check if NTP Sync task is running */
    if (CFE_PSP_TIME_NTPSync_Task_isRunning() == true)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync task is already running\n");
    }
    else
    {
        /* Attempt to create NTP Sync task */
        iReturnCode = OS_TaskCreate(&g_uiPSPNTPTask_id,
                                    NTPSYNC_TASK_NAME,
                                    CFE_PSP_TIME_NTPSync_Task,
                                    OSAL_TASK_STACK_ALLOCATE, 
                                    OSAL_SIZE_C(1024),
                                    g_ucNTPSyncTaskPriority,
                                    0
                                    );

        if (iReturnCode != OS_SUCCESS)
        {
            iReturnCode = CFE_PSP_ERROR;
            OS_printf(NTPSYNC_PRINT_SCOPE "Error creating NTP Sync task\n");
        }
        else
        {
            /* We have successfully started NTP Sync Task */
            g_bEnableGetTimeFromOS_flag = true;
        }
    }

    return iReturnCode;
}

/**
** \func Disables the CFE PSP Time Task synchronizing with the NTP server
**
** \par Description:
** This function disable the cFE PSP Time sync task with the NTP server.
**
** \par Assumptions, External Events, and Notes:
** Function will return CFE_PSP_SUCCESS if there is no NTP Sync
** task running
**
** \param None
** 
** \return #CFE_PSP_SUCCESS - If successfully started NTP Sync task
** \return #CFE_PSP_ERROR - If unsuccessfully started NTP Sync task
*/
int32 CFE_PSP_TIME_NTPSync_Task_Disable(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /* Check if NTP Sync task is running */
    if (CFE_PSP_TIME_NTPSync_Task_isRunning() == true)
    {
        /* Attempt to delete NTP Sync task */
        if (OS_TaskDelete(g_uiPSPNTPTask_id) == OS_SUCCESS)
        {
            iReturnCode = CFE_PSP_SUCCESS;
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "Unable to delete NTP Sync task\n");
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync task is not running\n");
        iReturnCode = CFE_PSP_SUCCESS;
    }

    /*
    ** No matter what happens, reset ID
    ** and disable get time from OS via flag
    */
    g_uiPSPNTPTask_id = 0;
    g_bEnableGetTimeFromOS_flag = false;

    return iReturnCode;
}

/**
 ** \func Check if the NTP Sync task is running
 **
 ** \par Description:
 ** This function checks on whether or not
 ** the NTP Sync task is running
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will check via OS_TaskGetIdByName using NTPSYNC_TASK_NAME
 ** as the name of NTP sync task
 **
 ** \param - None
 **
 ** \return true - If NTP Sync Task is running
 ** \return false - If NTP Sync Task is not running
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

/**
 ** \func Set the NTP Sync task priority
 **
 ** \par Description:
 ** This function sets the NTP Sync task priority
 **
 ** \par Assumptions, External Events, and Notes:
 ** New priority must be between NTPSYNC_PRIORITY_DOWN_RANGE
 ** and NTPSYNC_PRIORITY_UP_RANGE. If the new priority is not
 ** within this range, the default priority will be assigned.
 **
 ** \param[in] newTaskPriority - The new task priority
 **
 ** \return #CFE_PSP_SUCCESS - If successfully set new priority
 ** \return #CFE_PSP_ERROR - If unsuccessfully set new priority
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


/**
 ** \func Check if the NTP Daemon is running
 **
 ** \par Description:
 ** This function checks if the VxWorks NTP client task is running
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will not check if the task has successfully syncronized with
 ** an NTP server
 **
 ** \param None
 ** 
 ** \return True - If NTP client task is running
 ** \return False - If NTP client task is not running
 */
bool CFE_PSP_TIME_NTP_Daemon_isRunning(void)
{
    bool    bReturnValue = true;
    TASK_ID tidTaskId = 0;

    tidTaskId = taskNameToId(NTP_DAEMON_TASK_NAME);
    if (tidTaskId == CFE_PSP_ERROR)
    {
        bReturnValue = false;
    }

    return bReturnValue;
}

/**
 ** \func Gracefully shutdown NTP Sync Module 
 ** 
 ** \par Description:
 ** Function will attempt to delete the NTP Sync task
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** When this function is called, no matter what it's return status is,
 ** the g_bEnableGetTimeFromOS_flag will be set to false.
 ** Intended only for use upon module initialization, not for
 ** 'normal' use during starting/stopping of NTP Sync task
 ** 
 ** \return #CFE_PSP_SUCCESS 
 ** \return #CFE_PSP_ERROR
 */
int32 ntp_clock_vxworks_Destroy(void) //UndCC_Line(SSET106) Func. not prepended with correct component designator
{
    int32 iReturnValue = CFE_PSP_ERROR;

    if (CFE_PSP_TIME_NTPSync_Task_Disable() == CFE_PSP_SUCCESS)
    {
        iReturnValue = CFE_PSP_SUCCESS;
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Unable to destroy NTP Sync task\n");
    }

    /*
    ** No matter what happens, reset values
    ** and disable get time from os via flag
    */
    g_uiPSPNTPTask_id = 0;
    g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY;
    g_usOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;
    g_bEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP;

    return iReturnValue;
}

/**
 ** \func Entry point for the module
 ** 
 ** \par Description:
 ** Entry point for the module
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will initialize regardless of g_bEnableGetTimeFromOS_flag value.
 ** PSP Module ID is not used in the SP0 implementation
 ** Intended only for use upon module initialization, not for
 ** 'normal' use during starting/stopping of NTP Sync task
 **
 ** \param[in] PspModuleId - Unused
 **
 ** \return None
 */
void ntp_clock_vxworks_Init(uint32 PspModuleId) //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    /* No values need to be initialized here */
    if (CFE_PSP_TIME_NTPSync_Task_Enable() != CFE_PSP_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Unable to initialize NTP Sync\n");
    }
}

/**
 ** \func Get the currently set sync frequency
 **
 ** \par Description:
 ** This function returns the NTP time synchronization frequency, in seconds.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 ** 
 ** \return Current frequency
 */
uint16 CFE_PSP_TIME_NTPSync_GetFreq(void)
{
    /* Return the value of g_usOSTimeSync_Sec */
    return g_usOSTimeSync_Sec;
}

/**
 ** \func Change the sync frequency
 **
 ** \par Description:
 ** This function updates the NTP time synchronization frequency, in seconds.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] uiNewFreqSec - The new frequency, in seconds
 */
void CFE_PSP_TIME_NTPSync_SetFreq(uint16 uiNewFreqSec)
{
    g_usOSTimeSync_Sec = uiNewFreqSec;
}

/**
 ** \func Set the OS time
 **
 ** \par Description:
 ** This function sets the VxWorks OS time.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The changes do not occur if the NTP client is setup to synchronize with an NTP server.
 ** Set the OS CLOCK_REALTIME to a specified timestamp. Parameters are in UNIX time format,
 ** since Epoch 1/1/1970.
 **
 ** \param[in] ts_sec - Time in seconds
 ** \param[in] ts_nsec - Time in nanoseconds
 ** 
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_TIME_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec)
{
    struct timespec     unixTime = {0,0};
    int32               iResult = CFE_PSP_ERROR;
    int32               iReturnStatus = CFE_PSP_SUCCESS;

    unixTime.tv_sec = ts_sec;
    unixTime.tv_nsec = (long) ts_nsec;

    iResult = clock_settime(CLOCK_REALTIME, &unixTime);
    if (iResult == OK)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Clock set");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
        iReturnStatus = CFE_PSP_ERROR;
    }

    return iReturnStatus;
}

/**
 ** \func Gets the current time from VxWorks OS
 **
 ** \par Description:
 ** This function gets the current VxWorks OS time.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is used by the NTP Sync task to grab the current OS time via 
 ** clock_gettime and CLOCK_REALTIME. This function will also be exposed
 ** for use outside of this file
 ** NTP Sync will not occur if NTP time is less than CFE_MISSION_TIME_EPOCH_UNIX_DIFF
 ** Currently, the use of this function is not dependent on the global boolean
 ** g_bEnableGetTimeFromOS_flag
 **
 ** \param[out] myT - Pointer to the variable that stores the returned time value
 ** 
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_TIME_Get_OS_Time(CFE_TIME_SysTime_t *myT)
{
    struct timespec     unixTime = {0,0};
    uint32              tv_sec = 0;
    uint32              tv_msec = 0;
    int                 iResult = CFE_PSP_ERROR;
    int32               iReturnStatus = CFE_PSP_SUCCESS;

    if (myT != NULL)
    {
        /* Get real time clock from OS */
        iResult = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (iResult == OK)
        {
            /* If the unix time has synchronzed with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT->Seconds = unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32)unixTime.tv_nsec / 1000;
                myT->Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
            }
            else
            {
                /* OS has not sync with NTP server yet. */
                iReturnStatus = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
            iReturnStatus = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_TIME_Get_OS_Time called without a proper argument\n");
        iReturnStatus = CFE_PSP_ERROR;
    }
    
    return iReturnStatus;
}

/**
 ** \brief Check if CFS Time Service is up and running
 ** 
 ** \par Description:
 ** It is used on module initialization to wait until the CFE Time Service is
 ** running and ready.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return true - CFE Time Service is ready
 ** \return false - CFE Time Service is not ready
 **
 ** \sa CFE_PSP_TIME_NTPSync_Task
 **
 */
bool CFE_PSP_TIME_CFETimeService_isRunning(void)
{
    bool       iReturnCode = true;
    TASK_ID    tidTaskId = 0;
    
    tidTaskId = taskNameToId(CFE_1HZ_TASK_NAME);
    if (tidTaskId == CFE_PSP_ERROR)
    {
        iReturnCode = false;
    }
    return iReturnCode;
}

/**
 ** \func Update cFE time
 ** 
 ** \par Description:
 ** This function updates the time used by the cFE Time service.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This method is run on an independent thread and will continue to run until 
 ** the thread is deleted using net_clock_linux_destroy
 ** 
 ** \param None
 **
 ** \return None
 */
static void CFE_PSP_TIME_NTPSync_Task(void)
{
    CFE_TIME_SysTime_t  myT = {0,0};
    int32               iStatus = OS_SUCCESS;
    uint32              uiErrorCounter = 0;

   /*
   ** Delay the start of the synchronization until we verify that
   ** the CFE Time Service is running
   **
   ** IF the CFE Time Service does not load in the next
   ** NTPSYNC_INITIAL_TIME_DELAY * NTPSYNC_MAX_ITERATION_TIME_DELAY = 
   **           500 ms           *          120                     = 60 seconds,
   ** then something is broken
   */
    while (CFE_PSP_TIME_CFETimeService_isRunning() == false)
    {
        uiErrorCounter += 1;
        iStatus = OS_TaskDelay(NTPSYNC_INITIAL_TIME_DELAY);
        /* If Task Delay returns error, exit from the function */
        if (iStatus == OS_ERROR)
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "OS_TaskDelay error\n");
            /* Set to maximum value + 1 */
            uiErrorCounter = NTPSYNC_MAX_ITERATION_TIME_DELAY + 1;
            /* Exit loop */
            break;
        }
        if (uiErrorCounter > NTPSYNC_MAX_ITERATION_TIME_DELAY)
        {
            OS_printf(
                NTPSYNC_PRINT_SCOPE
                "(ERROR) CFE TIME Service did not start for 60 seconds - Shutting down NTP Sync\n");
            break;
        }
    }

    /* If within 60 seconds the CFE Time Service is ready, start NTP Sync */
    if (uiErrorCounter < NTPSYNC_MAX_ITERATION_TIME_DELAY)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE TIME Service is ready - Starting NTP Sync\n");
        /* This method is run on an independent thread and will continue to run until the thread is deleted or 
        the OS_TaskDelay function returns and error */
        while (1)
        {
            if (iStatus == OS_SUCCESS)
            {
                /* If the flag is enabled */
                if (g_bEnableGetTimeFromOS_flag)
                {
                    /* Get real time clock from OS */
                    iStatus = CFE_PSP_TIME_Get_OS_Time(&myT);
                    
                    /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
                    if (iStatus == CFE_PSP_SUCCESS)
                    {
                        CFE_TIME_SetTime(myT);
                    }
                    else
                    {
                        /* OS has not sync with NTP server yet. */
                        OS_printf(NTPSYNC_PRINT_SCOPE "OS has not sync with NTP server yet, trying again later.\n");
                    }
                }
                else
                {
                    OS_printf(NTPSYNC_PRINT_SCOPE "Get Time from OS disabled\n");
                }
            }
            else
            {
                OS_printf(NTPSYNC_PRINT_SCOPE "OS_TaskDelay error\n");
                /* If the OS_TaskDelay function returns an error, there is a chance that this
                while loop will consume too many resources. It should be terminated. */
                break;
            }
            
            /*
            ** We could move this OS_TaskDelay function call into 
            ** the if statement on L:658 to save another 32 bits,
            ** though we would have to delay for a time period
            ** before first time sync
            */
            iStatus = OS_TaskDelay(g_usOSTimeSync_Sec * 1000U);
        }
        /* The only way to get here is if there was a major error and the while 
        loop needed to be stopped. From here we need to clean up and get it ready
        to start again. */
        g_bEnableGetTimeFromOS_flag = false;
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Sync encountered an error that caused the main task to exit\n");
    }
}

/**
 ** \func Start the NTP client
 ** 
 ** \par Description:
 ** This function starts the NTP client task, ipntpd, on VxWorks.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param None
 **
 ** \return NTP client Task ID
 ** \return IPCOM_ERROR
 */
TASK_ID CFE_PSP_TIME_StartNTPDaemon(void)
{
    int32       iStatus = CFE_PSP_ERROR;
    TASK_ID     tidTaskId = 0;

    iStatus = ipcom_ipd_start(NTP_DAEMON_TASK_NAME);
    if (iStatus == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully\n");
        tidTaskId = taskNameToId(NTP_DAEMON_TASK_NAME);
    }
    else if (iStatus == IPCOM_ERR_ALREADY_STARTED)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already started\n");
        tidTaskId = taskNameToId(NTP_DAEMON_TASK_NAME);
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)\n", iStatus);
        tidTaskId = IPCOM_ERR_NOT_STARTED;
    }

    return tidTaskId;
}

/**
 ** \func Stop the NTP client
 ** 
 ** \par Description:
 ** This function stops the NTP client task, ipntpd, on VxWorks.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_TIME_StopNTPDaemon(void)
{
    int32       iReturnCode = CFE_PSP_SUCCESS;
    int32       iIPCOMStatus = CFE_PSP_ERROR;

    iIPCOMStatus = ipcom_ipd_kill(NTP_DAEMON_TASK_NAME);
    if (iIPCOMStatus == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully\n");
    }
    else
    {
        /* Although the name, OS user guide describes IPCOM_ERR_NOT_STARTED as "Dameon is not running" */
        if (iIPCOMStatus == IPCOM_ERR_NOT_STARTED)
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped\n");
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)\n",iIPCOMStatus);
        }
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}
