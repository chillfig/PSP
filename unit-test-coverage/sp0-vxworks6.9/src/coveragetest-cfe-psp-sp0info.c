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

#include "coveragetest-psp-sp0-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include "PCS_vxWorks.h"
#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"
#include "PCS_scratchRegMap.h"
#include "PCS_sysLib.h"
#include "PCS_sys950Lib.h"
#include "PCS_aimonUtil.h"
#include "PCS_bflashCt.h"
#include "PCS_time.h"
#include "PCS_sysApi.h"
#include "PCS_tempSensor.h"
#include "PCS_stdio.h"
#include "PCS_fcntl.h"
#include "PCS_unistd.h"
#include "PCS_sys_stat.h"
#include "PCS_errno.h"

#include <cfe_psp_sp0.h>

#include "cfe_psp.h"
#include "cfe_psp_sp0info.h"

#define UT_SP0_PRINT_SCOPE "_PSP_SP0_"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern int32 CFE_PSP_SP0PrintToBuffer(void);
extern int32 CFE_PSP_SP0ROMXCmd(uint32_t uiCode);

extern char g_cSP0DataDump[SP0_TEXT_BUFFER_MAX_SIZE];
extern int32 g_iSP0DataDumpLength;

extern CFE_PSP_SP0StaticInfoTable_t g_SP0StaticInfoTable;
extern CFE_PSP_SP0DynamicInfoTable_t g_SP0DynamicInfoTable;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_SP0CollectStaticInfo(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0CollectStaticInfo(void)
{
    int32 ret;
    /* Set "Marching Address Test" tests for long and word to run. The rest tests are not run */
    uint64 bitExecuted = 3ULL; 
    /* Set "Marching Address Test(L)" result for failed and "Marching Address Test(W)" for passed */
    uint64 bitResult   = 1ULL;

    char cMsg_readreset[] = {SP0_PRINT_SCOPE "Error collecting data from ReadResetSourceReg()\n"};
    char cMsg_safemode_nowrite[] = {SP0_PRINT_SCOPE "Could not save data in sp0 info table safeModeUserData\n"};
    char cMsg_safemode_retrieve[] = {SP0_PRINT_SCOPE "Error collecting data from ReadSafeModeUserData()\n"};
    char cMsg_aimongetbitexec[] = {SP0_PRINT_SCOPE "Error collecting data from aimonGetBITExecuted()\n"};
    char cMsg_aimongetbitres[] = {SP0_PRINT_SCOPE "Error collecting data from aimonGetBITResults()\n"};
    char cMsg_localtime[] = {SP0_PRINT_SCOPE "Error getting local time\n"};

    uint32 uiResetSrc = 0;

    PCS_USER_SAFE_MODE_DATA_STRUCT smud;
    smud.safeMode = true;
    smud.sbc = SM_LOCAL_SBC;
    smud.mckCause = 0;
    smud.reason = SM_REASON_SHUTDOWN_MODE_API;

    Ut_OS_printf_Setup();
    
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);

    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectStaticInfo - 1/11: Nominal SP0s");

    Ut_OS_printf_Setup();


    /* ----- Test case #2 - ReadResetSourceReg failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0 ,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);

    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_readreset,"_CFE_PSP_SP0CollectStaticInfo - 2/11: ReadResetSourceReg failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 2/11: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #3 - ReadSafeModeUserData failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_retrieve,"_CFE_PSP_SP0CollectStaticInfo - 3/11: ReadSafeModeUserData failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 3/11: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #4 - ReadSafeModeUserData - Nominal with REMOTE SBC ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    
    smud.sbc = SM_REMOTE_SBC;
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectStaticInfo - 4/11: Nominal with REMOTE SBC");
    
    Ut_OS_printf_Setup();


    /* ----- Test case #5 - ReadSafeModeUserData - snprintf failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 2, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);

    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_nowrite,"_CFE_PSP_SP0CollectStaticInfo - 5/11: ReadSafeModeUserData snprintf failed");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 5/11: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #6 - 5.5 returnSelectedBootFlash 2nd boot ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 2);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectStaticInfo - 6/11: Nominal 2nd boot device");

    Ut_OS_printf_Setup();

    /* ----- Test case #7 - aimonGetBITExecute Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitexec,"_CFE_PSP_SP0CollectStaticInfo - 7/11: aimonGetBITExecute failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 7/11: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #8 - aimonGetBITResults Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitres,"_CFE_PSP_SP0CollectStaticInfo - 8/11: aimonGetBITResults failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 8/11: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #9 - clock_gettime error ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_clock_gettime), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_localtime, "_CFE_PSP_SP0CollectStaticInfo - 9/11: clock_gettime error");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 9/11: Even though g_cSP0DataDump too small return code success");

    /* ----- Test case #10 - last snprintf error ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 1);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDeferredRetcode(UT_KEY(PCS_clock_gettime), 1, PCS_OK);

    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_ERROR);

    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True(g_iSP0DataDumpLength == -1, "_CFE_PSP_SP0CollectStaticInfo - 10/11: snprintf error, g_cSP0DataDump is -1");
    UtAssert_True((ret == CFE_PSP_ERROR_LEVEL_0), "_CFE_PSP_SP0CollectStaticInfo - 10/11: although snprintf return code success");

    Ut_OS_printf_Setup();
    
    /* ----- Test case #11 - ReadSafeModeUserData - snprintf failed with buffer size greater than max----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump, 0, sizeof(g_cSP0DataDump));
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(PCS_getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_aimonGetBITResults), &bitResult, sizeof(bitResult), false);

    UT_SetDefaultReturnValue(UT_KEY(PCS_ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_returnSelectedBootFlash), 1);

    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 2, SP0_SAFEMODEUSERDATA_BUFFER_SIZE + 1);

    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_nowrite,"_CFE_PSP_SP0CollectStaticInfo - 11/11: ReadSafeModeUserData snprintf failed");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 11/11: Failed return code");
}

/*=======================================================================================
** Ut_CFE_PSP_SP0CollectDynamicInfo(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0CollectDynamicInfo(void)
{
    int32 ret = CFE_PSP_ERROR;
    char cMsg_temp[] = {SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n"};
    char cMsg_volt[] = {SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n"};
    char cMsg_localtime[] = {SP0_PRINT_SCOPE "Error getting local time\n"};

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(PCS_GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectDynamicInfo - 1/4: Nominal - Return Success");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Fail Temperature ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(PCS_GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_tempSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectDynamicInfo - 2/4: Fail to collect Temperature - Return Error");
    UtAssert_OS_print(cMsg_temp, "_CFE_PSP_SP0CollectDynamicInfo - 2/4: Fail to collect Temperature - Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Fail Voltage ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(PCS_GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_volSensorRead), OS_ERROR);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectDynamicInfo - 3/4: Fail to collect Voltage - Return Error");
    UtAssert_OS_print(cMsg_volt, "_CFE_PSP_SP0CollectDynamicInfo - 3/4: Fail to collect Voltage - Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Fail clock ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_sysGetBoardGeneration), 1);
    UT_SetDefaultReturnValue(UT_KEY(PCS_clock_gettime), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectDynamicInfo - 4/4: Fail to get time - Return Error");
    UtAssert_OS_print(cMsg_localtime, "_CFE_PSP_SP0CollectDynamicInfo - 4/4: Fail to get time - Message");
}

/*=======================================================================================
** Ut_CFE_PSP_SP0PrintToBuffer(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0PrintToBuffer(void)
{
    int32 ret_code = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    ret_code = CFE_PSP_SP0PrintToBuffer();
    /* Verify outputs */
    UtAssert_True(ret_code == CFE_PSP_SUCCESS, "_CFE_PSP_SP0PrintToBuffer - 1/2: Print to buffer success");
    UtAssert_STUB_COUNT(PCS_snprintf, 1);

    /* ----- Test case #2 - Buffer dump too long ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump, 0, sizeof(g_cSP0DataDump));
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), SP0_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    ret_code = CFE_PSP_SP0PrintToBuffer();
    /* Verify outputs */
    UtAssert_True(ret_code == CFE_PSP_ERROR, "_CFE_PSP_SP0PrintToBuffer - 2/2: Print to buffer fail");
}

