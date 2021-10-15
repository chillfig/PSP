/*=======================================================================================
**
** File:  psp_sp0_info_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_sp0_info.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2021-08-05 | Claudio Olmi     | Initial revision
**=======================================================================================*/

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
#include "../src/cfe_psp_sp0_info.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_PSP_SP0_GetInfo(void) test cases
**=======================================================================================*/
void Ut_PSP_SP0_GetInfo(void)
{
    int32 ret;
    /* Set "Marching Address Test" tests for long and word to run. The rest tests are not run */
    uint64 bitExecuted = 3ULL; 
    /* Set "Marching Address Test(L)" result for failed and "Marching Address Test(W)" for passed */
    uint64 bitResult   = 1ULL;

    char cMsg_readreset[200] = {SP0_PRINT_SCOPE "Error collecting data from ReadResetSourceReg()\n"};
    char cMsg_safemode_nowrite[200] = {SP0_PRINT_SCOPE "Could not save data in sp0 info table safeModeUserData"};
    char cMsg_safemode_retrieve[200] = {SP0_PRINT_SCOPE "Error collecting data from ReadSafeModeUserData()\n"};
    char cMsg_temp[200] = {SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n"};
    char cMsg_volt[200] = {SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n"};
    char cMsg_aimongetbitexec[200] = {SP0_PRINT_SCOPE "Error collecting data from aimonGetBITExecuted()\n"};
    char cMsg_aimongetbitres[200] = {SP0_PRINT_SCOPE "Error collecting data from aimonGetBITResults()\n"};

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
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 1/10: Nominal");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - ReadResetSourceReg failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_readreset,"_PSP_SP0_GetInfo - 2/10: ReadResetSourceReg failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 2/10: Failed return code");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - ReadSafeModeUserData failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_retrieve,"_PSP_SP0_GetInfo - 3/10: ReadSafeModeUserData failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 3/01: Failed return code");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - ReadSafeModeUserData - Nominal with REMOTE SBC ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    smud.sbc = SM_REMOTE_SBC;
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 4/10: Nominal with REMOTE SBC");
    
    Ut_OS_printf_Setup();

    /* ----- Test case #5 - ReadSafeModeUserData - snprintf failed ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
/*     ret = PSP_SP0_GetInfo(); */
    /* Verify outputs */
/*     UtAssert_OS_print(cMsg_safemode_nowrite,"_PSP_SP0_GetInfo - 5/10: ReadSafeModeUserData snprintf failed");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 5/10: Failed return code"); */

    Ut_OS_printf_Setup();

    /* ----- Test case #6 - Temperature Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_temp,"_PSP_SP0_GetInfo - 6/10: Temperature failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 6/10: Failed return code");

    Ut_OS_printf_Setup();

    /* ----- Test case #7 - Voltage Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_ERROR);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_volt,"_PSP_SP0_GetInfo - 7/10: Voltage failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 7/10: Failed return code");

    Ut_OS_printf_Setup();

    /* ----- Test case #8 - aimonGetBITExecute Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitexec,"_PSP_SP0_GetInfo - 8/10: aimonGetBITExecute failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 8/10: Failed return code");

    Ut_OS_printf_Setup();

    /* ----- Test case #9 - aimonGetBITResults Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitres,"_PSP_SP0_GetInfo - 9/10: aimonGetBITResults failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 9/10: Failed return code");

    Ut_OS_printf_Setup();

    /* ----- Test case #10 - g_iSP0DataDumpLength too long ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
/*     ret = PSP_SP0_GetInfo(); */
    /* Verify outputs */
/*     UtAssert_OS_print(cMsg_aimongetbitres,"_PSP_SP0_GetInfo - 10/10: g_iSP0DataDumpLength too long");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 10/10: Failed return code"); */
}

/*=======================================================================================
** Ut_PSP_SP0_PrintInfoTable(void) test cases
**=======================================================================================*/
void Ut_PSP_SP0_PrintInfoTable(void)
{
    char cMsg[SP0_TEXT_BUFFER_MAX_SIZE] = {};

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    // snprintf(g_cSP0DataDump, SP0_TEXT_BUFFER_MAX_SIZE, "0123456789");
    /* Execute test */
    PSP_SP0_PrintInfoTable();
    /* Verify outputs */
    /**
     * Corresponding code does not use OS_printf(...)
     */
    UtAssert_NA("_PSP_SP0_PrintInfoTable - 1/1: N/A");
    /* UtAssert_OS_print(cMsg,"_PSP_SP0_PrintInfoTable - 1/1: Nominal"); */
}

/*=======================================================================================
** Ut_PSP_SP0_DumpData(void) test cases
**=======================================================================================*/
void Ut_PSP_SP0_DumpData(void)
{
    char cMsg_creat[200] = {};
    char cMsg_write[200] = {};
    char cMsg_close[200] = {};
    char cMsg_nodump[200] = {};
    
    Ut_OS_printf_Setup();
    
    sprintf(cMsg_creat, SP0_PRINT_SCOPE "Could not create the SP0 info file dump\n");
    sprintf(cMsg_write, SP0_PRINT_SCOPE "Error while writing SP0 info data dump to file\n");
    sprintf(cMsg_close, SP0_PRINT_SCOPE "Could not close file SP0 info data dump\n");
    sprintf(cMsg_nodump, SP0_PRINT_SCOPE "Data Dump has not been initialized or error occured\n");
    g_iSP0DataDumpLength = snprintf(g_cSP0DataDump, SP0_TEXT_BUFFER_MAX_SIZE, "0123456789");

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* UT_ResetState(0); */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_PSP_SP0_DumpData - 1/5: Nominal");

    /* ----- Test case #2 - Could not create dump file ----- */
    /* Setup additional inputs */
    /* UT_ResetState(0); */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), ERROR);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_creat, "_PSP_SP0_DumpData - 2/5: Could not create dump file");

    /* ----- Test case #3 - Could not write to dump file ----- */
    /* Setup additional inputs */
    /* UT_ResetState(0); */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_write, "_PSP_SP0_DumpData - 3/5: Could not write to dump file");

    /* ----- Test case #4 - Could not close dump file ----- */
    /* Setup additional inputs */
    /* UT_ResetState(0); */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), ERROR);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_close, "_PSP_SP0_DumpData - 4/5: Could not close dump file");

    /* ----- Test case #5 - Data not available ----- */
    /* Setup additional inputs */
    /* UT_ResetState(0); */
    /* Set the content of the output data to a fixed value for testing */
    g_iSP0DataDumpLength = -1;
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_nodump, "_PSP_SP0_DumpData - 5/5: Data not available");
}

/*=======================================================================================
** End of file psp_sp0_info_testcases.c
**=======================================================================================*/
