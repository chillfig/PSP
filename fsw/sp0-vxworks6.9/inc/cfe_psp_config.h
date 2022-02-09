/**
 ** \file cfe_psp_config.h
 **
 ** \brief Main PSP Configuration File for SP0
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** This file includes most of the PSP configuration
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef _CFE_PSP_CONFIG_
#define _CFE_PSP_CONFIG_

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
** \addtogroup psp_sp0vx69_platcfg PSP Platform Configurations - SP0-VxWorks6.9
** \{
*/

/**
 ** \brief Override OSAL OS_Application_Run
 ** \par Description:
 ** OSAL default OS_Application_Run suspends the shell task on VxWorks.
 ** If that behaviour is not wanted, set this define to TRUE. The PSP default
 ** function implementation is empty.
 */
#define OVERRIDE_OSAL_OS_APPLICATION_RUN             true


/** \brief The list of VxWorks tasks that PSP is tasked to adjust its priorites
 ** \par Description:
 ** PSP will adjust the priorities of each tasks according to the table.
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 ** The priority reassignment will be moved to kernel in a future release.
*/
#define VXWORKS_TASK_PRIORITIES                     {"tLogTask", 0},\
                                                    {"tShell0", 201},\
                                                    {"tWdbTask", 203},\
                                                    {"tVxdbgTask", 200},\
                                                    {"tNet0", 25},\
                                                    {"ipftps", 202},\
                                                    {"ipcom_syslogd", 205},\
                                                    {"ipcom_telnetd", 204},\
                                                    {"ipcom_egd", 253},\
                                                    {"FTCMP00", 253}


/**
 ** \brief List of physical and virtual paths
 **
 ** \par Description:
 ** This is used by PSP to push multiple file system mapping to the OSAL
 ** function OS_FileSysAddFixedMap()
 ** 
 */
#define OSAL_FS_SYMBOLIC_LINKS_MAPPING              {"/ram0/cf", "/cf"}


/**
 ** \name VxWorks timebase configuration parameters
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
 ** This is expressed as a ratio in case it is not a whole number. The numerator
 ** unit of measure is nanoseconds per tick.
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
#define CFE_PSP_SOFT_TIMEBASE_PERIOD        10000

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


/**
 ** \brief Maximum length of a task name created or spawn by PSP
 ** \par Description
 ** This value will be used to verify task name length during build-time,
 ** and used to verity CFE_PSP_SetTaskPrio task name at run-time
 */
#define CFE_PSP_MAXIMUM_TASK_LENGTH         30

/** \name Watchdog Configuration Parameters */
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


/** \name CDS File Location on FLASH Configuration Parameters */
/** \{ */
/**
 ** \brief CDS FLASH Memory File Location
 ** \par Note:
 ** File will be overwritten every time CFS starts.
*/
#define CFE_PSP_CDS_FLASH_FILEPATH          "/ffx0/CDS"
/** \} */

/** \brief RESET File location on FLASH */
#define CFE_PSP_RESET_FLASH_FILEPATH        "/ffx0/RRT"

/** \brief Volatile Disk File location on FLASH */
#define CFE_PSP_VOLATILEDISK_FLASH_FILEPATH        "/ffx0/RVD"

/** \brief User Reserved File location on FLASH */
#define CFE_PSP_USERRESERVED_FLASH_FILEPATH        "/ffx0/RUR"


/** \name Memory Scrubbing Configuration Parameters */
/** \{ */

/*
** \brief Memory Scrub Default Start Address
**
** \par Description:
** Default, configurable memory scrub start address
**
** \par Assumptions, External Events, and Notes:
** Value must be set to a valid address. This will not be checked when
** starting memory scrub task. Set to (-1) to allow program to 
** use general start address.
*/
#define MEM_SCRUB_DEFAULT_START_ADDR        (-1)

/*
** \brief Memory Scrub Default End Address
**
** \par Description:
** Default, configurable memory scrub end address
**
** \par Assumptions, External Events, and Notes:
** Value must be set to a valid address, in relation to physical
** end of RAM and configured start address. This will not be checked when
** starting memory scrub task. Set to (-1) to allow program to 
** use general start address (physical end of RAM).
*/
#define MEM_SCRUB_DEFAULT_END_ADDR          (-1)

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


/** \name SP0 Info Configuration Parameters */
/** \{ */

/** \brief SP0 Data Dump Filepath
 ** \par Description
 ** This file is written only in the case when CFE_PSP_Panic is called.
 */
#define SP0_DATA_DUMP_FILEPATH              "/ffx0/PSP_SP0_DUMP"

/** \} */


/** \name NTP Sync Configuration Parameters */
/** \{ */

/**
 ** \brief Task name of the NTP daemon task
 **
 ** \par Description:
 ** The default task name in VxWorks is "ipntpd", but it may need to be changed
 */
#define NTP_DAEMON_TASK_NAME                "ipntpd"

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
 ** 
 */
#define CFE_MISSION_TIME_EPOCH_UNIX_DIFF    946728000

/**
 ** \brief CFE Time Service Task Name
 ** \par Description:
 ** This is the task name used by CFE Time Service to update the mission time.
 ** \par Note:
 ** This value is not checked against the CFE configuration, and it is up
 ** to the end user to verify it matches the CFE configuration.\n
 ** Definition will be deleted once the NTP Sync App is ready to be released.
 */

#define CFE_1HZ_TASK_NAME                   "TIME_1HZ_TASK"

/**
 ** \brief Time delay in msec before checking CFE Time Service status
 ** \par Description:
 ** NTP Sync starts before the CFE Time Service.
 ** This parameter introduces and non-blocking time delay before checking 
 ** if the CFE Time Service has started. The goal is to start the NTP Sync
 ** as soon as possible after CFE Time Service starts.
 ** The time delay is defined in milliseconds and it will only occur during
 ** CFS booting.
 */
#define NTPSYNC_INITIAL_TIME_DELAY          500

/**
 ** \brief Time delay maximum iterations
 ** \par Description:
 ** If the time delay introduced with #NTPSYNC_INITIAL_TIME_DELAY is not enough
 ** the code will continue trying in a loop. This value sets the maximum 
 ** number of times to run the time delay.
 ** For example, if NTPSYNC_INITIAL_TIME_DELAY * NTPSYNC_MAX_ITERATION_TIME_DELAY
 ** is 500 ms * 120 = 60 seconds maximum wait time.
 */
#define NTPSYNC_MAX_ITERATION_TIME_DELAY    120

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

/** \brief MEM SCRUB Binary semaphore name */
#define PSP_MEM_SCRUB_BSEM_NAME             "PSP_BSEM_NAME"

/**
 ** \brief CDS sync to FLASH
 **
 ** \par Description:
 ** Default option used to determine to sync
 ** CDS to FLASH or not
 */
#define PSP_CDS_SYNC_TO_FLASH_DEFAULT       true

/**
 ** \brief NTP Sync maximum allowed Task priority
 **
 ** \par Description:
 ** Set the active NTP Sync task up range allowable priority.
 */
#define NTPSYNC_PRIORITY_UP_RANGE           255

/**
 ** \brief NTP Sync maximum allowed Task priority
 **
 ** \par Description:
 ** Set the active NTP Sync task down range allowable priority.
 */
#define NTPSYNC_PRIORITY_DOWN_RANGE           60

/** \} */

/**
** \} <!-- End of group "psp_sp0vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CFE_PSP_CONFIG_ */
