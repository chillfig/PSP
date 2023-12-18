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
*   \file psp_sp0_info_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_sp0info.c.
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

#include "psp_sp0_info_testcases.h"
#include "../src/cfe_psp_sp0info.c"

/**********************************************************
 * PREPROCESSOR DIRECTIVES
 *********************************************************/
#define UT_SP0_PRINT_SCOPE "_PSP_SP0_"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int       PCS_snprintf(char *s, size_t maxlen, const char *format, ...);

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_SP0CollectStaticInfo(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SP0CollectStaticInfo(void)
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

    USER_SAFE_MODE_DATA_STRUCT smud;
    smud.safeMode = true;
    smud.sbc = SM_LOCAL_SBC;
    smud.mckCause = 0;
    smud.reason = SM_REASON_SHUTDOWN_MODE_API;

    Ut_OS_printf_Setup();
    
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);

    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectStaticInfo - 1/10: Nominal SP0s");

    Ut_OS_printf_Setup();


    /* ----- Test case #2 - ReadResetSourceReg failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);

    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), true);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_readreset,"_CFE_PSP_SP0CollectStaticInfo - 2/10: ReadResetSourceReg failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 2/10: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #3 - ReadSafeModeUserData failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_retrieve,"_CFE_PSP_SP0CollectStaticInfo - 3/10: ReadSafeModeUserData failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 3/10: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #4 - ReadSafeModeUserData - Nominal with REMOTE SBC ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    
    smud.sbc = SM_REMOTE_SBC;
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), 1);

    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectStaticInfo - 4/10: Nominal with REMOTE SBC");
    
    Ut_OS_printf_Setup();


    /* ----- Test case #5 - ReadSafeModeUserData - snprintf failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);

    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_nowrite,"_CFE_PSP_SP0CollectStaticInfo - 5/10: ReadSafeModeUserData snprintf failed");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 5/10: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case # - 5.5 returnSelectedBootFlash 2nd boot ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 2);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectStaticInfo - 6/10: Nominal 2nd boot device");

    Ut_OS_printf_Setup();

    /* ----- Test case #8 - aimonGetBITExecute Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitexec,"_CFE_PSP_SP0CollectStaticInfo - 7/10: aimonGetBITExecute failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 7/10: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #9 - aimonGetBITResults Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitres,"_CFE_PSP_SP0CollectStaticInfo - 8/10: aimonGetBITResults failed message");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 8/10: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #12 - clock_gettime error ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_localtime, "_CFE_PSP_SP0CollectStaticInfo - 9/10: clock_gettime error");
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectStaticInfo - 9/10: Even though g_cSP0DataDump too small return code success");

    /* ----- Test case #13 - last snprintf error ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 1);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);

    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_ERROR);

    /* Execute test */
    ret = CFE_PSP_SP0CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True(g_iSP0DataDumpLength == -1, "_CFE_PSP_SP0CollectStaticInfo - 10/10: snprintf error, g_cSP0DataDump is -1");
    UtAssert_True((ret == CFE_PSP_ERROR_LEVEL_0), "_CFE_PSP_SP0CollectStaticInfo - 10/10: although snprintf return code success");
}

