/**
 **
 ** \file cfe_psp_ntp.c
 **
 ** \brief API to control NTP Sync
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved. \n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that 
 ** agreement.
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


/* VxWorks IPCOM for Starting/Stopping NTP Daemon */
/** \brief VxWorks IPCOM Specific Configuration */
#define IP_PORT_VXWORKS 69
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
#ifndef CFE_MISSION_TIME_SYNC_OS_ENABLE
#define CFE_MISSION_TIME_SYNC_OS_ENABLE true
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

/**** Global variables ****/

/**
 * \brief Contains the NTP Sync Task ID
 * If 0, task is not running
 */
static uint32 g_uiPSPNTPTask_id = 0;

/**
 * \brief Current value of NTP Sync priority task.
 * 
 */
static osal_priority_t g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY;

/**
 * \brief Boolean variable to control if to synchronize CFE Time Service with OS
 * local time. True, synch will occur. False, timer will not be disabled, but 
 * sync will not execute.
 */
static bool g_iEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

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
** None
**
** \param[in] timer_frequency_sec - The update frequency, in seconds
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_TIME_Init(uint16 timer_frequency_sec)
{
    /* Initialize */
    int32       status = OS_ERROR;
    
    /*Create the 1Hz task for synchronizing with OS time*/
    status = OS_TaskCreate(&g_uiPSPNTPTask_id,
                            NTPSYNC_TASK_NAME,
                            CFE_PSP_Update_OS_Time,
                            OSAL_TASK_STACK_ALLOCATE, 
                            OSAL_SIZE_C(1024),
                            g_ucNTPSyncTaskPriority,
                            0
                            );

    if (status != OS_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Failed to create task\n");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Task Initialized\n");
    }

    return status;
}

/**
** \func Enable/disable time sync
**
** \par Description:
** This function sets the enabling/disabling of time sync.
** When the flag is true, the NTP Sync task actively trys to sync clocks.
** When the flag is false, the NTP Sync task will remain active without sync.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] enable - Boolean flag for sync or not sync
** 
** \return True - If synchronized
** \return False - If not synchronized
*/
int32 CFE_PSP_Sync_From_OS_Enable(bool enable)
{
    
    if (enable)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Sync with OS is enabled\n");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Sync with OS is disabled\n");
    }

    /* Set flag */
    g_iEnableGetTimeFromOS_flag = enable;

    return (int32) g_iEnableGetTimeFromOS_flag;
}

/**
** \func Get the NTP daemon status
**
** \par Description:
** This function checks if the VxWorks NTP client task is running. It does not
** check if the task has successfully synchronized with an NTP server.
**
** \par Assumptions, External Events, and Notes:
** The task name for the VxWorks NTP client is the default "ipntpd".
**
** \param None
** 
** \return True - If NTP client task is running
** \return False - If NTP client task is not running
*/
bool CFE_PSP_NTP_Daemon_Get_Status(void)
{
    bool       return_code = true;
    TASK_ID    ret = CFE_PSP_ERROR;
    
    ret = taskNameToId("ipntpd");
    if (ret == CFE_PSP_ERROR)
    {
        return_code = false;
    }
    return return_code;
}

/**
 ** \func Gracefully shutdown NTP Sync Module 
 ** 
 ** \par Description:
 ** Function will attempt to delete the task. Usually this function will be 
 ** called when exiting cFS.
 ** 
 ** \return #CFE_PSP_SUCCESS 
 ** \return #CFE_PSP_ERROR
 */
int32 net_clock_vxworks_Destroy(void)  //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    int32       return_value = CFE_PSP_SUCCESS;
    int32       ret = CFE_PSP_ERROR;

    /* Disable time update */
    g_iEnableGetTimeFromOS_flag = false;
    
    /* Delete task */
    ret = OS_TaskDelete(g_uiPSPNTPTask_id);

    if (ret == OS_SUCCESS)
    {
        g_uiPSPNTPTask_id = 0;
    }
    else if (ret == OS_ERR_INVALID_ID)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Could not kill the NTP Sync task\n");
        return_value == CFE_PSP_ERROR;
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Could not kill the NTP Sync task\n");
        return_value == CFE_PSP_ERROR;
    }

    return return_value;
}

/**
 ** \func Entry point for the module
 ** 
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[inout] PspModuleId - Unused
 **
 ** \return None
 */
void ntp_clock_vxworks_Init(uint32 PspModuleId) //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    if (g_iEnableGetTimeFromOS_flag)
    {
        CFE_PSP_TIME_Init(g_usOSTimeSync_Sec);
    }
}

/**
** \func Change the sync frequency
**
** \par Description:
** This function updates the NTP time synchronization frequency, in seconds.
**
** \par Assumptions, External Events, and Notes:
** If 0 is passed in, the function returns the current frequency.
**
** \param[in] new_frequency_sec - The new frequency, in seconds
** 
** \return #CFE_PSP_SUCCESS - If successfully changed
** \return Current frequency - If passed in 0 for new_frequency_sec
*/
int32 CFE_PSP_Sync_From_OS_Freq(uint16 new_frequency_sec)
{
    int32       return_value = CFE_PSP_SUCCESS;

    if (new_frequency_sec == 0)
    {
        /* Return the value of g_usOSTimeSync_Sec */
        return_value = (int32)g_usOSTimeSync_Sec;
    }
    else
    {
        /* Set a new value of g_usOSTimeSync_Sec */
        /* Update frequency with new value */
        g_usOSTimeSync_Sec = new_frequency_sec;

        net_clock_vxworks_Destroy();

        if (return_value != CFE_PSP_ERROR)
        {
            /* Reinitialize timer with new updated value */
            ntp_clock_vxworks_Init((uint32)0);
        }
    }

    return return_value;
}

