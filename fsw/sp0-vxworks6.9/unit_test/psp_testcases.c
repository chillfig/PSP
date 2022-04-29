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
#define TEST_SP0_INFO   1
#define TEST_MEMORY     1
#define TEST_FLASH      1
#define TEST_MEM_SCRUB  1
#define TEST_EXCEPTION  1
#define TEST_SUPPORT    1
#define TEST_WATCHDOG   1
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

    #if TEST_SP0_INFO == 1
    /* cfe_psp_sp0info.c test cases area */

    UtTest_Add(Ut_CFE_PSP_SP0GetInfo,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0GetInfo");

    UtTest_Add(Ut_CFE_PSP_SP0PrintToBuffer,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0PrintToBuffer");

    UtTest_Add(Ut_CFE_PSP_SP0GetInfoTable,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0GetInfoTable");

    UtTest_Add(Ut_CFE_PSP_SP0DumpData,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0DumpData");

    UtTest_Add(Ut_CFE_PSP_SP0GetDiskFreeSize,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0GetDiskFreeSize");

    UtTest_Add(Ut_CFE_PSP_SP0ROM1Lock,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROM1Lock");

    UtTest_Add(Ut_CFE_PSP_SP0ROM1Unlock,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROM1Unlock");

    UtTest_Add(Ut_CFE_PSP_SP0ROM2Lock,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROM2Lock");

    UtTest_Add(Ut_CFE_PSP_SP0ROM2Unlock,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROM1Unlock");

    UtTest_Add(Ut_CFE_PSP_SP0ROMXCmd,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROMXCmd");

    UtTest_Add(Ut_CFE_PSP_SP0ROM1Status,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROM1Status");

    UtTest_Add(Ut_CFE_PSP_SP0ROM2Status,
              Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
              "Ut_CFE_PSP_SP0ROM2Status");

    #endif /* TEST_SP0_INFO == 1 */
    
    #if TEST_MEMORY == 1
    /* cfe_psp_memory.c test cases area */

    UtTest_Add(Ut_CFE_PSP_GetCDSSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCDSSize");

    UtTest_Add(Ut_CFE_PSP_CalculateCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CalculateCRC");

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

    UtTest_Add(Ut_CFE_PSP_GetRESETSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetRESETSize");

    UtTest_Add(Ut_CFE_PSP_GetVOLATILEDISKSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetVOLATILEDISKSize");

    UtTest_Add(Ut_CFE_PSP_GetUSERRESERVEDSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetUSERRESERVEDSize");

    UtTest_Add(Ut_CFE_PSP_GetMemSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetMemSize");

    UtTest_Add(Ut_CFE_PSP_GetMemArea,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetMemArea");

    UtTest_Add(Ut_CFE_PSP_GetCDSArea,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCDSArea");

    UtTest_Add(Ut_CFE_PSP_WriteToRESET,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteToRESET");

    UtTest_Add(Ut_CFE_PSP_ReadFromRESET,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadFromRESET");

    UtTest_Add(Ut_CFE_PSP_WriteToVOLATILEDISK,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteToVOLATILEDISK");

    UtTest_Add(Ut_CFE_PSP_ReadFromVOLATILEDISK,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadFromVOLATILEDISK");

    UtTest_Add(Ut_CFE_PSP_WriteToUSERRESERVED,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteToUSERRESERVED");

    UtTest_Add(Ut_CFE_PSP_ReadFromUSERRESERVED,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadFromUSERRESERVED");

    UtTest_Add(Ut_CFE_PSP_RestoreCDS,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_RestoreCDS");

    UtTest_Add(Ut_CFE_PSP_RestoreReset,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_RestoreReset");

    UtTest_Add(Ut_CFE_PSP_RestoreVolatileDisk,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_RestoreVolatileDisk");

    UtTest_Add(Ut_CFE_PSP_RestoreUserReserved,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_RestoreUserReserved");

    UtTest_Add(Ut_CFE_PSP_WriteToRAM,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteToRAM");

    UtTest_Add(Ut_CFE_PSP_ReadFromRAM,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadFromRAM");

    UtTest_Add(Ut_CFE_PSP_RestoreData,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_RestoreData");

    UtTest_Add(Ut_CFE_PSP_MemSyncInit,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncInit");

    UtTest_Add(Ut_CFE_PSP_MemSyncDestroy,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncDestroy");

    UtTest_Add(Ut_CFE_PSP_MemSyncStart,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncStart");

    UtTest_Add(Ut_CFE_PSP_MemSyncStop,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncStop");

    UtTest_Add(Ut_CFE_PSP_MemSyncIsRunning,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncIsRunning");

    UtTest_Add(Ut_CFE_PSP_MemSyncSetPriority,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncSetPriority");

    UtTest_Add(Ut_CFE_PSP_MemSyncGetPriority,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncGetPriority");

    UtTest_Add(Ut_CFE_PSP_MemSyncSetFrequency,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncSetFrequency");

    UtTest_Add(Ut_CFE_PSP_MemSyncGetFrequency,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncGetFrequency");

    UtTest_Add(Ut_CFE_PSP_MemSyncgetStatus,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncgetStatus");

    UtTest_Add(Ut_CFE_PSP_MemSyncTask,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_MemSyncTask");

    UtTest_Add(Ut_CFE_PSP_CDSFilepath,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CDSFilepath");

    UtTest_Add(Ut_CFE_PSP_ResetFilepath,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ResetFilepath");

    UtTest_Add(Ut_CFE_PSP_VolatileDiskFilepath,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_VolatileDiskFilepath");

    UtTest_Add(Ut_CFE_PSP_UserReservedFilepath,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_UserReservedFilepath");

    UtTest_Add(Ut_CFE_PSP_GenerateFilepath,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GenerateFilepath");

    UtTest_Add(Ut_CFE_PSP_CheckURMFilesExists,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CheckURMFilesExists");

    UtTest_Add(Ut_CFE_PSP_FlushToFLASH,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_FlushToFLASH");

    #endif /* TEST_MEMORY == 1 */

    #if TEST_FLASH == 1
    /* cfe_psp_flash.c test case area */

    UtTest_Add(Ut_CFE_PSP_ReadFromFlash,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadFromFlash");

    UtTest_Add(Ut_CFE_PSP_WriteToFlash,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteToFlash");

    UtTest_Add(Ut_CFE_PSP_DeleteFile,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_DeleteFile");

    UtTest_Add(Ut_CFE_PSP_CreateFile,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CreateFile");

    UtTest_Add(Ut_CFE_PSP_CheckFile,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CheckFile");

    UtTest_Add(Ut_CFE_PSP_CreateDirectory,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CreateDirectory");

    #endif /* TEST_FLASH == 1 */

    #if TEST_MEM_SCRUB == 1
    /* cfe_psp_memscrub.c test cases area*/

    UtTest_Add(Ut_CFE_PSP_MemScrubInit,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubInit");

    /* Below test will never exit unless task is deleted */
    UtTest_Add(Ut_CFE_PSP_MemScrubTask,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubTask");

    UtTest_Add(Ut_CFE_PSP_MemScrubIsRunning,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubIsRunning");

    UtTest_Add(Ut_CFE_PSP_MemScrubSet,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubSet");

    UtTest_Add(Ut_CFE_PSP_MemScrubStatus,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubStatus");

    UtTest_Add(Ut_CFE_PSP_MemScrubEnable,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubEnable");

    UtTest_Add(Ut_CFE_PSP_MemScrubDisable,
            Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
            "Ut_CFE_PSP_MemScrubDisable");

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

    UtTest_Add(Ut_CFE_PSP_LoadFromNVRAM,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_LoadFromNVRAM");

    UtTest_Add(Ut_CFE_PSP_SaveToNVRAM,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SaveToNVRAM");

    UtTest_Add(Ut_CFE_PSP_ClearNVRAM,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ClearNVRAM");

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

    UtTest_Add(Ut_CFE_PSP_GetBootStartupString,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetBootStartupString");

    UtTest_Add(Ut_CFE_PSP_ToggleCFSBootPartition,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ToggleCFSBootPartition");

    UtTest_Add(Ut_CFE_PSP_SetBootStartupString,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetBootStartupString");

    UtTest_Add(Ut_CFE_PSP_GetBootStructure,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetBootStructure");

    UtTest_Add(Ut_CFE_PSP_SetBootStructure,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetBootStructure");

    UtTest_Add(Ut_CFE_PSP_PrintBootParameters,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_PrintBootParameters");


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

    UtTest_Add(Ut_CFE_PSP_StartupFailed,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_StartupFailed");

    UtTest_Add(Ut_CFE_PSP_StartupClear,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_StartupClear");

    UtTest_Add(Ut_CFE_PSP_GetActiveCFSPartition,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetActiveCFSPartition");

    UtTest_Add(Ut_CFE_PSP_StartupTimer,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_StartupTimer");

    UtTest_Add(Ut_CFE_PSP_GetRestartType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetRestartType"); 

    UtTest_Add(Ut_CFE_PSP_SetTaskPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetTaskPrio");

    UtTest_Add(Ut_CFE_PSP_SetSysTasksPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetSysTasksPrio");

    UtTest_Add(Ut_CFE_PSP_SetFileSysAddFixedMap,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetFileSysAddFixedMap");

    UtTest_Add(Ut_CFE_PSP_InitSSR,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_InitSSR");

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
    wdbUserEvtPost ("End of Tests");
}

/*=======================================================================================
** End of file psp_testcases.c
**=======================================================================================*/

