/**
 ** \file
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

#ifndef CFE_PSP_CONFIG_H
#define CFE_PSP_CONFIG_H

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

#include <scratchRegMap.h>

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
#define CFE_PSP_SOFT_TIMEBASE_PERIOD                    10000

/**
 ** \brief Maximum length of active CFS flash partition name
 **
 ** \par Limits:
 ** This value should be kept small since it will take away from the failed
 ** startup filename max lenght.<br>
 ** Minimum is 1 character<br>
 ** Maximum is #CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH - 1
 **
 */
#define CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH             6

/**
 ** \brief Maximum length of full filepath
 **
 ** \par Limits:
 ** this value should be kept high enough to accomodate active partition
 ** name, reserved memory direction, and file name
 */
#define CFE_PSP_FILEPATH_MAX_LENGTH                     CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH + 26

/**
 ** \brief Maximum length of failed startup file name
 **
 ** \par Limits:
 ** This value should be kept high enough to accomodate the active partition
 ** name and the startup file name.
 ** Minimum of #CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH + 1
 */
#define CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH      32

/**
 ** \brief Maximum length of failed stratup filename with path
 **
 ** \par Limits:
 ** This value is the total length of the active partition max length plus
 ** the failed startup failname.
 **
 */
#define CFE_PSP_FAILED_STARTUP_FULLPATH_LENGTH \
                        CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH + CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH

/**
 ** \brief Define the file name of the boot startup
 **
 ** \par Limits:
 ** Minimum 1 character, and maximum of #CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH
 */
#define CFE_PSP_STARTUP_FILENAME                        "startup"

/**
 ** \brief Define the list of available CFS devices
 **
 ** \par Limits:
 ** There is a minimum of 1 device name
 */
#define CFE_PSP_STARTUP_AVAILABLE_PARTITIONS            {"/ffx0", "/ffx1"}

/**
 ** \brief Layout of Startup Structure
 **
 ** \par Description:
 ** This structure is used during the startup of CFS, but before the apps can
 ** start using the Watchdog.
 ** 
 */
typedef struct  {
    /** \brief  Timer ID */
    osal_id_t           timerID;

    /** \brief  Pointer to function that will log the failure to start */
    OS_TimerCallback_t  pStartupFailedFunction;
    
    /** \brief  Number of second to wait for startup to complete */
    uint32              uMaxWaitTime_sec;

    /** \brief  Timer Name */
    char                timer_name[OS_MAX_API_NAME];

    /** \brief  Name of the file storing the number of failed startup attempts */
    char                failed_startup_filename[CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH];

    /** \brief  Full Path of the failed startup file */
    char                fullpath_failed_startup_filename[CFE_PSP_FAILED_STARTUP_FULLPATH_LENGTH];

    /** \brief  Current Boot Startup String */
    char                boot_startup_filename[BOOT_FILE_LEN];

    /** \brief  Path of the currently active partition
     * \par Description:
     * The active CFS flash partition is the device name without the slash at the 
     * end. The function setting this variable will default to /ffx0 in case of
     * any error.
    */
    char                active_cfs_partition[CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH];

    /** \brief  Number of failed startup attempts */
    uint8               startup_failed_attempts;

    /** \brief  Number of failed startup resets
     * \par Description:
     * A reset occurs after startup_failed_attempts reaches the predefined
     * #CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS number of attempts. When a reset
     * occurs, the User Reserved Memory saved in Flash gets erased.
     */
    uint8               startup_failed_reset_attempts;

    /** \brief Reset Type */
    uint32              ResetType;

    /** \brief Reset Sub Type */
    uint32              ResetSubtype;

    /** \brief Safe Mode User Data */
    USER_SAFE_MODE_DATA_STRUCT safeModeUserData;
    
} CFE_PSP_Startup_structure_t;

/**
 ** \brief Define Number of PSP Resets Before Switching CFS Partition
 ** 
 ** \par Description:
 ** Number of PSP Resets before switching to the other partition. If set to 2,
 ** then 2 processor resets will occur, and the 3rd processor reset will be a
 ** power on reset instead.
 **
 ** \par Limits:
 ** There is a lower limit of 0.  There are no restrictions on the upper limit
 ** however, the maximum number of processor resets may be system dependent and
 ** should be verified.
 */
#define CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS            3

/**
 ** \brief Define Number of seconds to wait until PSP Restart target
 ** 
 ** \par Description:
 ** Number of seconds to wait before calling the PSP Restart function.
 ** 
 ** \par Limits:
 ** There is a lower limit of 0. There are no restrictions on the upper limit
 ** however, the maximum number of seconds to wait for CFS to start is dependent 
 ** on system performance.
 */