/*=======================================================================================
** Ut_CFE_PSP_SP0GetStaticInfoTable(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0GetStaticInfoTable(void)
{
    CFE_PSP_SP0StaticInfoTable_t sp0_data;
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal with no console print ----- */
    /* Setup additional inputs */
    g_SP0StaticInfoTable.lastUpdatedUTC.tv_sec = 1000;
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetStaticInfoTable(&sp0_data, sizeof(sp0_data), 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_SUCCESS,
        "_CFE_PSP_SP0GetStaticInfoTable - 1/5: Nominal return success"
        );
    UtAssert_True(
        sp0_data.lastUpdatedUTC.tv_sec == g_SP0StaticInfoTable.lastUpdatedUTC.tv_sec,
        "_CFE_PSP_SP0GetStaticInfoTable - 1/5: Nominal without console print"
        );

    /* ----- Test case #2 - Nominal with console print ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetStaticInfoTable(&sp0_data, sizeof(sp0_data), 1);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_SUCCESS,
        "_CFE_PSP_SP0GetStaticInfoTable - 2/5: Nominal return success"
        );
    UtAssert_STUB_COUNT(PCS_printf, 1);

    /* ----- Test case #3 - data dump string array length is zero ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    g_iSP0DataDumpLength = 0;
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetStaticInfoTable(&sp0_data, sizeof(sp0_data), 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_SUCCESS,
        "_CFE_PSP_SP0GetStaticInfoTable - 3/5: Data dump string length zero - return success"
        );

    /* ----- Test case #4 - Data structure size insufficient ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetStaticInfoTable(&sp0_data, 5, 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_ERROR,
        "_CFE_PSP_SP0GetStaticInfoTable - 4/5: Data structure size insufficient - return error"
        );

    /* ----- Test case #5 - Null structure ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetStaticInfoTable(NULL, 1000, 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_ERROR,
        "_CFE_PSP_SP0GetStaticInfoTable - 5/5: Nominal return success"
        );
}

/*=======================================================================================
** Ut_CFE_PSP_SP0GetDynamicInfoTable(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0GetDynamicInfoTable(void)
{
    CFE_PSP_SP0DynamicInfoTable_t sp0_data;
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal with no console print ----- */
    /* Setup additional inputs */
    g_SP0DynamicInfoTable.lastUpdatedUTC.tv_sec = 1000;
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetDynamicInfoTable(&sp0_data, sizeof(sp0_data), 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_SUCCESS,
        "_CFE_PSP_SP0GetDynamicInfoTable - 1/5: Nominal return success"
        );
    UtAssert_True(
        sp0_data.lastUpdatedUTC.tv_sec == g_SP0DynamicInfoTable.lastUpdatedUTC.tv_sec,
        "_CFE_PSP_SP0GetDynamicInfoTable - 1/5: Nominal without console print"
        );

    /* ----- Test case #2 - Nominal with console print ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetDynamicInfoTable(&sp0_data, sizeof(sp0_data), 1);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_SUCCESS,
        "_CFE_PSP_SP0GetDynamicInfoTable - 2/5: Nominal return success"
        );
    UtAssert_STUB_COUNT(PCS_printf, 1);

    /* ----- Test case #3 - data dump string array length is zero ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump, 0, SP0_TEXT_BUFFER_MAX_SIZE);
    g_iSP0DataDumpLength = 0;
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetDynamicInfoTable(&sp0_data, sizeof(sp0_data), 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_SUCCESS,
        "_CFE_PSP_SP0GetDynamicInfoTable - 3/5: Data dump string length zero - return success"
        );

    /* ----- Test case #4 - Data structure size insufficient ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetDynamicInfoTable(&sp0_data, 5, 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_ERROR,
        "_CFE_PSP_SP0GetDynamicInfoTable - 4/5: Data structure size insufficient - return error"
        );

    /* ----- Test case #5 - Null structure ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_SP0GetDynamicInfoTable(NULL, 1000, 0);
    /* Verify outputs */
    UtAssert_True(
        iRetCode == CFE_PSP_ERROR,
        "_CFE_PSP_SP0GetDynamicInfoTable - 5/5: Nominal return success"
        );
}

