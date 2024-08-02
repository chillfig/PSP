/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 *
 *    Copyright (c) 2020, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

/**
 * \file
 * \ingroup  vxworks
 * \author   
 *
 */

#include "coveragetest-psp-gr740-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include "cfe_psp.h"

#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"

#include "cfe_psp_watchdog.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern uint32 g_uiCFE_PSP_WatchdogValue_ms;
extern bool g_bWatchdogStatus;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Test_CFE_PSP_WatchdogInit(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogInit(void)
{
    /* ----- Test case #1 - Failed input pointer is NULL ----- */
    /* Setup additional inputs */

    /* Execute test */
    CFE_PSP_WatchdogInit();
    /* Verify outputs */
    UtAssert_True(TIMER0_REG->timer[WD_TIMER_ID].control == 0, "_CFE_PSP_WatchdogInit() - 1/1: Nominal");
}

/*=======================================================================================
** Test_CFE_PSP_WatchdogEnable(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogEnable(void)
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
** Test_CFE_PSP_WatchdogDisable(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogDisable(void)
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
** Test_CFE_PSP_WatchdogService(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogService(void)
{
    /* No content for implementing unit test */
    CFE_PSP_WatchdogService();
    UtAssert_True((TIMER0_REG->timer[WD_TIMER_ID].control & TIMER_CONTROL_LD) == TIMER_CONTROL_LD, "_CFE_PSP_WatchdogService() - 1/1: Nominal");
}

/*=======================================================================================
** Test_CFE_PSP_WatchdogGet(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogGet(void)
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
** Test_CFE_PSP_WatchdogSet(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogSet(void)
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
** Test_CFE_PSP_WatchdogStatus(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WatchdogStatus(void)
{
    bool bReturnValue;

    char cMsg[256] = {};
    snprintf(cMsg, sizeof(cMsg), "WD calculated value = %d ms\n", g_uiCFE_PSP_WatchdogValue_ms);

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