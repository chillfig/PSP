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

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERIFY_H_ */

/*==============================================================================
** End of file psp_verify.h
**============================================================================*/
    