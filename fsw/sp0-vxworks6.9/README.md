# SP0 implementation of PSP

This is the PSP implementation for Aitch SP0 and SP0s targets flight computers

> **NOTE:** Aitech SP0 and SP0s do not support User Reserved Memory nor Persistente Memory. 

[[_TOC_]]

___

## VxWorks Kernel Requirements


### Functions

Kernel must provide the following functions:
- GetWrsKernelTextStart()
- GetWrsKernelTextEnd()

#### Not included - from MCP750
- startCfeCore() - _"This function is used to unzip, load, and run CFE core"_

> **Note:** This function is not necessary during development time, but could be useful in production. The _startCfeCore_ function could load CFE on startup.

- CFE_PSP_InitFlashDisk() - _"Initialize the Compact flash disk in vxWorks"_

### Components
Kernel must include the following services:
- FTP
  - Port 21
- Telnet
- User Reserved Memory (Not Supported by Aitech)
  - Why CLEAR_USER_RESERVED_MEMORY_ON_COLD_BOOT in Kernel is set to True?
- Watchdog
- NTP
- Datalight FlashFX (Aitech FLASH Memory)


## API

### Base PSP

* Defines
  - CFE_PSP_MAIN_FUNCTION
  - CFE_PSP_NONVOL_STARTUP_FILE

* Global Variables
  - g_uiResetType
  - g_uiResetSubtype
  - g_safeModeUserData
  - g_uiShellTaskID
  - g_VxWorksTaskList

* Functions
  - CFE_PSP_Main
  - CFE_PSP_ProcessPOSTResults
  - CFE_PSP_ProcessResetType
  - CFE_PSP_LogSoftwareResetType
  - OS_Application_Startup
  - OS_Application_Run
  - CFE_PSP_SuspendConsoleShellTask "Interactive Console Shell"
  - CFE_PSP_GetRestartType
  - CFE_PSP_SetTaskPrio
  - CFE_PSP_SetSysTasksPrio

### Active Memory Scrubbing

* Defines
  - MEMSCRUB_TASK_STACK_SIZE
  - MEMSCRUB_DEFAULT_PRIORITY
  - MEMSCRUB_PRIORITY_UP_RANGE
  - MEMSCRUB_PRIORITY_DOWN_RANGE
  - MEMSCRUB_TASK_NAME

* Global Variables
  - g_uiMemScrubTaskPriority
  - g_uiMemScrubTask_id
  - g_uiMemScrubStartAddr
  - g_uiMemScrubEndAddr
  - g_uiMemScrubCurrentPage
  - g_uiMemScrubTotalPages
  - g_uiEndOfRam

* Functions
  - CFE_PSP_MEM_SCRUB_Set
  - CFE_PSP_MEM_SCRUB_Status
  - CFE_PSP_MEM_SCRUB_Task
  - CFE_PSP_MEM_SCRUB_Init
  - CFE_PSP_MEM_SCRUB_isRunning
  - CFE_PSP_MEM_SCRUB_Delete
  - CFE_PSP_MEM_SCRUB_Enable
  - CFE_PSP_MEM_SCRUB_Disable

### Critical Device Storage (CDS)

* Global Variables
  - g_cCDSFileName    "The full file path where the CDS file will be saved"
  - g_uiCDSReadMethod "The reading/writing method. Flash cannot be written on too often"
  - g_uiCDSCrc        "CDS CRC value"

* Functions
  - CFE_PSP_GetCDSSize
  - CFE_PSP_SetReadCDSMethod
  - CFE_PSP_GetReadCDSMethod
  - CFE_PSP_SetStaticCRC
  - CFE_PSP_GetStaticCRC
  - CFE_PSP_CalculateCRC
  - CFE_PSP_ReadCDSFromFlash
  - CFE_PSP_WriteCDSToFlash
  - CFE_PSP_WriteToCDS
  - CFE_PSP_ReadFromCDS

### SP0 Info

* Global Variables

* Functions

### NTP Time Sync

* Defines
  - CFE_MISSION_TIME_SYNC_OS_SEC
  - CFE_MISSION_TIME_SYNC_OS_ENABLE

* Global Variables
  - getTime_From_OS_flag
  - cfe_OS_Time_Sync_Sec

* Functions Internal
  - CFE_PSP_TIME_Init
  - CFE_PSP_Update_OS_Time
  - CFE_PSP_StartNTPDaemon
  - CFE_PSP_StopNTPDaemon

* Functions (include psp_time_sync.h)
  - CFE_PSP_Sync_From_OS_Enable
  - CFE_PSP_Sync_From_OS_Freq
  - CFE_PSP_Set_OS_Time
  - CFE_PSP_Get_OS_Time
  - CFE_PSP_NTP_Daemon_Get_Status
  - CFE_PSP_NTP_Daemon_Enable

