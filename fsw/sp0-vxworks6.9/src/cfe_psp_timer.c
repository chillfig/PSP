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

/*
**  Include Files
*/
#include <vxWorks.h>
#include <sysLib.h>
#include <vxLib.h>
#include "osapi.h"
#include "cfe.h"
#include "cfe_psp.h"
#include "cfe_time.h"
#include "target_config.h"
#include "sp0.h"

/* VxWorks IPCOM for Starting/Stopping NTP Daemon */
#define IP_PORT_VXWORKS 69
#include "ipcom_err.h"
#include "taskLib.h"

/* For supporting REALTIME clock */
#include <timers.h>
/* For supporting SetTime and SetSTCF */
#include "cfe_time_utils.h"
#include "osapi-os-core.h"
#include "cfe_psp_config.h"

/*
** External Function Prototypes
*/
extern TASK_ID taskNameToId (char * name);
extern IP_PUBLIC Ip_err ipcom_ipd_kill (const char *name );
extern IP_PUBLIC Ip_err ipcom_ipd_start (const char *name );  


/*
** Function Prototypes
*/
void CFE_PSP_Get_OS_Time(uint32 timer_id);

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


/*
** Macro Definitions
*/


/* CFE_PSP_TIMER_LOW32_ROLLOVER
**
**   The number that the least significant 32 bits of the 64-bit timestamp returned
**   by CFE_PSP_Get_Timebase() rolls over.
**
**   If the lower 32 bits rolls at 1 second, then
**     CFE_PSP_TIMER_LOW32_ROLLOVER will be 1000000.
**   If the lower 32 bits rolls at its maximum value (2^32), then
**     CFE_PSP_TIMER_LOW32_ROLLOVER will be 0.
*/
#define CFE_PSP_TIMER_LOW32_ROLLOVER  0

static uint32 tickPerSecond = 0;

/******************************************************************************
**  Function:  CFE_PSP_GetTime()
**
**  Purpose:
**    Gets the value of the time from the hardware using the processor TBR
**
**  Arguments:
**    Output - pLocalTime - pointer to local time variable
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_GetTime(OS_time_t *pLocalTime)
{
    uint32 tbu = 0;
    uint32 tbl = 0;
    unsigned long long tb = 0;
    static uint32 errorCount = 0;

    if (pLocalTime != NULL)
    {

    	vxTimeBaseGet((UINT32 *)&tbu, (UINT32 *)&tbl);

        /* Re-assemble the 64-bit count */
        tb = ((unsigned long long)tbu << 32) | (unsigned long long)tbl;
        /* If the ticks per second is default zero, then initialize it.
         * The SP0 value is 41666666 and the SP0-S value is 49999999
         */
        if (tickPerSecond == 0)
        {
            tickPerSecond = sysTimestampFreq();
        }
        if (tickPerSecond != 0)
        {
                /* Convert to seconds and microseconds using only integer computations.
                 * The max seconds value is 442721864852  for the SP0 and 36893488221 for
                 * the SP0-S. The conversion below will cause a loss of data which is just
                 * a roll over of the clock for the local time defined in osal.
                 */
                pLocalTime->seconds   = (tb / tickPerSecond);
                pLocalTime->microsecs = ((tb % tickPerSecond) * 1000000 )/(tickPerSecond);

        }
        else if (errorCount < 2)
        {
            OS_printf("CFE_PSP: CFE_PSP_GetTime() - tickPerSecond equals zero.\n");
            errorCount++;
        }
    }
}

/**
 * \brief Initialize the CFE PSP Time Task synchronizing with the NTP server
 */
