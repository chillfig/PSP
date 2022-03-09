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
extern char *PCS_strncpy(char *dst, const char *src, size_t size);
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
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 1/8:Nominal - POWERON Reset: Power Switch ON, correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE, "_ProcessResetType() - 1/8:Nominal - POWERON Reset: Power Switch ON, correct reset subtype");

    /* ----- Test case #2 - Nominal External FPGA Watchdog timer primary EEPROM boot failure. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP: PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.\n");
    uiResetSrc = RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_WDT;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");
    UtAssert_True(retCode == RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_WDT, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure. Correct Reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, "_ProcessResetType() - 2/8:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure. Correct reset subtype");

    /* ----- Test case #3 - Nominal Internal FPGA Watchdog timer application SW failure. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_FWDT;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");
    UtAssert_True(retCode == RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_FWDT, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, "_ProcessResetType() - 3/8:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure. Correct reset subtype");

    /* ----- Test case #4 - Nominal cPCI Reset initiated by FPGA from remote SBC. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_CPCI;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");
    UtAssert_True(retCode == RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_CPCI, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_ProcessResetType() - 4/8: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC. Correct reset subtype");

    /* ----- Test case #5 - Nominal Software Hard Reset ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_POR | RESET_SRC_SWR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: Software Hard Reset.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset");
    UtAssert_True(retCode == RESET_SRC_POR | RESET_SRC_SWR, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_ProcessResetType() - 5/8: Nominal - POWERON Reset: Software Hard Reset. Correct reset subtype");

    /* ----- Test case #6 - Failed to read safemode data ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_POR | RESET_SRC_SWR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDeferredRetcode(UT_KEY(ReadSafeModeUserData), 1, OS_ERROR);
    sprintf(cMsg, "PSP: PROCESSOR Reset: failed to read safemode data.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 6/8: Failed to read safemode data");
    UtAssert_True(retCode == RESET_SRC_POR | RESET_SRC_SWR, "_ProcessResetType() - 6/8: POWERON Reset: Software Hard Reset");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 6/8: POWERON 8Reset: Software Hard Reset. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_ProcessResetType() - 6/8: POWERON Reset: Software Hard Reset. Correct reset subtype");

    /* ----- Test case #7 - POWERON Resest: UNKNOWN Reset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Some other, not listed reset code */
    uiResetSrc = 0x0;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: UNKNOWN Reset.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 7/8: Unknown reset type");
    UtAssert_True(retCode == 0x0, "_ProcessResetType() - 7/8: Unknown reset type");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 7/8: Unknown reset type. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET, "_ProcessResetType() - 7/8: Unknown reset type. Correct reset subtype");

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
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_ProcessResetType() - 8/9: UNKNOWN Reset. Reset source read failed. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET, "_ProcessResetType() - 8/9: UNKNOWN Reset. Reset source read failed. Correct reset subtype");
}

