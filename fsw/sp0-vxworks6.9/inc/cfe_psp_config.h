/**
 ** \file cfe_psp_config.h
 **
 ** \brief Main PSP Configuration File for SP0
 **
 ** \copyright
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description:
 ** This file includes most of the PSP configuration
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef _cfe_psp_config_
#define _cfe_psp_config_

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <excLib.h>
#include <taskLib.h>
/* Defines SPE_CONTEXT */
#include <speLib.h>
/* Defines ESFPPC, the exception stack frame */
#include <arch/ppc/esfPpc.h>
/* Defines function getCoreClockSpeed() */
#include <sys950Lib.h>

#include "common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ** \name VxWorks timebase
 **
 ** \par Description:
 ** The SP0 uses the PowerPC decrementer register. The register is decremented
 ** at a speed of:
 ** - SP0-s DDR2 Configuration: 50 MHz (1/20 = 0.05)
 ** - SP0   DDR1 Configuration: 41.666 Mhz (1/24 = 0.041667)
 ** \n
 ** For SP0-s the ratio of Denominator/Numerator is 0.05, which is 50 MHz.\n
 ** Refer to Aitech 00-0092-01_17_SP0_Programmers_Guide sec. 5.9
 **
 ** \par Note:
 ** This is expressed as a ratio in case it is not a whole number.
 **
 ** \warning Numerator calculation has been validated only on SP0-s and SP0 with 
 ** a DDR memory bus speed of 50 MHz and 41.666 MHz respectively.
 */
/** \{ */
/** \brief Numerator */
#define CFE_PSP_VX_TIMEBASE_PERIOD_NUMERATOR   (uint32)(8000.0f / (float)getCoreClockSpeed())
/** \brief Denominator */
#define CFE_PSP_VX_TIMEBASE_PERIOD_DENOMINATOR 1
/** \} */

/** \brief Memory Table Size.
 ** 
 ** \par Descripion:
 ** This sets the number of memory ranges that are defined in the memory 
 ** range definition table.
 **
 ** \par Limits:
 ** Value > 0
 */
#define CFE_PSP_MEM_TABLE_SIZE              10

/**
 ** \brief Maximum Exception Entries
 **
 ** \par Description:
 ** This define sets the maximum number of exceptions that can be stored.
 ** It must always be a power of two.
 **
 ** \par Limits:
 ** Value > 0
 */
#define CFE_PSP_MAX_EXCEPTION_ENTRIES       4

/**
 ** \brief Layout of the vxWorks boot record structure
 **
 ** \par Description:
 ** This is statically placed at the beginning of system memory (sysMemTop)
 ** which should be reserved in the kernel.
 */
typedef struct
{
   /** \brief BSP Reset Type */
   uint32           bsp_reset_type;
   /** \brief Spare 1 */
   uint32           spare1;
   /** \brief Spare 2 */
   uint32           spare2;
   /** \brief Spare 3 */
   uint32           spare3;

} CFE_PSP_ReservedMemoryBootRecord_t;


/**
 * \brief The data type used by the underlying OS to represent a thread ID.
 */
typedef TASK_ID CFE_PSP_Exception_SysTaskId_t;

/**
** \brief Exception Context Data Entry
*/
typedef struct
{
    /**
     ** \brief Upper 32 bits of timebase as sampled by hook
     */
    UINT32          timebase_upper;
    /**
     ** \brief Lower 32 bits of timebase as sampled by hook
     */
    UINT32          timebase_lower;
    /**
     ** \brief vector number
     */
    int             vector;
    /**
     ** \brief Exception stack frame
     */
    ESFPPC          esf;
    /**
     ** \brief Force the spe register to 64 bit alignment
     */
    UINT64          force64BitAlign;
    /**
     ** \brief floating point registers
     */
    SPE_CONTEXT     fp;

} CFE_PSP_Exception_ContextDataEntry_t;

/** \name Watchdog Settings */
/** \{ */
/** \brief Watchdog minimum ( in milliseconds ) */
#define CFE_PSP_WATCHDOG_MIN                (0)
/** \brief Watchdog maximum ( in milliseconds ) */
#define CFE_PSP_WATCHDOG_MAX                (0xFFFFFFFF)
/** \brief Default Watchdog Value in milliseconds */
#define CFE_PSP_WATCHDOG_DEFAULT_MSEC       20000
/** \} */

/**
 ** \brief Memory Alignment Mask
 ** 
 ** \par Description:
 ** The alignment to use for each reserved memory block.\n
 ** This is a mask to be applied to each block base address\n
 ** Chosen as the cache line size of the SP0 processor (32 bytes)
 ** such that the blocks will be cached more efficiently.
 */
#define CFE_PSP_MEMALIGN_MASK               ((cpuaddr)0x1F)