/*=======================================================================================
** Ut_CFE_PSP_SP0DumpData(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0DumpData(void)
{
    char cMsg_creat[200] = {};
    char cMsg_write[200] = {};
    char cMsg_close[200] = {};
    char cMsg_nodump[200] = {};
    int  iFileID = 10;

    Ut_OS_printf_Setup();
    
    PCS_errno = 0;

    snprintf(cMsg_creat, sizeof(cMsg_creat), SP0_PRINT_SCOPE "Could not create the SP0 info file dump. errno=`%d`\n", PCS_errno);
    snprintf(cMsg_write, sizeof(cMsg_write), SP0_PRINT_SCOPE "Error while writing SP0 info data dump to file. errno=`%d`\n", PCS_errno);
    snprintf(cMsg_close, sizeof(cMsg_close), SP0_PRINT_SCOPE "Could not close file SP0 info data dump. errno=`%d`\n", PCS_errno);
    snprintf(cMsg_nodump, sizeof(cMsg_nodump), SP0_PRINT_SCOPE "Data Dump has not been initialized or error occurred\n");
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, SP0_TEXT_BUFFER_MAX_SIZE, "0123456789");

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(PCS_remove), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(PCS_write), g_iSP0DataDumpLength);
    UT_SetDefaultReturnValue(UT_KEY(PCS_close), PCS_OK);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SP0DumpData - 1/5: Nominal");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Could not create dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(PCS_remove), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_open), PCS_ERROR);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_creat, "_CFE_PSP_SP0DumpData - 2/5: Could not create dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Could not write to dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(PCS_remove), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(PCS_write), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_close), PCS_OK);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_write, "_CFE_PSP_SP0DumpData - 3/5: Could not write to dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Could not close dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(PCS_remove), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(PCS_write), g_iSP0DataDumpLength);
    UT_SetDefaultReturnValue(UT_KEY(PCS_close), PCS_ERROR);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_close, "_CFE_PSP_SP0DumpData - 4/5: Could not close dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #5 - Data not available ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    g_iSP0DataDumpLength = -1;
    UT_SetDefaultReturnValue(UT_KEY(PCS_remove), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(PCS_write), g_iSP0DataDumpLength);
    UT_SetDefaultReturnValue(UT_KEY(PCS_close), PCS_OK);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_nodump, "_CFE_PSP_SP0DumpData - 5/5: Data not available");
}

/*=======================================================================================
** Ut_PSP_SP0_GetRAMDiskFreeSize(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SP0GetDiskFreeSize(void)
{
    char        ram_disk_path_ram[] = "/ram0";
    char        ram_disk_path_bad[] = "/f";
    char        ram_disk_path_too_long[200];
    int64_t     return_value = 0;

    struct PCS_statfs nominal_statfs_struct = {5, 10};

    UT_SetDeferredRetcode(UT_KEY(PCS_statfs), 1, PCS_OK);
    UT_SetDataBuffer(UT_KEY(PCS_statfs), &nominal_statfs_struct, sizeof(nominal_statfs_struct), true);
    /* Execute test */
    return_value = CFE_PSP_SP0GetDiskFreeSize(ram_disk_path_ram);
    /* Verify outputs */
    UtAssert_True(return_value > 0, "_CFE_PSP_SP0GetDiskFreeSize - 1/4: Nominal");

    UT_SetDeferredRetcode(UT_KEY(PCS_statfs), 1, PCS_ERROR);
    /* Execute test */
    return_value = CFE_PSP_SP0GetDiskFreeSize(ram_disk_path_bad);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_CFE_PSP_SP0GetDiskFreeSize - 2/4: Bad path");

    /* Function is not stubbed */
    /* Execute test */
    return_value = CFE_PSP_SP0GetDiskFreeSize(NULL);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_CFE_PSP_SP0GetDiskFreeSize - 3/4: NULL path");

    memset(ram_disk_path_too_long,0x11,sizeof(ram_disk_path_too_long));
    /* Execute test */
    return_value = CFE_PSP_SP0GetDiskFreeSize(ram_disk_path_too_long);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_CFE_PSP_SP0GetDiskFreeSize - 4/4: Path too long");
}

