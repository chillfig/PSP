/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file cfe_psp_config.h
*
*   \brief Main PSP configuration file for GRP740
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef CFE_PSP_CONFIG_H
#define CFE_PSP_CONFIG_H


#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include "fppLib.h"
#include "excLib.h"
#include "taskLib.h"
#include "arch/sparc/esfSparc.h"
#include "arch/sparc/fppSparcLib.h"

#include "common_types.h"
#include "cfe_psp_gr740.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_gr740vx69_platcfg PSP Platform Configurations - GR740-VxWorks6.9
** \{
*/

/**
 ** \brief Override OSAL OS_Application_Run
 ** \par Description:
 ** OSAL default OS_Application_Run suspends the shell task on VxWorks.
 ** If that behaviour is not wanted, set this define to TRUE. The PSP default
 ** function implementation is empty.
 */
#define CFE_PSP_OVERRIDE_OSAL_OS_APPLICATION_RUN    true

/** \brief The list of VxWorks tasks that PSP is tasked to adjust its priorites
 ** \par Description:
 ** PSP will adjust the priorities of each tasks according to the table.
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 ** The priority reassignment will be moved to kernel in a future release.
*/
#define CFE_PSP_VXWORKS_TASK_PRIORITIES             {"tLogTask", 0},\
                                                    {"tShell0", 201},\
                                                    {"tNet0", 25},\
                                                    {"ipftps", 202},\
                                                    {"ipcom_syslogd", 205},\
                                                    {"ipcom_telnetd", 204}

/**
 ** \brief List of physical and virtual paths
 **
 ** \par Description:
 ** This is used by PSP to push multiple file system mapping to the OSAL
 ** function OS_FileSysAddFixedMap()
 ** 
 */
#define CFE_PSP_OSAL_FS_SYMBOLIC_LINKS_MAPPING      {"/ram0/cf", "/cf"},\
                                                    //{"/mnt/fmc", "/strm"}


/** \brief The list of CFS tasks affinity
 **
 ** \par Description:
 ** This is the list of task names with associated processor number. The function
 ** matching the task name will only match up to the string length identified in
 ** this list. Thus, if there is an entry as {"CFE", 0}, anything starting with 
 ** the 3 characters CFE will be assigned core zero. The entries order does
 ** matter. First matching task name will assign the core number.
 **
 ** \par Note:
 ** Processor number is up to number of available processors - 1.
 */
#define CFE_PSP_CFS_TASK_AFFINITY       {"CFE",        1}, \
                                        {"SBNG",       1}, \
                                        {"TO",         2}, \
                                        {"",           2}

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
    const char *    pCFSTaskName;
    /**
     ** \brief Processor Number from 0 to 255
     */
    uint8           uiProcessorNumber;

} CFE_PSP_TaskAffinity_t;

/******************************************************************************/
/**
 ** \name VxWorks timebase configuration parameters
 **
 ** \par Description:
 ** The GR740 does not have an integrated decrementer register.
 ** A timer (Timer 1) is used instead, and it is configured to run at 1 MHz.
 ** That is 1 micro second per tick, which is 1000 nano seconds per tick.
 **
 ** A timer (Timer 1) is used instead, and it is configured to run at 25 MHz. 
 ** That is 40 nano second per tick.
 **
 ** \par Note:
 ** This is expressed as a ratio in case it is not a whole number. The numerator
 ** unit of measure is nanoseconds per tick.
 **
 ** \warning Numerator value is dependent on the Timer configuration 
 ** #CFE_PSP_InitLocalTime.
 */
/** \{ */
/** \brief Numerator */
#define CFE_PSP_VX_TIMEBASE_PERIOD_NUMERATOR   40
/** \brief Denominator */
#define CFE_PSP_VX_TIMEBASE_PERIOD_DENOMINATOR 1

