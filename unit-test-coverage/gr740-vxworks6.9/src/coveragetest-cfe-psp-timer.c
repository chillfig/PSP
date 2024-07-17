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
#include "cfe_psp_timer.h"

#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"

/*=======================================================================================
** Preprocessor Directives
**=======================================================================================*/


/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern bool g_bTimerInitialized;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_InitLocalTime(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_InitLocalTime(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    CFE_PSP_InitLocalTime();
    /* Verify outputs */
    UtAssert_NA("Ut_CFE_PSP_InitLocalTime() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_vxTimeBaseGet(void) test cases
**=======================================================================================*/
void Test_vxTimeBaseGet(void)
{
    uint32 uiU = 0;
    uint32 uiL = 0;
    uint64 uiTicks = 0;

    char cMsg_input_null[] = TIMER_PRINT_SCOPE "Input variables are NULL\n";
    char cMsg_timer_not_init[] = TIMER_PRINT_SCOPE "Timer is not initialized\n";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = true;
    OS_TaskDelay(400);
    /* Execute test */
    vxTimeBaseGet(&uiU, &uiL);
    /* Verify outputs */
    uiTicks = uiU;
    uiTicks <<= 32;
    uiTicks |= uiL;
    UtAssert_True((uiTicks > 0), "Ut_vxTimeBaseGet() - 1/4: Nominal");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Timer not initialized ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = false;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(&uiU, &uiL);
    /* Verify outputs */
    UtAssert_True(((uiU == 0) && (uiL == 0)), "Ut_vxTimeBaseGet() - 2/4: Timer not initialized - unchanged inputs");
    UtAssert_OS_print(cMsg_timer_not_init, "Ut_vxTimeBaseGet - 2/4: Timer not initialized - message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - NULL Tbu ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = true;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(NULL, &uiL);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_input_null, "Ut_vxTimeBaseGet - 3/4: NULL Tbu");
    UtAssert_True(((uiU == 0) && (uiL == 0)), "Ut_vxTimeBaseGet() - 3/4: NULL Tbu");

    /* ----- Test case #4 - NULL Tbl ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = true;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(&uiU, NULL);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_input_null, "Ut_vxTimeBaseGet - 4/4: NULL Tbl");
    UtAssert_True(((uiU == 0) && (uiL == 0)), "Ut_vxTimeBaseGet() - 4/4: NULL Tbl");
}
