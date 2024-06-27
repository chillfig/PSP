*********************************************************************************************************
# Export Control Marking
 
All files under this top-level directory, including all files in its sub-directories, are subject to
NASA Export Control restrictions, as stated below:
 
EAR ECCN 9E515.a and/or 9E515.f (HALO)
 
    Export Administration Regulations (EAR) Notice
    This document contains information which falls under the purview of the Export Administration Regulations (EAR),
    15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
    or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
    furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
    or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
    of the Gateway MOUs and IA requires prior U.S. Government authorization.
**********************************************************************************************************

# GR740 implementation of PSP

This is the PSP implementation for Cobham Gaisler GR740 targets flight computers

[[_TOC_]]

___

## VxWorks Kernel Requirements

### Functions

Kernel must provide the following functions:

- GetWrsKernelTextStart()
- GetWrsKernelTextEnd()
- address_of_active_flash_partition_name (Optional)

### Components

Kernel must include the following services:

- FTP
  - Port 21
- Telnet
- User Reserved Memory
- Watchdog
- NTP

## API

> **NOTE 1**: A `*` next to a name means that it is located in _cfe_psp_config.h_ file
> **NOTE 2**: A `^` next to a name means that it is configured by another define
> **NOTE 3**: A `$` next to a name means that it is defined for all targets
> **NOTE 4**: A `@` next to a function name means that it is Thread-Safe and Reentrant

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
  - CFE_PSP_LogSoftwareResetType

- Functions
  - CFE_PSP_Main
  - CFE_PSP_ProcessPOSTResults
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
  - CFE_PSP_GetCDSSize `$`
  - CFE_PSP_WriteToCDS `$`
  - CFE_PSP_ReadFromCDS `$`
  - CFE_PSP_WriteFromCDS `$`
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

The define MEM_SCRUB_RUN_MODE defines how the Memory Scrubbing works in CFS.

When Run Mode is set to Idle Mode or Timed Mode, a Task is created during initialization via the function CFE_PSP_MemScrubInit. The Task runs in a low priority continuos while loop in automatic, but in Timed Mode, the while loop includes a task delay defined by MEM_SCRUB_TASK_DELAY_MSEC. It can only be terminated by using functions CFE_PSP_MemScrubDisable or CFE_PSP_MemScrubDelete.

When running in Manual Mode, no task is created. Run To call memory scrubbing

- Defines
  - MEMSCRUB_PRINT_SCOPE
  - MEMSCRUB_PAGE_SIZE
  - MEMSCRUB_TASK_START_ON_STARTUP `*`
  - MEMSCRUB_RUN_MODE `*`
  - MEMSCRUB_BLOCKSIZE_PAGES `*`
  - MEMSCRUB_TASK_DELAY_MSEC `*`
  - MEMSCRUB_DEFAULT_START_ADDR `*`
  - MEMSCRUB_DEFAULT_END_ADDR `*`
  - MEMSCRUB_DEFAULT_PRIORITY `*`
  - MEMSCRUB_PRIORITY_UP_RANGE `*`
  - MEMSCRUB_PRIORITY_DOWN_RANGE `*`
  - MEMSCRUB_TASK_NAME `*`
  - MEMSCRUB_BSEM_NAME `*`

- Structures
  - CFE_PSP_MemScrubStatus_t `^`
  - CFE_PSP_MemScrubErrStats_t `^`

- Public Functions
  - CFE_PSP_MemScrubInit
  - CFE_PSP_MemScrubSet
  - CFE_PSP_MemScrubGet
  - CFE_PSP_MemScrubEnable
  - CFE_PSP_MemScrubDisable
  - CFE_PSP_MemScrubDelete
  - CFE_PSP_MemScrubTrigger
  - CFE_PSP_MemScrubIsRunning
  - CFE_PSP_MemScrubErrStats

- Static Functions
  - CFE_PSP_MemScrubTask
  - CFE_PSP_MemScrubValidate

