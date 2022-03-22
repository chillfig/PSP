# SP0 implementation of PSP

This is the PSP implementation for Aitch SP0 and SP0s targets flight computers

> **NOTE:** Aitech SP0 and SP0s do not support User Reserved Memory nor Persistent Memory.

[[_TOC_]]

___

## VxWorks Kernel Requirements

### Functions

Kernel must provide the following functions:

- GetWrsKernelTextStart()
- GetWrsKernelTextEnd()

### Components

Kernel must include the following services:

- FTP
  - Port 21
- Telnet
- User Reserved Memory (Currently not supported by Aitech SP0 bootloader)
- Watchdog
- NTP
- Datalight FlashFX (Aitech FLASH Memory)

## API

> **NOTE 1**: A `*` next to a name means that it is located in _cfe_psp_config.h_ file

> **NOTE 2**: A `^` next to a name means that it is configured by another define

> **NOTE 3**: A `$` next to a name means that it is defined in both Linux and SP0 targets

### Base PSP

- Defines
  - CFE_PSP_MAIN_FUNCTION `^` `$`
  - CFE_PSP_NONVOL_STARTUP_FILE `^` `$`
  - CFE_PSP_MEM_TABLE_SIZE `*`
  - CFE_PSP_MAX_EXCEPTION_ENTRIES `*`
  - CFE_PSP_VX_TIMEBASE_PERIOD_NUMERATOR `*`
  - CFE_PSP_VX_TIMEBASE_PERIOD_DENOMINATOR `*`
  - CFE_PSP_MEMALIGN_MASK `*`
  - CFE_PSP_CPU_ID `$`
  - CFE_PSP_CPU_NAME `$`
  - CFE_PSP_SPACECRAFT_ID `$`
  - PSP_EXCEP_PRINT_SCOPE

- Global Variables
  - CFE_PSP_ReservedMemoryMap `$`

- Static Variables
  - g_VxWorksTaskList
  - g_StartupInfo
  - g_uiShellTaskID
  - g_ucOverRideDefaultedrPolicyHandlerHook
  - g_pDefaultedrPolicyHandlerHook

- Functions Internal "Because defined as static"
  - CFE_PSP_ProcessResetType
  - CFE_PSP_SetSysTasksPrio

- Functions
  - CFE_PSP_Main
  - CFE_PSP_ProcessPOSTResults
  - CFE_PSP_LogSoftwareResetType
  - CFE_PSP_GetActiveCFSPartition
  - CFE_PSP_StartupTimer
  - CFE_PSP_StartupClear
  - CFE_PSP_StartupFailedRestartSP0_hook
  - OS_Application_Startup `$`
  - OS_Application_Run `$`
  - CFE_PSP_SuspendConsoleShellTask
  - CFE_PSP_GetRestartType
  - CFE_PSP_SetTaskPrio
  - CFE_PSP_Restart `$`
  - CFE_PSP_Panic `$`
  - CFE_PSP_FlushCaches `$`
  - CFE_PSP_GetProcessorId `$`
  - CFE_PSP_GetSpacecraftId `$`
  - CFE_PSP_GetProcessorName `$`
  - CFE_PSP_edrPolicyHandlerHook
  - CFE_PSP_AttachExceptions `$`
  - CFE_PSP_SetDefaultExceptionEnvironment `$`
  - CFE_PSP_ExceptionGetSummary_Impl `$`
  - CFE_PSP_GetResetArea `$`
  - CFE_PSP_GetUserReservedArea `$`
  - CFE_PSP_GetVolatileDiskMem `$`
  - CFE_PSP_InitProcessorReservedMemory `$`
  - CFE_PSP_SetupReservedMemoryMap `$`
  - CFE_PSP_DeleteProcessorReservedMemory `$`
  - CFE_PSP_GetKernelTextSegmentInfo `$`
  - CFE_PSP_GetCFETextSegmentInfo `$`
  - CFE_PSP_GetBootStartupString
  - CFE_PSP_SetBootStartupString
  - CFE_PSP_GetBootStructure
  - CFE_PSP_SetBootStructure
  - CFE_PSP_PrintBootParameters

### Active Memory Scrubbing

