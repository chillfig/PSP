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
#include "../src/cfe_psp_watchdog.c"
#include "../src/cfe_psp_timer.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

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
    Ut_ES_WriteToSysLog_Setup();
    UT_SetDataBuffer(UT_KEY(aimonGetBITExecuted), &bitExecuted, sizeof(bitExecuted), true);
    UT_SetDataBuffer(UT_KEY(aimonGetBITResults), &bitResult, sizeof(bitResult), true);
    sprintf(cMsg, "CFE_PSP: CFE_PSP_ProcessPOSTResults: aimonGetBITExecuted() or aimonGetBITResults() failed.");
    /* Execute test */
    CFE_PSP_ProcessPOSTResults();
    /* Verify outputs */
    UtAssert_NoES_WriteToSysLog(cMsg, "_ProcessPOSTResults - 1/2:Nominal");

    /* ----- Test case #2 - aimonGetBITExecuted() or aimonGetBITResults() failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    UT_SetDeferredRetcode(UT_KEY(aimonGetBITExecuted), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP: CFE_PSP_ProcessPOSTResults: aimonGetBITExecuted() or aimonGetBITResults() failed.");
    /* Execute test */
    CFE_PSP_ProcessPOSTResults();
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, 
                    "_ProcessPOSTResults - 2/2 aimonGetBITExecuted() or aimonGetBITResults() failed");
}

/*=======================================================================================
** Ut_CFE_PSP_ProcessResetType() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ProcessResetType(void)
{
    uint32 uiResetSrc = 0;
    RESET_SRC_REG_ENUM retCode = 0;
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal Power Switch ON ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_POR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_True(retCode == RESET_SRC_POR, "_ProcessResetType() - 1/7:Nominal - POWERON Reset: Power Switch ON");

    /* ----- Test case #2 - Nominal External FPGA Watchdog timer primary EEPROM boot failure. ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_WDT;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    retCode = CFE_PSP_ProcessResetType();
    UtAssert_True(retCode == RESET_SRC_WDT, "_ProcessResetType() - 2/7:Nominal - PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.");

    /* ----- Test case #3 - Nominal Internal FPGA Watchdog timer application SW failure. ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_FWDT;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_True(retCode == RESET_SRC_FWDT, "_ProcessResetType() - 3/7:Nominal - PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.");

    /* ----- Test case #4 - Nominal cPCI Reset initiated by FPGA from remote SBC. ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_CPCI;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_True(retCode == RESET_SRC_CPCI, "_ProcessResetType() - 4/7:Nominal - PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.");

    /* ----- Test case #5 - Nominal Software Hard Reset ----- */
    /* Setup additional inputs */
    uiResetSrc = RESET_SRC_SWR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_True(retCode == RESET_SRC_SWR, "_ProcessResetType() - 5/7:Nominal - POWERON Reset: Software Hard Reset");

    /* ----- Test case #6 - Failed to read safemode data ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    uiResetSrc = RESET_SRC_SWR;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    UT_SetDeferredRetcode(UT_KEY(ReadSafeModeUserData), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP: PROCESSOR Reset: failed to read safemode data.\n");
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 6/7: Failed to read safemode data");
    UtAssert_True(retCode == RESET_SRC_SWR, "_ProcessResetType() - 6/7: POWERON Reset: Software Hard Reset");

    /* ----- Test case #7 - UNKNOWN Reset. Reset source read failed. ----- */
    /* Setup additional inputs */
    uiResetSrc = 0;
    UT_SetDeferredRetcode(UT_KEY(ReadResetSourceReg), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(ReadResetSourceReg), &uiResetSrc, sizeof(uiResetSrc), true);
    /* Execute test */
    retCode = CFE_PSP_ProcessResetType();
    /* Verify outputs */
    UtAssert_True(retCode == 0, "_ProcessResetType() - 7/7: UNKNOWN Reset. Reset source read failed.");
}

