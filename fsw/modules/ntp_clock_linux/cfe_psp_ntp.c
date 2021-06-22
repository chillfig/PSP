/************************************************************************************************
**
** File: cfe_psp_timer.c
**
**      Copyright (c) 2004-2011, United States Government as represented by
**      Administrator for The National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package (BSP).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
*************************************************************************************************/


/* For supporting REALTIME clock */
#include <time.h>
#include "cfe_time_extern_typedefs.h"

#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

/**
 *  \cfetimecfg Default EPOCH Values 
 * 
 *  \par Description:
 *      Enable or disable the Automatic time sync with the OS
 * 
 *  \par Limits
 *      Binary true or false
 */
#define CFE_MISSION_TIME_SYNC_OS_ENABLE true

/**
 *  \cfetimecfg Default EPOCH Values 
 * 
 *  \par Description:
 *      Default number of seconds between time synchronizations.
 *      CFE Time Service updates MET and STCF from VxWorks OS.
 *      When set to zero, CFE Time will be synchronized only once during start.
 * 
 *  \par Limits
 *      Positive integer
 */
#define CFE_MISSION_TIME_SYNC_OS_SEC 30


/*
** External Function Prototypes
*/

/* Below are defined in CFE module time cfe_time.h and cfe_time_utils.h */
extern uint32 CFE_TIME_Micro2SubSecs(uint32);

/* 
SetTime may not be necessary since the function CFE_TIME_SetTimeCmd in CFE module time
can take the ground command directly
*/
extern void CFE_TIME_SetTime(CFE_TIME_SysTime_t);


/* Declare this file a PSP Module */
CFE_PSP_MODULE_DECLARE_SIMPLE(ntp_clock_linux);

/*
** Function Prototypes
*/
int32 CFE_PSP_TIME_Init(uint16);
void CFE_PSP_Get_OS_Time(uint32);

/**
 * \brief Boolean variable to control if to synchronize CFE Time Service with OS
 * local time. True, synch will occur. False, timer will not be disabled, but 
 * sync will not execute.
 */
bool getTime_From_OS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

/**
 * \brief Change how often to sync CFE Time Service with OS Local Time. OS local
 * time is synchronized to NTP server(s) automatically from within OS if 
 * enabled.
 */
uint16 cfe_OS_Time_Sync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;

/**
 * @brief 
 * 
 * @param PspModuleId 
 */