/**********************************************************
 * void Test_CFE_PSP_SP0ROM1Lock(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROM1Lock(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - LOCK ROM1 nominal ----- */
    /* Set additional inputs */
    PCS_SP0_ROM1_MASK = 0x00000100;
    PCS_SP0_ROM1_STATUS_SHIFT = 8;
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM1Lock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM1_LOCK - 1/2: LOCK ROM1 nominal - return code");
    UtAssert_True(CFE_PSP_SP0ROM1Status() == true, UT_SP0_PRINT_SCOPE "ROM1_LOCK - 1/2: LOCK ROM1 nominal - Confirmed");

    /* ----- Test case #2 - LOCK ROM1 fail ----- */
    /* Set additional inputs */
    PCS_SP0_ROM1_MASK = 0x00001000;
    PCS_SP0_ROM1_STATUS_SHIFT = 12;
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM1Lock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_SP0_PRINT_SCOPE "ROM1_LOCK - 2/2: LOCK ROM1 fail - return code");
    UtAssert_True(CFE_PSP_SP0ROM1Status() == false, UT_SP0_PRINT_SCOPE "ROM1_LOCK - 2/2: LOCK ROM1 fail - Confirmed");
}

/**********************************************************
 * void Test_CFE_PSP_SP0ROM1Unlock(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROM1Unlock(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - UNLOCK ROM1 nominal ----- */
    /* Set additional inputs */
    PCS_SP0_ROM1_MASK = 0x00000100;
    PCS_SP0_ROM1_STATUS_SHIFT = 8;
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM1Unlock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM1_UNLOCK - 1/2: UNLOCK ROM1 nominal - return code");
    UtAssert_True(CFE_PSP_SP0ROM1Status() == false, UT_SP0_PRINT_SCOPE "ROM1_UNLOCK - 1/2: UNLOCK ROM1 nominal - Confirmed");

    /* ----- Test case #2 - UNLOCK ROM1 fail ----- */
    /* Set additional inputs */
    PCS_SP0_ROM1_MASK = 0x00000001;
    PCS_SP0_ROM1_STATUS_SHIFT = 0;
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM1Unlock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_SP0_PRINT_SCOPE "ROM1_UNLOCK - 2/2: UNLOCK ROM1 fail - return code");
    UtAssert_True(CFE_PSP_SP0ROM1Status() == true, UT_SP0_PRINT_SCOPE "ROM1_UNLOCK - 2/2: UNLOCK ROM1 fail - Confirmed");
}

