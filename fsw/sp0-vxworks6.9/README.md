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

# SP0 implementation of PSP

This is the PSP implementation for Aitch SP0 and SP0s targets flight computers

> **NOTE:** Aitech SP0 and SP0s do not support User Reserved Memory nor Persistent Memory.

[TOC]
___

## VxWorks Kernel Requirements

### Functions

Kernel must provide the following functions:

- GetWrsKernelTextStart()
- GetWrsKernelTextEnd()
- address_of_active_flash_partition_name (Optional)

### Components

Kernel services:

- FTP (Optional)
  - Used for transferring files from host to target
- Telnet (Optional)
  - Used for running Functional and Unit tests
- User Reserved Memory
- Watchdog
- NTP
  - PSP is configured for full fledged NTP. To use SNTP, the cfe_psp_config.h needs to be modified.
- Datalight FlashFX
  - This is the driver for communicating with the Aitech FLASH memory.

## API

> **NOTE 1**: A `*` next to a name means that it is located in _cfe_psp_config.h_ file

> **NOTE 2**: A `^` next to a name means that it is configured by another define

> **NOTE 3**: A `$` next to a name means that it is defined in both Linux and SP0 targets

> **NOTE 4**: A `@` next to a function name means that it is Thread-Safe and Reentrant

### Base PSP

- Defines
  - #CFE_PSP_MAIN_FUNCTION `^` `$`
  - #CFE_PSP_NONVOL_STARTUP_FILE `^` `$`
  - #CFE_PSP_MEM_TABLE_SIZE `*`
  - #CFE_PSP_MAX_EXCEPTION_ENTRIES `*`
  - #CFE_PSP_VX_TIMEBASE_PERIOD_NUMERATOR `*`
  - #CFE_PSP_VX_TIMEBASE_PERIOD_DENOMINATOR `*`
  - #CFE_PSP_MEMALIGN_MASK `*`
  - #CFE_PSP_CPU_ID `$`
  - #CFE_PSP_CPU_NAME `$`
  - #CFE_PSP_SPACECRAFT_ID `$`
  - #PSP_EXCEP_PRINT_SCOPE

- Global Variables
  - #CFE_PSP_ReservedMemoryMap `$`

- Public Functions
  - CFE_PSP_Main()
  - CFE_PSP_ProcessPOSTResults()
  - CFE_PSP_GetActiveCFSPartition()
  - CFE_PSP_StartupTimer()
  - CFE_PSP_StartupClear()
  - CFE_PSP_StartupFailedRestartSP0_hook()
  - OS_Application_Startup() `$`
  - OS_Application_Run() `$`
  - CFE_PSP_SuspendConsoleShellTask()
  - CFE_PSP_GetRestartType()
  - CFE_PSP_SetTaskPrio()
  - CFE_PSP_Restart() `$`
  - CFE_PSP_Panic() `$`
  - CFE_PSP_FlushCaches() `$`
  - CFE_PSP_GetProcessorId() `$`
  - CFE_PSP_GetSpacecraftId() `$`
  - CFE_PSP_GetProcessorName() `$`
  - CFE_PSP_edrPolicyHandlerHook()
  - CFE_PSP_AttachExceptions() `$`
  - CFE_PSP_SetDefaultExceptionEnvironment() `$`
  - CFE_PSP_ExceptionGetSummary_Impl() `$`
  - CFE_PSP_GetResetArea() `$`
  - CFE_PSP_GetUserReservedArea() `$`
  - CFE_PSP_GetVolatileDiskMem() `$`
  - CFE_PSP_InitProcessorReservedMemory() `$`
  - CFE_PSP_SetupReservedMemoryMap() `$`
  - CFE_PSP_DeleteProcessorReservedMemory() `$`
  - CFE_PSP_GetKernelTextSegmentInfo() `$`
  - CFE_PSP_GetCFETextSegmentInfo() `$`
  - CFE_PSP_GetBootStartupString()
  - CFE_PSP_SetBootStartupString()
  - CFE_PSP_GetBootStructure()
  - CFE_PSP_SetBootStructure()
  - CFE_PSP_PrintBootParameters()
  - CFE_PSP_KernelGetCRC()
  - CFE_PSP_KernelLoadNew()

- Static Variables
  - #g_VxWorksTaskList
  - #g_StartupInfo
  - #g_uiShellTaskID
  - #g_ucOverRideDefaultedrPolicyHandlerHook
  - #g_pDefaultedrPolicyHandlerHook

- Static Functions
  - CFE_PSP_ValidatePath()
  - CFE_PSP_ProcessResetType()
  - CFE_PSP_SetSysTasksPrio()
  - CFE_PSP_LogSoftwareResetType()

### Active Memory Scrubbing

The define #CFE_PSP_MEMSCRUB_RUN_MODE defines how the Memory Scrubbing works in CFS.

When Run Mode is set to Idle Mode or Timed Mode, a Task is created during initialization via the function CFE_PSP_MemScrubInit(). The Task runs in a low priority continuos while loop in automatic, but in Timed Mode, the while loop includes a task delay defined by #CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC. It can only be terminated by using functions CFE_PSP_MemScrubDisable() or CFE_PSP_MemScrubDelete().