int32 CFE_PSP_TIME_Init(uint16 timer_frequency_sec)
{
    
    /* Initialize */
    int32       Status;
    int32       return_status = CFE_SUCCESS;
    osal_id_t   PSP_NTP_TimerId = 0;
    uint32      PSP_NTP_ClockAccuracy = 0;
    uint32      timer_interval_msec = timer_frequency_sec * 1000000;

    /*Create the 1Hz timer for synchronizing the major frame*/
    Status = OS_TimerCreate(&PSP_NTP_TimerId,
                            "PSPNTPSync",
                            &PSP_NTP_ClockAccuracy,
                            CFE_PSP_Get_OS_Time);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Failed to create PSP_NTP_Sync task.\n");
        return_status = CFE_PSP_ERROR;
    }
    else
    {
        /*Set the interval to one second in microseconds for the first time call, then every timer_frequency_sec.*/
        Status = OS_TimerSet(PSP_NTP_TimerId, 1000000, timer_interval_msec);
        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Failed to set PSP_NTP_Sync task.\n");
            return_status = CFE_PSP_ERROR;
        }
        else
        {
            CFE_ES_WriteToSysLog("CFE_PSP: PSP_NTP_Sync task initialized at %u.\n",timer_frequency_sec);
        }
    }
    return return_status;
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
/**
 * \brief Enable or Disable Time Sync with OS 
 */