- Defines
  - MEM_SCRUB_PRINT_SCOPE
  - MEM_SCRUB_TASK_START_ON_STARTUP
  - MEM_SCRUB_DEFAULT_START_ADDR `*`
  - MEM_SCRUB_DEFAULT_END_ADDR `*`
  - MEMSCRUB_DEFAULT_PRIORITY `*`
  - MEMSCRUB_PRIORITY_UP_RANGE `*`
  - MEMSCRUB_PRIORITY_DOWN_RANGE `*`
  - MEMSCRUB_TASK_NAME `*`
  - PSP_MEM_SCRUB_BSEM_NAME `*`

- Structures
  - MEM_SCRUB_STATUS_t
  - MEM_SCRUB_ERRSTATS_t

- Public Functions
  - CFE_PSP_MEM_SCRUB_Set
  - CFE_PSP_MEM_SCRUB_isRunning
  - CFE_PSP_MEM_SCRUB_Delete
  - CFE_PSP_MEM_SCRUB_Status
  - CFE_PSP_MEM_SCRUB_Init
  - CFE_PSP_MEM_SCRUB_Enable
  - CFE_PSP_MEM_SCRUB_Disable
  - CFE_PSP_MEM_SCRUB_ErrStats

- Static Functions
  - CFE_PSP_MEM_SCRUB_Task

- Static Variables
  - g_uiEndOfRam
  - g_uiMemScrubTaskPriority
  - g_uiMemScrubTaskId
  - g_uiMemScrubStartAddr
  - g_uiMemScrubEndAddr
  - g_uiMemScrubCurrentPage
  - g_uiMemScrubTotalPages
  - g_uiSemUpdateMemAddr_id
  - g_bScrubAddrUpdates_flag

- Static Functions
  - CFE_PSP_MEM_SCRUB_Task

### FLASH Interaction

- Defines
  - CFE_PSP_CDS_FLASH_FILEPATH `*`
  - CFE_PSP_RESET_FLASH_FILEPATH `*`
  - CFE_PSP_VOLATILEDISK_FLASH_FILEPATH `*`
  - CFE_PSP_USERRESERVED_FLASH_FILEPATH `*`

- Structures

- Public Functions
  - CFE_PSP_FLASH_ReadFromFLASH
  - CFE_PSP_FLASH_WriteToFLASH
  - CFE_PSP_FLASH_DeleteFile
  - CFE_PSP_FLASH_CreateFile
  - CFE_PSP_FLASH_CheckFile
  - CFE_PSP_FLASH_CreateDirectory

- Static Functions

- Static Variables

### SP0 Info

- Defines
  - SP0_DATA_DUMP_FILEPATH `*`
  - SP0_TEXT_BUFFER_MAX_SIZE
  - SP0_SAFEMODEUSERDATA_BUFFER_SIZE
  - SP0_PRINT_SCOPE
  - SP0_UPGRADE_MAX_VOLTAGE_SENSORS'
  - SP0_ORIGINAL_MAX_VOLTAGE_SENSORS
  - SP0_ORIGINAL_MAX_TEMP_SENSORS

- Structures
  - SP0_info_table_t

- Public Functions
  - PSP_SP0_GetInfo
  - PSP_SP0_PrintToBuffer
  - PSP_SP0_GetInfoTable
  - PSP_SP0_DumpData
  - PSP_SP0_GetDiskFreeSize

- Static Functions
  - PSP_SP0_PrintToBuffer

- Static Variables
  - g_cSP0DataDump
  - g_iSP0DataDumpLength
  - g_sp0_info_table

### NTP Time Sync

- Defines
  - NTP_DAEMON_TASK_NAME `*`
  - CFE_MISSION_TIME_EPOCH_UNIX_DIFF `*`
  - CFE_1HZ_TASK_NAME `*`
  - NTPSYNC_INITIAL_TIME_DELAY `*`
  - NTPSYNC_MAX_ITERATION_TIME_DELAY `*`
  - CFE_MISSION_TIME_SYNC_OS_ENABLE `*`
  - CFE_MISSION_TIME_SYNC_OS_SEC `*`
  - NTPSYNC_TASK_NAME `*`
  - NTPSYNC_DEFAULT_PRIORITY `*`
  - NTPSYNC_PRIORITY_UP_RANGE `*`
  - NTPSYNC_PRIORITY_DOWN_RANGE `*`
  - NTPSYNC_PRINT_SCOPE
  - CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP

- Structures

