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
*   \brief API to enable/disable, and control FPGA watchdog
*
*   \brief Limitations, Assumptions, External Events, and Notes:
*   The FPGA watchdog timer has a counter with a tick precision of about 48 nano-seconds.
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
#include <ioLib.h>
#include <unistd.h>

/* Aitech BSP Specific */
#include <sysApi.h>

#include "common_types.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_config.h"

/*
** Global Variables
*/
/** \brief Watchdog current millisecond value */
static uint32 g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_DEFAULT_MSEC;  /* Watchdog time in msecs */

/** \brief Watchdog current status */
static bool g_bWatchdogStatus = false;

/**********************************************************
 * 
 * Function: CFE_PSP_WatchdogInit
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_WatchdogInit(void)
{
    g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_DEFAULT_MSEC;

    CFE_PSP_WatchdogSet(g_uiCFE_PSP_WatchdogValue_ms);  /* in msecs */
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
    int32 iStatus = OK;

    /*
    ** TRUE (allows slave SBC's WDT failure to reset all SBCs)
    ** FALSE (slave SBC's WDT failure resets slave SBC only)
    */
    iStatus = sysEnableFpgaWdt(true);
    if (iStatus != OK)
    {
        OS_printf("PSP Watchdog: Could not enable FPGA Watchdog\n");
    }
    else
    {
        g_bWatchdogStatus = true;
        OS_printf("PSP Watchdog: Watchdog successfully enabled\n");
    }
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
    int32 iStatus = OK;
    g_bWatchdogStatus = false;

    iStatus = sysDisableFpgaWdt();

    if (iStatus != OK)
    {
        OS_printf("PSP Watchdog: Could not disable FPGA Watchdog\n");
    }
    else
    {
        OS_printf("PSP Watchdog: Successfully disabled watchdog\n");
    }
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
     * This function toggles the watchdog timer and it must be called at least
     * once every watchdog timer period if the watchdog timer is enabled.
     * Otherwise, the board will perform a reset once the timer expires.
     */
    sysPulseFpgaWdt();
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
    int32  iStatus = OK;
    float fRate = 0.0f;

    /*Rate is in seconds*/
    fRate = (((float)watchDogValue_ms) * 0.001f);

    iStatus = sysSetFpgaWdt(fRate);

    if (iStatus != OK)
    {
        OS_printf("PSP Watchdog: Could not set FPGA Watchdog rate\n");
    }
    else
    {
        /*
        ** sysSetFpgaWdt was successful, set new watchdog value
        **
        ** Input already in msecs
        */
        g_uiCFE_PSP_WatchdogValue_ms = watchDogValue_ms;
    }
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
    return g_bWatchdogStatus;
}
