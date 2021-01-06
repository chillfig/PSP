/*=======================================================================================
**
** File:  psp_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-16 | Minh Luong       | Initial revision
**    2021-01-05 | Minh Luong       | Added test cases for psp_exception_testcase.c, 
**               |                  | psp_support_testcases.c, psp_timer_testcase.c, 
**               |                  | and psp_watchdog_testcases.c
**=======================================================================================*/

#include <unistd.h>
#include "uttest.h"
#include "utstubs.h"
#include "psp_testcases.h"

/*=======================================================================================
** Test Setup & Teardown functions
**=======================================================================================*/

void Ut_CFE_PSP_Setup(void)
{
    UT_ResetState(0);
}

void Ut_CFE_PSP_Teardown(void)
{
}

void UtTest_Setup(void)
{
    /* cfe_psp_start.c test cases area */
    UtTest_Add(Ut_CFE_PSP_ProcessPOSTResults,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ProcessPOSTResults");

    UtTest_Add(Ut_CFE_PSP_ProcessResetType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ProcessResetType");

    UtTest_Add(Ut_CFE_PSP_LogSoftwareResetType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_LogSoftwareResetType");

    UtTest_Add(Ut_OS_Application_Startup,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Startup"); 

    UtTest_Add(Ut_CFE_PSP_GetRestartType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetRestartType"); 

    UtTest_Add(Ut_SetTaskPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_SetTaskPrio");

    UtTest_Add(Ut_SetSysTasksPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_SetSysTasksPrio");

    UtTest_Add(Ut_PSP_1HzLocalCallback,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_PSP_1HzLocalCallback");

    UtTest_Add(Ut_OS_Application_Run,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Run");

    UtTest_Add(Ut_vxFpscrGet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_vxFpscrGet");

    UtTest_Add(Ut_vxFpscrSet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_vxFpscrSet");

    /* cfe_psp_memory.c test cases area */
    UtTest_Add(Ut_CFE_PSP_GetCDSSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCDSSize");

    UtTest_Add(Ut_CFE_PSP_SetReadCDSMethod,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetReadCDSMethod");

    UtTest_Add(Ut_CFE_PSP_GetReadCDSMethod,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetReadCDSMethod");

    UtTest_Add(Ut_CFE_PSP_SetStaticCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetStaticCRC");

    UtTest_Add(Ut_CFE_PSP_GetStaticCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetStaticCRC");

    UtTest_Add(Ut_CFE_PSP_CalculateCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CalculateCRC");

    UtTest_Add(Ut_CFE_PSP_ReadCDSFromFlash,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadCDSFromFlash");

    UtTest_Add(Ut_CFE_PSP_WriteCDSToFlash,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteCDSToFlash");

    UtTest_Add(Ut_CFE_PSP_WriteToCDS,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteToCDS");

    UtTest_Add(Ut_CFE_PSP_ReadFromCDS,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadFromCDS");

    UtTest_Add(Ut_CFE_PSP_GetResetArea,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetResetArea");

    UtTest_Add(Ut_CFE_PSP_GetUserReservedArea,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetUserReservedArea");

    UtTest_Add(Ut_CFE_PSP_GetVolatileDiskMem,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetVolatileDiskMem");
    
    UtTest_Add(Ut_CFE_PSP_InitProcessorReservedMemory,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_InitProcessorReservedMemory");

    UtTest_Add(Ut_CFE_PSP_SetupReservedMemoryMap,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetupReservedMemoryMap");

    UtTest_Add(Ut_CFE_PSP_DeleteProcessorReservedMemory,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_DeleteProcessorReservedMemory");

    UtTest_Add(Ut_CFE_PSP_GetKernelTextSegmentInfo,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetKernelTextSegmentInfo");

    UtTest_Add(Ut_CFE_PSP_GetCFETextSegmentInfo,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCFETextSegmentInfo");

    /* cfe_psp_exception.c test cases area */
    UtTest_Add(Ut_CFE_PSP_edrPolicyHandlerHook,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_edrPolicyHandlerHook");

    UtTest_Add(Ut_CFE_PSP_AttachExceptions,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_AttachExceptions");

    UtTest_Add(Ut_CFE_PSP_SetDefaultExceptionEnvironment,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetDefaultExceptionEnvironment");

    UtTest_Add(Ut_CFE_PSP_ExceptionGetSummary_Impl,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ExceptionGetSummary_Impl");

    /* cfe_psp_support.c test cases area */
    UtTest_Add(Ut_CFE_PSP_Restart,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_Restart");

    UtTest_Add(Ut_CFE_PSP_Panic,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_Panic");

    UtTest_Add(Ut_CFE_PSP_FlushCaches,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_FlushCaches");

    UtTest_Add(Ut_CFE_PSP_GetProcessorId,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetProcessorId");

    UtTest_Add(Ut_CFE_PSP_GetSpacecraftId,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetSpacecraftId");

    UtTest_Add(Ut_CFE_PSP_GetProcessorName,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetProcessorName");

    /* cfe_psp_timer.c test cases area */
    UtTest_Add(Ut_CFE_PSP_GetTime,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetTime");

    UtTest_Add(Ut_CFE_PSP_Get_Timer_Tick,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_Get_Timer_Tick");

    UtTest_Add(Ut_CFE_PSP_GetTimerTicksPerSecond,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetTimerTicksPerSecond");

    UtTest_Add(Ut_CFE_PSP_GetTimerLow32Rollover,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetTimerLow32Rollover");

    UtTest_Add(Ut_CFE_PSP_Get_Timebase,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_Get_Timebase");

    UtTest_Add(Ut_CFE_PSP_Get_Dec,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_Get_Dec");

    /* cfe_psp_watchdog.c test cases area */
    UtTest_Add(Ut_CFE_PSP_WatchdogInit,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogInit");

    UtTest_Add(Ut_CFE_PSP_WatchdogEnable,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogEnable");

    UtTest_Add(Ut_CFE_PSP_WatchdogDisable,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogDisable");

    UtTest_Add(Ut_CFE_PSP_WatchdogService,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogService");

    UtTest_Add(Ut_CFE_PSP_WatchdogGet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogGet");

    UtTest_Add(Ut_CFE_PSP_WatchdogSet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogSet");
} 

/*=======================================================================================
** End of file psp_testcases.c
**=======================================================================================*/

