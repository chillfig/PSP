/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/************************************************************************************************
** File:  cfe_psp_timer.c
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  K.Audra    | Initial version,
**
*************************************************************************************************/

/*
**  Include Files
*/


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

#include <time.h>
#include "cfe.h"
#include "cfe_time.h"
#include "cfe_time_utils.h"

/*
**  System Include Files
*/
#include <stdio.h>
#include <stdlib.h>

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_config.h"

/**
 * \brief Boolean variable to control if to synchronize CFE Time Service with OS
 * local time. True, synch will occur. False, timer will not be disabled,
 * but sync will not execute
 */
bool getTime_From_OS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

/**
 * \brief Change how often to sync CFE Time Service with OS Local Time. OS local
 * time is synchronized to NTP server(s) automatically from within OS if 
 * enabled.
 */
uint16 cfe_OS_Time_Sync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;

/*
** Function Prototypes
*/
void CFE_PSP_Get_OS_Time(uint32 timer_id);

/******************* Macro Definitions ***********************/

#define CFE_PSP_TIMER_TICKS_PER_SECOND       1000000     /* Resolution of the least significant 32 bits of the 64 bit
                                                           time stamp returned by OS_BSPGet_Timebase in timer ticks per second.
                                                           The timer resolution for accuracy should not be any slower than 1000000
                                                           ticks per second or 1 us per tick */
#define CFE_PSP_TIMER_LOW32_ROLLOVER         1000000     /* The number that the least significant 32 bits of the 64 bit
                                                           time stamp returned by OS_BSPGet_Timebase rolls over.  If the lower 32
                                                           bits rolls at 1 second, then the OS_BSP_TIMER_LOW32_ROLLOVER will be 1000000.
                                                           if the lower 32 bits rolls at its maximum value (2^32) then
                                                           OS_BSP_TIMER_LOW32_ROLLOVER will be 0. */

/******************************************************************************
**  Function:  CFE_PSP_GetTime()
**
**  Purpose: Gets the value of the time from the hardware
**
**  Arguments: LocalTime - where the time is returned through
******************************************************************************/

void CFE_PSP_GetTime( OS_time_t *LocalTime)
{

    /* since we don't have a hardware register to access like the mcp750,
     * we use a call to the OS to get the time */

    OS_GetLocalTime(LocalTime);
    
}/* end CFE_PSP_GetLocalTime */

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
    int32               return_status = CFE_PSP_ERROR;

    CFE_ES_WriteToSysLog("CFE_PSP_Set_OS_Time not implemented in Linux OS");

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
        /* Get real time clock from OS OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == CFE_PSP_SUCCESS)
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
**  Function:  CFE_PSP_Sync_From_OS_Freq(uint16_t)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    uint16 - seconds between updates. If zero, returns the current value
**
**  Return:
**    int - CFE_PSP_SUCCESS or the current PSP_OS_TIME_SYNC_SEC value
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
**  Functions:  CFE_PSP_NTP_Daemon_Get_Status()
**              CFE_PSP_StartNTPDaemon()
**              CFE_PSP_StopNTPDaemon()
**              CFE_PSP_NTP_Daemon_Enable()
**
**  Purpose: Provide stubs for the equivalent functions in VxWorks.
**
**  Note: To actually implement the below functions we need elavated privilages.
******************************************************************************/
/**
 * \brief Get status of NTP daemon
 */
int32 CFE_PSP_NTP_Daemon_Get_Status(void)
{
    int32       return_code = true;
    CFE_ES_WriteToSysLog("CFE_PSP_NTP_Daemon_Get_Status not implemented in Linux OS");
    return return_code;
}

/**
 * \brief Start NTP daemon
 */
int32 CFE_PSP_StartNTPDaemon(void)
{
    int32       return_code = CFE_PSP_ERROR;
    CFE_ES_WriteToSysLog("CFE_PSP_StartNTPDaemon not implemented in Linux OS");
    return return_code;
}

/**
 * \brief Stop NTP daemon
 */
int32 CFE_PSP_StopNTPDaemon(void)
{
    int32       return_code = CFE_PSP_ERROR;
    CFE_ES_WriteToSysLog("CFE_PSP_StopNTPDaemon not implemented in Linux OS");
    return return_code;
}

/**
 * \brief Enable or Disable the NTP daemon
 */
int32 CFE_PSP_NTP_Daemon_Enable(bool enable)
{
    int32       return_code = CFE_PSP_ERROR;
    CFE_ES_WriteToSysLog("CFE_PSP_NTP_Daemon_Enable not implemented in Linux OS");
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
**
**  Return:
**  OS system clock ticks per second
*/
uint32 CFE_PSP_Get_Timer_Tick(void)
{
   /* SUB -add function call code*/
   return (0);
}

/******************************************************************************
**  Function:  CFE_PSP_GetTimerTicksPerSecond()
**
**  Purpose:
**    Provides the resolution of the least significant 32 bits of the 64 bit
**    time stamp returned by OS_BSPGet_Timebase in timer ticks per second.
**    The timer resolution for accuracy should not be any slower than 1000000
**    ticks per second or 1 us per tick
**
**  Arguments:
**
**  Return:
**    The number of timer ticks per second of the time stamp returned
**    by CFE_PSP_Get_Timebase
*/
uint32 CFE_PSP_GetTimerTicksPerSecond(void)
{
    return(CFE_PSP_TIMER_TICKS_PER_SECOND);
}

/******************************************************************************
**  Function:  CFE_PSP_GetTimerLow32Rollover()
**
**  Purpose:
**    Provides the number that the least significant 32 bits of the 64 bit
**    time stamp returned by CFE_PSP_Get_Timebase rolls over.  If the lower 32
**    bits rolls at 1 second, then the CFE_PSP_TIMER_LOW32_ROLLOVER will be 1000000.
**    if the lower 32 bits rolls at its maximum value (2^32) then
**    CFE_PSP_TIMER_LOW32_ROLLOVER will be 0.
**
**  Arguments:
**
**  Return:
**    The number that the least significant 32 bits of the 64 bit time stamp
**    returned by CFE_PSP_Get_Timebase rolls over.
*/
uint32 CFE_PSP_GetTimerLow32Rollover(void)
{
    return(CFE_PSP_TIMER_LOW32_ROLLOVER);
}

/******************************************************************************
**  Function:  CFE_PSP_Get_Timebase()
**
**  Purpose:
**    Provides a common interface to system timebase. This routine
**    is in the BSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**
**  Return:
**  Timebase register value
*/
void CFE_PSP_Get_Timebase(uint32 *Tbu, uint32* Tbl)
{
   OS_time_t        time;

   OS_GetLocalTime(&time);
   *Tbu = time.seconds;
   *Tbl = time.microsecs;
}

/******************************************************************************
**  Function:  CFE_PSP_Get_Dec()
**
**  Purpose:
**    Provides a common interface to decrementer counter. This routine
**    is in the PSP because it is sometimes implemented in hardware and
**    sometimes taken care of by the RTOS.
**
**  Arguments:
**
**  Return:
**  Timebase register value
*/

uint32 CFE_PSP_Get_Dec(void)
{
   /* SUB -add function call code*/
   return(0);
}

