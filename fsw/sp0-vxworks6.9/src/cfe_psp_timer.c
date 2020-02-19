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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "vxLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "time.h"

#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_psp.h"
#include "cfe_time.h"

/*
** External Function Prototypes
*/
extern void CFE_TIME_Local1HzISR(void);
extern void CFE_TIME_Tone1HzISR(void);


/*
** Function Prototypes
*/
void CFE_PSP_Init1HzTimer(void);
void CFE_PSP_PPSHandler(void);
void CFE_PSP_AuxClkHandler(void);


/*
** Macro Definitions
*/

/* CFE_PSP_TIMER_TICKS_PER_SECOND
**
**   This corresponds to a 41.66666625 MHz Time-Base Register (TBR) clock,
**   which runs at 1/8 of the Core Complex Bus (CCB) clock.
**   (The system clock drives the CCB clock; local bus runs at 333.333330 MHz)
**
**   The TBR is a 64-bit register that is incremented every 8 bus clock cycles.
**   It is read as tbu and tbl, using vxTimeBaseGet(), where the upper 32-bits 
**   are placed in tbu and the lower 32-bits are placed in tbl.
**
**   The timer will take 
**     (2^64 ticks** 1/41,666,666 sec/ticks) / 3.15569 x 10^7 sec/year = 14,029 years
**   to roll over.
**
**   Seconds are stored in a 32-bit variable using the following computation,
**      seconds = tb / CFE_PSP_TIMER_TICKS_PER_SECOND
**   where tb is the 64-bit total number of ticks and seconds is a 32-bit variable.
**
**   The max number of seconds before a roll over is 
**     2^32-1 = 4,294,967,295 or 4,294,967,295 / 3.15569e7 sec/year = 136 years
**   Note: There is currently no roll over detection on the second value.
**
**   Resolution of the least significant 32 bits of the 64-bit timestamp returned
**   by CFE_PSP_Get_Timebase() is in timer ticks per second.
**
**   The timer resolution for accuracy should not be any slower than 1,000,000
**   ticks per second or 1 us per tick
*/
#define CFE_PSP_TIMER_TICKS_PER_SECOND  41666666


/* CFE_PSP_TIMER_TICKS_PER_SECOND
**
**   A tb tick or period is 
**     1 / 41,666,666 sec = 0.000000024 sec or 24 ns per tick 
*/
#define CFE_PSP_TIMER_PERIOD  0.000000024


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


/* CFE_PSP_TIMER_AUX_TICK_PER_SEC
**
**   The number of ticks or interrupts per second 
*/
#define CFE_PSP_TIMER_AUX_TICK_PER_SEC       100


/* CFE_PSP_AUXCLK
** Use auxilary clock for 1 HZ timer if defined 
** else Use 1 Pulse Per Second (1PPS) for 1 HZ timer
**/
#define CFE_PSP_AUXCLK


/* CFE_PSP_TIMER_PRINT_DBG
**
**   Debug flag
*/
#define CFE_PSP_TIMER_PRINT_DBG  FALSE



/******************************************************************************
**  Function:  CFE_PSP_InitLocalTime()
**
**  Purpose:
**    Initializes the vxWorks Aux and TimeStamp timers.
**
**  Arguments: 
**    None
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_InitLocalTime(void)
{
    /* Set the SysClk timer rate in ticks */
    sysClkRateSet(200);

    /* Disable the AuxClk interrupt */
    sysAuxClkDisable();

#ifdef CFE_PSP_AUXCLK 
    /* Set the AuxClk timer rate in ticks */
    if (sysAuxClkRateGet() != CFE_PSP_TIMER_AUX_TICK_PER_SEC)
    {
        if (sysAuxClkRateSet(CFE_PSP_TIMER_AUX_TICK_PER_SEC) == ERROR)
        {
            logMsg("CFE_PSP_InitLocalTime() - sysAuxClkRateSet() failed!\n", 
                   0,0,0,0,0,0);
        }

        if(CFE_PSP_TIMER_PRINT_DBG == TRUE)
        {
            logMsg("CFE_PSP_InitLocalTime() - Set AuxClk rate at %d\n", 
                   (int)sysAuxClkRateGet(), 0,0,0,0,0);
        }
    }