/*=======================================================================================
** Ut_CFE_PSP_LogSoftwareResetType() test cases
**=======================================================================================*/
void Ut_CFE_PSP_LogSoftwareResetType(void)
{
    char cMsg[256] = {};
    RESET_SRC_REG_ENUM resetSrc = RESET_SRC_POR;

    /* ----- Test case #1 - Nominal RESET_SRC_POR. No check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    Ut_ES_WriteToSysLog_Setup();
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 1/7:Nominal - RESET_SRC_POR");
    UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_OS_Application_Startup - 1/7:Nominal - No check for safeModeUserData.mckCause");

    /* ----- Test case #2 - Nominal RESET_SRC_WDT. No check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    resetSrc = RESET_SRC_WDT;
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_WDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 2/7:Nominal - RESET_SRC_WDT");
    UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_OS_Application_Startup - 2/7:Nominal - No check for safeModeUserData.mckCause");

    /* ----- Test case #3 - Nominal RESET_SRC_FWDT. No check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    resetSrc = RESET_SRC_FWDT;
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_FWDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 3/7:Nominal - RESET_SRC_FWDT");
    UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_OS_Application_Startup - 3/7:Nominal - No check for safeModeUserData.mckCause");

    /* ----- Test case #4 - Nominal RESET_SRC_CPCI. No check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    resetSrc = RESET_SRC_CPCI;
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_CPCI) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 4/7:Nominal - RESET_SRC_CPCI");
    UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_OS_Application_Startup - 4/7:Nominal - No check for safeModeUserData.mckCause");

    /* ----- Test case #5 - Nominal RESET_SRC_SWR. No check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    resetSrc = RESET_SRC_SWR;
    safeModeUserData.mckCause = 0;
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 5/7:Nominal - RESET_SRC_SWR");
    UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_OS_Application_Startup - 5/7:Nominal - No check for safeModeUserData.mckCause");

    /* ----- Test case #6 - Nominal RESET_SRC_SWR. Check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    resetSrc = RESET_SRC_SWR;
    safeModeUserData.mckCause = 1;
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 6/7:Nominal - RESET_SRC_SWR");
    UtAssert_ES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                              "_OS_Application_Startup - 6/7:Nominal - Check for safeModeUserData.mckCause");

    /* ----- Test case #7 - Nominal default to RESET_SRC_POR. No check for safeModeUserData.mckCause ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    resetSrc = 1;
    sprintf(cMsg, 
            "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
            resetSrc,
            safeModeUserData.safeMode,
            (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
            safeModeUserData.reason,
            safeModeUserData.mckCause);
    /* Execute test */
    CFE_PSP_LogSoftwareResetType(resetSrc);
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 7/7:Nominal - Default RESET_SRC_POR");
    UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
                                "_OS_Application_Startup - 7/7:Nominal - No check for safeModeUserData.mckCause");
}

/*=======================================================================================
** Ut_OS_Application_Startup() test cases
** Since there is a multiple definition issue for "OS_Application_Run()". This function
** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
** was located in unit_test/CMakeLists.txt
**=======================================================================================*/
void Ut_OS_Application_Startup(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_ES_WriteToSysLog_Setup();
    sprintf(cMsg, "CFE_PSP: CFE_PSP_Start() done.\n");
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 1/5:Nominal");
    
    /* ----- Test case #2 - Initialize the OS API data structures failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_API_Init), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 2/5: Initialize the OS API data structures failed");

    /* ----- Test case #3 - Setup FS mapping \"/cf\" directorys failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_FileSysAddFixedMap), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/5: Setup FS mapping \"/cf\" directorys failed");

    /* ----- Test case #4 - CFE_PSP_InitProcessorReservedMemory() failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP: Failed to create the CDS file(/ffx0/CDS) on Flash.\n");
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/5: CFE_PSP_InitProcessorReservedMemory() failed");

    /* ----- Test case #5 - vxWorks task priority set failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP: CFE_PSP_Start() At least one vxWorks task priority set failed. System may have degraded performance.\n");
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 5/5: vxWorks task priority set failed");
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
    UtAssert_True(uiRetCode == ResetType, "_GetRestartType() - 1/2:Nominal");
    
    /* ----- Test case #2 - Nominal pResetSubType was set value ----- */
    /* Setup additional inputs */
    pResetSubType = &uiResetSubType;
    /* Execute test */
    uiRetCode = CFE_PSP_GetRestartType(pResetSubType);
    /* Verify outputs */
    UtAssert_True(*pResetSubType == ResetSubtype, "_GetRestartType() - 2/2:pResetSubType was set value");
    UtAssert_True(uiRetCode == ResetType, "_GetRestartType() - 2/2:Nominal");
}

