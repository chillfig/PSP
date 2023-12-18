/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file psp_watchdog_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_watchdog.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
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
    g_uiCFE_PSP_WatchdogValue_ms = 20000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = g_uiCFE_PSP_WatchdogValue_ms;
    /* Execute test */
    CFE_PSP_WatchdogInit();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(g_uiCFE_PSP_WatchdogValue_ms, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogInit() - 1/1: Nominal");
    UtAssert_True(g_uiCFE_PSP_WatchdogValue_ms == BASE_g_uiCFE_PSP_WatchdogValue_ms, "_CFE_PSP_WatchdogInit() - 1/1: Nominal - watchdog value set");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogEnable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogEnable(void)
{
    char cMsg[256] = {};
    sprintf(cMsg, "PSP Watchdog: Could not enable FPGA Watchdog\n");

    /* ----- Test case #1 - Successfully enable watchdog ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP Watchdog: Watchdog successfully enabled\n");
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OK);
    /* Execute test */
    CFE_PSP_WatchdogEnable();
    /* Verify results */
    UtAssert_True(g_bWatchdogStatus == true, "_CFE_PSP_WatchdogEnable - 1/2: Nominal");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogEnable - 1/2: Nominal - message");

    /* ----- Test case #2 - Unsuccessfully enable watchdog ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), ERROR);
    /* Execute test */
    CFE_PSP_WatchdogEnable();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogEnable - 2/2: Could not enable Watchdog");   
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogDisable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogDisable(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Successfully disable watchdog ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP Watchdog: Successfully disabled watchdog\n");
    /* Execute test */
    UT_SetDefaultReturnValue(UT_KEY(sysDisableFpgaWdt), OK);
    CFE_PSP_WatchdogDisable();
    /* Verify results */
    UtAssert_True(g_bWatchdogStatus == false, "_CFE_PSP_WatchdogDisable - 1/2: Successfully disable watchdog");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogDisable - 1/2: Successfully disable watchdog");

    /* ----- Test case #2 - Unsuccessfully disable watchdog */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP Watchdog: Could not disable FPGA Watchdog\n");
    UT_SetDefaultReturnValue(UT_KEY(sysDisableFpgaWdt), ERROR);
    /* Execute test */
    CFE_PSP_WatchdogDisable();
    /* Verify results */
    UtAssert_True(g_bWatchdogStatus == false, "_CFE_PSP_WatchdogDisable - 2/2: Unsuccessfully disable watchdog");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogDisable - 2/2: Unsuccessfully disable watchdog");
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
    
    g_uiCFE_PSP_WatchdogValue_ms = 20000;
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
    char cMsg[256] = {};
    sprintf(cMsg, "PSP Watchdog: Could not set FPGA Watchdog rate\n");

    Ut_OS_printf_Setup();

    g_uiCFE_PSP_WatchdogValue_ms = 10000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = g_uiCFE_PSP_WatchdogValue_ms;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OK);
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(g_uiCFE_PSP_WatchdogValue_ms, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogSet() - 1/2: Nominal");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_WatchdogSet() - 1/2: Nominal No Print");

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), ERROR);
    g_uiCFE_PSP_WatchdogValue_ms = 99;
    BASE_g_uiCFE_PSP_WatchdogValue_ms = 3;
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_True(g_uiCFE_PSP_WatchdogValue_ms != BASE_g_uiCFE_PSP_WatchdogValue_ms, "_CFE_PSP_WatchdogSet() - 2/2: Nominal");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogSet() - 2/2: Nominal Error Print");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogStatus(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogStatus(void)
{
    bool bReturnValue;

    /* ----- Test case #1 - Watchdog is currently disabled ----- */
    /* Set additional inputs */
    g_bWatchdogStatus = false;
    /* Execute test */
    bReturnValue = CFE_PSP_WatchdogStatus();
    /* Verify results */
    UtAssert_True(bReturnValue == false, "_CFE_PSP_WatchdogStatus - 1/2: Watchdog currently disabled - return code");

    /* ----- Test case #2 - Watchdog is currently enabled ----- */
    /* Set additional inputs */
    g_bWatchdogStatus = true;
    /* Execute test */
    bReturnValue = CFE_PSP_WatchdogStatus();
    /* Verify results */
    UtAssert_True(bReturnValue == true, "_CFE_PSP_WatchdogStatus - 2/2: Watchdog currently enabled - return code");
}

/*=======================================================================================
** End of file psp_watchdog_testcases.c
**=======================================================================================*/