/*=======================================================================================
** Ut_CFE_PSP_SP0CollectDynamicInfo(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SP0CollectDynamicInfo(void)
{
    int32 ret = CFE_PSP_ERROR;
    char cMsg_temp[] = {SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n"};
    char cMsg_volt[] = {SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n"};
    char cMsg_localtime[] = {SP0_PRINT_SCOPE "Error getting local time\n"};

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_SUCCESS), "_CFE_PSP_SP0CollectDynamicInfo - 1/4: Nominal - Return Success");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Fail Temperature ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectDynamicInfo - 2/4: Fail to collect Temperature - Return Error");
    UtAssert_OS_print(cMsg_temp, "_CFE_PSP_SP0CollectDynamicInfo - 2/4: Fail to collect Temperature - Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Fail Voltage ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_ERROR);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectDynamicInfo - 3/4: Fail to collect Voltage - Return Error");
    UtAssert_OS_print(cMsg_volt, "_CFE_PSP_SP0CollectDynamicInfo - 3/4: Fail to collect Voltage - Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Fail clock ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysGetBoardGeneration), 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SP0CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((ret == CFE_PSP_ERROR), "_CFE_PSP_SP0CollectDynamicInfo - 4/4: Fail to get time - Return Error");
    UtAssert_OS_print(cMsg_localtime, "_CFE_PSP_SP0CollectDynamicInfo - 4/4: Fail to get time - Message");
}

/*=======================================================================================
** Ut_CFE_PSP_SP0GetStaticInfoTable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SP0GetStaticInfoTable(void)
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
    UtAssert_NA("_CFE_PSP_SP0GetStaticInfoTable - 2/5: Nominal with console print");

    /* ----- Test case #3 - data dump string array length is zero ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump,(int)NULL, SP0_TEXT_BUFFER_MAX_SIZE);
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
void Ut_CFE_PSP_SP0GetDynamicInfoTable(void)
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
    UtAssert_NA("_CFE_PSP_SP0GetDynamicInfoTable - 2/5: Nominal with console print");

    /* ----- Test case #3 - data dump string array length is zero ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump,(int)NULL, SP0_TEXT_BUFFER_MAX_SIZE);
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
** Ut_CFE_PSP_SP0PrintToBuffer(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SP0PrintToBuffer(void)
{
    int32 ret_code = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    ret_code = CFE_PSP_SP0PrintToBuffer();
    /* Verify outputs */

    UtAssert_NA("_CFE_PSP_SP0PrintToBuffer - 1/1: N/A");
}

/*=======================================================================================
** Ut_CFE_PSP_SP0DumpData(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SP0DumpData(void)
{
    char cMsg_creat[200] = {};
    char cMsg_write[200] = {};
    char cMsg_close[200] = {};
    char cMsg_nodump[200] = {};
    int  iFileID = 10;

    Ut_OS_printf_Setup();
    
    errno = 9;

    sprintf(cMsg_creat, SP0_PRINT_SCOPE "Could not create the SP0 info file dump. errno=`%d`\n", errno);
    sprintf(cMsg_write, SP0_PRINT_SCOPE "Error while writing SP0 info data dump to file. errno=`%d`\n", errno);
    sprintf(cMsg_close, SP0_PRINT_SCOPE "Could not close file SP0 info data dump. errno=`%d`\n", errno);
    sprintf(cMsg_nodump, SP0_PRINT_SCOPE "Data Dump has not been initialized or error occurred\n");
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, SP0_TEXT_BUFFER_MAX_SIZE, "0123456789");

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(write), g_iSP0DataDumpLength);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SP0DumpData - 1/5: Nominal");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Could not create dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), ERROR);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_creat, "_CFE_PSP_SP0DumpData - 2/5: Could not create dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Could not write to dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(write), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_write, "_CFE_PSP_SP0DumpData - 3/5: Could not write to dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Could not close dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(write), g_iSP0DataDumpLength);
    UT_SetDefaultReturnValue(UT_KEY(close), ERROR);
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
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), iFileID);
    UT_SetDefaultReturnValue(UT_KEY(write), g_iSP0DataDumpLength);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    CFE_PSP_SP0DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_nodump, "_CFE_PSP_SP0DumpData - 5/5: Data not available");
}

/*=======================================================================================
** Ut_PSP_SP0_GetRAMDiskFreeSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SP0GetDiskFreeSize(void)
{
    char        ram_disk_path_ram[] = "/ram0";
    char        ram_disk_path_bad[] = "/f";
    char        ram_disk_path_too_long[200];
    int64_t     return_value = 0;

    /* Function is not stubbed */
    /* Execute test */
    return_value = CFE_PSP_SP0GetDiskFreeSize(ram_disk_path_ram);
    /* Verify outputs */
    UtAssert_True(return_value > 0, "_CFE_PSP_SP0GetDiskFreeSize - 1/4: Nominal");

    /* Function is not stubbed */
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
 * void Ut_CFE_PSP_SP0ROM1Lock(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROM1Lock(void)
{
    /* NOTE: Testing on target will actually modify the ROM lock/unlock status.
     * By default, this unit test will keep ROM1 LOCKED after testing.
     */

    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - LOCK ROM1 ----- */
    /* Set additional inputs */
    CFE_PSP_SP0ROM1Unlock();
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM1Lock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM1_LOCK - 1/1: LOCK ROM1 - return code");
    UtAssert_True(CFE_PSP_SP0ROM1Status(), UT_SP0_PRINT_SCOPE "ROM1_LOCK - 1/1: LOCK ROM1 - Confirmed");
}

/**********************************************************
 * void Ut_CFE_PSP_SP0ROM1Unlock(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROM1Unlock(void)
{
    /* NOTE: Testing on target will actually modify the ROM lock/unlock status.
     * By default, this unit test will keep ROM1 UNLOCKED after testing.
     */

    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - UNLOCK ROM1 ----- */
    /* Set additional inputs */
    CFE_PSP_SP0ROM1Lock();
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM1Unlock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM1_UNLOCK - 1/1: UNLOCK ROM1 - return code");
    UtAssert_True(!CFE_PSP_SP0ROM1Status(), UT_SP0_PRINT_SCOPE "ROM1_UNLOCK - 1/1: UNLOCK ROM1 - Confirmed");
}

