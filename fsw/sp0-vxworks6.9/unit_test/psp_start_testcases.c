/*=======================================================================================
**
** File:  psp_start_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_start.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-18 | Minh Luong       | Initial revision
**=======================================================================================*/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
#include <target_config.h>
#include <fcntl.h>

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_start_testcases.h"
#include "../src/cfe_psp_start.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern int volSensorRead(int8 sensor, uint8 dataType, float *voltage, bool talkative );
extern int tempSensorRead (int8 sensor, uint8 dataType, float *temperature, bool talkative );

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_Main() test cases
**=======================================================================================*/
void Ut_CFE_PSP_Main(void)
{
    /* ----- Test case #1: - Nominal ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_SUCCESS);

    CFE_PSP_Main();

    /* Verify results */
    UtAssert_NA("_CFE_PSP_Main - 1/1: Nominal, OS_BSPMain return success");

    /* ----- Test case #1: - Nominal ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_ERROR);

    CFE_PSP_Main();

    /* Verify results */
    UtAssert_NA("_CFE_PSP_Main - 1/1: Nominal, OS_BSPMain returned error");
}

/*=======================================================================================
** Ut_CFE_PSP_ProcessPOSTResults() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ProcessPOSTResults(void)
{
    char cMsg[256] = {};
    /* Set "Marching Address Test" tests for long and word to run. The rest tests are not run */
    uint64 bitExecuted = 3ULL; 
    /* Set "Marching Address Test(L)" result for failed and "Marching Address Test(W)" for passed */
    uint64 bitResult   = 1ULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), true);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), true);
    sprintf(cMsg, "PSP: CFE_PSP_ProcessPOSTResults: aimonGetBITExecuted() or aimonGetBITResults() failed.\n");
    /* Execute test */
    CFE_PSP_ProcessPOSTResults();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_ProcessPOSTResults - 1/2:Nominal");

    /* ----- Test case #2 - aimonGetBITExecuted() failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITExecuted), 1, OS_ERROR);
    sprintf(cMsg, "PSP: POST aimonGetBITExecuted() or aimonGetBITResults() failed.\n");

    /* Execute test */
    CFE_PSP_ProcessPOSTResults();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessPOSTResults - 2/2 aimonGetBITExecuted() or aimonGetBITResults() failed");

    /* ----- Test case #2 - aimonGetBITResults() failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITResults), 1, OS_ERROR);
    sprintf(cMsg, "PSP: POST aimonGetBITExecuted() or aimonGetBITResults() failed.\n");

    /* Execute test */
    CFE_PSP_ProcessPOSTResults();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessPOSTResults - 2/2 aimonGetBITExecuted() or aimonGetBITResults() failed");
}

