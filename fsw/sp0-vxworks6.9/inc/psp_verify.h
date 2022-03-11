/**
 ** \file psp_verify.h
 **
 ** \brief Macros to run preprocessor checks on psp configuration
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
 ** The file includes preprocessor statements to check the validity of the PSP
 ** configuration saved in cfe_psp_config.h
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef _PSP_VERIFY_H_
#define _PSP_VERIFY_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
** Include Files
*/
#include "cfe_psp_config.h"


/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/*
** Macro Definitions
*/

/** \brief MEM SCRUB Task Name Verification */
#ifndef MEMSCRUB_TASK_NAME
    #error "MEMSCRUB_TASK_NAME must be defined"
#endif

/** \brief Check that the MEM SCRUB Task name is no longer than the maximum allowed name length */
CompileTimeAssert(sizeof(MEMSCRUB_TASK_NAME) <= CFE_PSP_MAXIMUM_TASK_LENGTH, MEMSCRUB_TASK_NAME_TOO_LONG);

/** \brief MEM SCRUB Priority Verification */
#if (MEMSCRUB_DEFAULT_PRIORITY > MEMSCRUB_PRIORITY_UP_RANGE)
    #error "MEMSCRUB_DEFAULT_PRIORITY must be less than or equal to MEMSCRUB_PRIORITY_UP_RANGE!"
#endif

#if (MEMSCRUB_DEFAULT_PRIORITY < MEMSCRUB_PRIORITY_DOWN_RANGE)
    #error "MEMSCRUB_DEFAULT_PRIORITY must be greater than or equal to MEMSCRUB_PRIORITY_DOWN_RANGE!"
#endif

/** \brief Verify that memory scrub start address defined, acceptable values */
#ifndef MEM_SCRUB_DEFAULT_START_ADDR
    #error "MEM_SCRUB_DEFAULT_START_ADDR must be defined"
#endif

#ifdef MEM_SCRUB_DEFAULT_START_ADDR
    #if (MEM_SCRUB_DEFAULT_START_ADDR < -1)
        #error "MEM_SCRUB_DEFAULT_START_ADDR must be larger than -1"
    #endif
#endif

#ifdef MEM_SCRUB_DEFAULT_END_ADDR
    #if (MEM_SCRUB_DEFAULT_END_ADDR < -1)
        #error "MEM_SCRUB_DEFAULT_END_ADDR must be larger than -1"
    #endif
#endif


/** \brief Verify that memory scrub end address defined */
#ifndef MEM_SCRUB_DEFAULT_END_ADDR
    #error "MEM_SCRUB_DEFAULT_END_ADDR must be defined"
#endif


/** \brief SP0 File Path Verification */
#ifndef SP0_DATA_DUMP_FILEPATH
    #error "SP0_DATA_DUMP_FILEPATH must be defined"
#endif

/** \brief WatchDog Default Time Verification */
#ifndef CFE_PSP_WATCHDOG_DEFAULT_MSEC
    #error "CFE_PSP_WATCHDOG_DEFAULT_MSEC must be defined"
#elif CFE_PSP_WATCHDOG_DEFAULT_MSEC > 60000
    #error "Watchdog default value is set to larger than 60 seconds"
#elif CFE_PSP_WATCHDOG_DEFAULT_MSEC < 1
    #error "Watchdog default value is set to less than 1 millisecond"
#endif

#if CFE_PSP_MEM_TABLE_SIZE < 0
    #error "CFE_PSP_MEM_TABLE_SIZE must be positive"
#endif

#if CFE_PSP_MAX_EXCEPTION_ENTRIES < 1
    #error "CFE_PSP_MAX_EXCEPTION_ENTRIES must be equal of above 1"
#endif
/* Verify power of two */
#if ((CFE_PSP_MAX_EXCEPTION_ENTRIES & (CFE_PSP_MAX_EXCEPTION_ENTRIES - 1)) != 0)
#error CFE_PSP_MAX_EXCEPTION_ENTRIES must be a power of 2
#endif

#if NTPSYNC_DEFAULT_PRIORITY < 50
    #error "NTPSYNC_DEFAULT_PRIORITY must have lower priority than NTP Daemon task"
#elif NTPSYNC_DEFAULT_PRIORITY >= MEMSCRUB_DEFAULT_PRIORITY
    #error "NTPSYNC_DEFAULT_PRIORITY must have higher priority than MEMSCRUB_DEFAULT_PRIORITY"
#endif

/** \brief NTP SYNC Task Name Verification */
#ifndef NTPSYNC_TASK_NAME
    #error "NTPSYNC_TASK_NAME must be defined"
#endif

/** \brief Check that the NTP SYNC Task name is no longer than the maximum allowed name length */
CompileTimeAssert(sizeof(NTPSYNC_TASK_NAME) <= CFE_PSP_MAXIMUM_TASK_LENGTH, NTPSYNC_TASK_NAME_TOO_LONG);

/** \brief Check reserved memory section file name macros */
#ifndef CFE_PSP_CDS_FLASH_FILEPATH
    #error "CFE_PSP_CDS_FLASH_FILEPATH must be defined"