/**
** \func Set the OS time
**
** \par Description:
** This function sets the VxWorks OS time.
**
** \par Assumptions, External Events, and Notes:
** The changes do not occur if the NTP client is setup to synchrone with an NTP server.
** Set the OS CLOCK_REALTIME to a specified timestamp. Parameters are in UNIX time format,
** since Epoch 1/1/1970.
**
** \param[in] ts_sec - Time in seconds
** \param[in] ts_nsec - Time in nanoseconds
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec)
{
    struct timespec     unixTime = {0,0};
    int                 ret = CFE_PSP_ERROR;
    int32               return_status = CFE_PSP_SUCCESS;

    if ((ts_sec > 0) && (ts_nsec >=0))
    {
        unixTime.tv_sec = ts_sec;
        unixTime.tv_nsec = (long) ts_nsec;

        ret = clock_settime(CLOCK_REALTIME, &unixTime);
        if (ret == OK)
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "Clock set");
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
            return_status = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "ERROR: Invalid timestamp");
        return_status = CFE_PSP_ERROR;
    }

    return return_status;
}

/**
** \func Gets the current time from VxWorks OS
**
** \par Description:
** This function gets the current VxWorks OS time.
**
** \par Assumptions, External Events, and Notes:
** This function is used by the NTP Sync task to grab the current OS time.
** It uses CLOCK_REALTIME.
**
** \param[inout] myT - Pointer to the variable that stores the returned time value
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_Get_OS_Time(CFE_TIME_SysTime_t *myT)
{
    struct timespec     unixTime = {0,0};
    uint32              tv_sec = 0;
    uint32              tv_msec = 0;
    int                 ret = CFE_PSP_ERROR;
    int32               return_code = CFE_PSP_SUCCESS;

    if (myT != NULL)
    {
        /* Get real time clock from OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == OK)
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
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_Get_OS_Time called without a proper argument\n");
        return_code = CFE_PSP_ERROR;
    }
    
    return return_code;
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
 ** \sa CFE_PSP_Update_OS_Time
 **
 */
bool CFE_PSP_TimeService_Ready(void)
{
    bool       return_code = true;
    TASK_ID    ret = 0;
    
    ret = taskNameToId("TIME_1HZ_TASK");
    if (ret == CFE_PSP_ERROR)
    {
        return_code = false;
    }
    return return_code;
}


/**
 ** \func Update cFE time
 ** 
 ** \par Description:
 ** This function updates the time used by the cFE Time service.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will run forever until its task is deleted.
 ** 
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_Update_OS_Time(void)
{
    CFE_TIME_SysTime_t  myT = {0,0};
    int32               ret = OS_SUCCESS;
    uint32              sleep_time = 0;
    uint32              error_counter = 0;

    /*
    Delay the start of the synchronization until we verify that the 
    CFE Time Service is running
    If the CFE Time Service does not load in the next 500ms * 120 = 60 seconds,
    something is broken.
    */
    while (CFE_PSP_TimeService_Ready() == false)
    {
        error_counter += 1;
        OS_TaskDelay(500);
        if (error_counter > 120)
        {
            OS_printf(
                NTPSYNC_PRINT_SCOPE
                "(ERROR) CFE TIME Service did not start for 60 seconds - Shutting down NTP Sync\n");
            break;
        }
    }

    /* If within 60 seconds the CFE Time Service is ready, start NTP Sync */
    if (error_counter < 120)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE TIME Service is ready - Starting NTP Sync\n");
        while (1)
        {
            if (ret == OS_SUCCESS)
            {
                /* If the flag is enabled */
                if (g_iEnableGetTimeFromOS_flag)
                {
                    /* Get real time clock from OS */
                    ret = CFE_PSP_Get_OS_Time(&myT);
                    
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

            sleep_time = g_usOSTimeSync_Sec * 1000U;
            ret = OS_TaskDelay(sleep_time);
        }
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
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_StartNTPDaemon(void)
{
    int32       return_code = 0;
    int32       ret = CFE_PSP_ERROR;
    
    ret = ipcom_ipd_start("ipntp");
    if (ret == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
        return_code = taskNameToId("ipntpd");
    }
    else
    {
        if (ret == IPCOM_ERR_ALREADY_STARTED)
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
            return_code = taskNameToId("ipntpd");
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)",ret);
            return_code = CFE_PSP_ERROR;
        }
    }
    return return_code;
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
int32 CFE_PSP_StopNTPDaemon(void)
{
    int32       return_code = CFE_PSP_SUCCESS;
    int32       ret = CFE_PSP_ERROR;

    ret = ipcom_ipd_kill("ipntp");
    if (ret == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully");
    }
    else
    {
        /* Although the name, OS user guide describes IPCOM_ERR_NOT_STARTED as "Dameon is not running" */
        if (ret == IPCOM_ERR_NOT_STARTED)
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped");
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)",ret);
        }
        return_code = CFE_PSP_ERROR;
    }

    return return_code;
}

/**
** \func Enable/disable the NTP client
** 
** \par Description:
** This function enables/disables the NTP client task, ipntpd, on VxWorks.
**
** \par Assumptions, External Events, and Notes:
** None
** 
** \param[in] enable - Boolean flag for enable or disable
**
** \return NTP client task ID - If successfully starts the NTP clien task
** \return #CFE_PSP_SUCCESS - If successfully stops the NTP client task
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_NTP_Daemon_Enable(bool enable)
{
    int32   return_code = CFE_PSP_SUCCESS;

    if (enable)
    {
        return_code = CFE_PSP_StartNTPDaemon();
    }
    else
    {
        return_code = CFE_PSP_StopNTPDaemon();
    }

    return return_code;
}