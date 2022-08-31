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
**               |                  | psp_watchdog_testcases.c, and psp_memory_testcases.c
**    2021-09-08 | Claudio Olmi     | Added psp_ntp_testcases.c
**    2022-02-01 | Matthew Rumbel   | Memory sync support
**=======================================================================================*/

#include <unistd.h>
#include <wdb/wdbLib.h>
#include "uttest.h"
#include "utstubs.h"
#include "psp_testcases.h"

#define TEST_NTP        1
#define TEST_MEMORY     1
#define TEST_FLASH      1
#define TEST_MEM_SCRUB  1
#define TEST_EXCEPTION  1
#define TEST_SUPPORT    1
#define TEST_WATCHDOG   1
#define TEST_SSR        1
#define TEST_GR740_INFO 1
#define TEST_TIMER      1
#define TEST_START      1


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
    /* Test NTP Sync */
    #if TEST_NTP == 1

    UtTest_Add(Ut_CFE_PSP_SetOSTime,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SetOSTime");
    
    UtTest_Add(Ut_CFE_PSP_GetOSTime,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_GetOSTime");
    
    UtTest_Add(Ut_CFE_PSP_NTPDaemonIsRunning,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_NTPDaemonIsRunning");

    UtTest_Add(Ut_CFE_PSP_StartNTPDaemon,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_StartNTPDaemon");

    UtTest_Add(Ut_CFE_PSP_StopNTPDaemon,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_StopNTPDaemon");

    #endif /* TEST_NTP_SYNC == 1 */
    
    #if TEST_MEMORY == 1
    /* cfe_psp_memory.c test cases area */

    UtTest_Add(Ut_CFE_PSP_GetCDSSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCDSSize");

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

    #endif /* TEST_MEMORY == 1 */

    #if TEST_MEM_SCRUB == 1
    /* cfe_psp_memscrub.c test cases area*/

    UtTest_Add(Ut_CFE_PSP_MemScrubInit,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubInit");

    UtTest_Add(Ut_CFE_PSP_MemScrubValidate,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubValidate");

    UtTest_Add(Ut_CFE_PSP_MemScrubSet,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubSet");

    UtTest_Add(Ut_CFE_PSP_MemScrubGet,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubGet");

    UtTest_Add(Ut_CFE_PSP_MemScrubDelete,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubDelete");

    UtTest_Add(Ut_CFE_PSP_MemScrubErrStats,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubErrStats");

    #endif /* TEST_MEM_SCRUB */

    #if TEST_EXCEPTION == 1
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

    #endif /* TEST_EXCEPTION == 1 */

    #if TEST_SUPPORT == 1
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

    UtTest_Add(Ut_CFE_PSP_GetProcessorName,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetProcessorName");

    UtTest_Add(Ut_CFE_PSP_GetSpacecraftId,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetSpacecraftId");

    #endif /* TEST_SUPPORT == 1 */

    #if TEST_WATCHDOG == 1
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

    UtTest_Add(Ut_CFE_PSP_WatchdogStatus,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WatchdogStatus");

    #endif /* TEST_WATCHDOG == 1 */

    #if TEST_SSR == 1

    UtTest_Add(Ut_CFE_PSP_InitSSR,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_InitSSR");

    #endif /* TEST_SSR == 1 */

    #if TEST_GR740_INFO == 1

    UtTest_Add(Ut_CFE_PSP_TempSensorInit,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_TempSensorInit");

    UtTest_Add(Ut_CFE_PSP_GetTemperatureAwait,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetTemperatureAwait");

    #endif /* TEST_GR740_INFO == 1 */

    #if TEST_TIMER == 1

    UtTest_Add(Ut_CFE_PSP_InitLocalTime,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_InitLocalTime");

    UtTest_Add(Ut_vxTimeBaseGet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_vxTimeBaseGet");

    #endif /* TEST_TIMER == 1 */


    #if TEST_START == 1
    /* cfe_psp_start.c test cases area */

    UtTest_Add(Ut_CFE_PSP_SuspendConsoleShellTask,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SuspendConsoleShellTask");

    UtTest_Add(Ut_CFE_PSP_FindProcessor,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_FindProcessor");

    UtTest_Add(Ut_CFE_PSP_SetTaskPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetTaskPrio");

    UtTest_Add(Ut_CFE_PSP_SetSysTasksPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetSysTasksPrio");

    UtTest_Add(Ut_CFE_PSP_SetFileSysAddFixedMap,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetFileSysAddFixedMap");

    UtTest_Add(Ut_OS_Application_Run,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Run");

    UtTest_Add(Ut_OS_Application_Startup,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Startup"); 

    #endif  /* TEST_START == 1 */

    UtTest_Add(Ut_CFE_PSP_EndTests,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_EndTests");
} 

/**
 ** \brief Special function to send WDB Event Message to calling WTX Script
 ** 
 */
void Ut_CFE_PSP_EndTests(void)
{
    char cEventMessage[] = "End of Tests";
    wdbUserEvtPost(cEventMessage);
}

/*=======================================================================================
** End of file psp_testcases.c
**=======================================================================================*/

