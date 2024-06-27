/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file psp_start_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_start.c.
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
#include <ioLib.h>

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "target_config.h"
#include "cfe_psp.h"
#include "cfe_psp_start.h"

#include "psp_start_testcases.h"
#include "../src/cfe_psp_start.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern int volSensorRead(int8 sensor, uint8 dataType, float *voltage, bool talkative );
extern int tempSensorRead (int8 sensor, uint8 dataType, float *temperature, bool talkative );
extern char UserReservedMemory[URM_SIZE];
extern char *pEndOfURM;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_Main() test cases
**=======================================================================================*/
void Ut_CFE_PSP_Main(void)
{
    /* ----- Test case #1: - Previous instance of CFS found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), 10);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 0);
    UtAssert_NA("_CFE_PSP_Main - 1/3: Found previous instance of CFS");

    /* ----- Test case #2: - OS_BSPMain returns error ----- */
    UT_ResetState(0);
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), TASK_ID_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_ERROR);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_NA("_CFE_PSP_Main - 2/3: OS_BSPMain returned error");

    /* ----- Test case #3: - Nominal ----- */
    UT_ResetState(0);
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), TASK_ID_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_SUCCESS);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_NA("_CFE_PSP_Main - 3/3: Nominal, OS_BSPMain success");
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
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: Power Switch ON.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg,  "_CFE_PSP_ProcessResetType() - 1/9: Nominal - POWERON Reset: Power Switch ON");
    UtAssert_True(retCode == RESET_SRC_POR, "_CFE_PSP_ProcessResetType() - 1/9: Nominal - POWERON Reset: Power Switch ON");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 1/9: Nominal - POWERON Reset: Power Switch ON, correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE, "_CFE_PSP_ProcessResetType() - 1/9: Nominal - POWERON Reset: Power Switch ON, correct reset subtype");

    /* ----- Test case #2 - Nominal Power Switch ON ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_POR;
    UT_SetDefaultReturnValue(UT_KEY(open), 10);
    UT_SetDefaultReturnValue(UT_KEY(close), OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: Power Switch ON.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg,  "_CFE_PSP_ProcessResetType() - 2/9: Nominal - POWERON Reset: Power Switch ON");
    UtAssert_True(retCode == RESET_SRC_POR, "_CFE_PSP_ProcessResetType() - 2/9: Nominal - POWERON Reset: Power Switch ON");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_PROCESSOR, "_CFE_PSP_ProcessResetType() - 2/9: Nominal - POWERON Reset: Power Switch ON, correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_POWER_CYCLE, "_CFE_PSP_ProcessResetType() - 2/9: Nominal - POWERON Reset: Power Switch ON, correct reset subtype");

    /* ----- Test case #3 - Nominal External FPGA Watchdog timer primary EEPROM boot failure. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP: PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.\n");
    uiResetSrc = RESET_SRC_WDT;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 3/9: Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");
    UtAssert_True(retCode == RESET_SRC_WDT, "_CFE_PSP_ProcessResetType() - 3/9: Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 3/9: Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure. Correct Reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, "_CFE_PSP_ProcessResetType() - 3/9: Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure. Correct reset subtype");

    /* ----- Test case #4 - Nominal Internal FPGA Watchdog timer application SW failure. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_FWDT;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 4/9: Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");
    UtAssert_True(retCode == RESET_SRC_FWDT, "_CFE_PSP_ProcessResetType() - 4/9: Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 4/9: Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_HW_WATCHDOG, "_CFE_PSP_ProcessResetType() - 4/9: Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure. Correct reset subtype");

    /* ----- Test case #5 - Nominal cPCI Reset initiated by FPGA from remote SBC. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_CPCI;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 5/9: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");
    UtAssert_True(retCode == RESET_SRC_CPCI, "_CFE_PSP_ProcessResetType() - 5/9: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 5/9: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, "_CFE_PSP_ProcessResetType() - 5/9: Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC. Correct reset subtype");

    /* ----- Test case #6 - Nominal Software Hard Reset ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_SWR;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: PROCESSOR Reset: Software Hard Reset.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 6/9: Nominal - PROCESSOR Reset: Software Hard Reset");
    UtAssert_True(retCode == RESET_SRC_SWR, "_CFE_PSP_ProcessResetType() - 6/9: Nominal - PROCESSOR Reset: Software Hard Reset");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 6/9: Nominal - PROCESSOR Reset: Software Hard Reset. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_CFE_PSP_ProcessResetType() - 6/9: Nominal - PROCESSOR Reset: Software Hard Reset. Correct reset subtype");

    /* ----- Test case #7 - Failed to read safemode data ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_SWR;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDeferredRetcode(UT_KEY(ReadSafeModeUserData), 1, OS_ERROR);
    sprintf(cMsg, "PSP: PROCESSOR Reset: failed to read safemode data.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 7/9: Failed to read safemode data");
    UtAssert_True(retCode == RESET_SRC_SWR, "_CFE_PSP_ProcessResetType() - 7/9: POWERON Reset: Software Hard Reset");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 7/9: POWERON 8Reset: Software Hard Reset. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_RESET_COMMAND, "_CFE_PSP_ProcessResetType() - 7/9: POWERON Reset: Software Hard Reset. Correct reset subtype");

    /* ----- Test case #8 - POWERON Resest: UNKNOWN Reset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Some other, not listed reset code */
    uiResetSrc = 0x0;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: UNKNOWN Reset.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 8/9: Unknown reset type");
    UtAssert_True(retCode == 0x0, "_CFE_PSP_ProcessResetType() - 8/9: Unknown reset type");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 8/9: Unknown reset type. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET, "_CFE_PSP_ProcessResetType() - 8/9: Unknown reset type. Correct reset subtype");

    /* ----- Test case #9 - UNKNOWN Reset. Reset source read failed. ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = 0;
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    sprintf(cMsg, "PSP: POWERON Reset: UNKNOWN Reset. Reset source read failed.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ProcessResetType() - 9/9: UNKNOWN Reset. Reset source read failed.");
    UtAssert_True(retCode == 0, "_CFE_PSP_ProcessResetType() - 9/9: UNKNOWN Reset. Reset source read failed.");
    UtAssert_True(g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_ProcessResetType() - 9/9: UNKNOWN Reset. Reset source read failed. Correct reset type");
    UtAssert_True(g_StartupInfo.ResetSubtype == CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET, "_CFE_PSP_ProcessResetType() - 9/9: UNKNOWN Reset. Reset source read failed. Correct reset subtype");
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
            "PSP: Reset Source = 0x%x = (POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
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

    /* ----- Test case #2 - Nominal WDT. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_WDT;
    g_StartupInfo.safeModeUserData.sbc = SM_REMOTE_SBC;
    sprintf(cMsg, 
            "PSP: Reset Source = 0x%x = (WDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 2/7:Nominal - WDT");
    UtAssert_NoOS_print(cMsg_mkcause,
                      "_CFE_PSP_LogSoftwareResetType - 2/7:Nominal - No check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #3 - Nominal FWDT. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_FWDT;
    sprintf(cMsg, 
            "PSP: Reset Source = 0x%x = (FWDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 3/7:Nominal - FWDT");
    UtAssert_NoOS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 3/7:Nominal - No check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #4 - Nominal CPCI. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_CPCI;
    sprintf(cMsg, 
            "PSP: Reset Source = 0x%x = (CPCI) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 4/7:Nominal - CPCI");
    UtAssert_NoOS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 4/7:Nominal - No check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #5 - Nominal SWR. Check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_SWR;
    g_StartupInfo.safeModeUserData.mckCause = 1U;
    sprintf(cMsg, 
            "PSP: Reset Source = 0x%x = (SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            g_StartupInfo.safeModeUserData.safeMode,
            "REMOTE",
            g_StartupInfo.safeModeUserData.reason,
            g_StartupInfo.safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_LogSoftwareResetType - 5/7:Nominal - SWR");
    UtAssert_OS_print(cMsg_mkcause, "_CFE_PSP_LogSoftwareResetType - 5/7:Nominal - Check for g_StartupInfo.safeModeUserData.mckCause");

    /* ----- Test case #6 - Nominal default to POR. No check for g_StartupInfo.safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    resetSrc = RESET_SRC_POR;
    g_StartupInfo.safeModeUserData.mckCause = 0U;
    sprintf(cMsg, 
            "PSP: Reset Source = 0x%x = (POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
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
            "PSP: Reset Source = 0x%x = (Unknown) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
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
    osal_id_t tmpID;

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    tmpID = OS_ObjectIdFromInteger(1);
    g_StartupInfo.timerID = OS_ObjectIdFromInteger(99);
    g_StartupInfo.startup_failed_attempts = 0;
    g_StartupInfo.startup_failed_reset_attempts = 0;
    sprintf(g_StartupInfo.fullpath_failed_startup_filename, "/ffx0/fail.txt");
    g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_PROCESSOR;

    /* Setup for the CFE_PSP_Restart */
    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    /* ----- Test case #1 - Wrong TimerID ----- */
    /* Set additional inputs */

    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_error1, "_CFE_PSP_StartupFailed - 1/9: Wrong TimerID");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Error, could not open file ----- */
    /* Set additional inputs */
    tmpID = OS_ObjectIdFromInteger(99);
    UT_SetDefaultReturnValue(UT_KEY(open), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not open `%s`\n", g_StartupInfo.fullpath_failed_startup_filename);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 2/9: Startup timer expired message");
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 2/9: Error, could not open file");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_CFS_TOGGLE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 2/9: Restart Message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - open, read 0 bytes, lseek error, close error, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), 0);
    UT_SetDefaultReturnValue(UT_KEY(lseek), -1);
    UT_SetDefaultReturnValue(UT_KEY(close), -1);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 3/9: Startup timer expired message");
    UtAssert_True((g_StartupInfo.startup_failed_attempts == 1), "_CFE_PSP_StartupFailed - 3/9: Increase number of failed attempts");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not set the write pointer for `%s`\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 3/9: lseek error message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not close file `%s`\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 3/9: close error message");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 3/9: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - open, read -1, lseek, write, close, remove ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buf));
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 4/9: Startup timer expired message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not read structure from `%s`, deleting file.\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 4/9: read error message");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 4/9: Restart Message");

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
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 5/9: Startup timer expired message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not read structure from `%s`, deleting file.\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 5/9: lseek error message");
    sprintf(cMsg, PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not write to `%s`, deleting file.\n", g_StartupInfo.fullpath_failed_startup_filename);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 5/9: write error message");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 5/9: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #6 - open, read, lseek, write, close, remove, processor reset----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buffer));
    buffer.startup_failed_attempts = 0;
    UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false);
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 6/9: Startup timer expired message");
    UtAssert_True((g_StartupInfo.startup_failed_attempts == 1), "_CFE_PSP_StartupFailed - 6/9: Increase number of failed attempts");
    UtAssert_True((g_StartupInfo.startup_failed_reset_attempts == 0), "_CFE_PSP_StartupFailed - 6/9: Reset attempts not changed");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 6/9: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #7 - open, read, lseek, write, close, remove, poweron reset ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buffer));
    buffer.startup_failed_attempts = CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS - 1;
    g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false);
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 7/9: Startup timer expired message");
    UtAssert_True((g_StartupInfo.startup_failed_attempts == 0), "_CFE_PSP_StartupFailed - 7/9: Increase number of failed attempts");
    UtAssert_True((g_StartupInfo.startup_failed_reset_attempts == 1), "_CFE_PSP_StartupFailed - 7/9: Reset attempts not changed");
    sprintf(cMsg, "PSP Startup: Too many startup failures. Attempt next restart with CFE_PSP_RST_TYPE_POWERON.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 7/9: PSP DEBUG message");    
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_POWERON);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 7/9: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #8 - open, read, lseek, write, close, remove, toggle cfs reset ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buffer));
    buffer.startup_failed_attempts = 0;
    UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false);
    g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_POWERON;
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 8/9: Startup timer expired message");
    UtAssert_True((g_StartupInfo.startup_failed_attempts == 1), "_CFE_PSP_StartupFailed - 8/9: Increase number of failed attempts");
    UtAssert_True((g_StartupInfo.startup_failed_reset_attempts == 0), "_CFE_PSP_StartupFailed - 8/9: Reset attempts not changed");  
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_CFS_TOGGLE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 8/9: Restart Message");

    Ut_OS_printf_Setup();

    /* ----- Test case #9 - Control flow works as intended - processor, processor, poweron, toggle  ----- */
    /* Variable declarations*/
    char cBootString[] = "/ffx1/startup";
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(open), 1);
    UT_SetDefaultReturnValue(UT_KEY(read), sizeof(buffer));
    buffer.startup_failed_attempts = 0;
    g_StartupInfo.startup_failed_reset_attempts = 0;
    UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false); 
    g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    UT_SetDefaultReturnValue(UT_KEY(lseek), 0);
    UT_SetDefaultReturnValue(UT_KEY(write), sizeof(g_StartupInfo));
    UT_SetDefaultReturnValue(UT_KEY(close), 0);
    UT_SetDefaultReturnValue(UT_KEY(remove), 0);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_bootStructToString), OK);
    UT_SetDataBuffer(UT_KEY(PCS_bootStructToString), cBootString, sizeof(cBootString), false);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    for (int i = 0; i < CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS + 1; i++)
    {
        UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
        CFE_PSP_StartupFailedRestartSP0_hook(tmpID);
        buffer.startup_failed_attempts++;
        if (i == CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS - 1)
        {
            buffer.startup_failed_attempts = 0;
            buffer.startup_failed_reset_attempts = 1;
            g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_POWERON;
        }
        UT_SetDataBuffer(UT_KEY(read), &buffer, sizeof(buffer), false);
    }
    
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_CFE_PSP_StartupFailed - 9/9: Startup timer expired message");
    UtAssert_True((g_StartupInfo.startup_failed_attempts == 1), "_CFE_PSP_StartupFailed - 9/9: Number failed attempts correct");
    UtAssert_True((g_StartupInfo.startup_failed_reset_attempts == 1), "_CFE_PSP_StartupFailed - 9/9: Reset attempts changed");  
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: Restart Message - Processor 1");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: Restart Message - Processor 2");
    sprintf(cMsg, "PSP Startup: Too many startup failures. Attempt next restart with CFE_PSP_RST_TYPE_POWERON.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: PSP DEBUG: Poweron Message");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_POWERON);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: Restart Message - Poweron 3");
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_CFS_TOGGLE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: Restart Message - Toggle 4");
    sprintf(cMsg, "PSP: New Boot String:\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: cFS Partition Sucessfully Toggled 1/2");
    sprintf(cMsg, "`%s`\n", cBootString);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartupFailed - 9/9: cFS Partition Sucessfully Toggled 2/2");
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

    g_StartupInfo.uMaxWaitTime_sec = 10;
    /* Set watchdog static g_uiCFE_PSP_WatchdogValue_ms */
    CFE_PSP_WatchdogSet(20000); 
    sprintf(g_StartupInfo.fullpath_failed_startup_filename,"/ffx0/fail2.txt");

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    OS_TimerCreate(&g_StartupInfo.timerID, timer_name, &clock_accuracy, NULL);
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
    int32 iRetCode = CFE_PSP_SUCCESS;
    char buffer[40] = {'\0'};
    char PartitionName[CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH] = "/ffx1";
    char longPartitionName[CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH + 1] = "/ffx11";
    cpuaddr pPartitionName = (cpuaddr)&PartitionName;
    cpuaddr plongPartitionName = (cpuaddr)&longPartitionName; 
    cpuaddr pbuffer = (cpuaddr)&buffer;

    /* ----- Test case #1 - Nominal kernel support ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &pPartitionName, sizeof(pPartitionName), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH-1);
    /* Execute test */
    iRetCode = CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    UtAssert_True(memcmp(buffer, "/ffx1\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 1/7: Output buffer nominal with kernel support");
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_GetActiveCFSPartition - 1/7: Nominal ret code");

    UT_ResetState(0);

    /* ----- Test case #2 - Nominal kernel does not support ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH-1);
    /* Execute test */
    iRetCode = CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    UtAssert_True(memcmp(buffer, "/ffx0\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 2/7: Output buffer nominal without kernel support");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GetActiveCFSPartition - 2/7: Nominal no Kernel support return code");

    UT_ResetState(0);

    /* ----- Test case #3 - NULL buffer string ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    /* Execute test */
    iRetCode = CFE_PSP_GetActiveCFSPartition(NULL, sizeof(buffer));
    /* Verify outputs */
    UtAssert_True(memcmp(buffer, "\0\0\0\0\0\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 3/7: NULL buffer pointer - Buffer untouched");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GetActiveCFSPartition - 3/7: NULL buffer pointer - Return Error");

    /* ----- Test case #4 - buffer length < then CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    /* Execute test */
    iRetCode = CFE_PSP_GetActiveCFSPartition(buffer, CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH-1);
    /* Verify outputs */
    UtAssert_True(memcmp(buffer, "\0\0\0\0\0\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 4/7: Buffer size too small - Buffer untouched");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GetActiveCFSPartition - 4/7: Buffer size too small - Return Error");

    /* ----- Test case #5 - NULL buffer string ----- */
    /* Set additional inputs */
    // memset(buffer, '0', sizeof(buffer));
    // UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    // UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &pbuffer, sizeof(pbuffer), false);
    // UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH);
    // /* Execute test */
    // iRetCode = CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    // /* Verify outputs */
    // UtAssert_True(memcmp(buffer, "/ffx0\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 5/7: snprintf failed");
    // UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GetActiveCFSPartition - 5/7: snprintf failed - Return Error");

    UT_ResetState(0);

    /* ----- Test case #6 - Partition name is too long ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &plongPartitionName, sizeof(plongPartitionName), false);
    /* Execute test */
    iRetCode = CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    UtAssert_True(memcmp(buffer, "/ffx0\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 6/7: Output buffer nominal without kernel support");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GetActiveCFSPartition - 6/7: Partition name too large");

    UT_ResetState(0);

    /* ----- Test case #7 - Snprintf error ----- */
    /* Set additional inputs */
    memset(buffer, '\0', sizeof(buffer));
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &pPartitionName, sizeof(pPartitionName), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), sizeof(buffer)+1);
    /* Execute test */
    iRetCode = CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    /* Verify outputs */
    UtAssert_True(memcmp(buffer, "/ffx1\0", 6) == 0, "_CFE_PSP_GetActiveCFSPartition - 7/7: Snprintf failure");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GetActiveCFSPartition - 7/7: Snprintf failure");

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
    char cMsg_notice[] = "PSP: Application Startup Complete\n";
    char cMsg_mem_init_failed[] = "PSP: CFS is rebooting due to not having enough User Reserved Memory\n";
    uint32_t uiResetSrc = 0;
    uint64_t bitResult   = 1ULL;
    uint64_t bitExecuted = 3ULL;

    Ut_OS_printf_Setup();

    /* For CFE_PSP_GetActiveCFSPartition */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);

    /* For CFE_PSP_GetBootStartupString */
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), ERROR);

    /* For CFE_PSP_StartupFailedRestartSP0_hook */
    UT_SetDefaultReturnValue(UT_KEY(read), 0);
    UT_SetDefaultReturnValue(UT_KEY(lseek), -1);
    UT_SetDefaultReturnValue(UT_KEY(remove), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);

    /* For CFE_PSP_StartupTimer - return CFE_PSP_ERROR */
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerCreate), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OS_SUCCESS);
    /* For CFE_PSP_WatchdogInit */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OK);

    /*
    CFE_PSP_SP0CollectStaticInfo and CFE_PSP_SP0CollectDynamicInfo
    We are setting them once for all cases
    */
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0);
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITExecuted), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false);
    UT_SetDefaultReturnValue(UT_KEY(aimonGetBITResults), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false);
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1);
    UT_SetDefaultReturnValue(UT_KEY(sysGetBoardGeneration), 2);
    UT_SetDefaultReturnValue(UT_KEY(GetUsecTime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    

    /* For CFE_PSP_ProcessResetType */
    uiResetSrc = RESET_SRC_POR;
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);

    /* For CFE_PSP_InitProcessorReservedMemory */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(open), 99);
    UT_SetDefaultReturnValue(UT_KEY(write), 10);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    UT_SetDefaultReturnValue(UT_KEY(userMemAlloc), OK);
    
    /* For CFE_PSP_SetupReservedMemoryMap */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);

    /* For CFE_PSP_MemScrubInit */
    /* For CFE_PSP_MemSyncInit */
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemCreate), OS_ERROR);

    /* For CFE_PSP_SetSysTasksPrio */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(taskPriorityGet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(taskPrioritySet), OS_SUCCESS);

    /* For CFE_PSP_LogSoftwareResetType */
    g_StartupInfo.safeModeUserData.safeMode = 0;
    g_StartupInfo.safeModeUserData.sbc = SM_LOCAL_SBC;
    g_StartupInfo.safeModeUserData.mckCause = 1;
    g_StartupInfo.safeModeUserData.reason = 0;

    /* For CFE_PSP_StartupClear */
    g_StartupInfo.uMaxWaitTime_sec = 10;
    sprintf(g_StartupInfo.fullpath_failed_startup_filename,"/ffx0/fail2.txt");
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerDelete), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysDisableFpgaWdt), OK);

    /* For CFE_PSP_Reset */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 1/6: Successful startup notice message");
    UtAssert_STUB_COUNT(userReservedGet, 2);

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Failed to initialize URM ----- */
    /* Setup additional inputs */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), 100);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 2/6: Missing startup notice message");
    UtAssert_OS_print(cMsg_mem_init_failed, "_OS_Application_Startup - 2/6: Initialization of User Reserved Memory failed message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Initialize the OS API data structures failed ----- */
    /* Setup additional inputs */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    sprintf(cMsg, "PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/6: OS_API_Init error message");
    UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 3/6: Successful startup notice message missing");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Fail to add FS mapping folder and fail to set priorities ----- */
    /* Setup additional inputs */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    sprintf(cMsg, "PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/6: CFE_PSP_SetFileSysAddFixedMap error Message");
    sprintf(cMsg, "PSP: Some or all Virtual FS Mapping has failed\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/6: OS_Application_Startup error Message");
    sprintf(cMsg, "PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/6: SetSysTasksPrio error Message");
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 4/6: Successful startup notice message");

    Ut_OS_printf_Setup();

    /* ----- Test case #5 - Static info able to be collected ----- */
    /* Setup additional inputs */
    CFE_PSP_SP0StaticInfoTable_t localStaticTable;
    USER_SAFE_MODE_DATA_STRUCT smud; 
    smud.safeMode = 0; 
    smud.sbc = SM_LOCAL_SBC; 
    smud.mckCause = 1; 
    smud.reason = 0; 
    uiResetSrc = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS); 
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), CFE_PSP_SUCCESS); 
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_SUCCESS); 
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS); 
    UT_SetDefaultReturnValue(UT_KEY(sysModel), 0); 
    /* getCoreClockSpeed for SP0s = 0 */
    UT_SetDefaultReturnValue(UT_KEY(getCoreClockSpeed), 0); 
    UT_SetDefaultReturnValue(UT_KEY(ReadResetSourceReg), OS_SUCCESS); 
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true); 
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITExecuted), 1 , OS_SUCCESS); 
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITExecuted), 1 , OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), false); 
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITResults), 1, OS_SUCCESS); 
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITResults), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), false); 
    UT_SetDefaultReturnValue(UT_KEY(ReadSafeModeUserData), OS_SUCCESS); 
    UT_SetDataBuffer(UT_KEY(ReadSafeModeUserData), &smud, sizeof(smud), false); 
    UT_SetDefaultReturnValue(UT_KEY(returnSelectedBootFlash), 1); 
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    CFE_PSP_SP0GetStaticInfoTable(&localStaticTable, sizeof(localStaticTable), 0);
    snprintf(cMsg, sizeof(cMsg), "{\"Safe mode\": %d, \"sbc\": \"%s\", \"reason\": %d, \"cause\": \"0x%08x\"}", smud.safeMode, "LOCAL", smud.reason, smud.mckCause);
    UtAssert_True(strncmp(cMsg, localStaticTable.safeModeUserData, sizeof(cMsg)) == 0, "_OS_Application_Startup - 5/6: Collect static info success");

    Ut_OS_printf_Setup();

    /* ----- Test case #6 - Dynamic info able to be collected ----- */
    /* Setup additional inputs */
    CFE_PSP_SP0DynamicInfoTable_t localDynamicTable;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(tempSensorRead), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(volSensorRead), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    CFE_PSP_SP0GetDynamicInfoTable(&localDynamicTable, sizeof(localDynamicTable), 0);
    /* tempSensorRead and volSensorRead are stubbed to return 25.5f and 5.05f */
    for (int i = 0; i < sizeof(localDynamicTable.temperatures)/sizeof(localDynamicTable.temperatures[0]); i++)
    {
        UtAssert_True(localDynamicTable.temperatures[i] == 25.5f, "_OS_Application_Startup - 6/6: Collect dynamic info success - temperature correct %d/4", i);
    }
    for (int i = 0; i < sizeof(localDynamicTable.voltages)/sizeof(localDynamicTable.voltages[0]); i++)
    {
        UtAssert_True(localDynamicTable.voltages[i] == 5.05f, "_OS_Application_Startup - 6/6: Collect dynamic info success - voltage correct %d/6", i);
    }
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
    char taskName[] = "tLogTask";
    char taskName_too_long[] = "123456789012345678901234567890";
    char *tName = &taskName[0];
    char *tName_null = NULL;
    char *tName_too_long = &taskName_too_long[0];

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
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 1/7: Nominal - Priority set to 0");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 1/7: Nominal - Priority set to 0 ");
    
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
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 2/7: Nominal - Priority set to 255");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 2/7: Nominal - Priority set to 255");

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
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 3/7: Failed - Failed to set priority to 25");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 3/7: Failed - Failed to set priority to 25");

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
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 4/7: Failed - Task ID lookup failure");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 4/7: Task ID lookup failure");

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
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 5/7: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 5/7: Failed - task name is null does not print any message");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 5/7: Null task name returns error");

    /* ----- Test case #6 - task name is null,
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_too_long, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 6/7: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 6/7: Failed - task name array does not contain NULL terminating string");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 6/7: task name string without NULL returns error");

    /* ----- Test case #7 - Failed to get task priority ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 0;
    iCurPrio = 10;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 7/7: Failed to get task priority");

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
    int8        cNumberOfLinks = 0;
    
    /* Calculate the number of entries in the structure */
    cNumberOfLinks = sizeof(g_SymbolicFolderLinks) / sizeof(CFE_PSP_SymbolicLinks_t);

    sprintf(cMsg, "PSP: Set %d Virtual Path(s)\n",cNumberOfLinks);

    /* ----- Test #1 - Nominal ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), CFE_PSP_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SetFileSysAddFixedMap(&fid);
    /* Verify results */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_CFE_PSP_SetFileSysAddFixedMap - 1/2: Virtual Mapping Returned Success");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetFileSysAddFixedMap - 1/2: Printed Nominal String");

    /* ----- Test #2 - Error mapping one or more filesystems ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), CFE_PSP_ERROR);
    /* Execute test */
    ret = CFE_PSP_SetFileSysAddFixedMap(&fid);
    /* Verify results */
    snprintf(cMsg, 256, "PSP: OS_FileSysAddFixedMap() failure: %d\n", (int)CFE_PSP_ERROR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Printed Error String");
    UtAssert_True(ret == CFE_PSP_ERROR, "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Virtual Mapping Returned Error");
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
    UtAssert_True((ret == CFE_PSP_ERROR_NOT_IMPLEMENTED),"_CFE_PSP_InitSSR - 1/1: Not Implemented Return Code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitSSR - 1/1: Not Implemented Message");
}

/*=======================================================================================
** End of file psp_start_testcases.c
**=======================================================================================*/
