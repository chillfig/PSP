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
*   \file psp_watchdog_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_watchdog.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
#include <target_config.h>
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_watchdog_testcases.h"
#include "../src/cfe_psp_watchdog.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_WatchdogInit(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogInit(void)
{
    /* ----- Test case #1 - Failed input pointer is NULL ----- */
    /* Setup additional inputs */

    /* Execute test */
    CFE_PSP_WatchdogInit();
    /* Verify outputs */
    UtAssert_NA("_CFE_PSP_WatchdogInit() - 1/1: NA");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogEnable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogEnable(void)
{
    /* ----- Test case #1 - Successfully enable watchdog ----- */
    /* Set additional inputs */
    g_bWatchdogStatus = false;
    /* Execute test */
    CFE_PSP_WatchdogEnable();
    /* Verify results */
    UtAssert_True(g_bWatchdogStatus == true, "_CFE_PSP_WatchdogEnable - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogDisable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogDisable(void)
{
    /* ----- Test case #1 - Successfully disable watchdog ----- */
    /* Set additional inputs */
    g_bWatchdogStatus = true;
    /* Execute test */
    CFE_PSP_WatchdogDisable();
    /* Verify results */
    UtAssert_True(g_bWatchdogStatus == false, "_CFE_PSP_WatchdogDisable - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogService(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogService(void)
{
    /* No content for implementing unit test */
    CFE_PSP_WatchdogService();
    UtAssert_NA("_CFE_PSP_WatchdogService() - 1/1: NA");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogGet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogGet(void)
{
    uint32  uiRetVal = 0;
    
    g_uiCFE_PSP_WatchdogValue_ms = 10000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = g_uiCFE_PSP_WatchdogValue_ms;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetVal = CFE_PSP_WatchdogGet();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogGet() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogSet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogSet(void)
{
    g_uiCFE_PSP_WatchdogValue_ms = 10000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = 20000;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(g_uiCFE_PSP_WatchdogValue_ms, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogSet() - 1/3: Nominal");

    /* ----- Test case #2 - Nominal - Capped to Max ----- */
    /* Setup additional inputs */
    g_uiCFE_PSP_WatchdogValue_ms = 99;
    BASE_g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_MAX_MS + 1;
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_True(g_uiCFE_PSP_WatchdogValue_ms == CFE_PSP_WATCHDOG_MAX_MS, "_CFE_PSP_WatchdogSet() - 2/3: Nominal - Capped to Max");

    /* ----- Test case #1 - Nominal - Capped to Min ----- */
    /* Setup additional inputs */
    g_uiCFE_PSP_WatchdogValue_ms = 99;
    BASE_g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_MIN_MS - 1;
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_True(g_uiCFE_PSP_WatchdogValue_ms == CFE_PSP_WATCHDOG_MIN_MS, "_CFE_PSP_WatchdogSet() - 3/3: Nominal - Capped to Min");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogStatus(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogStatus(void)
{
    bool bReturnValue;

    char cMsg[256] = {};
    sprintf(cMsg, "WD calculated value = %d ms\n", g_uiCFE_PSP_WatchdogValue_ms);

    Ut_OS_printf_Setup();   

    /* ----- Test case #1 - Watchdog is currently disabled ----- */
    /* Set additional inputs */
    g_bWatchdogStatus = false;
    /* Execute test */
    bReturnValue = CFE_PSP_WatchdogStatus();
    /* Verify results */
    UtAssert_True(bReturnValue == false, "_CFE_PSP_WatchdogStatus - 1/1: Watchdog return code");
}

/*=======================================================================================
** End of file psp_watchdog_testcases.c
**=======================================================================================*/