/*=======================================================================================
** Ut_SetTaskPrio() test cases
**=======================================================================================*/
void Ut_SetTaskPrio(void)
{
    char cMsg[256] = {};
    int32 iNewPrio = 0;
    int32 iCurPrio = 10;
    
    /* ----- Test case #1 - Nominal Priority set to 0 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP: SetTaskPrio() - Setting tLogTask priority from %d to %d\n", iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from -1 to 0 */
    SetTaskPrio("tLogTask", -1);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_SetTaskPrio - 1/3:Nominal - Priority set to 0");
    
    /* ----- Test case #2 - Nominal Priority set to 255 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 255;
    sprintf(cMsg, "PSP: SetTaskPrio() - Setting tLogTask priority from %d to %d\n", iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from 260 to 255 */
    SetTaskPrio("tLogTask", 260);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_SetTaskPrio - 2/3:Nominal - Priority set to 255");

    /* ----- Test case #3 - Failed to set priority to 25 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    sprintf(cMsg, "PSP: taskPrioritySet() - Failed for tLogTask priority from %d to %d\n", iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask" */
    SetTaskPrio("tLogTask", iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_SetTaskPrio - 3/3:Failed - Failed to set priority to 25");
}

/*=======================================================================================
** Ut_SetSysTasksPrio() test cases
**=======================================================================================*/
void Ut_SetSysTasksPrio(void)
{
    uint32 uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    uiRetCode = SetSysTasksPrio();
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_SetSysTasksPrio - 1/2:Nominal");

    /* ----- Test case #2 - Failed to set priority for a task ----- */
    /* Setup additional inputs */
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    uiRetCode = SetSysTasksPrio();
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_SetSysTasksPrio - 2/2: Set priority for a task failed");
}

/*=======================================================================================
** Ut_PSP_1HzLocalCallback() test cases
**=======================================================================================*/
void Ut_PSP_1HzLocalCallback(void)
{
    /* No useful content to test */
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    PSP_1HzLocalCallback(100);
    /* Verify outputs */
    UtAssert_True(TRUE, "_PSP_1HzLocalCallback - 1/1:Nominal");
}

/*=======================================================================================
** Ut_OS_Application_Run() test cases
** Since there is a multiple definition issue for "OS_Application_Run()". This function
** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
** was located in unit_test/CMakeLists.txt
**=======================================================================================*/
void Ut_OS_Application_Run(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_ES_WriteToSysLog_Setup();
    /* Execute test */
    UT_OS_Application_Run();
    /* Verify outputs */
    sprintf(cMsg, "Failed to create OS_Timer for 1Hz local time.\n");
    UtAssert_NoES_WriteToSysLog(cMsg, "_OS_Application_Run - 1/3:Nominal - OS_TimerCreate successed");
    sprintf(cMsg, "Failed to set OS_Timer for 1Hz local time.\n");
    UtAssert_NoES_WriteToSysLog(cMsg, "_OS_Application_Run - 1/3:Nominal - OS_TimerSet successed");
    
    /* ----- Test case #2 - OS_TimerCreate failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TimerCreate), 1, OS_ERROR);
    sprintf(cMsg, "Failed to create OS_Timer for 1Hz local time.\n");
    /* Execute test */
    UT_OS_Application_Run();
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Run - 2/3: OS_TimerCreate failed");

    /* ----- Test case #3 - OS_TimerSet failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_ES_WriteToSysLog_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TimerSet), 1, OS_ERROR);
    sprintf(cMsg, "Failed to set OS_Timer for 1Hz local time.\n");
    /* Execute test */
    UT_OS_Application_Run();
    /* Verify outputs */
    UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Run - 3/3: OS_TimerSet failed");
}

/*=======================================================================================
** Ut_vxFpscrGet() test cases
**=======================================================================================*/
void Ut_vxFpscrGet(void)
{
    /* No useful content to test */
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    vxFpscrGet();
    /* Verify outputs */
    UtAssert_True(TRUE, "_vxFpscrGet - 1/1:Nominal");
}

/*=======================================================================================
** Ut_vxFpscrSet() test cases
**=======================================================================================*/
void Ut_vxFpscrSet(void)
{
    /* No useful content to test */
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    vxFpscrSet(0);
    /* Verify outputs */
    UtAssert_True(TRUE, "_vxFpscrSet - 1/1:Nominal");
}

/*=======================================================================================
** End of file psp_start_testcases.c
**=======================================================================================*/
