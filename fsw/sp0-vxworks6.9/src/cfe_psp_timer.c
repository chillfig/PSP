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

/* For supporting REALTIME clock */
#include <timers.h>
/* For supporting SetTime and SetSTCF */
#include "cfe_time_utils.h"
#include "osapi-os-core.h"
#include "psp_time_sync.h"

/*
** External Function Prototypes
*/


/*
** Function Prototypes
*/


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
    int32       Status;
    int32       return_status = CFE_SUCCESS;
    uint32      PSP_NTP_TimerId = 0;
    uint32      PSP_NTP_ClockAccuracy = 0;
    uint32      PSP_1HZ_INTERVAL = 1000000;
    uint32      timer_interval_msec = timer_frequency_sec * PSP_1HZ_INTERVAL;

    /*Create the 1Hz timer for synchronizing the major frame*/
    Status = OS_TimerCreate(&PSP_NTP_TimerId,
                            "PSPNTPSync",
                            &PSP_NTP_ClockAccuracy,
                            CFE_PSP_SetTime_From_VxWorks);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Failed to create PSP_NTP_Sync task.\n");
        return_status = CFE_PSP_ERROR;
    }
    else
    {
        /*Set the interval to one second in microseconds for the first time call, then every timer_frequency_sec.*/
        Status = OS_TimerSet(PSP_NTP_TimerId, PSP_1HZ_INTERVAL, timer_interval_msec);
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
**  Function:  CFE_PSP_SetTime_Enable(bool)
**
**  Purpose:
**    Enable or Disable CFE_PSP_SetTime_From_VxWorks
**
**  Arguments:
**    bool - When True, CFE_PSP_SetTime_From_VxWorks will be called to synchronize time
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_SetTime_Enable(bool enable)
{
    
    if (enable)
    {
        CFE_ES_WriteToSysLog("Sync with VxWorks is enabled\n");
    }
    else
    {
        CFE_ES_WriteToSysLog("Sync with VxWorks is disabled\n");
    }
    /* Set flag */
    setTime_From_VxWorks = enable;

    return CFE_PSP_SUCCESS;
}

/******************************************************************************
**  Function:  CFE_PSP_Update_Sync_Frequency(uint16_t)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    uint16 - seconds between updates. If zero, returns the current value
**
**  Return:
**    int - CFE_PSP_SUCCESS or the current PSP_VXWORKS_TIME_SYNC_SEC value
******************************************************************************/
int32 CFE_PSP_Update_Sync_Frequency(uint16 new_frequency_sec)
{
    
    int32 return_value = CFE_PSP_SUCCESS;

    if (new_frequency_sec == 0)
    {
        return_value = (int32)PSP_VXWORKS_TIME_SYNC_SEC;
    }
    else
    {
        PSP_VXWORKS_TIME_SYNC_SEC = new_frequency_sec;
    }
    return return_value;
}

/******************************************************************************
**  Function:  CFE_PSP_SetTime_From_VxWorks()
**
**  Purpose:
**    Syncronize CFE Time Service with VxWorks local time. VxWorks time is 
**    automatically syncronized to NTP server
**
**    IMPORTANT: Function declaration is dictated by OS_TimerCreate function.
**
**  Arguments:
**    timer_id
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_SetTime_From_VxWorks(uint32 timer_id)
{
    struct timespec     unixTime;
    uint32              tv_sec = 0;
    uint32              tv_msec = 0;
    CFE_TIME_SysTime_t  myT;
    int                 ret;

    /* If the flag is enabled */
    if (setTime_From_VxWorks)
    {
        /* CFE_ES_WriteToSysLog("CFE_PSP: Syncing Time with VxWorks\n"); */
        /* Get real time clock from VxWorks OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == OK)
        {
            /* CFE_ES_WriteToSysLog("NTP Real Time Clock: {tv_sec: %ld}, {tv_nsec: %ld}\n",unixTime.tv_sec,unixTime.tv_nsec); */
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
                /* VxWorks has not sync with NTP server yet. */
                CFE_ES_WriteToSysLog("CFE_PSP: VxWorks OS has not sync with NTP server yet, trying again later.\n");
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("clock_gettime function failed\n");
        }
    }
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


