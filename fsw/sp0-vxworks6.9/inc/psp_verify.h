/** \file  psp_verify.h
 **
 ** \brief Macros to run preprocessor checks on psp configuration
 **
 */
    
#ifndef _PSP_VERIFY_H_
#define _PSP_VERIFY_H_

/*
** Pragmas
*/

/*
** Local Defines  
*/

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

/* MEM SCRUB */
#ifndef MEMSCRUB_TASK_NAME
    #error "MEMSCRUB_TASK_NAME must be defined"
#endif

#if (MEMSCRUB_DEFAULT_PRIORITY > MEMSCRUB_PRIORITY_UP_RANGE)
    #error "MEMSCRUB_DEFAULT_PRIORITY must be less than or equal to MEMSCRUB_PRIORITY_UP_RANGE!"
#endif

#if (MEMSCRUB_DEFAULT_PRIORITY < MEMSCRUB_PRIORITY_DOWN_RANGE)
    #error "MEMSCRUB_DEFAULT_PRIORITY must be greater than or equal to MEMSCRUB_PRIORITY_DOWN_RANGE!"
#endif


/* SP0 */
#ifndef SP0_DATA_DUMP_FILEPATH
    #error "SP0_DATA_DUMP_FILEPATH must be defined"
#endif

/* CDS */
#ifndef CFE_PSP_CFE_FLASH_FILEPATH
    #error "CFE_PSP_CFE_FLASH_FILEPATH must be defined"
#endif

/* Watchdog */
#ifndef CFE_PSP_WATCHDOG_DEFAULT_MSEC
    #error "CFE_PSP_WATCHDOG_DEFAULT_MSEC must be defined"
#elif CFE_PSP_WATCHDOG_DEFAULT_MSEC > 60000
    #error "Watchdog default value is set to larger than 60 seconds"
#elif CFE_PSP_WATCHDOG_DEFAULT_MSEC < 1
    #error "Watchdog default value is set to less than 1 millisecond"
#endif


#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERIFY_H_ */

/*==============================================================================
** End of file psp_verify.h
**============================================================================*/
    