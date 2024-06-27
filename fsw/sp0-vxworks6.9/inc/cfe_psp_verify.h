/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file cfe_psp_verify.h
*
*   \brief Macros to run preprocessor checks on psp configuration
*
*   \brief The file includes preprocessor statements to check the validity of the PSP configuration saved
*   in cfe_psp_config.h
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_VERIFY_H
#define PSP_VERIFY_H

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

/** \brief CFE_PSP_MEMSCRUB_RUN_MODE Verification */
#ifndef CFE_PSP_MEMSCRUB_RUN_MODE
    #error "CFE_PSP_MEMSCRUB_RUN_MODE must be defined"
#else
    #if (!((CFE_PSP_MEMSCRUB_RUN_MODE == CFE_PSP_MEMSCRUB_IDLE_MODE) || (CFE_PSP_MEMSCRUB_RUN_MODE == CFE_PSP_MEMSCRUB_TIMED_MODE) || (CFE_PSP_MEMSCRUB_RUN_MODE == CFE_PSP_MEMSCRUB_MANUAL_MODE)))
        #error "CFE_PSP_MEMSCRUB_RUN_MODE must be a valid value"
    #endif
#endif

/** \brief CFE_PSP_MEMSCRUB_RUN_MODE Verification */
#ifndef CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES
    #error "CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES must be defined"
#else
    #if (CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES < 1) || (CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES > 259765)
        #error "CFE_PSP_MEMSCRUB_BLOCKSIZE_PAGES must be bigger than 0 and less than 259765 (0xFFFF_FFFF / 4096)"
    #endif
#endif

/** \brief CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC Verification */
#ifndef CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC
    #error "CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC must be defined"
#else
    #if (CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC < 1)
        #error "CFE_PSP_MEMSCRUB_TASK_DELAY_MSEC must be bigger than 0"
    #endif
#endif

/** \brief MEM SCRUB Task Name Verification */
#ifndef CFE_PSP_MEMSCRUB_TASK_NAME
    #error "CFE_PSP_MEMSCRUB_TASK_NAME must be defined"
#endif

/** \brief Check that the MEM SCRUB Task name is no longer than the maximum allowed name length */
CompileTimeAssert(sizeof(CFE_PSP_MEMSCRUB_TASK_NAME) <= CFE_PSP_MAXIMUM_TASK_LENGTH, CFE_PSP_MEMSCRUB_TASK_NAME_TOO_LONG);

/** \brief MEM SCRUB Priority Verification */
#if (CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY > CFE_PSP_MEMSCRUB_PRIORITY_UP_RANGE)
    #error "CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY must be less than or equal to CFE_PSP_MEMSCRUB_PRIORITY_UP_RANGE!"
#endif

#if (CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY < CFE_PSP_MEMSCRUB_PRIORITY_DOWN_RANGE)
    #error "CFE_PSP_MEMSCRUB_DEFAULT_PRIORITY must be greater than or equal to CFE_PSP_MEMSCRUB_PRIORITY_DOWN_RANGE!"
#endif

/** \brief Verify that memory scrub start address defined, acceptable values */
#ifndef CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR
    #error "CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR must be defined"
#endif

#ifdef CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR
    #if (CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR < -1)
        #error "CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR must be larger than -1"
    #endif
#endif

#ifdef CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR
    #if (CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR < -1)
        #error "CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR must be larger than -1"
    #endif
#endif


/** \brief Verify that memory scrub end address defined */
#ifndef CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR
    #error "CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR must be defined"
#endif


/** \brief SP0 File Path Verification */
#ifndef CFE_PSP_SP0_DATA_DUMP_FILEPATH
    #error "CFE_PSP_SP0_DATA_DUMP_FILEPATH must be defined"
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

/** \brief NTP Verification */
#ifndef CFE_PSP_NTP_DAEMON_TASK_NAME
    #error "CFE_PSP_NTP_DAEMON_TASK_NAME must be defined"
#endif

#ifndef CFE_PSP_NTP_SERVICE_NAME
    #error "CFE_PSP_NTP_SERVICE_NAME must be defined"
#endif

/** \brief Check backup directory is defined */
#ifndef CFE_PSP_BACKUP_DIRECTORY
    #error "CFE_PSP_BACKUP_DIRECTORY must be defined"