/**
 ** \brief Soft Timebase Period
 **
 ** \par Description:
 ** The tick period that will be configured in the RTOS for the simulated
 ** time base, in microseconds.  This in turn is used to drive the 1hz clock
 ** and other functions.
 ** 
 ** \par Note:
 ** To minimize jitter in the resulting callbacks, it should be an even
 ** divisor of 1000000 usec.
 ** 10ms/100Hz is chosen to also allow this same timebase to be
 ** used to drive the CFS SCH minor frame callbacks in its default config.
 **
 */
#define CFE_PSP_SOFT_TIMEBASE_PERIOD                    10000
/** \} */

/** \brief Memory Table Size.
 ** 
 ** \par Description:
 ** This sets the number of memory ranges that are defined in the memory 
 ** range definition table.
 **
 ** \par Limits:
 ** Value > 0
 */
#define CFE_PSP_MEM_TABLE_SIZE 10

/**
 ** \brief Maximum Exception Entries
 **
 ** \par Description:
 ** This define sets the maximum number of exceptions that can be stored.
 **
 ** \par Limits:
 ** Value > 0\n
 ** Must be a power of two
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
     ** \brief Tick Count
     */
    UINT64     ticks;
    /**
     ** \brief vector number
     */
    int         vector;
    /**
     ** \brief Exception stack frame
     */
    ESF         esf;
    /**
     ** \brief floating point registers
     */
    FP_CONTEXT  fp;

} CFE_PSP_Exception_ContextDataEntry_t;

/**
 ** \brief Maximum length of a task name created or spawn by PSP
 ** \par Description
 ** This value will be used to verify task name length during build-time,
 ** and used to verity CFE_PSP_SetTaskPrio task name at run-time
 */
#define CFE_PSP_MAXIMUM_TASK_LENGTH         30

/******************************************************************************/

/** \name Watchdog Configuration Parameters */
/** \{ */

/**
 ** \brief Ticks per milliseconds conversion factor
 ** \par Notes:
 ** 250MHz clk / prescaler = 250 (default in VxWorks SPARC Leon4 BSP)
 ** 1,000,000 ticks per second (1,000 ticks/ms)
 */
#define CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC  1000U

/** \brief Watchdog minimum
 ** \par Notes:
 ** Minimum is set to 1 millisecond.
 */
#define CFE_PSP_WATCHDOG_MIN CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC

/** \brief Watchdog maximum
 ** \par Notes:
 ** 0xFFFF_FFFF is special and fires the watchdog
 ** Based on a 1 MHz timer (see #CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC), 
 ** the maximum time is: 
 ** #CFE_PSP_WATCHDOG_MAX/1000000 = ~4295 seconds or ~71.5 minutes
 */
#define CFE_PSP_WATCHDOG_MAX (0xFFFFFFFEU)

/** \brief Default Watchdog Value in milliseconds */
#define CFE_PSP_WATCHDOG_DEFAULT_MSEC       20000
/** \} */

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
    uint8           VxWorksTaskPriority;

} CFE_PSP_OS_Task_and_priority_t;

/**
 ** \brief Symbolic Link Mapping Structure
 **
 ** \par Description:
 ** This structure is used by PSP to map physical paths to virtual paths
 */
typedef struct
{
    /**
     ** \brief Physical Path
     */
    const char * fullpath_real;

    /**
     ** \brief Virtual Path
     */
    const char * fullpath_map;

} CFE_PSP_SymbolicLinks_t;

// TODO
/**
 ** Number of EEPROM banks on this platform
 */
#define CFE_PSP_NUM_EEPROM_BANKS 1

/**
 ** \brief Memory Alignment Mask
 ** 
 ** \par Description:
 ** The alignment to use for each reserved memory block.\n
 ** This is a mask to be applied to each block base address\n
 ** Chosen as the cache line size of the LEON4 processor (32 bytes)
 ** such that the blocks will be cached more efficiently.
 */
#define CFE_PSP_MEMALIGN_MASK  ((cpuaddr)0x1F)

/*
** Default reading CDS method. Assume the reserved CDS memory is 
** always correct(Don't need CRC or alway read from Flash)
*/
#define CFE_PSP_CDS_READ_METHOD_DEFAULT 0