#endif
#ifndef CFE_PSP_RESET_FLASH_FILEPATH
    #error "CFE_PSP_RESET_FLASH_FILEPATH must be defined"
#endif
#ifndef CFE_PSP_VOLATILEDISK_FLASH_FILEPATH
    #error "CFE_PSP_VOLATILEDISK_FLASH_FILEPATH must be defined"
#endif
#ifndef CFE_PSP_USERRESERVED_FLASH_FILEPATH
    #error "CFE_PSP_USERRESERVED_FLASH_FILEPATH must be defined"
#endif

/** \brief Check MEMORY SYNC preprocessor directives */
#ifndef MEMORY_SYNC_DEFAULT_SYNC_TIME_MS
    #error "MEMORY_SYNC_DEFAULT_SYNC_TIME_MS must be defined"
#endif
#ifndef MEMORY_SYNC_TASK_NAME
    #error "MEMORY_SYNC_TASK_NAME must be defined"
#endif
#ifndef MEMORY_SYNC_BSEM_NAME
    #error "MEMORY_SYNC_BSEM_NAME must be defined"
#endif
#ifndef MEMORY_SYNC_PRIORITY_DEFAULT
    #error "MEMORY_SYNC_PRIORITY_DEFAULT must be defined"
#endif
#ifndef MEMORY_SYNC_PRIORITY_UP_RANGE
    #error "MEMORY_SYNC_PRIORITY_UP_RANGE must be defined"
#endif
#ifndef MEMORY_SYNC_PRIORITY_UP_RANGE
    #error "MEMORY_SYNC_PRIORITY_DOWN_RANGE must be defined"
#endif
#ifndef MEMORY_SYNC_START_ON_STARTUP
    #error "MEMORY_SYNC_START_ON_STARTUP must be defined"
#endif
#if MEMORY_SYNC_PRIORITY_DEFAULT < MEMORY_SYNC_PRIORITY_LOW_RANGE
    #error "MEMORY_SYNC_PRIORITY_DEFAULT too low"
#endif
#if MEMORY_SYNC_PRIORITY_DEFAULT > MEMORY_SYNC_PRIORITY_UP_RANGE
    #error "MEMORY_SYNC_PRIORITY_DEFAULT too low"
#endif
#ifndef MEMORY_RESET_BIN_SEM_NAME
    #error "MEMORY_RESET_BIN_SEM_NAME must be dfeined"
#endif
#ifndef MEMORY_CDS_BIN_SEM_NAME
    #error "MEMORY_CDS_BIN_SEM_NAME must be dfeined"
#endif
#ifndef MEMORY_VOLATILEDISK_BIN_SEM_NAME
    #error "MEMORY_VOLATILEDISK_BIN_SEM_NAME must be dfeined"
#endif
#ifndef MEMORY_USERRESERVED_BIN_SEM_NAME
    #error "MEMORY_USERRESERVED_BIN_SEM_NAME must be dfeined"
#endif
CompileTimeAssert(sizeof(MEMORY_SYNC_TASK_NAME) <= CFE_PSP_MAXIMUM_TASK_LENGTH, NTPSYNC_TASK_NAME_TOO_LONG);
/* \brief Check that CFE_PSP_STARTUP_AVAILABLE_PARTITIONS is defined */
#ifndef CFE_PSP_STARTUP_AVAILABLE_PARTITIONS
    #define CFE_PSP_STARTUP_AVAILABLE_PARTITIONS    {"/ffx0"}
#endif

/** \brief Check that we have a maximum string length defined for the current active CFS partition or device */
#ifndef CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH
    #define CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH 6
#endif
/** \brief Check the maximum length of the startup filename */
#ifndef CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH
    #define CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH 32
#endif

/** \brief Check the minimum length of the startup filename */
#if CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH < CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH - 1
    #error "CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH must be longer than CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH - 1"
#endif

/** \brief Check the maximum and minimum length of the active CFS partition */
#if CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH < 1
    #error "CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH must be at least 1 character"
#endif
#if CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH >= CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH - 1
    #error "CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH must be no more than CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH - 1"
#endif

/** \brief Verify that the startup wait time before calling the StartupFailed function is at least 1 second */
#ifndef CFE_PSP_STARTUP_MAX_WAIT_TIME_SEC
    #define CFE_PSP_STARTUP_MAX_WAIT_TIME_SEC 60
#endif
#if CFE_PSP_STARTUP_MAX_WAIT_TIME_SEC < 1
    #error "CFE_PSP_STARTUP_MAX_WAIT_TIME_SEC must be at least 1 second"
#endif

/** \brief  Verify max number of processor resets is defined and its value is more than 1 */
#ifndef CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS
    #define CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS 3
#endif
#if CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS < 1
    #error "CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS must be positive"
#endif

/** \brief  Verify failed startup filename is defined and its length is between 1 and CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH */
#ifndef CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME
    #define CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME "fail.txt"
#endif
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FILENAME) <= CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH, CFE_PSP_STARTUP_FILENAME_TOO_LONG);
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME) <= CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH, CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME_TOO_LONG);
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME) > 1, CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME_TOO_SHORT);


/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERIFY_H_ */

/*==============================================================================
** End of file psp_verify.h
**============================================================================*/
    
