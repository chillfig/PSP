# PSP SP0 VxWorks Implementation


## API

### Active Memory Scrubbing

- Defines
  - MEMSCRUB_TASK_STACK_SIZE
  - MEMSCRUB_DEFAULT_PRIORITY
  - MEMSCRUB_PRIORITY_UP_RANGE
  - MEMSCRUB_PRIORITY_DOWN_RANGE
  - MEMSCRUB_TASK_NAME

- Global Variables Used
  - sg_uiMemScrubTaskPriority
  - sg_uiMemScrubTask_id
  - sg_uiMemScrubStartAddr
  - sg_uiMemScrubEndAddr
  - sg_uiMemScrubCurrentPage
  - sg_uiMemScrubTotalPages
  - sg_endOfRam

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

- Global Variables
  - g_cCDSFileName  "The full file path where the CDS file will be saved"
  - g_CDSReadMethod "The reading/writing method. Flash cannot be written on too often"
  - sg_uiCDSCrc     "CDS CRC value"

- Functions
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


### NTP Time Sync