/*
** CRC reading CDS method. This will verify the CRC every time reading
** CDS. The CRC also gets update on every CDS writing
*/
#define CFE_PSP_CDS_READ_METHOD_CRC 1

/*
** Flash reading CDS method. This will always read from Flash for every
** CDS reading.
*/
#define CFE_PSP_CDS_READ_METHOD_FLASH 2

/******************************************************************************/
/** \name SNTP Sync Configuration Parameters */
/** \{ */

/**
 ** \brief Task name of the SNTP daemon task
 **
 ** \par Description:
 ** The default task name in VxWorks is "ipsntpd", but it may need to be changed.
 ** This string is used to find the SNTP task in the VxWorks task list.
 */
#define CFE_PSP_NTP_DAEMON_TASK_NAME                "ipsntps"

/**
 ** \brief SNTP Name
 **
 ** \par Description:
 ** Name of SNTP service. This is used to start or kill the OS NTP service.
 */
#define CFE_PSP_NTP_SERVICE_NAME                    "ipsntp"

/**
 **  \brief EPOCH to Mission Time Difference
 ** 
 **  \par Description:
 **  Default value corresponding to the difference in seconds between 
 **  CFE Mission Epoch and UNIX Epoch. It is left to the end user to 
 **  calculate the correct value.
 ** 
 **  \par Note:
 **  Value could be positive or negative depending if Mission Epoch is before 
 **  or after UNIX Epoch.
 **  NTP Sync will not occur if NTP time is less than this value
 **  This UNIX epoch difference should be against the mission epoch time specified in cfe_mission_cfg.h
 */
#define CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS 946727935
#define CFE_MISSION_TIME_EPOCH_UNIX_DIFF_MICROSECS 816000

/** \} */

/******************************************************************************/
/** \name Memory Scrubbing Configuration Parameters */
/** \{ */

/**
 ** \brief Start mem scrub on startup option 
 ** \par Description:
 ** This option can be set to indicate if PSP should start 
 ** mem scrub task on startup.
 ** 0 = Do not start task during startup
 ** 1 = Start task during startup
*/
#define CFE_PSP_MEMSCRUB_TASK_START_ON_STARTUP      0

/**
 ** \brief Memory Scrub Run Mode
 **
 ** \par Description:
 ** Mem Scrub can be run in multiple modes depending on the PSP config.
 ** - Idle Mode
 **   - A task is created that runs continuously in a while loop with
 **     low priority
 ** - Timed Mode
 **   - A task is created that runs every X number of seconds. The start and
 **     end address are scrubbed through in blocks.
 ** - Manual Mode
 **   - End user calls the mem scrub function when appropriate. The start and
 **     end address are scrubbed through in blocks.
 **
 ** \par Assumptions, External Events, and Notes:
 ** Value must be one of three options:
 ** - MEMSCRUB_IDLE_MODE
 ** - MEMSCRUB_TIMED_MODE
 ** - MEMSCRUB_MANUAL_MODE
 **
 */
#define CFE_PSP_MEMSCRUB_RUN_MODE                  MEMSCRUB_AUTOMATIC_MODE

/**
 ** \brief Memory Scrub Default Start Address
 **
 ** \par Description:
 ** Default, configurable memory scrub start address
 **
 ** \par Assumptions, External Events, and Notes:
 ** Value must be set to a valid address.
 */
#define CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR        0

/**
 ** \brief Memory Scrub Default End Address
 **
 ** \par Description:
 ** Default, configurable memory scrub end address
 **
 ** \par Assumptions, External Events, and Notes:
 ** Value must be set to a valid address, in relation to physical
 ** end of RAM and configured start address. This will not be checked when
 ** starting memory scrub task. Set to (0) to allow program to 
 ** use general start address (physical end of RAM).
 */
#define CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR          0


/** \} */

/**
** \} <!-- End of group "psp_gr740vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CFE_PSP_CONFIG_ */
