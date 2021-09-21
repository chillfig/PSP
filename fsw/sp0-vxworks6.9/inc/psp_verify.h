/** \file  psp_verify.h
 **
 ** \brief Macros to run preprocessor checks on psp configuration
 **
 */
    
#ifndef _PSP_VERIFY_H_
#define _PSP_VERIFY_H_

/*
** Include Files
*/
#include "cfe_psp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
** Macro Definitions
*/

/** \brief MEM SCRUB Task Name Verification */
#ifndef MEMSCRUB_TASK_NAME
    #error "MEMSCRUB_TASK_NAME must be defined"
#endif

/** \brief MEM SCRUB Priority Verification */
#if (MEMSCRUB_DEFAULT_PRIORITY > MEMSCRUB_PRIORITY_UP_RANGE)
    #error "MEMSCRUB_DEFAULT_PRIORITY must be less than or equal to MEMSCRUB_PRIORITY_UP_RANGE!"
#endif

#if (MEMSCRUB_DEFAULT_PRIORITY < MEMSCRUB_PRIORITY_DOWN_RANGE)
    #error "MEMSCRUB_DEFAULT_PRIORITY must be greater than or equal to MEMSCRUB_PRIORITY_DOWN_RANGE!"
#endif


/** \brief SP0 File Path Verification */
#ifndef SP0_DATA_DUMP_FILEPATH
    #error "SP0_DATA_DUMP_FILEPATH must be defined"
#endif

/** \brief CDS File Path Verification */
#ifndef CFE_PSP_CFE_FLASH_FILEPATH
    #error "CFE_PSP_CFE_FLASH_FILEPATH must be defined"
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

#if NTPSYNC_DEFAULT_PRIORITY < 50
    #error "NTPSYNC_DEFAULT_PRIORITY must have lower priority than NTP Daemon task"
#elif NTPSYNC_DEFAULT_PRIORITY >= MEMSCRUB_DEFAULT_PRIORITY
    #error "NTPSYNC_DEFAULT_PRIORITY must have higher priority than MEMSCRUB_DEFAULT_PRIORITY"
#endif

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERIFY_H_ */

/*==============================================================================
** End of file psp_verify.h
**============================================================================*/
    