/**
 ** \brief Task name and priority of tasks
 **
 ** \par Description:
 ** This structure will be used to build an array of VxWorks tasks. The task
 ** priority of each task name in the array will be modified according to the 
 ** assigned priority.
 */
typedef struct
{
    /**
     ** \brief Pointer to the task name
     */
    const char *    VxWorksTaskName;
    /**
     ** \brief Task priority from 0 to 255
     */
    int32           VxWorksTaskPriority;

} CFE_PSP_OS_Task_and_priority_t;

/** \name CDS File Location on FLASH */
/** \{ */
/**
 ** \brief CDS FLASH Memory File Location
*/
#define CFE_PSP_CFE_FLASH_FILEPATH          "/ffx0/CDS"
/** \} */

/** \name CDS Reading Method Configuration */
/** \{ */

/**
** \brief Default reading method.
**
** \par Description:
** There are 3 methods to read from CDS:\n
** *RAM Only*: Assume the reserved CDS memory is always correct. This will not 
** perform CRC and it will not read from Flash.\n
** *CRC*: On every read, calculate CRC of CDS on RAM. If does not match, read from
** FLASH and update CRC.\n
** *FLASH Only*: This will always read from Flash for every CDS reading and update the CRC value.
**
** \warning Reading from FLASH is considerably slower.
** \warning On the SP0, the reserved memory gets erased on reboot.
*/
#define CFE_PSP_CDS_READ_METHOD_RAM         0   /**< \brief  Always use RAM */
#define CFE_PSP_CDS_READ_METHOD_CRC         1   /**< \brief  Confirm with CRC */
#define CFE_PSP_CDS_READ_METHOD_FLASH       2   /**< \brief  Always use FLASH */

/** \} */

/** \name CDS Default Reading Method */
#define CFE_PSP_CDS_READ_METHOD_DEFAULT     CFE_PSP_CDS_READ_METHOD_RAM


/** \name Memory Scrubbing Configuration */
/** \{ */

/**
 ** \brief Memory Scrub Default Priority
 ** \par Description:
 ** Set the Active Memory Scrub Task Default Priority
 **
 ** \par Note: Must be set to lowest possible priority
 ** 
 */
#define MEMSCRUB_DEFAULT_PRIORITY           254

/**
 ** \brief Memory Scrub Maximum Allowed Priority
 ** \par Description:
 ** Set the Active Memory Scrub Task Up Range Allowable Priority
 ** Task Priority can be changed using CFE_PSP_MEM_SCRUB_Set.
 ** Up Range priority is capped by VxWorks OS.
 */
#define MEMSCRUB_PRIORITY_UP_RANGE          255

/**
 ** \brief Memory Scrub Minimum Allowed Priority
 ** \par Description:
 ** Set the Active Memory Scrub Task Down Range Allowable Priority
 ** Task Priority can be changed using CFE_PSP_MEM_SCRUB_Set.
 ** Down Range priority should not be lower than your apps.
 */
#define MEMSCRUB_PRIORITY_DOWN_RANGE        120

/**
 ** \brief Memory Scrub Task Name
 ** \par Description:
 ** Set the Active Memory Scrub Task Name
 */
#define MEMSCRUB_TASK_NAME                  "PSPMemScrub"

/** \} */


/** \name SP0 Info Module */
/** \{ */

/** \brief SP0 Data Dump Filepath
 ** \par Description
 ** This file is written only in the case when CFE_PSP_Panic is called.
 */
#define SP0_DATA_DUMP_FILEPATH              "/ffx0/PSP_SP0_DUMP"

/** \} */


/** \name NTP Sync Configuration */
/** \{ */

/**
 **  \brief Default NTP Sync Start/Stop on Startup
 ** 
 **  \par Description:
 **  Enable or disable the Automatic time sync with the OS
 */
#define CFE_MISSION_TIME_SYNC_OS_ENABLE     true

/**
 **  \brief Default Synchronization Frequency
 ** 
 **  \par Description:
 **  Default number of seconds between time synchronizations.
 **  CFE Time Service updates MET and STCF from VxWorks OS.
 **  When set to zero, CFE Time will be synchronized only once during start.
 ** 
 **  \par Limits
 **  Positive integer up to 255.
 **  If this value is too low, it could starve other processes.
 */
#define CFE_MISSION_TIME_SYNC_OS_SEC        30

/** \brief Default NTP Sync Task Name */
#define NTPSYNC_TASK_NAME                   "PSPNTPSync"

/** \brief Default NTP Sync Task Priority
 ** 
 ** \par Limits:
 ** Value must be above NTP Daemon task and below Mem Scrub task
 **
*/
#define NTPSYNC_DEFAULT_PRIORITY            60

/** \} */

#ifdef __cplusplus
}
#endif

#endif  /* _cfe_psp_config_ */