When running in Manual Mode, no task is created. Run To call memory scrubbing

- Defines
  - #MEMSCRUB_PRINT_SCOPE
  - #MEMSCRUB_PAGE_SIZE
  - #CFE_PSP_MEMSCRUB_TASK_START_ON_STARTUP `*`
  - #CFE_PSP_MEMSCRUB_RUN_MODE `*`
  - #CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES `*`
  - #CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC `*`
  - #CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR `*`
  - #CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR `*`
  - #CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY `*`
  - #CFE_PSP_MEMSCRUB_PRIORITY_UP_RANGE `*`
  - #CFE_PSP_MEMSCRUB_PRIORITY_DOWN_RANGE `*`
  - #CFE_PSP_MEMSCRUB_TASK_NAME `*`
  - #CFE_PSP_MEMSCRUB_BSEM_NAME `*`

- Structures
  - #CFE_PSP_MemScrubStatus_t `^`
  - #CFE_PSP_MemScrubErrStats_t `^`

- Public Functions
  - CFE_PSP_MemScrubInit()
  - CFE_PSP_MemScrubSet()
  - CFE_PSP_MemScrubGet()
  - CFE_PSP_MemScrubEnable()
  - CFE_PSP_MemScrubDisable()
  - CFE_PSP_MemScrubDelete()
  - CFE_PSP_MemScrubTrigger()
  - CFE_PSP_MemScrubIsRunning()
  - CFE_PSP_MemScrubErrStats()

- Static Functions
  - CFE_PSP_MemScrubTask()
  - CFE_PSP_MemScrubValidate()

### FLASH Interaction

- Defines
  - #CFE_PSP_CDS_FLASH_FILEPATH `*`
  - #CFE_PSP_RESET_FLASH_FILEPATH `*`
  - #CFE_PSP_VOLATILEDISK_FLASH_FILEPATH `*`
  - #CFE_PSP_USERRESERVED_FLASH_FILEPATH `*`

- Public Functions
  - CFE_PSP_ReadFromFlash()
  - CFE_PSP_WriteToFlash()
  - CFE_PSP_DeleteFile()
  - CFE_PSP_CreateFile()
  - CFE_PSP_CheckFile()
  - CFE_PSP_CreateDirectory()

### SP0 Info

- Defines
  - #CFE_PSP_SP0_DATA_DUMP_FILEPATH `*`
  - #SP0_TEXT_BUFFER_MAX_SIZE
  - #SP0_SAFEMODEUSERDATA_BUFFER_SIZE
  - #SP0_PRINT_SCOPE
  - #SP0_UPGRADE_MAX_VOLT_SENSORS
  - #SP0_ORIGINAL_MAX_VOLT_SENSORS
  - #SP0_UPGRADE_MAX_TEMP_SENSORS
  - #SP0_ORIGINAL_MAX_TEMP_SENSORS
  - #SP0_ROM1_CODE_LOCK
  - #SP0_ROM1_CODE_UNLOCK
  - #SP0_ROM2_CODE_LOCK
  - #SP0_ROM2_CODE_UNLOCK
  - #SP0_BOOT_ROM_STATUS_ADDR
  - #SP0_ROM1_MASK
  - #SP0_ROM2_MASK
  - #SP0_ROM1_STATUS_SHIFT
  - #SP0_ROM2_STATUS_SHIFT

- Structures
  - #CFE_PSP_SP0StaticInfoTable_t
  - #CFE_PSP_SP0DynamicInfoTable_t

- Public Functions
  - CFE_PSP_SP0CollectStaticInfo()
  - CFE_PSP_SP0CollectDynamicInfo()
  - CFE_PSP_SP0PrintToBuffer()
  - CFE_PSP_SP0GetStaticInfoTable()
  - CFE_PSP_SP0GetDynamicInfoTable()
  - CFE_PSP_SP0DumpData()
  - CFE_PSP_SP0GetDiskFreeSize()
  - CFE_PSP_SP0ROM1Lock()
  - CFE_PSP_SP0ROM1Status()
  - CFE_PSP_SP0ROM1Unlock()
  - CFE_PSP_SP0ROM2Lock()
  - CFE_PSP_SP0ROM2Status()
  - CFE_PSP_SP0ROM2Unlock()

- Static Functions
  - CFE_PSP_SP0PrintToBuffer()
  - CFE_PSP_SP0ROMXCmd()

- Static Variables
  - #g_cSP0DataDump
  - #g_iSP0DataDumpLength
  - #g_SP0StaticInfoTable
  - #g_SP0DynamicInfoTable

### NTP Time Sync

- Defines
  - #CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS `*`
  - #CFE_MISSION_TIME_EPOCH_UNIX_DIFF_MICROSECS `*`
  - #CFE_PSP_NTP_DAEMON_TASK_NAME `*`
  - #NTPSYNC_PRINT_SCOPE

- Public Functions
  - CFE_PSP_NTPDaemonIsRunning()
  - CFE_PSP_SetOSTime()
  - CFE_PSP_GetOSTime()
  - CFE_PSP_NTPDaemonIsRunning()
  - CFE_PSP_StartNTPDaemon()
  - CFE_PSP_StopNTPDaemon()