void ntp_clock_linux_Init(uint32 PspModuleId)
{
    if (getTime_From_OS_flag)
    {
        CFE_PSP_TIME_Init(cfe_OS_Time_Sync_Sec);
    }
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_Init()
**
**  Purpose:
**    Initialize the CFE PSP Time Task synchronizing with the NTP server
**
**  Arguments:
**    timer_frequency_sec is the update frequency in seconds
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_TIME_Init(uint16 timer_frequency_sec)
{
    
    /* Initialize */
    int32       status;
    osal_id_t   PSP_NTP_TimerId = 0;
    uint32      PSP_NTP_ClockAccuracy = 0;
    uint32      timer_interval_msec = timer_frequency_sec * 1000000U;

    /*Create the 1Hz timer for synchronizing the major frame*/
    status = OS_TimerCreate(&PSP_NTP_TimerId,
                            "PSPNTPSync",
                            &PSP_NTP_ClockAccuracy,
                            CFE_PSP_Get_OS_Time);
    if (status != CFE_PSP_SUCCESS)
    {
        printf("Failed to create PSP_NTP_Sync task.\n");
    }
    else
    {
        /*Set the interval to one second in microseconds for the first time call, then every timer_frequency_sec.*/
        status = OS_TimerSet(PSP_NTP_TimerId, 1000000, timer_interval_msec);
        if (status != CFE_PSP_SUCCESS)
        {
            /* TODO: Need to delete the Timer created by OS_TimerCreate? */
            printf("Failed to set PSP_NTP_Sync task.\n");
        }
        else
        {
            printf("CFE_PSP: PSP_NTP_Sync task initialized at %u.\n",timer_frequency_sec);
        }
    }

    return status;
}

/******************************************************************************
**  Function:  CFE_PSP_Sync_From_OS_Enable(bool)
**
**  Purpose:
**    Enable or Disable CFE_PSP_Get_OS_Time
**
**  Arguments:
**    bool - When True, CFE_PSP_Get_OS_Time will be called to synchronize time
**
**  Return:
**    int32 - input argument
******************************************************************************/
int32 CFE_PSP_Sync_From_OS_Enable(bool enable)
{
    
    if (enable)
    {
        printf("Sync with OS is enabled\n");
    }
    else
    {
        printf("Sync with OS is disabled\n");
    }
    /* Set flag */
    getTime_From_OS_flag = enable;

    return (int32) getTime_From_OS_flag;
}

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Get_Status(void)
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
bool CFE_PSP_NTP_Daemon_Get_Status(void)
{
    int32       return_code = true;
    
    printf("CFE_PSP_NTP_Daemon_Get_Status not implemented in Linux OS");

    return return_code;
}

/**
 * @brief 
 * 
 * @return int32 
 */
int32 net_clock_linux_Destroy()
{
    osal_id_t   timer_id;
    const char  *task_name = "PSPNTPSync";
    int32       return_value = CFE_PSP_SUCCESS;
    int32       ret;

    /* Find timer by name */
    ret = OS_TimerGetIdByName(&timer_id,task_name);
    if (ret != OS_SUCCESS)
    {
        printf("Could not update the Time Sync frequency");
        return_value = CFE_PSP_ERROR;
    }

    if (return_value != CFE_PSP_ERROR)
    {
        /* Delete timer */
        ret = OS_TimerDelete(timer_id);
        if (ret != OS_SUCCESS)
        {
            printf("Could not delete Time Sync process");
            return_value = CFE_PSP_ERROR;
        }
    }

    return return_value;
}

/******************************************************************************
**  Function:  CFE_PSP_Sync_From_OS_Freq(uint16_t)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    uint16 - seconds between updates. If zero, returns the current value
**
**  Return:
**    int - CFE_PSP_SUCCESS or the current cfe_OS_Time_Sync_Sec value
******************************************************************************/
int32 CFE_PSP_Sync_From_OS_Freq(uint16 new_frequency_sec)
{
    int32       return_value = CFE_PSP_SUCCESS;

    if (new_frequency_sec == 0)
    {
        /* Return the value of cfe_OS_Time_Sync_Sec */
        return_value = (int32)cfe_OS_Time_Sync_Sec;
    }
    else
    {
        /* Set a new value of cfe_OS_Time_Sync_Sec */
        /* Update frequency with new value */
        cfe_OS_Time_Sync_Sec = new_frequency_sec;

        net_clock_linux_Destroy();

        if (return_value != CFE_PSP_ERROR)
        {
            /* Reinitialize timer with new updated value */
            ntp_clock_linux_Init((uint32)0);
        }
    }

    return return_value;
}

/******************************************************************************
**  Function:  CFE_PSP_Set_OS_Time()
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
int32 CFE_PSP_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec)
{
    int32               return_status = CFE_PSP_SUCCESS;

    printf("CFE_PSP_Set_OS_Time not implemented in Linux OS");

    return return_status;
}

/******************************************************************************
**  Function:  CFE_PSP_Get_OS_Time()
**
**  Purpose:
**    Syncronize CFE Time Service with OS local time. OS time is 
**    automatically syncronized to NTP server
**    Declaration is dictated by OSAL, see osapi-os-timer.h OS_TimerCreate info
**
**    IMPORTANT: Function declaration is dictated by OS_TimerCreate function.
**
**  Arguments:
**    timer_id
**
**  Return:
**    void
******************************************************************************/
void CFE_PSP_Get_OS_Time(uint32 timer_id)
{
    struct timespec     unixTime;
    uint32              tv_msec = 0;
    CFE_TIME_SysTime_t  myT;
    int                 ret;

    /* If the flag is enabled */
    if (getTime_From_OS_flag)
    {
        /* Get real time clock from OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == CFE_PSP_SUCCESS)
        {

            /* If the unix time has synchronzed with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT.Seconds = (uint32) unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32)unixTime.tv_nsec / 1000;
                myT.Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
                CFE_TIME_SetTime(myT);
            }
            else
            {
                /* OS has not sync with NTP server yet. */
                printf("CFE_PSP: OS has not sync with NTP server yet, trying again later.\n");
            }
        }
        else
        {
            printf("clock_gettime function failed\n");
        }
    }
}

/******************************************************************************
**  Function:  CFE_PSP_StartNTPDaemon()
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
int32 CFE_PSP_StartNTPDaemon(void)
{
    int32       return_code = 0;
    
    printf("CFE_PSP_StartNTPDaemon not implemented in Linux OS");

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_StopNTPDaemon()
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
int32 CFE_PSP_StopNTPDaemon(void)
{
    int32       return_code = CFE_PSP_SUCCESS;

    printf("CFE_PSP_StopNTPDaemon not implemented in Linux OS");

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Enable(bool)
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
int32 CFE_PSP_NTP_Daemon_Enable(bool enable)
{
    int32   return_code = CFE_PSP_SUCCESS;

    printf("CFE_PSP_NTP_Daemon_Enable not implemented in Linux OS");

    return return_code;
}