/**********************************************************
 * void Test_PSP_SP1_ROM2_LOCK(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROM2Lock(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - UNLOCK ROM2 nominal ----- */
    /* Set additional inputs */
    PCS_SP0_ROM2_MASK = 0x00000200;
    PCS_SP0_ROM2_STATUS_SHIFT = 9;
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM2Lock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM2_LOCK - 1/2: LOCK ROM2 nominal - return code");
    UtAssert_True(CFE_PSP_SP0ROM2Status() == true, UT_SP0_PRINT_SCOPE "ROM2_LOCK - 1/2: LOCK ROM2 nominal - Confirmed");

    /* ----- Test case #2 - UNLOCK ROM2 fail ----- */
    /* Set additional inputs */
    PCS_SP0_ROM2_MASK = 0x00002000;
    PCS_SP0_ROM2_STATUS_SHIFT = 13;
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM2Lock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_SP0_PRINT_SCOPE "ROM2_LOCK - 2/2: LOCK ROM2 fail - return code");
    UtAssert_True(CFE_PSP_SP0ROM2Status() == false, UT_SP0_PRINT_SCOPE "ROM2_LOCK - 2/2: LOCK ROM2 fail - Confirmed");
}

/**********************************************************
 * void Test_CFE_PSP_SP0ROM2Unlock(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROM2Unlock(void)
{
	int32 iReturnCode = CFE_PSP_SUCCESS;
    /* ----- Test case #1 - UNLOCK ROM1 nominal ----- */
    /* Set additional inputs */
    PCS_SP0_ROM2_MASK = 0x00000200;
    PCS_SP0_ROM2_STATUS_SHIFT = 9;
	/* Execute test */
	iReturnCode = CFE_PSP_SP0ROM2Unlock();
    /* Verify results */
	UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM2_UNLOCK - 1/2: UNLOCK ROM2 nominal - return code");
    UtAssert_True(CFE_PSP_SP0ROM2Status() == false, UT_SP0_PRINT_SCOPE "ROM2_UNLOCK - 1/2: UNLOCK ROM2 nominal - Confirmed");

    /* ----- Test case #2 - UNLOCK ROM1 fail ----- */
    /* Set additional inputs */
    PCS_SP0_ROM2_MASK = 0x00000001;
    PCS_SP0_ROM2_STATUS_SHIFT = 0;
	/* Execute test */
	iReturnCode = CFE_PSP_SP0ROM2Unlock();
    /* Verify results */
	UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_SP0_PRINT_SCOPE "ROM2_UNLOCK - 2/2: UNLOCK ROM2 fail - return code");
    UtAssert_True(CFE_PSP_SP0ROM2Status() == true, UT_SP0_PRINT_SCOPE "ROM2_UNLOCK - 2/2: UNLOCK ROM2 fail - Confirmed");
}

