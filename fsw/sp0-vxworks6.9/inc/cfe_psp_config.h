/*
** cfe_psp_config.h
**
**      Copyright (c) 2004-2011, United States Government as represented by
**      Administrator for The National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
*/

#ifndef _cfe_psp_config_
#define _cfe_psp_config_

#include "common_types.h"

#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include "speLib.h"
#include "excLib.h"
#include "taskLib.h"
#include "arch/ppc/esfPpc.h"

/**
 * \brief Period of the VxWorks timebase, in nanoseconds
 *
 * This is expressed as a ratio in case it is not a whole number.
 *
 * Multiplying the timebase register by 60 should yield a result
 * in nanoseconds, and then further dividing by the OSAL OS_time_t tick
 * resolution will convert to an OS_time_t compatible value.
 *
 * On the MCP750 - the PPC timebase runs at 60ns period or ~16.67 MHz.
 *
 * Note this is distinct from the VxWorks system timer tick which runs,
 * confusingly, at 60Hz or a ~16.67ms period.
 */
#define CFE_PSP_VX_TIMEBASE_PERIOD_NUMERATOR   60
#define CFE_PSP_VX_TIMEBASE_PERIOD_DENOMINATOR 1

/*
** This define sets the number of memory ranges that are defined in the memory range defintion
** table.
*/
#define CFE_PSP_MEM_TABLE_SIZE 10

/**
 * This define sets the maximum number of exceptions
 * that can be stored.
 *
 * It must always be a power of two.
 */
#define CFE_PSP_MAX_EXCEPTION_ENTRIES       4

/*
 * The tick period that will be configured in the RTOS for the simulated
 * time base, in microseconds.  This in turn is used to drive the 1hz clock
 * and other functions.
 *
 * On the MCP750 the sysClockRate runs at 60Hz so this is the same period
 * that the cFE software timebase will be configured at.
 */
/* Valid only when adding softtime module */
/* #define CFE_PSP_SOFT_TIMEBASE_PERIOD 16666 */

/*
** Typedef for the layout of the vxWorks boot record structure
**
** This is statically placed at the beginning of system memory (sysMemTop)
** which should be reserved in the kernel.
*/
typedef struct
{
   uint32 bsp_reset_type;
   uint32 spare1;
   uint32 spare2;
   uint32 spare3;

} CFE_PSP_ReservedMemoryBootRecord_t;


/**
 * \brief The data type used by the underlying OS to represent a thread ID.
 */
typedef TASK_ID CFE_PSP_Exception_SysTaskId_t;

/*
** Global variables
*/
typedef struct
{
    UINT32          timebase_upper; /* Upper 32 bits of timebase as sampled by hook */
    UINT32          timebase_lower; /* Lower 32 bits of timebase as sampled by hook */
    int             vector;         /* vector number */
    ESFPPC          esf;            /* Exception stack frame */
    UINT64          force64BitAlign;/* Force the spe register to 64 bit alignment*/
    SPE_CONTEXT     fp;             /* floating point registers */

} CFE_PSP_Exception_ContextDataEntry_t;

/*
** Watchdog minimum and maximum values ( in milliseconds )
*/
#define CFE_PSP_WATCHDOG_MIN (0)
#define CFE_PSP_WATCHDOG_MAX (0xFFFFFFFF)

/*
** Number of EEPROM banks on this platform
*/
#define CFE_PSP_NUM_EEPROM_BANKS 1

/*
 * The alignment to use for each reserved memory block.
 *
 * This is a mask to be applied to each block base address
 *
 * Chosen as the cache line size of the SP0 processor (32 bytes)
 * such that the blocks will be cached more efficiently.
 */
#define CFE_PSP_MEMALIGN_MASK  ((cpuaddr)0x1F)

/* The task and task priority for existing VxWorks task.
 */
typedef struct
{
    const char * VxWorksTaskName;
    int32        VxWorksTaskPriority;
} CFE_PSP_OS_Task_and_priority_t;

/*
** Default reading CDS method. Assume the reserved CDS memory is 
** always correct(Don't need CRC or alway read from Flash)
*/
#define CFE_PSP_CDS_READ_METHOD_DEFAULT     0

/*
** CRC reading CDS method. This will verify the CRC everytime reading
** CDS. The CRC also gets update on every CDS writing
*/
#define CFE_PSP_CDS_READ_METHOD_CRC         1

/*
** Flash reading CDS method. This will always read from Flash for every
** CDS reading.
*/
#define CFE_PSP_CDS_READ_METHOD_FLASH       2

/**
 *  \cfetimecfg Default EPOCH Values 
 * 
 *  \par Description:
 *      Enable or disable the Automatic time sync with the OS
 * 
 *  \par Limits
 *      Binary true or false
 */
#define CFE_MISSION_TIME_SYNC_OS_ENABLE true

/**
 *  \cfetimecfg Default EPOCH Values 
 * 
 *  \par Description:
 *      Default number of seconds between time synchronizations.
 *      CFE Time Service updates MET and STCF from VxWorks OS.
 *      When set to zero, CFE Time will be synchronized only once during start.
 * 
 *  \par Limits
 *      Positive integer
 */
#define CFE_MISSION_TIME_SYNC_OS_SEC 30

#endif  /* _cfe_psp_config_ */


