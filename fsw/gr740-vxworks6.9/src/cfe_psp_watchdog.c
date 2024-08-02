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
*   \file cfe_psp_watchdog.c
*
*   \brief API to enable/disable, and control gr740 SPARC LEON4 board watchdog
*
*   \brief Limitations, Assumptions, External Events, and Notes:
*   The gr740 SPARC LEON4 board watchdog timer has a counter with a tick precision of about XX nano-seconds.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vxWorks.h>

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_timer.h"
#include <cfe_psp_gr740.h>
#include "cfe_psp_watchdog.h"

/*
** Types and prototypes for this module
*/


/*
** Global Variables
*/

/** \brief Watchdog current millisecond value */
uint32 g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_DEFAULT_MSEC;  /* Watchdog time in msecs */

/** \brief Watchdog current status */
bool g_bWatchdogStatus = false;

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogInit
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_WatchdogInit(void)
{
    TIMER0_REG->scaler                    = CFE_PSP_TIMER_1MHZ_SCALER;
    TIMER0_REG->scalerReload              = CFE_PSP_TIMER_1MHZ_SCALER;
    TIMER0_REG->timer[WD_TIMER_ID].reload = CFE_PSP_WATCHDOG_MAX;

    /* clear timer control register */
    TIMER0_REG->timer[WD_TIMER_ID].control = 0x00000000U;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogEnable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_WatchdogEnable(void)
{
    uint32 reg = TIMER0_REG->timer[WD_TIMER_ID].control;

    reg &= ~TIMER_CONTROL_IP; /* clear a pending interrupt  */

    reg |= TIMER_CONTROL_IE; /* Enable interrupt */

    reg |= TIMER_CONTROL_EN; /* Enable timer */

    TIMER0_REG->timer[WD_TIMER_ID].control = reg;

    g_bWatchdogStatus = true;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogDisable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_WatchdogDisable(void)
{
    /*
     ** Disable the timer
     */
    TIMER0_REG->timer[WD_TIMER_ID].control &= ~TIMER_CONTROL_EN;

    g_bWatchdogStatus = false;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogService
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_WatchdogService(void)
{
    /*
     ** Reload the timer with value in the reload reload register
     */
    TIMER0_REG->timer[WD_TIMER_ID].control |= TIMER_CONTROL_LD;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogGet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
uint32 CFE_PSP_WatchdogGet(void)
{
    return g_uiCFE_PSP_WatchdogValue_ms;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogSet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_WatchdogSet(uint32 watchDogValue_ms)
{
    uint32 uiNewValue = 0;

    /* convert WD time in millisec to timer ticks */
    if (watchDogValue_ms <= CFE_PSP_WATCHDOG_MIN_MS)
    {
        uiNewValue = CFE_PSP_WATCHDOG_MIN;
        g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_MIN_MS;
    }
    else if (watchDogValue_ms >= CFE_PSP_WATCHDOG_MAX_MS)
    {
        uiNewValue = CFE_PSP_WATCHDOG_MAX;
        g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_MAX_MS;
    }
    else {
        uiNewValue = watchDogValue_ms * CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC;
        g_uiCFE_PSP_WatchdogValue_ms = watchDogValue_ms;
    }

    TIMER0_REG->timer[WD_TIMER_ID].reload = uiNewValue;
    TIMER0_REG->timer[WD_TIMER_ID].control |= TIMER_CONTROL_LD;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogStatus
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
bool CFE_PSP_WatchdogStatus(void)
{

    /* Prints out the WDT registers for debugging */
    OS_printf("Scaler: %08lX\n", (unsigned long)TIMER0_REG->scaler);
    OS_printf("Scaler Reload: %08lX\n", (unsigned long)TIMER0_REG->scalerReload);
    OS_printf("Config: %08lX\n\n", (unsigned long)TIMER0_REG->config);

    OS_printf("Counter: %08lX\n", (unsigned long)TIMER0_REG->timer[WD_TIMER_ID].counter);
    OS_printf("Reload:  %08lX\n", (unsigned long)TIMER0_REG->timer[WD_TIMER_ID].reload);
    OS_printf("Ctrl:    %08lX\n", (unsigned long)TIMER0_REG->timer[WD_TIMER_ID].control);

    return g_bWatchdogStatus;
}