#endif

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
#ifndef CFE_PSP_MEMORY_SYNC_DEFAULT_SYNC_TIME_MS
    #error "CFE_PSP_MEMORY_SYNC_DEFAULT_SYNC_TIME_MS must be defined"
#endif
#ifndef CFE_PSP_MEMORY_SYNC_TASK_NAME
    #error "CFE_PSP_MEMORY_SYNC_TASK_NAME must be defined"
#endif
#ifndef CFE_PSP_MEMORY_SYNC_BSEM_NAME
    #error "CFE_PSP_MEMORY_SYNC_BSEM_NAME must be defined"
#endif
#ifndef CFE_PSP_MEMORY_SYNC_PRIORITY_DEFAULT
    #error "CFE_PSP_MEMORY_SYNC_PRIORITY_DEFAULT must be defined"
#endif
#ifndef CFE_PSP_MEMORY_SYNC_PRIORITY_UP_RANGE
    #error "CFE_PSP_MEMORY_SYNC_PRIORITY_UP_RANGE must be defined"
#endif
#ifndef CFE_PSP_MEMORY_SYNC_PRIORITY_UP_RANGE
    #error "CFE_PSP_MEMORY_SYNC_PRIORITY_DOWN_RANGE must be defined"
#endif
#ifndef CFE_PSP_MEMORY_SYNC_START_ON_STARTUP
    #error "CFE_PSP_MEMORY_SYNC_START_ON_STARTUP must be defined"
#endif
#if CFE_PSP_MEMORY_SYNC_PRIORITY_DEFAULT < CFE_PSP_MEMORY_SYNC_PRIORITY_LOW_RANGE
    #error "CFE_PSP_MEMORY_SYNC_PRIORITY_DEFAULT too low"
#endif
#if CFE_PSP_MEMORY_SYNC_PRIORITY_DEFAULT > CFE_PSP_MEMORY_SYNC_PRIORITY_UP_RANGE
    #error "CFE_PSP_MEMORY_SYNC_PRIORITY_DEFAULT too low"
#endif
#ifndef CFE_PSP_MEMORY_RESET_BIN_SEM_NAME
    #error "CFE_PSP_MEMORY_RESET_BIN_SEM_NAME must be defined"
#endif
#ifndef CFE_PSP_MEMORY_CDS_BIN_SEM_NAME
    #error "CFE_PSP_MEMORY_CDS_BIN_SEM_NAME must be defined"
#endif
#ifndef CFE_PSP_MEMORY_VOLATILEDISK_BIN_SEM_NAME
    #error "CFE_PSP_MEMORY_VOLATILEDISK_BIN_SEM_NAME must be defined"
#endif
#ifndef CFE_PSP_MEMORY_USERRESERVED_BIN_SEM_NAME
    #error "CFE_PSP_MEMORY_USERRESERVED_BIN_SEM_NAME must be defined"
#endif
CompileTimeAssert(sizeof(CFE_PSP_MEMORY_SYNC_TASK_NAME) <= CFE_PSP_MAXIMUM_TASK_LENGTH, NTPSYNC_TASK_NAME_TOO_LONG);
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

/** \brief Verify that the startup file name is within required length */
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FILENAME) <= CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH, CFE_PSP_STARTUP_FILENAME_TOO_LONG);
/** \brief Verify that the failed startup file name is within required length */
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME) <= CFE_PSP_FAILED_STARTUP_FILENAME_MAX_LENGTH, CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME_TOO_LONG);
/** \brief Verify that the failed startup file name is within required length */
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME) > 1, CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME_TOO_SHORT);
/** 
 * \brief Verify that the boot string does not exceed max boot line size 
 * 
 * \note The format of the boot string is %s/%s where the first is the active partition name and the second is the startup file name
*/
CompileTimeAssert(sizeof(CFE_PSP_STARTUP_FILENAME)+sizeof(CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH)+1 < MAX_BOOT_LINE_SIZE, CFE_PSP_BOOTSTRING_TOO_LONG);

/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERIFY_H_ */

/*==============================================================================
** End of file cfe_psp_verify.h
**============================================================================*/
    