/*=======================================================================================
** Ut_CFE_PSP_LogSoftwareResetType() test cases
**=======================================================================================*/
void Ut_CFE_PSP_LogSoftwareResetType(void)
{
    char cMsg[256] = {'\0'};
    char cMsg_mkcause[256] = {'\0'};
    RESET_SRC_REG_ENUM resetSrc = RESET_SRC_POR;

    sprintf(cMsg_mkcause, "PSP L1 instruction cache error\n");

    /* ----- Test case #1 - Nominal POR. No check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    g_StartupInfo.safeModeUserData.safeMode = 0;
    g_StartupInfo.safeModeUserData.sbc = SM_LOCAL_SBC;
    g_StartupInfo.safeModeUserData.mckCause = 1;
    g_StartupInfo.safeModeUserData.reason = 0;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "LOCAL",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 1/7:Nominal - POR");
    UtAssert_NoOS_print("PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                        "_CFE_PSP_LogSoftwareResetType - 1/7:Nominal - No check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #2 - Nominal POR SWR WDT. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_WDT;
    g_StartupInfo.safeModeUserData.sbc = SM_REMOTE_SBC;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (POR SWR WDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 2/7:Nominal - POR SWR WDT");
    UtAssert_OS_print(cMsg_mkcause,
                      "_CFE_PSP_LogSoftwareResetType - 2/7:Nominal - Check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #3 - Nominal POR SWR FWDT. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_FWDT;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (POR SWR FWDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 3/7:Nominal - POR SWR FWDT");
    UtAssert_OS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 3/7:Nominal - Check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #4 - Nominal POR SWR CPCI. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_CPCI;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (POR SWR CPCI) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 4/7:Nominal - POR SWR CPCI");
    UtAssert_OS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 6/7:Nominal - Check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #5 - Nominal POR SWR. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_POR | RESET_SRC_SWR;
    g_StartupInfo.safeModeUserData.mckCause = 1U;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (POR SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 5/7:Nominal - POR SWR");
    UtAssert_OS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 5/7:Nominal - Check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #6 - Nominal default to POR. No check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_POR;
    g_StartupInfo.safeModeUserData.mckCause = 0U;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 6/7:Nominal - Default POR");
    UtAssert_NoOS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 6/7:Nominal - No check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #7 - g_StartupInfo.safeModeUserData.sbc != SM_LOCAL_SBC ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = 0x00;
    g_StartupInfo.safeModeUserData.mckCause = 1U;
    g_StartupInfo.safeModeUserData.sbc = SM_REMOTE_SBC;
    sprintf(cMsg, 
            "PSP: PROCESSOR rst Source = 0x%x = (Unknown) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 7/7:Nominal - Unknown reset type");
}

/*=======================================================================================
** Ut_CFE_PSP_StartupFailed() test cases
**=======================================================================================*/
void Ut_CFE_PSP_StartupFailed(void)
{
    char cMsg[256] = {'\0'};
    char cMsg_notice[] = PSP_STARTUP_TIMER_PRINT_SCOPE "Startup Timer Expired!!\nRestarting\n";
    char cMsg_error1[] = "PSP: Error, Timer ID does not match `CFE_PSP_StartupFailedRestartSP0_hook`\n";
    CFE_PSP_Startup_structure_t buffer = {};
    uint8_t buf[10] = {'\0'};

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    g_StartupInfo.timerID = 0;
    g_StartupInfo.startup_failed_attempts = 0;
    g_StartupInfo.startup_failed_reset_attempts = 0;
    sprintf(g_StartupInfo.fullpath_failed_startup_filename, "/ffx0/fail.txt");
    g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_POWERON;

    /* Setup for the CFE_PSP_Restart */
    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    /* ----- Test case #1 - Wrong TimerID ----- */
    /* Set additional inputs */

    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(1);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_error1, "_CFE_PSP_StartupFailed - 1/7: Wrong TimerID");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Error, could not open file ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not open `%s`\n", g_StartupInfo.fullpath_failed_startup_filename);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(0);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 2/7: Startup timer expired message");
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 2/7: Error, could not open file");
    sprintf(cMsg, "PSP Restart called with %d\n", CFE_PSP_RST_TYPE_CFS_TOGGLE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 2/7: Restart Message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - open, read 0 bytes, lseek error, close error, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), 0);
    UT_SetDefaultReturnValue(UT_KEY(lseek), -1);
    UT_SetDefaultReturnValue(UT_KEY(close), -1);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(0);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 3/7: Startup timer expired message");
    UtAssert_True(g_StartupInfo.startup_failed_attempts = 1, "_CFE_PSP_StartupFailed - 3/7: Increase number of failed attempts");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not set the write pointer for `%s`\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 3/7: lseek error message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not close file `%s`\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 3/7: close error message");
    sprintf(cMsg, "PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 3/7: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - open, read -1, lseek, write, close, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buf));
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(0);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 4/7: Startup timer expired message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not read structure from `%s`, deleting file.\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 4/7: read error message");
    sprintf(cMsg, "PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 4/7: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #5 - open, read -1, lseek, write error, close, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buf));
    UT_SetDataBuffer(UT_KEY(read), &buf, sizeof(buf), false);
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), -1);
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(0);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 5/7: Startup timer expired message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not read structure from `%s`, deleting file.\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 5/7: lseek error message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not write to `%s`, deleting file.\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 5/7: write error message");
    sprintf(cMsg, "PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 5/7: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #6 - open, read, lseek, write, close, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buffer));
    buffer.startup_failed_attempts = 0;
    UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false);
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(0);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 6/7: Startup timer expired message");
    UtAssert_True(g_StartupInfo.startup_failed_attempts == 1, "_CFE_PSP_StartupFailed - 6/7: Increase number of failed attempts");
    UtAssert_True(g_StartupInfo.startup_failed_reset_attempts == 0, "_CFE_PSP_StartupFailed - 6/7: Reset attempts not changed");
    sprintf(cMsg, "PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 6/7: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #7 - open, read, lseek, write, close, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buffer));
    buffer.startup_failed_attempts = CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS - 1;
    UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false);
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(0);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 7/7: Startup timer expired message");
    UtAssert_True(g_StartupInfo.startup_failed_attempts == 0, "_CFE_PSP_StartupFailed - 7/7: Increase number of failed attempts");
    UtAssert_True(g_StartupInfo.startup_failed_reset_attempts == 1, "_CFE_PSP_StartupFailed - 7/7: Reset attempts not changed");
    sprintf(cMsg, "PSP DEBUG: iBytes_read = %d --> POWERON\n", sizeof(buffer));
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 7/7: PSP DEBUG message");    
    sprintf(cMsg, "PSP Restart called with %d\n", CFE_PSP_RST_TYPE_POWERON);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 7/7: Restart Message");
}