#endif
}


/******************************************************************************
**  Function:  CFE_PSP_Init1HzTimer()
**
**  Purpose:
**    Initializes the 1Hz Timer and connects it to the cFE TIME 1Hz routine.
**
**    If this signal is not driven, then the 1Hz ISR will not run.
**    Either use the AuxClk or the PPS to drive it.
**
**    Note: This function must be called after CFE_ES_Main() in CFE_PSP_Start()
**          because the 1Hz ISR has a semaphore that is created in CFE_ES_Main().
**
**  Arguments: 
**    None
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_Init1HzTimer(void)
{
    /* Attach a handler to the timer interrupt */
#ifdef CFE_PSP_AUXCLK 
    /* - Either via the AuxClk */
    if (sysAuxClkConnect((FUNCPTR)CFE_PSP_AuxClkHandler,
                         CFE_PSP_TIMER_AUX_TICK_PER_SEC) == ERROR)
    {
        logMsg("CFE_PSP_Init1HzTimer() - sysAuxClkConnect() failed!\n", 
               0,0,0,0,0,0);
    }
    else
    {
        sysAuxClkEnable();
    }

#else
    /* - Or via the PPS */
    /* Requires sys950Lib */
    if (ppsIntConnect((VOIDFUNCPTR)CFE_PSP_PPSHandler) == ERROR)
    {
    	logMsg("CFE_PSP_Init1HzTimer() - ppsIntConnect() failed!\n", 
    	               0,0,0,0,0,0);     	
    }
    else
    {
        if (ppsIntEnable() == ERROR)
        {
        	logMsg("CFE_PSP_Init1HzTimer() - ppsIntEnable() failed!\n", 
        	               0,0,0,0,0,0);
        }
    }
#endif
}


/******************************************************************************
**  Function:  CFE_PSP_PPSHandler()
**
**  Purpose:
**    Ties PPS 1Hz ISR handler to Tone1HzISR
**
**  Arguments:
**    None
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_PPSHandler(void)
{
	CFE_TIME_Tone1HzISR();
}


/******************************************************************************
**  Function:  CFE_PSP_AuxClkHandler()
**
**  Purpose:
**    A timer intterupt handler to keep track of seconds
**
**  Arguments:
**    None
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_AuxClkHandler(void)
{
    static int auxCount = 0;
    static int secCount = 0;

    if (++auxCount >= CFE_PSP_TIMER_AUX_TICK_PER_SEC)
    {
    	secCount++;
        auxCount = 0;
        CFE_TIME_Local1HzISR();

        if (CFE_PSP_TIMER_PRINT_DBG == TRUE)
        {
        	OS_time_t LocalTime;
        	CFE_PSP_GetTime(&LocalTime);
        	logMsg("CFE_PSP_AuxClkHandler: PSP Local Time: %d.%d\n", LocalTime.seconds,LocalTime.microsecs,0,0,0,0);                            	
        }
    }
}


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

    if (pLocalTime != NULL)
    {
     
    	vxTimeBaseGet((UINT32 *)&tbu, (UINT32 *)&tbl);

        /* Re-assemble the 64-bit count */
        tb = ((unsigned long long)tbu << 32) | (unsigned long long)tbl;

        /* Convert to seconds and microseconds using only integer computations */
        pLocalTime->seconds   = (tb / CFE_PSP_TIMER_TICKS_PER_SECOND);
        pLocalTime->microsecs = ((tb % CFE_PSP_TIMER_TICKS_PER_SECOND) * 1000000 )/(CFE_PSP_TIMER_TICKS_PER_SECOND);
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
**    Value of CFE_PSP_TIMER_TICKS_PER_SECOND macro constant
******************************************************************************/
uint32 CFE_PSP_GetTimerTicksPerSecond(void)
{
    return ((uint32)CFE_PSP_TIMER_TICKS_PER_SECOND);
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


