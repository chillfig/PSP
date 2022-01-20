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
extern int       PCS_snprintf(char *s, size_t maxlen, const char *format, ...);

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

    char cMsg_readreset[] = {SP0_PRINT_SCOPE "Error collecting data from ReadResetSourceReg()\n"};
    char cMsg_safemode_nowrite[] = {SP0_PRINT_SCOPE "Could not save data in sp0 info table safeModeUserData"};
    char cMsg_safemode_retrieve[] = {SP0_PRINT_SCOPE "Error collecting data from ReadSafeModeUserData()\n"};
    char cMsg_temp[] = {SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n"};
    char cMsg_volt[] = {SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n"};
    char cMsg_aimongetbitexec[] = {SP0_PRINT_SCOPE "Error collecting data from aimonGetBITExecuted()\n"};
    char cMsg_aimongetbitres[] = {SP0_PRINT_SCOPE "Error collecting data from aimonGetBITResults()\n"};

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 1/13: Nominal SP0s");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_readreset,"_PSP_SP0_GetInfo - 2/13: ReadResetSourceReg failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 2/13: Failed return code");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_retrieve,"_PSP_SP0_GetInfo - 3/13: ReadSafeModeUserData failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 3/13: Failed return code");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 4/13: Nominal with REMOTE SBC");
    
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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_ERROR);

    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_safemode_nowrite,"_PSP_SP0_GetInfo - 5/13: ReadSafeModeUserData snprintf failed");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 5/13: Failed return code");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 6/13: Nominal 2nd boot device");

    Ut_OS_printf_Setup();


    /* ----- Test case #6 - Temperature Fail ----- */
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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_temp,"_PSP_SP0_GetInfo - 7/13: Temperature failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 7/13: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #6.5 - Temperature Fail ----- */
    /* Setup additional inputs */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 1);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 8/13: Nominal SP0");

    Ut_OS_printf_Setup();


    /* ----- Test case #7 - Voltage Fail ----- */
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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_ERROR);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_volt,"_PSP_SP0_GetInfo - 9/13: Voltage failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 9/13: Failed return code");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitexec,"_PSP_SP0_GetInfo - 10/13: aimonGetBITExecute failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 10/13: Failed return code");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_aimongetbitres,"_PSP_SP0_GetInfo - 11/13: aimonGetBITResults failed message");
    UtAssert_True(ret == CFE_PSP_ERROR, "_PSP_SP0_GetInfo - 11/13: Failed return code");

    Ut_OS_printf_Setup();


    /* ----- Test case #12 - g_iSP0DataDumpLength too long ----- */
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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);
    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(g_iSP0DataDumpLength > SP0_TEXT_BUFFER_MAX_SIZE, "_PSP_SP0_GetInfo - 12/13: g_cSP0DataLength larger than max buffer");
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_PSP_SP0_GetInfo - 12/13: Even though g_cSP0DataDump too small return code success");

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
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);


    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_ERROR);

    /* Execute test */
    ret = PSP_SP0_GetInfo();
    /* Verify outputs */
    UtAssert_True(g_iSP0DataDumpLength == -1, "_PSP_SP0_GetInfo - 13/13: snprintf error, g_cSP0DataDump is -1");
    UtAssert_True(ret == CFE_PSP_ERROR_LEVEL_0, "_PSP_SP0_GetInfo - 13/13: although snprintf return code success");
}

/*=======================================================================================
** Ut_PSP_SP0_PrintToBuffer(void) test cases
**=======================================================================================*/
void Ut_PSP_SP0_PrintToBuffer(void)
{
    int32 ret_code = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    ret_code = PSP_SP0_PrintToBuffer();
    /* Verify outputs */

    UtAssert_NA("_PSP_SP0_PrintToBuffer - 1/1: N/A");
}

/*=======================================================================================
** Ut_PSP_SP0_GetInfoTable(void) test cases
**=======================================================================================*/
void Ut_PSP_SP0_GetInfoTable(void)
{
    SP0_info_table_t sp0_data;

    /* ----- Test case #1 - Nominal with no console print ----- */
    /* Setup additional inputs */
    g_sp0_info_table.lastUpdatedUTC.tv_sec = 1000;
    /* Execute test */
    sp0_data = PSP_SP0_GetInfoTable(false);
    /* Verify outputs */
    UtAssert_True(
        sp0_data.lastUpdatedUTC.tv_sec == g_sp0_info_table.lastUpdatedUTC.tv_sec,
        "_PSP_SP0_GetInfoTable - 1/3: Nominal without console print"
        );

    /* ----- Test case #1 - Nominal with console print ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump,(int)1, 20);
    g_iSP0DataDumpLength = 20;
    /* Execute test */
    sp0_data = PSP_SP0_GetInfoTable(true);
    /* Verify outputs */

    UtAssert_NA("_PSP_SP0_GetInfoTable - 2/3: Nominal with console print");

    /* ----- Test case #1 - Nothing to print ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cSP0DataDump,(int)NULL,SP0_TEXT_BUFFER_MAX_SIZE);
    g_iSP0DataDumpLength = 0;
    /* Execute test */
    sp0_data = PSP_SP0_GetInfoTable(true);
    /* Verify outputs */

    UtAssert_NA("_PSP_SP0_GetInfoTable - 3/3: Nothing to print");
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
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_PSP_SP0_DumpData - 1/5: Nominal");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Could not create dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), ERROR);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_creat, "_PSP_SP0_DumpData - 2/5: Could not create dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Could not write to dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_write, "_PSP_SP0_DumpData - 3/5: Could not write to dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Could not close dump file ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(open), OK);
    UT_SetDefaultReturnValue(UT_KEY(write), OK);
    UT_SetDefaultReturnValue(UT_KEY(close), ERROR);
    /* Execute test */
    PSP_SP0_DumpData();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_close, "_PSP_SP0_DumpData - 4/5: Could not close dump file");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #5 - Data not available ----- */
    /* Setup additional inputs */
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
** Ut_PSP_SP0_GetRAMDiskFreeSize(void) test cases
**=======================================================================================*/
void Ut_PSP_SP0_GetDiskFreeSize(void)
{
    char        ram_disk_path_ram[] = "/ram0";
    char        ram_disk_path_flash[] = "/ffx0";
    char        ram_disk_path_bad[] = "/f";
    char        ram_disk_path_too_long[200];
    int64_t     return_value = 0;

    /* Function is not stubbed */
    /* Execute test */
    return_value = PSP_SP0_GetDiskFreeSize(ram_disk_path_ram);
    /* Verify outputs */
    UtAssert_True(return_value > 0, "_PSP_SP0_GetDiskFreeSize - 1/4: Nominal");

    /* Function is not stubbed */
    /* Execute test */
    return_value = PSP_SP0_GetDiskFreeSize(ram_disk_path_bad);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_PSP_SP0_GetDiskFreeSize - 2/4: Bad path");

    /* Function is not stubbed */
    /* Execute test */
    return_value = PSP_SP0_GetDiskFreeSize(NULL);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_PSP_SP0_GetDiskFreeSize - 3/4: NULL path");

    memset(ram_disk_path_too_long,0x11,sizeof(ram_disk_path_too_long));
    /* Execute test */
    return_value = PSP_SP0_GetDiskFreeSize(ram_disk_path_too_long);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_PSP_SP0_GetDiskFreeSize - 4/4: Path too long");
}

/*=======================================================================================
** End of file psp_sp0_info_testcases.c
**=======================================================================================*/