/*=======================================================================================
** Ut_CFE_PSP_StartupClear() test cases
**=======================================================================================*/
void Ut_CFE_PSP_StartupClear(void)
{
    char cMsg[256] = {'\0'};
    char cMsg_notice[] = PSP_STARTUP_TIMER_PRINT_SCOPE "Deactivating Startup Timer\n";
    int32 iRetCode = CFE_PSP_ERROR;
    uint32 clock_accuracy = 0;
    char timer_name[] = "TEST";

    g_StartupInfo.timerID = 0;
    g_StartupInfo.uMaxWaitTime_sec = 10;
    sprintf(g_StartupInfo.fullpath_failed_startup_filename,"/ffx0/fail2.txt");

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    OS_TimerCreate(&g_StartupInfo.timerID, timer_name, &clock_accuracy, &UT_ResetState);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerDelete), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysDisableFpgaWdt), OK);
    /* Execute test */
    iRetCode = CFE_PSP_StartupClear();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_StartupClear - 1/3: Nominal return code");
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupClear - 1/3: Notice Message");
    sprintf(cMsg, "PSP Watchdog: Successfully disabled watchdog\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupClear - 1/3: Watchdog Disabled Message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_StartupInfo.uMaxWaitTime_sec = 60;

    /* ----- Test case #2 - Timer error, remove error, EBADF error ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerDelete), ERROR);
    UT_SetDefaultReturnValue(UT_KEY(remove), ERROR);
    errno = EBADF;
    /* Execute test */
    iRetCode = CFE_PSP_StartupClear();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_StartupClear - 2/3: Error return code");
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupClear - 2/3: Notice Message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not delete timer [%d] errno [%d]\n", ERROR, EBADF);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupClear - 2/3: Timer delay error message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Timer error, remove error, ENOENT error ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerDelete), ERROR);
    UT_SetDefaultReturnValue(UT_KEY(remove), ERROR);
    errno = ENOENT;
    /* Execute test */
    iRetCode = CFE_PSP_StartupClear();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_StartupClear - 3/3: Error return code");
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupClear - 3/3: Notice Message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not delete timer [%d] errno [%d]\n", ERROR, ENOENT);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupClear - 3/3: Timer delay error message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not delete failed startup file at `%s` errno [%d]\n", g_StartupInfo.fullpath_failed_startup_filename, ENOENT);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupClear - 3/3: remove error message");
}