- Public Functions
  - CFE_PSP_TIME_NTPSync_Task_Enable
  - CFE_PSP_TIME_NTPSync_Task_Disable
  - CFE_PSP_TIME_NTPSync_Task_isRunning
  - CFE_PSP_TIME_NTPSync_Task_Priority_Set
  - CFE_PSP_TIME_NTP_Daemon_isRunning
  - ntp_clock_vxworks_Destroy
  - CFE_PSP_TIME_NTPSync_GetFreq
  - CFE_PSP_TIME_NTPSync_SetFreq
  - CFE_PSP_TIME_Set_OS_Time
  - CFE_PSP_TIME_Get_OS_Time
  - CFE_PSP_TIME_CFETimeService_isRunning
  - CFE_PSP_TIME_StartNTPDaemon
  - CFE_PSP_TIME_StopNTPDaemon

- Static Functions
  - CFE_PSP_TIME_NTPSync_Task

- Static Variables
  - g_uiPSPNTPTask_id
  - g_ucNTPSyncTaskPriority
  - g_bEnableGetTimeFromOS_flag
  - g_usOSTimeSync_Sec

### Watchdog
- Defines
  - CFE_PSP_WATCHDOG_MIN `*`
  - CFE_PSP_WATCHDOG_MAX `*`
  - CFE_PSP_WATCHDOG_DEFAULT_MSEC `*`

- Structures

- Public Functions
  - CFE_PSP_WatchdogInit `$`
  - CFE_PSP_WatchdogEnable `$`
  - CFE_PSP_WatchdogDisable `$`
  - CFE_PSP_WatchdogService `$`
  - CFE_PSP_WatchdogGet `$`     "getter for g_uiCFE_PSP_WatchdogValue"
  - CFE_PSP_WatchdogSet `$`     "setter for g_uiCFE_PSP_WatchdogValue"
  - CFE_PSP_WatchdogStatus `$`

- Static Functions

- Static Variables
  - g_uiCFE_PSP_WatchdogValue_ms
  - g_bWatchdogStatus

### Memory/Memory Sync
- Defines
  - enum MEMORY_SECTION

- Structures

- Public Functions
  - CFE_PSP_MEMORY_SYNC_Init
  - CFE_PSP_MEMORY_SYNC_Destroy
  - CFE_PSP_MEMORY_SYNC_Start
  - CFE_PSP_MEMORY_SYNC_Stop
  - CFE_PSP_MEMORY_SYNC_isRunning
  - CFE_PSP_MEMORY_SYNC_setPriority
  - CFE_PSP_MEMORY_SYNC_getPriority
  - CFE_PSP_MEMORY_SYNC_setFrequency
  - CFE_PSP_MEMORY_SYNC_getFrequency
  - CFE_PSP_MEMORY_SYNC_getStatus

- Static Functions
  - CFE_PSP_MEMORY_GetMemSize
  - CFE_PSP_MEMORY_GetMemArea
  - CFE_PSP_MEMORY_ReadFromRAM
  - CFE_PSP_MEMORY_WriteToRAM
  - CFE_PSP_MEMORY_RestoreRESET
  - CFE_PSP_MEMORY_RestoreCDS
  - CFE_PSP_MEMORY_RestoreVOLATILEDISK
  - CFE_PSP_MEMORY_RestoreUSERRESERVED
  - CFE_PSP_MEMORY_RestoreDATA
  - CFE_PSP_MEMORY_SYNC_Task
  - CFE_PSP_MEMORY_SYNC_CDS_FPATH
  - CFE_PSP_MEMORY_SYNC_RESET_FPATH
  - CFE_PSP_MEMORY_SYNC_VOLATILEDISK_FPATH
  - CFE_PSP_MEMORY_SYNC_USERRESERVED_FPATH
  - CFE_PSP_MEMORY_SYNC_GenerateFilepath
  - CFE_PSP_MEMORY_FlushToFLASH
  - CFE_PSP_MEMORY_CheckURMFilesExists

- Static Variables
  - g_ReservedMemBlock
  - g_RESETBinSemId
  - g_CDSBinSemId
  - g_VOLATILEDISKBinSemId
  - g_USERRESERVEDBinSemId
  - g_RESETUpdateFlag
  - g_CDSUpdateFlag
  - g_VOLATILEDISKUpdateFlag
  - g_USERRESERVEDUpdateFlag
  - g_uiMemorySyncTime
  - g_MemorySYncStatistics
  - g_uiMemorySyncTaskId
  - g_uiMemorySyncTaskPriority
  - g_MemorySyncTaskBinSem
  - g_CDSFilepath
  - g_RESETFilepath
  - g_VOLATILEDISKFilepath
  - g_USERRESERVEDFilepath