/*=======================================================================================
** Ut_CFE_PSP_ProcessResetType() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ProcessResetType(void)
{
    uint32 uiResetSrc = 0;
    RESET_SRC_REG_ENUM retCode = 0;
    char cMsg[256] = {};

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal Power Switch ON ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_POR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: Power Switch ON.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg,  "_ProcessResetType() - 1/8:Nominal - POWERON Reset: Power Switch ON");
    UtAssert_True(retCode == RESET_SRC_POR, "_ProcessResetType() - 1/8:Nominal - POWERON Reset: Power Switch ON");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 1/8:Nominal - POWERON Reset: Power Switch ON, correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE, "_ProcessResetType() - 1/8:Nominal - POWERON Reset: Power Switch ON, correct reset subtype");

    /* ----- Test case #2 - Nominal External FPGA Watchdog timer primary EEPROM boot failure. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP: PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.\n");
    uiResetSrc = RESET_SRC_WDT;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");
    UtAssert_True(retCode == RESET_SRC_WDT, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure. Correct Reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure. Correct reset subtype");

    /* ----- Test case #3 - Nominal Internal FPGA Watchdog timer application SW failure. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_FWDT;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");
    UtAssert_True(retCode == RESET_SRC_FWDT, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure. Correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure. Correct reset subtype");

    /* ----- Test case #4 - Nominal cPCI Reset initiated by FPGA from remote SBC. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_CPCI;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");
    UtAssert_True(retCode == RESET_SRC_CPCI, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC. Correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC. Correct reset subtype");

    /* ----- Test case #5 - Nominal Software Hard Reset ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_SWR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: Software Hard Reset.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset");
    UtAssert_True(retCode == RESET_SRC_SWR, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset. Correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset. Correct reset subtype");

    /* ----- Test case #6 - Failed to read safemode data ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_SWR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDeferredRetcode(UT_KEY(ReadSafeModeUserData), 1, OS_ERROR);
    sprintf(cMsg, "PSP: PROCESSOR Reset: failed to read safemode data.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 6/8: Failed to read safemode data");
    UtAssert_True(retCode == RESET_SRC_SWR, "_ProcessResetType() - 6/8: POWERON Reset: Software Hard Reset");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 6/8: POWERON 8Reset: Software Hard Reset. Correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_ProcessResetType() - 6/8: POWERON Reset: Software Hard Reset. Correct reset subtype");

    /* ----- Test case #7 - POWERON Resest: UNKNOWN Reset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Some other, not listed reset code */
    uiResetSrc = 0xFF;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: UNKNOWN Reset.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_ProcessorResetType() - 7/8: Unknown reset type");
    UtAssert_True(retCode == 0xFF, "_ProcessorResetType() - 7/8: Unknown reset type");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessorResetType() - 7/8: Unknown reset type. Correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET, "_ProcessorResetType() - 7/8: Unknown reset type. Correct reset subtype");

    /* ----- Test case #8 - UNKNOWN Reset. Reset source read failed. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = 0;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: UNKNOWN Reset. Reset source read failed.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 8/9: UNKNOWN Reset. Reset source read failed.");
    UtAssert_True(retCode == 0, "_ProcessResetType() - 8/9: UNKNOWN Reset. Reset source read failed.");
    UtAssert_True(g_uiResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 8/9: UNKNOWN Reset. Reset source read failed. Correct reset type");
    UtAssert_True(g_uiResetSubtype == CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET, "_ProcessResetType() - 8/9: UNKNOWN Reset. Reset source read failed. Correct reset subtype");
}

/*=======================================================================================
** Ut_CFE_PSP_LogSoftwareResetType() test cases
**=======================================================================================*/
void Ut_CFE_PSP_LogSoftwareResetType(void)
{
    char cMsg[256] = {};
    char cMsg_mkcause[256] = {};
    RESET_SRC_REG_ENUM resetSrc = RESET_SRC_POR;

    sprintf(cMsg_mkcause, "PSP L1 instruction cache error\n");

    /* ----- Test case #1 - Nominal RESET_SRC_POR. No check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    g_safeModeUserData.safeMode = 0;
    g_safeModeUserData.sbc = SM_LOCAL_SBC;
    g_safeModeUserData.mckCause = 0;
    g_safeModeUserData.reason = 0;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "LOCAL",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 1/7:Nominal - RESET_SRC_POR");
    UtAssert_NoOS_print("PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_CFE_PSP_LogSoftwareResetType - 1/7:Nominal - No check for g_safeModeUserData.mckCause");

    /* ----- Test case #2 - Nominal RESET_SRC_WDT. No check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_WDT;
    g_safeModeUserData.sbc = SM_REMOTE_SBC;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_WDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "REMOTE",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 2/7:Nominal - RESET_SRC_WDT");
    UtAssert_NoOS_print("PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_CFE_PSP_LogSoftwareResetType - 2/7:Nominal - No check for g_safeModeUserData.mckCause");

    /* ----- Test case #3 - Nominal RESET_SRC_FWDT. No check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_FWDT;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_FWDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "REMOTE",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 3/7:Nominal - RESET_SRC_FWDT");
    UtAssert_NoOS_print("PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_CFE_PSP_LogSoftwareResetType - 3/7:Nominal - No check for g_safeModeUserData.mckCause");

    /* ----- Test case #4 - Nominal RESET_SRC_CPCI. No check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_CPCI;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_CPCI) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "REMOTE",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 4/7:Nominal - RESET_SRC_CPCI");
    UtAssert_NoOS_print("PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_CFE_PSP_LogSoftwareResetType - 4/7:Nominal - No check for g_safeModeUserData.mckCause");

    /* ----- Test case #5 - Nominal RESET_SRC_SWR. No check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_SWR;
    g_safeModeUserData.mckCause = 0U;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "REMOTE",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 5/7:Nominal - RESET_SRC_SWR");
    UtAssert_NoOS_print("PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_CFE_PSP_LogSoftwareResetType - 5/7:Nominal - No check for g_safeModeUserData.mckCause");

    /* ----- Test case #6 - Nominal RESET_SRC_SWR. Check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_SWR;
    g_safeModeUserData.mckCause = 1U;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "REMOTE",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 6/7:Nominal - RESET_SRC_SWR");
    UtAssert_OS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 6/7:Nominal - Check for g_safeModeUserData.mckCause");

    /* ----- Test case #7 - Nominal default to RESET_SRC_POR. No check for g_safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = 1;
    g_safeModeUserData.mckCause = 0U;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "REMOTE",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 7/7:Nominal - Default RESET_SRC_POR");
    UtAssert_NoOS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 7/7:Nominal - No check for g_safeModeUserData.mckCause");

    /* ----- Test case #8 - g_safeModeUserData.sbc != SM_LOCAL_SBC ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_SWR;
    g_safeModeUserData.mckCause = 1U;
    g_safeModeUserData.sbc = SM_LOCAL_SBC;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_safeModeUserData.safeMode,
            "LOCAL",
            g_safeModeUserData.reason,
            g_safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 8/8:Nominal - SM_REMOTE_SBC");
    UtAssert_OS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 8/8:Nominal - Check for g_safeModeUserData.mckCause");
}

/*=======================================================================================
** Ut_OS_Application_Startup() test cases
** Since there is a multiple definition issue for "OS_Application_Run()". This function
** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
** is located in unit_test/CMakeLists.txt
**=======================================================================================*/
void Ut_OS_Application_Startup(void)
{
    char cMsg[256] = {""};
    uint32 uiResetSrc = 0;
    uint64 bitResult   = 1ULL;
    uint64 bitExecuted = 3ULL;
    USER_SAFE_MODE_DATA_STRUCT smud;

    /* PSP_SP0_GetInfo SPE Functions - We are setting them once for all cases */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_ERROR);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    sprintf(cMsg, "PSP: PSP Application Startup Complete\n");
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 1/5: Nominal");

    /* ----- Test case #2 - Initialize the OS API data structures failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_ERROR);
    sprintf(cMsg, "PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 2/5: Initialize the OS API data structures failed");

    /* ----- Test case #3 - Setup FS mapping \"/cf\" directorys failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_ERROR);
    sprintf(cMsg, "PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/5: Setup FS mapping \"/cf\" directorys failed");

    /* ----- Test case #4 - CFE_PSP_InitProcessorReservedMemory() failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(creat), OS_ERROR);    
    sprintf(cMsg, "CFE_PSP: Failed to create the CDS file(/ffx0/CDS) on Flash.\n");
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/5: CFE_PSP_InitProcessorReservedMemory() failed");

    /* ----- Test case #5 - vxWorks task priority set failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(open), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(creat), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(taskPriorityGet), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(taskPrioritySet), OS_ERROR);
    sprintf(cMsg, "PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 5/5: vxWorks task priority set failed");
}

/*=======================================================================================
** Ut_CFE_PSP_GetRestartType() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetRestartType(void)
{
    uint32 uiResetSubType = 1;
    uint32 *pResetSubType = NULL;
    uint32 uiRetCode = 0;
    
    /* ----- Test case #1 - Nominal pResetSubType is NULL ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetCode = CFE_PSP_GetRestartType(pResetSubType);
    /* Verify outputs */
    UtAssert_True(pResetSubType == NULL, "_GetRestartType() - 1/2:pResetSubType is NULL");
    UtAssert_True(uiRetCode == g_uiResetType, "_GetRestartType() - 1/2:Nominal");
    
    /* ----- Test case #2 - Nominal pResetSubType was set value ----- */
    /* Setup additional inputs */
    pResetSubType = &uiResetSubType;
    /* Execute test */
    uiRetCode = CFE_PSP_GetRestartType(pResetSubType);
    /* Verify outputs */
    UtAssert_True(*pResetSubType == g_uiResetSubtype, "_GetRestartType() - 2/2:pResetSubType was set value");
    UtAssert_True(uiRetCode == g_uiResetType, "_GetRestartType() - 2/2:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_SetTaskPrio() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetTaskPrio(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    int32 iNewPrio = 0;
    int32 iCurPrio = 10;
    char *tName = "tLogTask";
    char *tName_null = NULL;
    char *tName_too_long = "123456789012345678901234567890";

    /* ----- Test case #1 - Nominal Priority set to 0 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP: Setting %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), false);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OK);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from -1 to 0 */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 1/6: Nominal - Priority set to 0");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 1/6: Nominal - Priority set to 0 ");
    
    /* ----- Test case #2 - Nominal Priority set to 255 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 255;
    sprintf(cMsg, "PSP: Setting %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OK);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from 260 to 255 */
    /* We cast 260 to uint8 type so ccppc doesn't give build error. Result is the same? */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 2/6: Nominal - Priority set to 255");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 2/6: Nominal - Priority set to 255");

    /* ----- Test case #3 - Failed to set priority to 25 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iCurPrio = 0;
    iNewPrio = 25;
    sprintf(cMsg, "PSP: taskPrioritySet() - Failed for %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask" */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 3/6: Failed - Failed to set priority to 25");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 3/6: Failed - Failed to set priority to 25");

    /* ----- Test case #4 - Failed to set priority to 25 for task name "tLogTask",
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iCurPrio = 0;
    iNewPrio = 25;
    sprintf(cMsg, "PSP: Could not find task %s \n", tName);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 4/6: Failed - Task ID lookup failure");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 4/6: Task ID lookup failure");

    /* ----- Test case #5 - task name is null,
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_null, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 5/6: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 5/6: Failed - task name is null does not print any message");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 5/6: Null task name returns error");

    /* ----- Test case #5 - task name is null,
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_too_long, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 6/6: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 6/6: Failed - task name array does not contain NULL terminating string");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 6/6: task name string without NULL returns error");
}

/*=======================================================================================
** Ut_CFE_PSP_SetSysTasksPrio() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetSysTasksPrio(void)
{
    uint32 uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    uiRetCode = CFE_PSP_SetSysTasksPrio();
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_SetSysTasksPrio - 1/2: Nominal");

    /* ----- Test case #2 - Failed to set priority for a task ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_SetSysTasksPrio();
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_SetSysTasksPrio - 2/2: Set priority for a task failed");
}

/*=======================================================================================
** Ut_OS_Application_Run() test cases
** Since there is a multiple definition issue for "OS_Application_Run()". This function
** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
** was located in unit_test/CMakeLists.txt
**=======================================================================================*/
void Ut_OS_Application_Run(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    UT_OS_Application_Run();
    /* Verify outputs */
    UtAssert_NA("_OS_Application_Run - 1/1: NA - Function is empty");
}