### Watchdog

- Defines
  - #CFE_PSP_WATCHDOG_MIN `*`
  - #CFE_PSP_WATCHDOG_MAX `*`
  - #CFE_PSP_WATCHDOG_DEFAULT_MSEC `*`

- Public Functions
  - CFE_PSP_WatchdogInit() `$`
  - CFE_PSP_WatchdogEnable() `$`
  - CFE_PSP_WatchdogDisable() `$`
  - CFE_PSP_WatchdogService() `$`
  - CFE_PSP_WatchdogGet() `$`
  - CFE_PSP_WatchdogSet() `$`
  - CFE_PSP_WatchdogStatus() `$`

- Static Variables
  - #g_uiCFE_PSP_WatchdogValue_ms
  - #g_bWatchdogStatus

### Memory/Memory Sync

- Define
  - #MEMORY_PRINT_SCOPE
  - #MEMORY_SYNC_PRINT_SCOPE

- Structure
  - #MEMORY_SECTION_t

- Public Functions
  - CFE_PSP_MemSyncInit()
  - CFE_PSP_MemSyncDestroy()
  - CFE_PSP_MemSyncStart()
  - CFE_PSP_MemSyncStop()
  - CFE_PSP_MemSyncIsRunning()
  - CFE_PSP_MemSyncSetPriority()
  - CFE_PSP_MemSyncGetPriority()
  - CFE_PSP_MemSyncSetFrequency()
  - CFE_PSP_MemSyncGetFrequency()
  - CFE_PSP_MemSyncGetStatistics()
  - CFE_PSP_FlushToFLASH()
  - CFE_PSP_CheckURMFilesExists()

- Static Functions
  - CFE_PSP_CalculateCRC()
  - CFE_PSP_GetMemSize()
  - CFE_PSP_GetMemArea()
  - CFE_PSP_ReadFromRAM()
  - CFE_PSP_WriteToRAM()
  - CFE_PSP_RestoreBootRecord()
  - CFE_PSP_RestoreExceptionData()
  - CFE_PSP_RestoreReset()
  - CFE_PSP_RestoreCDS()
  - CFE_PSP_RestoreVolatileDisk()
  - CFE_PSP_RestoreUserReserved()
  - CFE_PSP_RestoreData()
  - CFE_PSP_MemSyncTask()
  - CFE_PSP_CDSFilepath()
  - CFE_PSP_ResetFilepath()
  - CFE_PSP_VolatileDiskFilepath()
  - CFE_PSP_UserReservedFilepath()
  - CFE_PSP_GenerateFilepath()

- Static Variables
  - #g_uiTotalReservedAllocSize
  - #g_uiEndOfRam
  - #g_RESETBinSemId
  - #g_CDSBinSemId
  - #g_VOLATILEDISKBinSemId
  - #g_USERRESERVEDBinSemId
  - #g_bRESETSyncFlag
  - #g_bCDSSyncFlag
  - #g_bVOLATILEDISKSyncFlag
  - #g_bUSERRESERVEDSyncFlag
  - #g_bBOOTRECORDSyncFlag
  - #g_bEXCEPTIONDATASyncFlag
  - #g_uiMemorySyncStartup
  - #g_uiMemorySyncTime
  - #g_uiMemorySyncStatistics
  - #g_uiMemorySyncTaskId
  - #g_uiMemorySyncTaskPriority
  - #g_MemorySyncTaskBinSem
  - #g_CDSFilepath
  - #g_RESETFilepath
  - #g_VOLATILEDISKFilepath
  - #g_USERRESERVEDFilepath

## Functional and Unit Tests

### Prerequisite

1. Computer with WindRiver environment (For building tests)
2. Network connection to Target in the same subnet (For running tests)
3. Kernel file `vxWorks.bin` loaded on the target (For establishing communication with target)
4. Serial Port connection to target (To capture test results)

### Procedure (Valid for both Unit and Functional Tests)

1. Change directory to Test folder

   ```bash
   cd cert_testbed/psp/fsw/sp0-vxworks6.9/unit_test
   ```

   or

   ```shell
   cd cert_testbed/psp/fsw/sp0-vxworks6.9/functional_test
   ```

2. Enable WindRiver environment

   ```shell
   wrenv
   ```

3. Build PSP Test

   ```shell
   sh build_psp.sh
   ```

4. Start tests

   run_psp.sh requires TargetIP and Kernel vxWorks.bin file location

   ```shell
   sh run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH] [TARGET_SERIAL]
   ```

   > **Example:** ```sh run_psp.sh 172.27.5.100 ~/GATEWAY/kernels/prebuilt-kernels/vx6.9_sp0-s_tt-cpci-3-103-2_cfs/vxWorks /dev/ttyFC1```

### Results

Both Functional and Unit Tests do only print out on console the results of the individual tests. The script will save the output of the console to a file.

Unit Test also runs WindRiver Code Coverage and the results are downloaded to the unit_test folder and compressed to a Zip file automatically.