/*=======================================================================================
** Ut_CFE_PSP_GetActiveCFSPartition() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetActiveCFSPartition(void)
{
    char cMsg[256] = {'\0'};
    char buffer[40] = {'\0'};
    char filepath[40] = "/ffx0/fail.txt";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_strncpy), 0);
    /* Execute test */
    CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_GetActiveCFSPartition - 1/3: Nominal no Messages");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Bad memory range ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_ERROR);
    /* Execute test */
    CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    sprintf(cMsg, "PSP: Error, address does not pass validation. Code [%d]\n", CFE_PSP_ERROR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetActiveCFSPartition - 2/3: Bad memory range Message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Bad memory range ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    /* Execute test */
    CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    sprintf(cMsg, "PSP: Warning, kernel does not support reading the currently active CFS flash partition\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetActiveCFSPartition - 3/3: Using fallback filepath Message");
}

/*=======================================================================================
** Ut_CFE_PSP_StartupTimer() test cases
**=======================================================================================*/
void Ut_CFE_PSP_StartupTimer(void)
{
    char cMsg[256] = {'\0'};
    char cMsg_watchdog[] = "PSP Watchdog: Watchdog successfully enabled\n";
    int32 iReturn_code = CFE_PSP_SUCCESS;

    /* Reset structure */
    memset(&g_StartupInfo, '\0', sizeof(g_StartupInfo));

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerCreate), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OS_SUCCESS);
    /* Execute test */
    iReturn_code = CFE_PSP_StartupTimer();
    /* Verify outputs */
    UtAssert_True(iReturn_code == CFE_PSP_SUCCESS, "_CFE_PSP_StartupTimer - 1/3: Return success");
    UtAssert_OS_print(cMsg_watchdog, "_CFE_PSP_StartupTimer - 1/3: Started watchdog Messages");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Error creating Timer ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerCreate), OS_ERROR);
    /* UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS); */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OS_SUCCESS);
    /* Execute test */
    iReturn_code = CFE_PSP_StartupTimer();
    /* Verify outputs */
    UtAssert_True(iReturn_code == CFE_PSP_ERROR, "_CFE_PSP_StartupTimer - 2/3: Error creating Timer return code");
    snprintf(cMsg, sizeof(cMsg), PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not start the Startup Timer [%d]\n",OS_ERROR);
    UtAssert_OS_print(cMsg_watchdog, "_CFE_PSP_StartupTimer - 2/3: Error creating Timer Message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Error setting the Timer ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerCreate), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OS_SUCCESS);
    /* Execute test */
    iReturn_code = CFE_PSP_StartupTimer();
    /* Verify outputs */
    UtAssert_True(iReturn_code == CFE_PSP_ERROR, "_CFE_PSP_StartupTimer - 3/3: Error creating Timer return code");
    snprintf(cMsg, sizeof(cMsg), PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not set the Startup Timer [%d]\n",OS_ERROR);
    UtAssert_OS_print(cMsg_watchdog, "_CFE_PSP_StartupTimer - 3/3: Error creating Timer Message");
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
    char cMsg_notice[] = "PSP: PSP Application Startup Complete\n";
    uint32 uiResetSrc = 0;
    uint64 bitResult   = 1ULL;
    uint64 bitExecuted = 3ULL;
    USER_SAFE_MODE_DATA_STRUCT smud;

    Ut_OS_printf_Setup();

    /* For CFE_PSP_ProcessResetType */
    uiResetSrc = RESET_SRC_POR;
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);

    /* For CFE_PSP_InitProcessorReservedMemory */
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), 4000);

    UT_SetDefaultReturnValue(UT_KEY(open), 99);
    UT_SetDefaultReturnValue(UT_KEY(write), 10);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);

    UT_SetDefaultReturnValue(UT_KEY(userNvRamSet), 0);

    UT_SetDefaultReturnValue(UT_KEY(userMemAlloc), OK);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);

    /* For CFE_PSP_GetActiveCFSPartition */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);

    /* For CFE_PSP_GetBootStartupString */
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), ERROR);

    /* For CFE_PSP_StartupTimer - return CFE_PSP_ERROR */
    UT_SetDefaultReturnValue(UT_KEY(PCS_strncpy), 0);
    /* UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), 0); */
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerCreate), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_ERROR);
    /* UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OS_SUCCESS); */
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OS_SUCCESS);

    /* For CFE_PSP_StartupFailedRestartSP0_hook */
    UT_SetDefaultReturnValue(UT_KEY(read), 0);
    UT_SetDefaultReturnValue(UT_KEY(lseek), -1);
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);

    /* PSP_SP0_GetInfo SPE Functions - We are setting them once for all cases */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