/*=======================================================================================
** Ut_CFE_PSP_SuspendConsoleShellTask() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SuspendConsoleShellTask(void)
{
    bool suspend;
    int32 status;
    char cMsg[256] = {};

    /* ----- Test #1 - Nominal true, no task id, task suspend successful ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = true;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskSuspend), 1, OK);
    sprintf(cMsg, "Shell Task Suspended [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_SUCCESS, "_CFE_PSP_SuspendConsoleShellTask - 1/4: Task successfully suspended");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 1/4: Task successfully suspended");

    /* ----- Test #2 - true, no task id, task suspend failure ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = true;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskSuspend), 1, CFE_PSP_ERROR);
    sprintf(cMsg, "Shell Task could not be suspended [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 2/4: Task did not successfully suspend");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 2/4: Task did not successfully suspend");

    /* ----- Test #3 - Nominal false, no task id, task resume successful ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskResume), 1, OK);
    sprintf(cMsg, "Shell Task Resumed [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_SUCCESS, "_CFE_PSP_SuspendConsoleShellTask - 3/4: Task successfully resumed");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 3/4: Task successfully resumed");

    /* ----- Test #4 - true, no task id, task resume failure ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskResume), 1, CFE_PSP_ERROR);
    sprintf(cMsg, "Shell Task could not be resumed [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");

    /* ----- Test #5 - Shell task id already acquired ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = 1;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskResume), 1, CFE_PSP_ERROR);
    sprintf(cMsg, "Shell Task could not be resumed [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
}

/*=======================================================================================
** Ut_CFE_PSP_InitSSR() test cases
**=======================================================================================*/
void Ut_CFE_PSP_InitSSR(void)
{
    Ut_OS_printf_Setup();
    int32 ret = OS_SUCCESS;
    char cMsg[256] = {};
    uint32 bus = 1;
    uint32 device = 1;
    char DeviceName[] = "Sample";
    sprintf(cMsg, "PSP: CFE_PSP_InitSSR function not implemented for SP0\n");

    ret = CFE_PSP_InitSSR(bus, device, DeviceName);
    UtAssert_True(ret = CFE_PSP_ERROR_NOT_IMPLEMENTED,"_CFE_PSP_InitSSR - 1/1: Not Implemented Return Code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitSSR - 1/1: Not Implemented Message");
}

/*=======================================================================================
** End of file psp_start_testcases.c
**=======================================================================================*/