#define CFE_PSP_STARTUP_MAX_WAIT_TIME_SEC               120

/**
 ** \brief Define file name containing the number of attempts to start
 ** 
 ** \par Description:
 ** The file name that contains the number of attempts to startup CFS. The file
 ** is located in the currently active flash partition.
 ** 
 ** \par Limits:
 ** File name must be at least 1 character, and no more than 
 ** #CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH - 1 characters.
 */
#define CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME         "fail.txt"

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

/** \brief Backup file directory */
#define CFE_PSP_BACKUP_DIRECTORY                    "/URM"

/** \brief CDS File name on FLASH */
#define CFE_PSP_CDS_FLASH_FILEPATH                  "CDS.bkp"

/** \brief RESET File name on FLASH */
#define CFE_PSP_RESET_FLASH_FILEPATH                "RESET.bkp"

/** \brief Volatile Disk File name on FLASH */
#define CFE_PSP_VOLATILEDISK_FLASH_FILEPATH         "VODI.bkp"

/** \brief User Reserved File name on FLASH */
#define CFE_PSP_USERRESERVED_FLASH_FILEPATH        "USRR.bkp"


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
#define MEM_SCRUB_TASK_START_ON_STARTUP      1

/**
 ** \brief Memory Scrub Default Start Address
 **
 ** \par Description:
 ** Default, configurable memory scrub start address
 **
 ** \par Assumptions, External Events, and Notes:
 ** Value must be set to a valid address.
 */
#define MEM_SCRUB_DEFAULT_START_ADDR        (0)

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
#define MEM_SCRUB_DEFAULT_END_ADDR          (0)

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
 ** Task Priority can be changed using CFE_PSP_MemScrubSet.
 ** Up Range priority is capped by VxWorks OS.
 */
#define MEMSCRUB_PRIORITY_UP_RANGE          255

/**
 ** \brief Memory Scrub Minimum Allowed Priority
 ** \par Description:
 ** Set the Active Memory Scrub Task Down Range Allowable Priority
 ** Task Priority can be changed using CFE_PSP_MemScrubSet.
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
 ** The default task name in VxWorks is "ipntpd", but it may need to be changed.
 ** This string is used to find the NTP task in the VxWorks task list.
 */
#define NTP_DAEMON_TASK_NAME                "ipntpd"

/**
 ** \brief NTP Name
 **
 ** \par Description:
 ** Name of NTP service. This is used to start or kill the OS NTP service.
 */
#define NTP_SERVICE_NAME                    "ipntp"

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
#define CFE_MISSION_TIME_EPOCH_UNIX_DIFF    946727936

/** \brief MEM SCRUB Binary semaphore name */
#define PSP_MEM_SCRUB_BSEM_NAME             "PSP_BSEM_NAME"


/** \brief MEMORY SYNC Default time between sync attempts */
#define MEMORY_SYNC_DEFAULT_SYNC_TIME_MS     3000
/** \brief MEMORY SYNC Task Name */
#define MEMORY_SYNC_TASK_NAME               "MEMSYNCTASK"
/** \brief MEMORY SYNC Binary Semaphore Name */
#define MEMORY_SYNC_BSEM_NAME               "MEMSYNCBSEM"
/** \brief MEMORY SYNC Priority Defualt */
#define MEMORY_SYNC_PRIORITY_DEFAULT        190
/** \brief MEMORY SYNC Priority Upper Limit */
#define MEMORY_SYNC_PRIORITY_UP_RANGE       230
/** \brief MEMORY SYNC Priority Lower Limit */
#define MEMORY_SYNC_PRIORITY_LOW_RANGE      60
/** \brief MEMORY SYNC Start on Startup */
#define MEMORY_SYNC_START_ON_STARTUP        false
/** \brief RESET Binary Semaphore Name */
#define MEMORY_RESET_BIN_SEM_NAME           "RSTBSEM"
/** \brief CDS Binary Semaphore Name */
#define MEMORY_CDS_BIN_SEM_NAME             "CDSBSEM"
/** \brief VOLATILE DISK Binary Semaphore Name */
#define MEMORY_VOLATILEDISK_BIN_SEM_NAME    "VODBSEM"
/** \brief USER RESERVED Binary Semaphore Name */
#define MEMORY_USERRESERVED_BIN_SEM_NAME    "USRBSEM"

/** \} */

/**
** \} <!-- End of group "psp_sp0vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CFE_PSP_CONFIG_ */