/**********************************************************
 * void Test_CFE_PSP_SP0ROMXCmd(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROMXCmd(void)
{
    int32_t iReturnCode = CFE_PSP_SUCCESS;

    /* Setup inputs for nominal cases */
    PCS_SP0_ROM1_MASK = 0x00000100;
    PCS_SP0_ROM1_STATUS_SHIFT = 8;
    PCS_SP0_ROM2_MASK = 0x00000200;
    PCS_SP0_ROM2_STATUS_SHIFT = 9;

    /* ----- Test case #1 - ROM1 LOCK ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROMXCmd(SP0_ROM1_CODE_LOCK);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROMX_COMMAND - 1/5: ROM1 LOCK - return code");

    /* ----- Test case #2 - ROM1 UNLOCK ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROMXCmd(SP0_ROM1_CODE_UNLOCK);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROMX_COMMAND - 2/5: ROM1 UNLOCK - return code");

    /* ----- Test case #3 - ROM2 LOCK ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROMXCmd(SP0_ROM2_CODE_LOCK);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROMX_COMMAND - 3/5: ROM2 LOCK - return code");

    /* ----- Test case #4 - ROM2 UNLOCK ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROMXCmd(SP0_ROM2_CODE_UNLOCK);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROMX_COMMAND - 4/5: ROM2 UNLOCK - return code");

    /* ----- Test case #5 - Hit Default case ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROMXCmd((uint32_t) 0x00000000);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_SP0_PRINT_SCOPE "ROMX_COMMAND - 5/5: Default switch case - return code");

}

/**********************************************************
 * void Test_CFE_PSP_SP0ROM1Status(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROM1Status(void)
{
    bool bRetval = false;

    /* ----- Test case #1 - Get Status ----- */
    /* Set additional inputs */
    PCS_SP0_ROM1_MASK = 0x00000100;
    PCS_SP0_ROM1_STATUS_SHIFT = 8;
    *PCS_SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM1_CODE_LOCK;
    /* Execute test */
    bRetval = CFE_PSP_SP0ROM1Status();
    /* Verify results */
    UtAssert_True(bRetval == true, UT_SP0_PRINT_SCOPE "ROM1_Status - 1/1: Get ROM1 Status - NA");
}

/**********************************************************
 * void Test_CFE_PSP_SP0ROM2Status(void) test cases
 *********************************************************/
void Test_CFE_PSP_SP0ROM2Status(void)
{
    bool bRetval = false;

    /* ----- Test case #1 - Get Status ----- */
    /* Set additional inputs */
    PCS_SP0_ROM2_MASK = 0x00000200;
    PCS_SP0_ROM2_STATUS_SHIFT = 9;
    *PCS_SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM2_CODE_LOCK;
    /* Execute test */
    bRetval = CFE_PSP_SP0ROM2Status();
    /* Verify results */
    UtAssert_True(bRetval == true, UT_SP0_PRINT_SCOPE "ROM2_Status - 1/1: Get ROM2 Status - NA");
}

/*=======================================================================================
** End of file psp_gr740info_testcases.c
**=======================================================================================*/