/*     UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true); */
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

    /* For CFE_PSP_WatchdogInit */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OK);

    /* For CFE_PSP_MEM_SCRUB_Init */
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemCreate), OS_ERROR);

    /* For CFE_PSP_LogSoftwareResetType */
    g_StartupInfo.safeModeUserData.sbc == SM_LOCAL_SBC;

    /* For CFE_PSP_SetSysTasksPrio */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_SUCCESS);

    /* For CFE_PSP_StartupClear */
    g_StartupInfo.uMaxWaitTime_sec = 10;
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerDelete), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 1/3: Successful startup notice message");
    sprintf(cMsg, "PSP: OS_Application_Startup() - CFE_PSP_InitProcessorReservedMemory() failed (0x0)\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 1/3: Initialization of User Reserved Memory failed message");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Initialize the OS API data structures failed ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    sprintf(cMsg, "PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 2/3: OS_API_Init error message");
    /* Cannot check for No OS_printf because the function keeps running after reboot is called */
    /* UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 2/3: Successful startup notice message missing"); */

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Setup FS mapping \"/cf\" directorys failed ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    sprintf(cMsg, "PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/3: CFE_PSP_SetFileSysAddFixedMap error Message");
    sprintf(cMsg, "PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/3: SetSysTasksPrio error Message");
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 3/3: Successful startup notice message");
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
    UtAssert_True(uiRetCode == g_StartupInfo.ResetType, "_GetRestartType() - 1/2:Nominal");
    
    /* ----- Test case #2 - Nominal pResetSubType was set value ----- */
    /* Setup additional inputs */
    pResetSubType = &uiResetSubType;
    /* Execute test */
    uiRetCode = CFE_PSP_GetRestartType(pResetSubType);
    /* Verify outputs */
    UtAssert_True(*pResetSubType == g_StartupInfo.ResetSubtype, "_GetRestartType() - 2/2:pResetSubType was set value");
    UtAssert_True(uiRetCode == g_StartupInfo.ResetType, "_GetRestartType() - 2/2:Nominal");
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
** Ut_CFE_PSP_SetFileSysAddFixedMap() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetFileSysAddFixedMap(void)
{
    osal_id_t   fid;
    int32       ret = CFE_PSP_SUCCESS;
    char        cMsg[256] = {};

    sprintf(cMsg, "PSP: Set 1 Virtual Path(s)\n");

    /* ----- Test #1 - Nominal ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_FileSysAddFixedMap), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SetFileSysAddFixedMap(&fid);
    /* Verify results */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_CFE_PSP_SetFileSysAddFixedMap - 1/1: Virtual Mapping Returned Success");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetFileSysAddFixedMap - 1/1: Printed Nominal String");
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
