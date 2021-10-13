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
  - CFE_MODULE_NAME
  - CFE_PSP_CPU_ID `$`
  - CFE_PSP_CPU_NAME `$`
  - CFE_PSP_SPACECRAFT_ID `$`
  - PSP_EXCEP_PRINT_SCOPE

- Global Variables
  - CFE_PSP_ReservedMemoryMap `$`

- Static Variables
  - g_ReservedMemBlock
  - g_VxWorksTaskList
  - g_uiResetType
  - g_uiResetSubtype
  - g_safeModeUserData
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

### Active Memory Scrubbing

- Defines
  - MEMSCRUB_TASK_STACK_SIZE
  - MEMSCRUB_DEFAULT_PRIORITY `*`
  - MEMSCRUB_PRIORITY_UP_RANGE `*`
  - MEMSCRUB_PRIORITY_DOWN_RANGE `*`
  - MEMSCRUB_TASK_NAME `*`
  - MEM_SCRUB_PRINT_SCOPE

- Static Variables
  - g_uiMemScrubTaskPriority
  - g_uiMemScrubTaskId
  - g_uiMemScrubStartAddr
  - g_uiMemScrubEndAddr
  - g_uiMemScrubCurrentPage
  - g_uiMemScrubTotalPages
  - g_uiEndOfRam

- Functions
  - CFE_PSP_MEM_SCRUB_Set
  - CFE_PSP_MEM_SCRUB_Status
  - CFE_PSP_MEM_SCRUB_Task
  - CFE_PSP_MEM_SCRUB_Init
  - CFE_PSP_MEM_SCRUB_isRunning
  - CFE_PSP_MEM_SCRUB_Delete
  - CFE_PSP_MEM_SCRUB_Enable
  - CFE_PSP_MEM_SCRUB_Disable

### Critical Device Storage (CDS)

- Defines
  - CFE_PSP_CFE_FLASH_FILEPATH `*`

- Static Variables
  - g_cCDSFileName    "The full file path where the CDS file will be saved"
  - g_uiCDSCrc        "CDS CRC value"
  - g_bCorruptedCDSFlash "Identify if the CDS file in Flash memory is corrupted of not"

- Functions
  - CFE_PSP_SetStaticCRC
  - CFE_PSP_GetStaticCRC
  - CFE_PSP_CalculateCRC
  - CFE_PSP_ReadCDSFromFlash
  - CFE_PSP_WriteCDSToFlash
  - CFE_PSP_GetCDSSize `$`
  - CFE_PSP_WriteToCDS `$`
  - CFE_PSP_ReadFromCDS `$`

### SP0 Info

- Defines
  - SP0_DATA_DUMP_FILEPATH `*`
  - SP0_TEXT_BUFFER_MAX_SIZE
  - SP0_SAFEMODEUSERDATA_BUFFER_SIZE
  - SP0_PRINT_SCOPE

- Static Variables
  - g_cSP0DataDump
  - g_iSP0DataDumpLength

- Functions
  - PSP_SP0_GetInfo
  - PSP_SP0_PrintInfoTable
  - PSP_SP0_DumpData

### NTP Time Sync

- Defines
  - CFE_MISSION_TIME_SYNC_OS_SEC
  - CFE_MISSION_TIME_SYNC_OS_ENABLE
  - NTPSYNC_TASK_NAME
  - NTPSYNC_DEFAULT_PRIORITY
  - NTPSYNC_PRINT_SCOPE

- Static Variables
  - g_uiPSPNTPTask_id
  - g_ucNTPSyncTaskPriority
  - g_iEnableGetTimeFromOS_flag
  - g_usOSTimeSync_Sec

- Functions (include psp_time_sync.h)
  - CFE_PSP_TIME_Init
  - CFE_PSP_Update_OS_Time
  - CFE_PSP_StartNTPDaemon
  - CFE_PSP_StopNTPDaemon
  - CFE_PSP_TimeService_Ready
  - CFE_PSP_Sync_From_OS_Enable
  - CFE_PSP_Sync_From_OS_GetFreq
  - CFE_PSP_Sync_From_OS_SetFreq
  - CFE_PSP_Set_OS_Time
  - CFE_PSP_Get_OS_Time
  - CFE_PSP_NTP_Daemon_Get_Status
  - CFE_PSP_NTP_Daemon_Enable

### Watchdog

- Defines
  - CFE_PSP_WATCHDOG_MIN `*`
  - CFE_PSP_WATCHDOG_MAX `*`
  - CFE_PSP_WATCHDOG_DEFAULT_MSEC `*`

- Static Variables
  - g_uiCFE_PSP_WatchdogValue_ms

- Functions
  - CFE_PSP_WatchdogInit `$`
  - CFE_PSP_WatchdogEnable `$`
  - CFE_PSP_WatchdogDisable `$`
  - CFE_PSP_WatchdogService `$`
  - CFE_PSP_WatchdogGet `$`     "getter for g_uiCFE_PSP_WatchdogValue"
  - CFE_PSP_WatchdogSet `$`     "setter for g_uiCFE_PSP_WatchdogValue"
