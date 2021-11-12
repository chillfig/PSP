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
**=======================================================================================*/

#include <unistd.h>
#include "uttest.h"
#include "utstubs.h"
#include "psp_testcases.h"

#define TEST_NTP        1
#define TEST_SP0_INFO   1
#define TEST_MEMORY     1
#define TEST_MEM_SCRUB  1
#define TEST_START      1
#define TEST_EXCEPTION  1
#define TEST_SUPPORT    1
#define TEST_WATCHDOG   1

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
    #if TEST_NTP == 1

    UtTest_Add(Ut_CFE_PSP_Sync_From_OS_Enable,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "CFE_PSP_Sync_From_OS_Enable");
    
    UtTest_Add(Ut_CFE_PSP_Sync_From_OS_SetFreq,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_Sync_From_OS_SetFreq");

    UtTest_Add(Ut_CFE_PSP_Sync_From_OS_GetFreq,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_Sync_From_OS_GetFreq");

    UtTest_Add(Ut_CFE_PSP_Set_OS_Time,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_Set_OS_Time");
    
    UtTest_Add(Ut_CFE_PSP_Get_OS_Time,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_Get_OS_Time");
    
    UtTest_Add(Ut_CFE_PSP_NTP_Daemon_Get_Status,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_NTP_Daemon_Get_Status");
    
    UtTest_Add(Ut_CFE_PSP_NTP_Daemon_Enable,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_NTP_Daemon_Enable");
    
    UtTest_Add(Ut_CFE_PSP_TIME_Init,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_TIME_Init");

    UtTest_Add(Ut_CFE_PSP_TimeService_Ready,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_TimeService_Ready");

    UtTest_Add(Ut_CFE_PSP_Update_OS_Time,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_Update_OS_Time");

    UtTest_Add(Ut_CFE_PSP_StartNTPDaemon,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_StartNTPDaemon");

    UtTest_Add(Ut_CFE_PSP_StopNTPDaemon,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_StopNTPDaemon");

    UtTest_Add(Ut_net_clock_vxworks_Destroy,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_net_clock_vxworks_Destroy");

    UtTest_Add(Ut_ntp_clock_vxworks_Init,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_ntp_clock_vxworks_Init");

    #endif

    #if TEST_SP0_INFO == 1
    /* cfe_psp_sp0_info.c test cases area */

    UtTest_Add(Ut_PSP_SP0_GetInfo,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_PSP_SP0_GetInfo");

    UtTest_Add(Ut_PSP_SP0_PrintInfoTable,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_PSP_SP0_PrintInfoTable");

    UtTest_Add(Ut_PSP_SP0_DumpData,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_PSP_SP0_DumpData");

    UtTest_Add(Ut_PSP_SP0_GetDiskFreeSize,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_PSP_SP0_GetDiskFreeSize");

    #endif /* TEST_SP0_INFO == 1 */

    #if TEST_START == 1
    /* cfe_psp_start.c test cases area */

    UtTest_Add(Ut_CFE_PSP_ProcessPOSTResults,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ProcessPOSTResults");
               
    UtTest_Add(Ut_CFE_PSP_Main,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_Main");

    UtTest_Add(Ut_OS_Application_Run,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Run");

    UtTest_Add(Ut_CFE_PSP_SuspendConsoleShellTask,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SuspendConsoleShellTask");

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

    UtTest_Add(Ut_CFE_PSP_SetTaskPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetTaskPrio");

    UtTest_Add(Ut_CFE_PSP_SetSysTasksPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetSysTasksPrio");

    UtTest_Add(Ut_CFE_PSP_InitSSR,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_InitSSR");

    #endif  /* TEST_START == 1 */
    
    #if TEST_MEMORY == 1
    /* cfe_psp_memory.c test cases area */

    UtTest_Add(Ut_CFE_PSP_GetCDSSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCDSSize");

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

    #endif /* TEST_MEMORY == 1 */

    #if TEST_MEM_SCRUB == 1
    /* cfe_psp_mem_scrub.c test cases area*/

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Init,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Init");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Task,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Task");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_isRunning,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_isRunning");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Set,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Set");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Status,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Status");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Enable,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Enable");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Disable,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Disable");

    UtTest_Add(Ut_CFE_PSP_MEM_SCRUB_Delete,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MEM_SCRUB_Delete");

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

    UtTest_Add(Ut_CFE_PSP_GetSpacecraftId,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetSpacecraftId");

    UtTest_Add(Ut_CFE_PSP_GetProcessorName,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetProcessorName");

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

    #endif /* TEST_WATCHDOG == 1 */

    /* Test NTP Sync */
    #if TEST_NTP_SYNC == 1

    

    #endif /* TEST_NTP_SYNC == 1 */
} 

/*=======================================================================================
** End of file psp_testcases.c
**=======================================================================================*/

