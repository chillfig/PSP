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
*   \file psp_timer_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_timer.c.
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
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "psp_timer_testcases.h"
#include "../src/cfe_psp_timer.c"

/*=======================================================================================
** Preprocessor Directives
**=======================================================================================*/


/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_InitLocalTime(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_InitLocalTime(void)
{
    UT_SetDefaultReturnValue(UT_KEY(sysAuxClkDisable), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysAuxClkRateGet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysClkRateSet), OS_SUCCESS);

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
void Ut_vxTimeBaseGet(void)
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
    UtAssert_True((uiTicks > 0), "Ut_vxTimeBaseGet() - 1/3: Nominal");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Timer not initialized ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = false;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(&uiU, &uiL);
    /* Verify outputs */
    UtAssert_True(((uiU == 0) && (uiL == 0)), "Ut_vxTimeBaseGet() - 2/3: Timer not initialized - unchanged inputs");
    UtAssert_OS_print(cMsg_timer_not_init, "Ut_vxTimeBaseGet - 2/32: Timer not initialized - message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - NULL Inputs ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = true;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(NULL, NULL);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_input_null, "Ut_vxTimeBaseGet - 3/3: NULL Inputs - message");
}
