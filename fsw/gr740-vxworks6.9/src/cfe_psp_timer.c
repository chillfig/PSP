/************************************************************************************************
** File:  cfe_psp_timer.c
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**  All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2005/06/05  K.Audra    | Initial version,
**   2014/08/08  S. Duran   | Modified for ut699 SPARC LEON3 board
**   2020/06/03  J. Pham    | Modified for GR740 SPARC LEON4 board
**
*************************************************************************************************/
/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vxWorks.h>
#include "sysLib.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

#include <target_config.h>
#include "cfe_psp_gr740.h"
#include "cfe_psp_timer.h"

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"

/******************* Macro Definitions ***********************/


#define CFE_PSP_TIMER_PRINT_DBG             true
#define CFE_PSP_TIMER_AUX_TICK_PER_SEC      1 /* The number of tics or interrupts per second */

/** \brief Timer Scaler to set timer to 1MHz */
#define CFE_PSP_TIMER_1MHZ_SCALER           (uint32_t) GR740_SYSTEM_CLOCK/1000000
/** \brief Timer Scaler to set timer to 20MHz */
#define CFE_PSP_TIMER_20MHZ_SCALER          (uint32_t) GR740_SYSTEM_CLOCK/20000000
/** \brief Default Timer Scaler */
#define CFE_PSP_TIMER_SCALER_DEFAULT        CFE_PSP_TIMER_20MHZ_SCALER

#define TS_TIMER_LOW_ID     (0)
#define TS_TIMER_HIGH_ID    (1)

/*
 ** local global variables for PSP Timer
 */
/* static void CFE_PSP_AuxClkHandler(int32 arg); */
static bool g_bTimerInitialized = false;


void CFE_PSP_InitLocalTime(void)
{
    /* Set the sys clock rate */
    /* This is set by the VxWorks OS to 1000 */
    //sysClkRateSet(200);

    /*
     ** Disable the Aux timer interrupt, and disable the Timestamp timer
     */
    sysAuxClkDisable();

    /*
     ** Set the Aux timer
     */
    if(sysAuxClkRateGet() != CFE_PSP_TIMER_AUX_TICK_PER_SEC)
    {
        if(sysAuxClkRateSet(CFE_PSP_TIMER_AUX_TICK_PER_SEC) == ERROR)
        {
            printf(TIMER_PRINT_SCOPE "Unable to set Aux Clock Rate!\n");
        }


        if(CFE_PSP_TIMER_PRINT_DBG == TRUE)
        {      
            printf(TIMER_PRINT_SCOPE "Aux Clock Rate %d.\n", sysAuxClkRateGet());

        }     
    }

    TIMER1_REG->scaler                          = CFE_PSP_TIMER_SCALER_DEFAULT;
    TIMER1_REG->scalerReload                    = CFE_PSP_TIMER_SCALER_DEFAULT;
    TIMER1_REG->timer[TS_TIMER_LOW_ID].control  = 0x0;
    TIMER1_REG->timer[TS_TIMER_HIGH_ID].control = 0x0;
    TIMER1_REG->timer[TS_TIMER_LOW_ID].reload   = 0xFFFFFFFF;
    TIMER1_REG->timer[TS_TIMER_HIGH_ID].reload  = 0xFFFFFFFF;
    TIMER1_REG->timer[TS_TIMER_LOW_ID].control  = TIMER_CONTROL_LD | TIMER_CONTROL_RS | TIMER_CONTROL_EN;
    TIMER1_REG->timer[TS_TIMER_HIGH_ID].control =
        TIMER_CONTROL_CH | TIMER_CONTROL_LD | TIMER_CONTROL_RS | TIMER_CONTROL_EN;


    g_bTimerInitialized = TRUE;

}/* end CFE_PSP_InitLocalTime */


void vxTimeBaseGet(uint32 *Tbu, uint32 *Tbl) //UndCC_Line(SSET106) Func. name part of the VxWorks API, cannot change
{
    uint32_t lower = 0;
    uint32_t upper0 = 0;
    uint32_t upper1 = 0;
    uint64_t time = 0;

    uint32_t uiRet_Code = CFE_PSP_SUCCESS;

    /* If any of the pointers provided are NULL, return immediately */
    if ((Tbu == NULL) || (Tbl == NULL))
    {
        uiRet_Code = CFE_PSP_ERROR;
        OS_printf(TIMER_PRINT_SCOPE "Input variables are NULL\n");
    }

    /* Check that the timer is initialized */
    if (g_bTimerInitialized == true)
    {
        uiRet_Code = CFE_PSP_ERROR;
        OS_printf(TIMER_PRINT_SCOPE "Timer is not initialized\n");
    }

    /*
     ** We don't want an interrupt to occur when the time requested.
     **
     ** sysTimestampLock() automatically blocks interrupts during execution.
     **
     */
    if (uiRet_Code = CFE_PSP_SUCCESS)
    {
        /*
         ** The resolution of our timer is given by:
         **   one timer tick = 1 / sysTimestampFreq() in seconds
         */

        upper0 = TIMER1_REG->timer[TS_TIMER_HIGH_ID].counter;
        lower  = TIMER1_REG->timer[TS_TIMER_LOW_ID].counter;
        upper1 = TIMER1_REG->timer[TS_TIMER_HIGH_ID].counter;

        if (upper0 != upper1)
        {
            lower = 0xFFFFFFFF; // lower rolled over before reading upper1
        }
        time = upper1;
        time = (time << 32) | lower;
        time = 0xFFFFFFFFFFFFFFFFULL - time;

        *Tbu = (time >> 32) & 0xFFFFFFFFU;
        *Tbl = time & 0xFFFFFFFFU;
    }
}

// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
// This function is for debug only.
// Remove this function after the CFE_PSP_Get_Timebase() function has been fully debugged.
// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
int DEBUG_GetTime(void)
{
    uint32 Tbu, Tbl;
    OS_time_t localTime;

    CFE_PSP_Get_Timebase(&Tbu, &Tbl);
    CFE_PSP_GetTime(&localTime);

    OS_printf("CFE_PSP_Get_Timebase(0x%08x, 0x%08x)\n", Tbu, Tbl);
    OS_printf("CFE_PSP_GetTime(%lld)\n", localTime.ticks);

    int64_t subsecondDiv = 10000000ULL;
    uint32_t seconds = localTime.ticks / subsecondDiv;
    uint32_t subsecs = localTime.ticks % subsecondDiv;

    OS_printf("%d sec, %d us\n", seconds, subsecs / 10U);

    return 0;
}