- Static Variables
  - g_uiEndOfRam
  - g_MemScrub_Status
  - g_uiSemUpdateMemAddr_id
  - g_bScrubAddrUpdates_flag

- Static Functions
  - CFE_PSP_MemScrubTask

### FLASH Interaction

- Defines
  - CFE_PSP_CDS_FLASH_FILEPATH `*`
  - CFE_PSP_RESET_FLASH_FILEPATH `*`
  - CFE_PSP_VOLATILEDISK_FLASH_FILEPATH `*`
  - CFE_PSP_USERRESERVED_FLASH_FILEPATH `*`

- Structures

- Public Functions
  - CFE_PSP_ReadFromFlash
  - CFE_PSP_WriteToFlash
  - CFE_PSP_DeleteFile
  - CFE_PSP_CreateFile
  - CFE_PSP_CheckFile
  - CFE_PSP_CreateDirectory

- Static Functions

- Static Variables

### GR740 Info

- Defines
  - SP0_DATA_DUMP_FILEPATH `*`
  - SP0_TEXT_BUFFER_MAX_SIZE
  - SP0_SAFEMODEUSERDATA_BUFFER_SIZE
  - SP0_PRINT_SCOPE
  - SP0_UPGRADE_MAX_VOLTAGE_SENSORS'
  - SP0_ORIGINAL_MAX_VOLTAGE_SENSORS
  - SP0_ORIGINAL_MAX_TEMP_SENSORS

- Structures
  - CFE_PSP_SP0InfoTable_t

- Public Functions
  - CFE_PSP_SP0GetInfo
  - CFE_PSP_SP0PrintToBuffer
  - CFE_PSP_SP0GetInfoTable
  - CFE_PSP_SP0DumpData
  - CFE_PSP_SP0GetDiskFreeSize

- Static Functions
  - CFE_PSP_SP0PrintToBuffer

- Static Variables
  - g_cSP0DataDump
  - g_iSP0DataDumpLength
  - g_sp0_info_table

### NTP Time Sync

- Defines
  - CFE_MISSION_TIME_EPOCH_UNIX_DIFF `*`
  - NTP_DAEMON_TASK_NAME `*`
  - NTPSYNC_PRINT_SCOPE

- Structures

- Public Functions
  - CFE_PSP_NTPDaemonIsRunning
  - CFE_PSP_SetOSTime
  - CFE_PSP_GetOSTime
  - CFE_PSP_NTPDaemonIsRunning
  - CFE_PSP_StartNTPDaemon
  - CFE_PSP_StopNTPDaemon

- Static Functions

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

### Memory/Memory Sync (**MISSING**)

- Defines
  - MEMORY_SECTION_t

- Structures

- Public Functions
  - CFE_PSP_MemSyncInit
  - CFE_PSP_MemSyncDestroy
  - CFE_PSP_MemSyncStart
  - CFE_PSP_MemSyncStop
  - CFE_PSP_MemSyncIsRunning
  - CFE_PSP_MemSyncSetPriority
  - CFE_PSP_MemSyncGetPriority
  - CFE_PSP_MemSyncSetFrequency
  - CFE_PSP_MemSyncGetFrequency
  - CFE_PSP_MemSyncgetStatus

- Static Functions
  - CFE_PSP_GetMemSize
  - CFE_PSP_GetMemArea
  - CFE_PSP_ReadFromRAM
  - CFE_PSP_WriteToRAM
  - CFE_PSP_RestoreReset
  - CFE_PSP_RestoreCDS
  - CFE_PSP_RestoreVolatileDisk
  - CFE_PSP_RestoreUserReserved
  - CFE_PSP_RestoreData
  - CFE_PSP_MemSyncTask
  - CFE_PSP_CDSFilepath
  - CFE_PSP_ResetFilepath
  - CFE_PSP_VolatileDiskFilepath
  - CFE_PSP_UserReservedFilepath
  - CFE_PSP_GenerateFilepath
  - CFE_PSP_FlushToFLASH
  - CFE_PSP_CheckURMFilesExists

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