int32 CFE_PSP_Sync_From_OS_Enable(bool enable)
{
    
    if (enable)
    {
        CFE_ES_WriteToSysLog("Sync with OS is enabled\n");
    }
    else
    {
        CFE_ES_WriteToSysLog("Sync with OS is disabled\n");
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
**    int32 - True if NTP Daemon is running, False if it is not running
******************************************************************************/
/**
 * \brief Get status of NTP daemon
 */
int32 CFE_PSP_NTP_Daemon_Get_Status(void)
{
    int32       return_code = true;
    TASK_ID     ret;
    
    ret = taskNameToId("ipntpd");
    if (ret == ERROR)
    {
        return_code = false;
    }
    return return_code;
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
/**
 * \brief Changes the synchronization frequency with local OS time
 */
int32 CFE_PSP_Sync_From_OS_Freq(uint16 new_frequency_sec)
{
    osal_id_t   timer_id;
    const char  *task_name = "PSPNTPSync";
    int32       return_value = CFE_PSP_SUCCESS;
    int32       ret;

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

        /* Find timer by name */
        ret = OS_TimerGetIdByName(&timer_id,task_name);
        if (ret != OS_SUCCESS)
        {
            CFE_ES_WriteToSysLog("Could not update the Time Sync frequency");
            return_value == CFE_PSP_ERROR;
        }

        if (return_value != CFE_PSP_ERROR)
        {
            /* Delete timer */
            ret = OS_TimerDelete(timer_id);
            if (ret != OS_SUCCESS)
            {
                CFE_ES_WriteToSysLog("Could not delete Time Sync process");
                return_value == CFE_PSP_ERROR;
            }
        }

        if (return_value != CFE_PSP_ERROR)
        {
            /* Reinitialize timer with new updated value */
            ret = CFE_PSP_TIME_Init(cfe_OS_Time_Sync_Sec);
            if (ret != CFE_PSP_SUCCESS)
            {
                return_value == CFE_PSP_ERROR;
            }
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
/**
 * \brief Set local OS clock to specific timestamp 
 */
int32 CFE_PSP_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec)
{
    struct timespec     unixTime;
    int                 ret;
    int32               return_status = CFE_PSP_SUCCESS;

    if (ts_sec > 0 && ts_nsec >=0)
    {
        unixTime.tv_sec = ts_sec;
        unixTime.tv_nsec = ts_nsec;

        ret = clock_settime(CLOCK_REALTIME, &unixTime);
        if (ret == OK)
        {
            CFE_ES_WriteToSysLog("Clock set");
        }
        else
        {
            CFE_ES_WriteToSysLog("ERROR Clock not set");
            return_status = CFE_PSP_ERROR;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("ERROR: Invalid timestamp");
        return_status = CFE_PSP_ERROR;
    }

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
/**
 * \brief Get local OS timestamp
 */
void CFE_PSP_Get_OS_Time(uint32 timer_id)
{
    struct timespec     unixTime;
    uint32              tv_sec = 0;
    uint32              tv_msec = 0;
    CFE_TIME_SysTime_t  myT;
    int                 ret;

    /* If the flag is enabled */
    if (getTime_From_OS_flag)
    {
        /* Get real time clock from OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == OK)
        {

            /* If the unix time has synchronzed with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT.Seconds = unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32)unixTime.tv_nsec / 1000;
                myT.Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
                CFE_TIME_SetTime(myT);
            }
            else
            {
                /* OS has not sync with NTP server yet. */
                CFE_ES_WriteToSysLog("CFE_PSP: OS has not sync with NTP server yet, trying again later.\n");
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("clock_gettime function failed\n");
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
/**
 * \brief Start NTP daemon
 */
int32 CFE_PSP_StartNTPDaemon(void)
{
    int32       return_code = 0;
    int32       ret;
    
    ret = ipcom_ipd_start("ipntp");
    if (ret == IPCOM_SUCCESS)
    {
        CFE_ES_WriteToSysLog("NTP Daemon Started Successfully");
        return_code = taskNameToId("ipntpd");
    }
    else
    {
        if (ret == IPCOM_ERR_ALREADY_STARTED)
        {
            CFE_ES_WriteToSysLog("NTP Daemon already started");
            return_code = taskNameToId("ipntpd");
        }
        else
        {
            CFE_ES_WriteToSysLog("ERROR NTP Daemon did not Start (ip_err = %d)",ret);
            return_code = CFE_PSP_ERROR;
        }
    }
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
/**
 * \brief Stop NTP daemon
 */
int32 CFE_PSP_StopNTPDaemon(void)
{
    int32       return_code = CFE_PSP_SUCCESS;
    int32       ret;

    ret = ipcom_ipd_kill("ipntp");
    if (ret == IPCOM_SUCCESS)
    {
        CFE_ES_WriteToSysLog("NTP Daemon Stopped Successfully");
    }
    else
    {
        /* Although the name, OS user guide describes IPCOM_ERR_NOT_STARTED as "Dameon is not running" */
        if (ret == IPCOM_ERR_NOT_STARTED)
        {
            CFE_ES_WriteToSysLog("NTP Daemon already stopped");
        }
        else
        {
            CFE_ES_WriteToSysLog("ERROR NTP Daemon did not stop (ip_err = %d)",ret);
        }
        return_code = CFE_PSP_ERROR;
    }

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
/**
 * \brief Enable or Disable the NTP daemon
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

/******************************************************************************
**  Function:  CFE_PSP_Get_Timer_Tick()
**
**  Purpose:
**    Provides a common interface to system clock tick. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**    None
**
**  Return:
**    OS system clock ticks per second
******************************************************************************/
uint32 CFE_PSP_Get_Timer_Tick(void)
{
    return ((uint32)sysClkRateGet());
}


/******************************************************************************
**  Function:  CFE_PSP_GetTimerTicksPerSecond()
**
**  Purpose:
**    Provides number of timer ticks per second
**
**  Arguments:
**    None
**
**  Return:
**    Value of ticks per second  returned by sysTimestampFreq
******************************************************************************/
uint32 CFE_PSP_GetTimerTicksPerSecond(void)
{
    return (tickPerSecond);
}


/******************************************************************************
**  Function:  CFE_PSP_GetTimerLow32Rollover()
**
**  Purpose:
**    Provides the value of the least significant 32 bits of the 64-bit timestamp
**
**  Arguments:
**    None
**
**  Return:
**    Value of CFE_PSP_TIMER_LOW32_ROLLOVER macro constant
******************************************************************************/
uint32 CFE_PSP_GetTimerLow32Rollover(void)
{
    return ((uint32)CFE_PSP_TIMER_LOW32_ROLLOVER);
}


/******************************************************************************
**  Function:  CFE_PSP_Get_Timebase()
**
**  Purpose:
**    Provides the Time-Base Register (TBR) values
**
**  Arguments:
**    Output - tbu - pointer to tbu variable
**    Output - tbl - pointer to tbl variable
**
**  Return:
**      Time-Base Register (TBR) values
******************************************************************************/
void CFE_PSP_Get_Timebase(uint32 *tbu, uint32* tbl)
{
    if ((tbu != NULL) && (tbl != NULL))
    {
        vxTimeBaseGet((UINT32 *)tbu, (UINT32 *)tbl);
    }
}


/******************************************************************************
**  Function:  CFE_PSP_Get_Dec()
**
**  Purpose:
**    Provides a common interface to decrementer counter. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**    None
**
**  Return:
**    ??? value
******************************************************************************/
uint32 CFE_PSP_Get_Dec(void)
{
    return ((uint32)vxDecGet());
}