/**********************************************************
 * void Ut_PSP_SP1_ROM2_LOCK(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROM2Lock(void)
{
    /* NOTE: Testing on target will actually modify the ROM lock/unlock status.
     * By default, this unit test will keep ROM2 LOCKED after testing.
     */

    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - UNLOCK ROM2 ----- */
    /* Set additional inputs */
    CFE_PSP_SP0ROM2Unlock();
    /* Execute test */
    iReturnCode = CFE_PSP_SP0ROM2Lock();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM2_LOCK - 1/1: LOCK ROM2 - return code");
    UtAssert_True(CFE_PSP_SP0ROM2Status(), UT_SP0_PRINT_SCOPE "ROM2_LOCK - 1/1: LOCK ROM2 - Confirmed");
}

/**********************************************************
 * void Ut_CFE_PSP_SP0ROM2Unlock(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROM2Unlock(void)
{
    /* NOTE: Testing on target will actually modify the ROM lock/unlock status.
     * By default, this unit test will keep ROM2 UNLOCKED after testing.
     */

	int32 iReturnCode = CFE_PSP_SUCCESS;
    /* ----- Test case #1 - UNLOCK ROM1 ----- */
    /* Set additional inputs */
    CFE_PSP_SP0ROM2Lock();
	/* Execute test */
	iReturnCode = CFE_PSP_SP0ROM2Unlock();
    /* Verify results */
	UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_SP0_PRINT_SCOPE "ROM2_UNLOCK - 1/1: UNLOCK ROM2 - return code");
    UtAssert_True(!CFE_PSP_SP0ROM2Status(), UT_SP0_PRINT_SCOPE "ROM2_UNLOCK - 1/1: UNLOCK ROM2 - Confirmed");
}

/**********************************************************
 * void Ut_CFE_PSP_SP0ROMXCmd(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROMXCmd(void)
{
    /* NOTE: Testing on target will actually modify the ROM lock/unlock status.
     * By default, the unit test will keep the ROMs UNLOCKED after testing.
     */

    int32_t iReturnCode = CFE_PSP_SUCCESS;

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
 * void Ut_CFE_PSP_SP0ROM1Status(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROM1Status(void)
{
    bool bReturnValue = false;
    /* ----- Test case #1 - Get Status ----- */
    /* Set additional inputs */
    /* Execute test */
    bReturnValue = CFE_PSP_SP0ROM1Status();
    /* Verify results */
    UtAssert_NA(UT_SP0_PRINT_SCOPE "ROM1_Status - 1/1: Get ROM1 Status - NA");
}

/**********************************************************
 * void Ut_CFE_PSP_SP0ROM2Status(void) test cases
 *********************************************************/
void Ut_CFE_PSP_SP0ROM2Status(void)
{
    bool bReturnValue = false;
    /* ----- Test case #1 - Get Status ----- */
    /* Set additional inputs */
    /* Execute test */
    bReturnValue = CFE_PSP_SP0ROM2Status();
    /* Verify results */
    UtAssert_NA(UT_SP0_PRINT_SCOPE "ROM2_Status - 1/1: Get ROM2 Status - NA");
}

/*=======================================================================================
** End of file psp_sp0_info_testcases.c
**=======================================================================================*/
