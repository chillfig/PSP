/*=======================================================================================
**
** File:  psp_memory_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_memory.c.
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
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_memory_testcases.h"
#include "../src/cfe_psp_memory.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_GetCDSSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCDSSize(void)
{
    int32   iRetCode = 0;
    uint32  *puiSizeOfCDS = NULL;
    uint32  uiSize = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiSizeOfCDS = &uiSize;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 100;
    /* Execute test */
    iRetCode = CFE_PSP_GetCDSSize(puiSizeOfCDS);
    /* Verify outputs */
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 
                  "_ProcessResetType() - 1/2: Get CDS file correctly");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCDSSize() - 1/2: Nominal");

    /* ----- Test case #2 - Failed Argument is NULL ----- */
    /* Setup additional inputs */
    puiSizeOfCDS = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_GetCDSSize(puiSizeOfCDS);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "CFE_PSP_GetCDSSize() - 1/2: Argument is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_SetReadCDSMethod() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetReadCDSMethod(void)
{
    uint8 ucMethod = 10;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    CFE_PSP_SetReadCDSMethod(ucMethod);
    /* Verify outputs */
    UtAssert_True(g_CDSReadMethod = ucMethod, "_CFE_PSP_SetReadCDSMethod - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetReadCDSMethod() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetReadCDSMethod(void)
{
    uint8 ucRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    g_CDSReadMethod = 20;
    /* Execute test */
    ucRetCode = CFE_PSP_GetReadCDSMethod();
    /* Verify outputs */
    UtAssert_True(ucRetCode = g_CDSReadMethod, "_CFE_PSP_GetReadCDSMethod - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_SetStaticCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetStaticCRC(void)
{
    uint32 uiNewCRC = 10;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    CFE_PSP_SetStaticCRC(uiNewCRC);
    /* Verify outputs */
    UtAssert_True(sg_uiCDSCrc = uiNewCRC, "_CFE_PSP_SetStaticCRC - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetStaticCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetStaticCRC(void)
{
    uint32 uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    sg_uiCDSCrc = 20;
    /* Execute test */
    uiRetCode = CFE_PSP_GetStaticCRC();
    /* Verify outputs */
    UtAssert_True(uiRetCode = sg_uiCDSCrc, "_CFE_PSP_GetStaticCRC - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_CalculateCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_CalculateCRC(void)
{
    uint32 uiRetCode = 0;
    uint8  pData[100] = {};
    uint8  ucIdx = 0; 
    uint32  uiCrcCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    for (ucIdx = 0; ucIdx < 100; ucIdx++)
    {
        pData[ucIdx] = ucIdx;
    }
    /* Set pre-calculated CRC code (0xFFFFDFEA) for above array pData */
    uiCrcCode = 0xFFFFDFEA;
    /* Execute test */
    uiRetCode = CFE_PSP_CalculateCRC(pData, sizeof(pData), 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode = uiCrcCode, "_CFE_PSP_CalculateCRC - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_ReadCDSFromFlash() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ReadCDSFromFlash(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint32 *puiReadBytes = NULL;
    uint32  uiReadBytes = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiReadBytes = &uiReadBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiReadBytes);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadCDSFromFlash() - 1/4: Nominal");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    puiReadBytes = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 2/4: Input argument is NULL");

    /* ----- Test case #3 - Open CDS file failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    puiReadBytes = &uiReadBytes;
    sprintf(cMsg, "CFE_PSP: Failed to open the CDS file.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 3/4: Failed to open the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 3/4: Failed case for open()");

    /* ----- Test case #4 - Read CDS file failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiReadBytes = &uiReadBytes;
    sprintf(cMsg, "CFE_PSP: Failed to read the CDS data on Flash.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 4/4: Failed to read the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 4/4: Failed case for read()");
}

/*=======================================================================================
** Ut_CFE_PSP_WriteCDSToFlash() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteCDSToFlash(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint32 *puiWriteBytes = NULL;
    uint32  uiWriteBytes = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiWriteBytes = &uiWriteBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiWriteBytes);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteCDSToFlash() - 1/4: Nominal");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    puiWriteBytes = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteCDSToFlash() - 2/4: Input argument is NULL");

    /* ----- Test case #3 - Open CDS file failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    puiWriteBytes = &uiWriteBytes;
    sprintf(cMsg, "CFE_PSP: Failed to open the CDS file.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 3/4: Failed to open the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteCDSToFlash() - 3/4: Failed case for open()");

    /* ----- Test case #4 - Read CDS file failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiWriteBytes = &uiWriteBytes;
    sprintf(cMsg, "CFE_PSP: Failed to write the CDS data to Flash.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 4/4: Failed to write the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteCDSToFlash() - 4/4: Failed case for write()");
}
// /*=======================================================================================
// ** Ut_CFE_PSP_GetRestartType() test cases
// **=======================================================================================*/
// void Ut_CFE_PSP_LogSoftwareResetType(void)
// {
//     char cMsg[256] = {};
//     RESET_SRC_REG_ENUM resetSrc = RESET_SRC_POR;

//     /* ----- Test case #1 - Nominal RESET_SRC_POR. No check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     Ut_ES_WriteToSysLog_Setup();
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 1/7:Nominal - RESET_SRC_POR");
//     UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                                 "_OS_Application_Startup - 1/7:Nominal - No check for safeModeUserData.mckCause");

//     /* ----- Test case #2 - Nominal RESET_SRC_WDT. No check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     resetSrc = RESET_SRC_WDT;
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_WDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 2/7:Nominal - RESET_SRC_WDT");
//     UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                                 "_OS_Application_Startup - 2/7:Nominal - No check for safeModeUserData.mckCause");

//     /* ----- Test case #3 - Nominal RESET_SRC_FWDT. No check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     resetSrc = RESET_SRC_FWDT;
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_FWDT) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 3/7:Nominal - RESET_SRC_FWDT");
//     UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                                 "_OS_Application_Startup - 3/7:Nominal - No check for safeModeUserData.mckCause");

//     /* ----- Test case #4 - Nominal RESET_SRC_CPCI. No check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     resetSrc = RESET_SRC_CPCI;
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_CPCI) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 4/7:Nominal - RESET_SRC_CPCI");
//     UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                                 "_OS_Application_Startup - 4/7:Nominal - No check for safeModeUserData.mckCause");

//     /* ----- Test case #5 - Nominal RESET_SRC_SWR. No check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     resetSrc = RESET_SRC_SWR;
//     safeModeUserData.mckCause = 0;
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 5/7:Nominal - RESET_SRC_SWR");
//     UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                                 "_OS_Application_Startup - 5/7:Nominal - No check for safeModeUserData.mckCause");

//     /* ----- Test case #1 - Nominal RESET_SRC_SWR. Check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     resetSrc = RESET_SRC_SWR;
//     safeModeUserData.mckCause = 1;
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_SWR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 6/7:Nominal - RESET_SRC_SWR");
//     UtAssert_ES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                               "_OS_Application_Startup - 6/7:Nominal - Check for safeModeUserData.mckCause");

//     /* ----- Test case #7 - Nominal default to RESET_SRC_POR. No check for safeModeUserData.mckCause ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     resetSrc = 1;
//     sprintf(cMsg, 
//             "CFE_PSP: PROCESSOR rst Source = 0x%x = (RESET_SRC_POR) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
//             resetSrc,
//             safeModeUserData.safeMode,
//             (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
//             safeModeUserData.reason,
//             safeModeUserData.mckCause);
//     /* Execute test */
//     CFE_PSP_LogSoftwareResetType(resetSrc);
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 7/7:Nominal - Default RESET_SRC_POR");
//     UtAssert_NoES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n", 
//                                 "_OS_Application_Startup - 7/7:Nominal - No check for safeModeUserData.mckCause");
// }

// /*=======================================================================================
// ** Ut_OS_Application_Startup() test cases
// ** Since there is a multiple definition issue for "OS_Application_Run()". This function
// ** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
// ** was located in unit_test/CMakeLists.txt
// **=======================================================================================*/
// void Ut_OS_Application_Startup(void)
// {
//     char cMsg[256] = {};

//     /* ----- Test case #1 - Nominal ----- */
//     /* Setup additional inputs */
//     Ut_ES_WriteToSysLog_Setup();
//     sprintf(cMsg, "CFE_PSP: CFE_PSP_Start() done.\n");
//     /* Execute test */
//     UT_OS_Application_Startup();
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 1/5:Nominal");
    
//     /* ----- Test case #2 - Initialize the OS API data structures failed ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_OS_printf_Setup();
//     UT_SetDeferredRetcode(UT_KEY(OS_API_Init), 1, OS_ERROR);
//     sprintf(cMsg, "CFE_PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", OS_ERROR);
//     /* Execute test */
//     UT_OS_Application_Startup();
//     /* Verify outputs */
//     UtAssert_OS_print(cMsg, "_OS_Application_Startup - 2/5: Initialize the OS API data structures failed");

//     /* ----- Test case #3 - Setup FS mapping \"/cf\" directorys failed ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_OS_printf_Setup();
//     UT_SetDeferredRetcode(UT_KEY(OS_FileSysAddFixedMap), 1, OS_ERROR);
//     sprintf(cMsg, "CFE_PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
//     /* Execute test */
//     UT_OS_Application_Startup();
//     /* Verify outputs */
//     UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/5: Setup FS mapping \"/cf\" directorys failed");

//     /* ----- Test case #4 - CFE_PSP_InitProcessorReservedMemory() failed ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_OS_printf_Setup();
//     UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
//     UT_SetDeferredRetcode(UT_KEY(creat), 1, OS_ERROR);
//     sprintf(cMsg, "CFE_PSP: Failed to create the CDS file(/ffx0/CDS) on Flash.\n");
//     /* Execute test */
//     UT_OS_Application_Startup();
//     /* Verify outputs */
//     UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/5: CFE_PSP_InitProcessorReservedMemory() failed");

//     /* ----- Test case #5 - vxWorks task priority set failed ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_ERROR);
//     UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
//     sprintf(cMsg, "CFE_PSP: CFE_PSP_Start() At least one vxWorks task priority set failed. System may have degraded performance.\n");
//     /* Execute test */
//     UT_OS_Application_Startup();
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Startup - 5/5: vxWorks task priority set failed");
// }

// /*=======================================================================================
// ** Ut_CFE_PSP_GetRestartType() test cases
// **=======================================================================================*/
// void Ut_CFE_PSP_GetRestartType(void)
// {
//     uint32 uiResetSubType = 1;
//     uint32 *pResetSubType = NULL;
//     uint32 uiRetCode = 0;
    
//     /* ----- Test case #1 - Nominal pResetSubType is NULL ----- */
//     /* Setup additional inputs */
//     /* Execute test */
//     uiRetCode = CFE_PSP_GetRestartType(pResetSubType);
//     /* Verify outputs */
//     UtAssert_True(pResetSubType == NULL, "_GetRestartType() - 1/2:pResetSubType is NULL");
//     UtAssert_True(uiRetCode == ResetType, "_GetRestartType() - 1/2:Nominal");
    
//     /* ----- Test case #2 - Nominal pResetSubType was set value ----- */
//     /* Setup additional inputs */
//     pResetSubType = &uiResetSubType;
//     /* Execute test */
//     uiRetCode = CFE_PSP_GetRestartType(pResetSubType);
//     /* Verify outputs */
//     UtAssert_True(*pResetSubType == ResetSubtype, "_GetRestartType() - 2/2:pResetSubType was set value");
//     UtAssert_True(uiRetCode == ResetType, "_GetRestartType() - 2/2:Nominal");
// }

// /*=======================================================================================
// ** Ut_SetTaskPrio() test cases
// **=======================================================================================*/
// void Ut_SetTaskPrio(void)
// {
//     char cMsg[256] = {};
//     int32 iNewPrio = 0;
//     int32 iCurPrio = 10;
    
//     /* ----- Test case #1 - Nominal Priority set to 0 for task name "tLogTask" ----- */
//     /* Setup additional inputs */
//     Ut_OS_printf_Setup();
//     sprintf(cMsg, "PSP: SetTaskPrio() - Setting tLogTask priority from %d to %d\n", iCurPrio, iNewPrio);
//     UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
//     UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
//     UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
//     /* Execute test */
//     /* Test for task name "tLogTask". New priority will change from -1 to 0 */
//     SetTaskPrio("tLogTask", -1);
//     /* Verify outputs */
//     UtAssert_OS_print(cMsg, "_SetTaskPrio - 1/3:Nominal - Priority set to 0");
    
//     /* ----- Test case #2 - Nominal Priority set to 255 for task name "tLogTask" ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_OS_printf_Setup();
//     iNewPrio = 255;
//     sprintf(cMsg, "PSP: SetTaskPrio() - Setting tLogTask priority from %d to %d\n", iCurPrio, iNewPrio);
//     UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
//     UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
//     UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
//     /* Execute test */
//     /* Test for task name "tLogTask". New priority will change from 260 to 255 */
//     SetTaskPrio("tLogTask", 260);
//     /* Verify outputs */
//     UtAssert_OS_print(cMsg, "_SetTaskPrio - 2/3:Nominal - Priority set to 255");

//     /* ----- Test case #3 - Failed to set priority to 25 for task name "tLogTask" ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_OS_printf_Setup();
//     iNewPrio = 25;
//     sprintf(cMsg, "PSP: taskPrioritySet() - Failed for tLogTask priority from %d to %d\n", iCurPrio, iNewPrio);
//     UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
//     UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
//     UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
//     /* Execute test */
//     /* Test for task name "tLogTask" */
//     SetTaskPrio("tLogTask", iNewPrio);
//     /* Verify outputs */
//     UtAssert_OS_print(cMsg, "_SetTaskPrio - 3/3:Failed - Failed to set priority to 25");
// }

// /*=======================================================================================
// ** Ut_SetSysTasksPrio() test cases
// **=======================================================================================*/
// void Ut_SetSysTasksPrio(void)
// {
//     uint32 uiRetCode = 0;

//     /* ----- Test case #1 - Nominal ----- */
//     /* Setup additional inputs - None */
//     /* Execute test */
//     uiRetCode = SetSysTasksPrio();
//     /* Verify outputs */
//     UtAssert_True(uiRetCode == OS_SUCCESS, "_SetSysTasksPrio - 1/2:Nominal");

//     /* ----- Test case #2 - Failed to set priority for a task ----- */
//     /* Setup additional inputs */
//     UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
//     UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
//     /* Execute test */
//     uiRetCode = SetSysTasksPrio();
//     /* Verify outputs */
//     UtAssert_True(uiRetCode == OS_ERROR, "_SetSysTasksPrio - 2/2: Set priority for a task failed");
// }

// /*=======================================================================================
// ** Ut_PSP_1HzLocalCallback() test cases
// **=======================================================================================*/
// void Ut_PSP_1HzLocalCallback(void)
// {
//     /* No useful content to test */
//     /* ----- Test case #1 - Nominal ----- */
//     /* Setup additional inputs - None */
//     /* Execute test */
//     PSP_1HzLocalCallback(100);
//     /* Verify outputs */
//     UtAssert_True(TRUE, "_PSP_1HzLocalCallback - 1/1:Nominal");
// }

// /*=======================================================================================
// ** Ut_OS_Application_Run() test cases
// ** Since there is a multiple definition issue for "OS_Application_Run()". This function
// ** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
// ** was located in unit_test/CMakeLists.txt
// **=======================================================================================*/
// void Ut_OS_Application_Run(void)
// {
//     char cMsg[256] = {};

//     /* ----- Test case #1 - Nominal ----- */
//     /* Setup additional inputs */
//     Ut_ES_WriteToSysLog_Setup();
//     /* Execute test */
//     UT_OS_Application_Run();
//     /* Verify outputs */
//     sprintf(cMsg, "Failed to create OS_Timer for 1Hz local time.\n");
//     UtAssert_NoES_WriteToSysLog(cMsg, "_OS_Application_Run - 1/3:Nominal - OS_TimerCreate successed");
//     sprintf(cMsg, "Failed to set OS_Timer for 1Hz local time.\n");
//     UtAssert_NoES_WriteToSysLog(cMsg, "_OS_Application_Run - 1/3:Nominal - OS_TimerSet successed");
    
//     /* ----- Test case #2 - OS_TimerCreate failed ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     UT_SetDeferredRetcode(UT_KEY(OS_TimerCreate), 1, OS_ERROR);
//     sprintf(cMsg, "Failed to create OS_Timer for 1Hz local time.\n");
//     /* Execute test */
//     UT_OS_Application_Run();
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Run - 2/3: OS_TimerCreate failed");

//     /* ----- Test case #3 - OS_TimerSet failed ----- */
//     /* Setup additional inputs */
//     UT_ResetState(0);
//     Ut_ES_WriteToSysLog_Setup();
//     UT_SetDeferredRetcode(UT_KEY(OS_TimerSet), 1, OS_ERROR);
//     sprintf(cMsg, "Failed to set OS_Timer for 1Hz local time.\n");
//     /* Execute test */
//     UT_OS_Application_Run();
//     /* Verify outputs */
//     UtAssert_ES_WriteToSysLog(cMsg, "_OS_Application_Run - 3/3: OS_TimerSet failed");
// }

// /*=======================================================================================
// ** Ut_vxFpscrGet() test cases
// **=======================================================================================*/
// void Ut_vxFpscrGet(void)
// {
//     /* No useful content to test */
//     /* ----- Test case #1 - Nominal ----- */
//     /* Setup additional inputs - None */
//     /* Execute test */
//     vxFpscrGet();
//     /* Verify outputs */
//     UtAssert_True(TRUE, "_vxFpscrGet - 1/1:Nominal");
// }

// /*=======================================================================================
// ** Ut_vxFpscrSet() test cases
// **=======================================================================================*/
// void Ut_vxFpscrSet(void)
// {
//     /* No useful content to test */
//     /* ----- Test case #1 - Nominal ----- */
//     /* Setup additional inputs - None */
//     /* Execute test */
//     vxFpscrSet(0);
//     /* Verify outputs */
//     UtAssert_True(TRUE, "_vxFpscrSet - 1/1:Nominal");
// }

/*=======================================================================================
** End of file psp_memory_testcases.c
**=======================================================================================*/
