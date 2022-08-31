/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
**  Author:  A. Cudmore
**
**  Purpose:  This file contains the cFE Platform Support Package(PSP)
**            prototypes.
**            The PSP routines serve as the "glue" between the RTOS and
**            the cFE Flight Software. The routines fill gaps that are not
**            really considered part of the OS Abstraction, but are required
**            for the cFE flight software implementation. It is possible that
**            some of these routines could migrate into the OS AL.
**
*/

/**
 ** \file
 **
 ** \brief Main PSP public API functions
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved.\n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description:
 ** This file contains the cFE Platform Support Package(PSP) prototypes.
 ** The PSP functions serve as the "glue" between the RTOS and the cFS.\n
 ** The functions fill gaps that are not really considered part of the 
 ** OSAL, but are required for the cFE implementation. It is possible
 ** that some of these functions could migrate into the OSAL.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef CFE_PSP_H
#define CFE_PSP_H

/*
** Include Files
*/

#include "common_types.h"
#include "osapi.h"
#include "cfe_psp_error.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

/*
** Macro Definitions
*/

/* Definitions for PSP PANIC types */
/** \name Definitions for PSP PANIC types */
/** \{ */
#define CFE_PSP_PANIC_STARTUP            1   /**< \brief  Startup */
#define CFE_PSP_PANIC_VOLATILE_DISK      2   /**< \brief  Volatile Disk */
#define CFE_PSP_PANIC_MEMORY_ALLOC       3   /**< \brief  Memory Allocation */
#define CFE_PSP_PANIC_NONVOL_DISK        4   /**< \brief  Nonvolatile Disk */
#define CFE_PSP_PANIC_STARTUP_SEM        5   /**< \brief  Startup Semaphore */
#define CFE_PSP_PANIC_CORE_APP           6   /**< \brief  Core App */
#define CFE_PSP_PANIC_GENERAL_FAILURE    7   /**< \brief  Generic Failure */
/** \} */

/* Macros for the file loader */
/** \name Macros for the file loader */
/** \{ */
#define BUFF_SIZE    256   /**< \brief Buffer Size */
#define SIZE_BYTE      1   /**< \brief Size Byte */
#define SIZE_HALF      2   /**< \brief Size Half */
#define SIZE_WORD      3   /**< \brief Size Word */
/** \} */

/*
** Define memory types
*/
/** \name Define Memory Types */
/** \{ */
#define CFE_PSP_MEM_RAM        1   /**< \brief Memory RAM */
#define CFE_PSP_MEM_EEPROM     2   /**< \brief Memory EEPROM */
#define CFE_PSP_MEM_ANY        3   /**< \brief Memory ANY */
#define CFE_PSP_MEM_INVALID    4   /**< \brief Memory INVALID */
/** \} */

/*
** Define Memory Read/Write Attributes
*/
/** \name Define Memory Read/Write Attributes */
/** \{ */
#define CFE_PSP_MEM_ATTR_WRITE        0x01   /**< \brief Memory Attribute Write */
#define CFE_PSP_MEM_ATTR_READ         0x02   /**< \brief Memory Attribute Read */
#define CFE_PSP_MEM_ATTR_READWRITE    0x03   /**< \brief Memory Attribute ReadWrite */
/** \} */

/*
** Define the Memory Word Sizes
*/
/** \name Define the Memory Word Sizes */
/** \{ */
#define CFE_PSP_MEM_SIZE_BYTE     0x01   /**< \brief Memory Size Byte */
#define CFE_PSP_MEM_SIZE_WORD     0x02   /**< \brief Memory Size Word */
#define CFE_PSP_MEM_SIZE_DWORD    0x04   /**< \brief Memory Size DoubleWord */
/** \} */

/*
 * Common definition for reset types at the PSP layer
 */
/** \name Reset Types */
/** \{ */
#define CFE_PSP_RST_TYPE_PROCESSOR      1  /**< Volatile disk, Critical Data Store and User Reserved memory could still be valid */
#define CFE_PSP_RST_TYPE_POWERON        2  /**< All memory has been cleared */
#define CFE_PSP_RST_TYPE_CFS_TOGGLE     3  /**< Switch CFS partition and set to clear all memory */
#define CFE_PSP_RST_TYPE_SHELL          4  /**< Restart to VxWorks shell */
#define CFE_PSP_RST_TYPE_MAX            5  /**< Placeholder to indicate 1+ the maximum value that the PSP will ever use. */
/** \} */

/*
** Reset Sub-Types
*/
/** \name Reset Sub-Types */
/** \{ */
#define CFE_PSP_RST_SUBTYPE_POWER_CYCLE           1  /**< \brief  Reset caused by power having been removed and restored */
#define CFE_PSP_RST_SUBTYPE_PUSH_BUTTON           2  /**< \brief  Reset caused by reset button on the board having been pressed */
#define CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND    3  /**< \brief  Reset was caused by a reset line having been stimulated by a hardware special command */
#define CFE_PSP_RST_SUBTYPE_HW_WATCHDOG           4  /**< \brief  Reset was caused by a watchdog timer expiring */
#define CFE_PSP_RST_SUBTYPE_RESET_COMMAND         5  /**< \brief  Reset was caused by cFE ES processing a \link #CFE_ES_RESTART_CC Reset Command \endlink */
#define CFE_PSP_RST_SUBTYPE_EXCEPTION             6  /**< \brief  Reset was caused by a Processor Exception */
#define CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET       7  /**< \brief  Reset was caused in an unknown manner */
#define CFE_PSP_RST_SUBTYPE_HWDEBUG_RESET         8  /**< \brief  Reset was caused by a JTAG or BDM connection */
#define CFE_PSP_RST_SUBTYPE_BANKSWITCH_RESET      9  /**< \brief  Reset reverted to a cFE POWERON due to a boot bank switch */
#define CFE_PSP_RST_SUBTYPE_MAX                  10  /**< \brief  Placeholder to indicate 1+ the maximum value that the PSP will ever use. */
/** \} */

/**
 ** \brief The name of the software/RTOS timebase for general system timers.
 **
 ** This name may be referred to by CFE TIME and/or SCH when setting up its own timers.
 */
#define CFE_PSP_SOFT_TIMEBASE_NAME "cFS-Master"

/*
** Type Definitions
*/


/*
** Memory table type
*/


/*
** Function prototypes
*/

/**
** \func Main entry-point
**
** \par Description:
** This function is the entry point that the real time OS calls to start cFS.
** This function will do any BSP/OS-specific setup, then call the entry point
** of cFS, which is this function.
**
** \par Assumptions, External Events, and Notes:
** cFE should not call this function.  See the description.
**
** \param None
**
** \return None
*/
void  CFE_PSP_Main(void);

/**
** \func Get time
**
** \par Description:
** Sample/Read a monotonic platform clock with normalization
** \n\n
** Outputs an OS_time_t value indicating the time elapsed since an epoch.  The
** epoch is not defined, but typically represents the system boot time.  The
** value increases continuously over time and cannot be reset by software.
** \n\n
** This is similar to the CFE_PSP_Get_Timebase(), but additionally it normalizes
** the output value to an OS_time_t, thereby providing consistent units to
** the calling application.  Any OSAL-provided routine accepts OS_time_t inputs
** may be used to convert this value into other standardized time units.
**
** \par Assumptions, External Events, and Notes:
** This should refer to the same time domain as CFE_PSP_Get_Timebase(),
** the primary difference being the format and units of the output value.
**
** \param[out] LocalTime - Pointer to the structure that stores the returned time value
**
** \return None
*/
void  CFE_PSP_GetTime(OS_time_t *LocalTime);

/**
** \func Restart target
**
** \par Description:
** This function is the entry point back to the BSP to restart the processor.
** cFE calls this function to restart the processor.
**
** \par Assumptions, External Events, and Notes:
** Depending on the resetType, the function will modify boot startup string.
** \li #CFE_PSP_RST_TYPE_SHELL will clear the boot string and allow
** the VxWorks kernel to load the shell terminal. Then restart with
** #CFE_PSP_RST_TYPE_POWERON.
** \li #CFE_PSP_RST_TYPE_CFS_TOGGLE will modify the boot startup
** string to the next available CFS device name. Then restart with
** #CFE_PSP_RST_TYPE_PROCESSOR.
** \li #CFE_PSP_RST_TYPE_PROCESSOR & #CFE_PSP_RST_TYPE_POWERON will not modify
** the boot startup string and let the CFE code handle it.
** \n
** By default, the restart function is called with the BOOT_CLEAR option.
**
** TODO: Need to evaluate if resetControl(0,0,1) would be a better option
**
** TODO: Need to evaluate if resetControl(0,0,1) would be a better option
**
** \param[in] resetType - Type of cFE reset
**                        #CFE_PSP_RST_TYPE_POWERON
**                        #CFE_PSP_RST_TYPE_PROCESSOR
**                        #CFE_PSP_RST_TYPE_SHELL
**                        #CFE_PSP_RST_TYPE_CFS_TOGGLE
**
** \return None
*/
void  CFE_PSP_Restart(uint32 resetType);

/**
** \func Get restart type
**
** \par Description:
** This function returns the last reset type.  
**
** \par Assumptions, External Events, and Notes:
** If a pointer to a valid memory space is passed in, it returns the reset 
** sub-type in that memory. Right now the reset types are application-specific.
**
** \param[out] resetSubType - Pointer to the variable that stores the returned reset sub-type
**
** \return Last reset type
*/
uint32  CFE_PSP_GetRestartType(uint32 *resetSubType );

/**
** \func Flush memory caches
**
** \par Description:
** This function flushes the processor caches. 
**
** \par Assumptions, External Events, and Notes:
** This function is in the PSP because it is sometimes implemented in hardware 
** and sometimes taken care of by the OS.
** \n\n
** This function is not implemented for the SP0-vxworks6.9 PSP since it is 
** managed by the SP0 BSP/VxWorks OS.
**
** \param[in] type - Cache memory type
** \param[in] address - Pointer to the cache memory address
** \param[in] size - Cache memory size
**
** \return None
*/
void  CFE_PSP_FlushCaches(uint32 type, void* address, uint32 size);

/**
** \func Get the CPU ID
**
** \par Description:
** This function returns the CPU ID as pre-defined by the cFE for
** specific board and BSP.
**
** \par Assumptions, External Events, and Notes:
** The macro is defined in cfe_platform_cfg.h.
**
** \param None
**
** \return #CFE_PSP_CPU_ID
*/
uint32  CFE_PSP_GetProcessorId(void);

/**
** \func Get the spacecraft ID
**
** \par Description:
** This function returns the spacecraft ID as pre-defined by the cFE.
**
** \par Assumptions, External Events, and Notes:
** The macro is defined in cfe_platform_cfg.h.
**
** \param None
**
** \return #CFE_PSP_SPACECRAFT_ID
*/
uint32  CFE_PSP_GetSpacecraftId(void);

/**
** \func Get the processor name
**
** \par Description:
** This function returns the CPU name as pre-defined by the cFE.
**
** \par Assumptions, External Events, and Notes:
** The macro is defined in cfe_platform_cfg.h.
**
** \param None
**
** \return #CFE_PSP_CPU_NAME
*/
const char*  CFE_PSP_GetProcessorName(void);

/**
** \func Get the timer ticks per second
**
** \par Description:
** This function provides the number of ticks per second based on the memory
** bus clock speed. For example, an SP0s uses 400 MHz core clock speed. Memory
** bus speed is 1/8 of the core clock speed, or 50 MHz, thus 50 million ticks
** per second.
** 
** \par Assumptions, External Events, and Notes:
** The timer resolution for accuracy should not be any slower than 1000000 ticks 
** per second, or 1 microsecond per tick.
**
** \param None
**
** \return Number of timer ticks per second
*/
uint32 CFE_PSP_GetTimerTicksPerSecond(void);

/**
** \func Get the lower 32-bit roll-over time value
**
** \par Description:
** This function provides the number that the least significant 32-bit
** of the 64-bit timestamp returned by CFE_PSP_Get_Timebase() rolls over.
** 
** \par Assumptions, External Events, and Notes:
** If the lower 32-bits rolls at 1 second, then the CFE_PSP_TIMER_LOW32_ROLLOVER
** will be 1000000.  If the lower 32-bits rolls at its maximum value (2^32) then
** CFE_PSP_TIMER_LOW32_ROLLOVER will be 0.
**
** \param None
**
** \return The lower 32-bit value of the roll-over time value
*/
uint32  CFE_PSP_GetTimerLow32Rollover(void);

/**
** \func Get the timebase values
**
** \par Description:
** This function provides the time values of the 32-bit upper and lower registers.
** 
** \par Assumptions, External Events, and Notes:
** This function is in the BSP because it is sometimes implemented in hardware and
** sometimes taken care of by the OS.
**
** \param[out] Tbu - Pointer to the returned value of the 32-bit upper register
** \param[out] Tbl - Pointer to the returned value of the 32-bit lower register
**
** \return None
*/
void  CFE_PSP_Get_Timebase(uint32 *Tbu, uint32 *Tbl);

/**
** \func Get the size of the Critical Data Store memory area
**
** \par Description:
** This function fetches the size of the OS Critical Data Store memory area.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] SizeOfCDS - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_GetCDSSize(uint32 *SizeOfCDS);

/**
** \func Write to the Critical Data Store memory area
**
** \par Description:
** This function write the specified data to the specified memory area of the CDS.
** 
** \par Assumptions, External Events, and Notes:
** Inability to write to FLASH does not affect return code because the reserve memory
** is the golden copy while flash is just a backup
**
** \param[in] PtrToDataToWrite - Pointer to the data buffer to be written
** \param[in] CDSOffset - Memory offset from the beginning of the CDS block
** \param[in] NumBytes - Number of bytes to be written
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_WriteToCDS(const void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes);

/**
** \func Read from the Critical Data Store memory area
**
** \par Description:
** This function reads from the CDS memory area.
** 
** \par Assumptions, External Events, and Notes:
** Inability to read from FLASH does not affect return code because the reserve memory
** is the golden copy while flash is just a backup
**
** \param[out] PtrToDataFromRead - Pointer to the data buffer that stores the read data
** \param[in] CDSOffset - Memory offset from the beginning of the CDS block
** \param[in] NumBytes - Number of bytes to be read
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_ReadFromCDS(void *PtrToDataFromRead, uint32 CDSOffset, uint32 NumBytes);

/**
** \func Get the location and size of the ES Reset memory area
**
** \par Description:
** This function returns the location and size of the ES Reset memory area. This area 
** is preserved during a processor reset and is used to store the ER Log, System Log 
** and reset related variables.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToResetArea - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfResetArea - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetResetArea (cpuaddr *PtrToResetArea, uint32 *SizeOfResetArea);

/**
** \func Get the location and size of the cFE user-reserved memory area
**
** \par Description:
** This function returns the location and size of the cFE user-reserved memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToUserArea - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfUserArea - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetUserReservedArea(cpuaddr *PtrToUserArea, uint32 *SizeOfUserArea );

/**
** \func Get the location and size of the cFE volatile memory area
**
** \par Description:
** This function returns the location and size of the cFE volatile memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToVolDisk - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfVolDisk - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetVolatileDiskMem(cpuaddr *PtrToVolDisk, uint32 *SizeOfVolDisk );

/**
** \func Get the location and size of the kernel text segment
**
** \par Description:
** This function returns the location and size of the kernel text segment of the memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToKernelSegment - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfKernelSegment - Pointer to the variable that stores returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_GetKernelTextSegmentInfo(cpuaddr *PtrToKernelSegment, uint32 *SizeOfKernelSegment);

/**
** \func Get the location and size of the cFE text segment
**
** \par Description:
** This function returns the location and size of the cFE text segment of the memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToCFESegment - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfCFESegment - Pointer to the variable that stores returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_GetCFETextSegmentInfo(cpuaddr *PtrToCFESegment, uint32 *SizeOfCFESegment);

/**
** \func Initialize the watchdog timer
**
** \par Description:
** This function configures and initializes the watchdog timer.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void  CFE_PSP_WatchdogInit(void);

/**
** \func Enable the watchdog timer
**
** \par Description:
** This function enables the watchdog timer.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void  CFE_PSP_WatchdogEnable(void);

/**
** \func Disable the watchdog timer
**
** \par Description:
** This function disables the watchdog timer.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void  CFE_PSP_WatchdogDisable(void);

/**
** \func Service the watchdog timer
**
** \par Description:
** This function services the watchdog timer according to the value set in CFE_PSP_WatchdogSet().
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void  CFE_PSP_WatchdogService(void);

/**
** \func Get the watchdog time
**
** \par Description:
** This function fetches the watchdog time, in milliseconds.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return The watchdog time in milliseconds
*/
uint32  CFE_PSP_WatchdogGet(void);

/**
** \func Set the watchdog time
**
** \par Description:
** This function sets the current watchdog time, in milliseconds.
**
** \par Assumptions, External Events, and Notes:
** Although the WatchDog can be set to nano-seconds precision, the implementation
** only allows milliseconds precision.
**
** \param[in] watchDogValue_ms - watchdog time in milliseconds
**
** \return None
*/
void  CFE_PSP_WatchdogSet(uint32 watchDogValue_ms);


/**
 ** \func Check if watchdog is enabled or disabled
 **
 ** \par Description:
 ** This functions returns the status of the Watchdog
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will return true of watchdog is enabled or false if
 ** watchdog is disabled
 **
 ** \param None
 **
 ** \return true - if Watchdog is currently enabled
 ** \return false - if Watchdog is current disabled
 */
bool CFE_PSP_WatchdogStatus(void);

/**
** \func Abort cFE startup
**
** \par Description:
** This function provides the mechanism to abort the cFE startup process, 
** attempt to recover or clean up, and then reboots. If too many reboot 
** attempts, function will exit to console.
**
** \par Assumptions, External Events, and Notes:
** This function should not be called by the cFS applications.
**
** \param[in] errorCode - Error code that causes the exit
**
** \return None
*/
void  CFE_PSP_Panic(int32 errorCode);

/**
** \func Initialize the Solid State Recorder
**
** \par Description:
** This function configures and initializes the Solid State Recorder for 
** a particular platform.
**
** \par Assumptions, External Events, and Notes:
** - For the SP0-vxworks6.9, this is not implemented
** - For the GR740-vxworks6.9, this simply creates a ram drive
**
** \param[in] bus - ATA controller number
** \param[in] device - ATA drive number
** \param[in] DeviceName - Name of the XBD device to create
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED
*/
int32  CFE_PSP_InitSSR(uint32 bus, uint32 device, char *DeviceName );

/**
** \func Initialize exception handling
**
** \par Description:
** This function sets up the exception environment for a particular platform.
** 
** \par Assumptions, External Events, and Notes:
** For VxWorks, this function initializes the EDR policy handling. The handler is 
** called for every exception that other handlers do not handle.
** \n
** - For SP0-vxworks6.9, note that the floating point exceptions are handled by the default floating 
** point exception handler, which does a graceful recovery from floating point 
** exceptions in the file speExcLib.c.
**
** \param None
**
** \return None
*/
void  CFE_PSP_AttachExceptions(void);

/**
** \func Initialize default exception handling
**
** \par Description:
** This function sets up a default exception environment for a particular platform.
** 
** \par Assumptions, External Events, and Notes:
** For VxWorks, the exception environment is local to each task. Therefore, this
** must be called for each task that wants to do floating point and catch exceptions.
** Currently, this is automatically called from OS_TaskRegister() for every task.
**
** \param None
**
** \return None
*/
void  CFE_PSP_SetDefaultExceptionEnvironment(void);

/**
** \func Get the exception count
**
** \par Description:
** This function fetches the exception count.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return The exception count
*/
uint32  CFE_PSP_Exception_GetCount(void);

/**
** \func Translate a stored exception log entry into a summary string
**
** \par Description:
** This function takes a stored exception-log entry and converts it into a summary string.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] ContextLogId - Pointer to the variable that stores the returned log ID
** \param[out] TaskId - Pointer to the variable that stores the returned OSAL task ID
** \param[out] ReasonBuf - The buffer that stores the returned string
** \param[in] ReasonSize - The maximum length of the buffer, ReasonBuf
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_Exception_GetSummary(uint32 *ContextLogId, osal_id_t *TaskId, char *ReasonBuf, uint32 ReasonSize);

/**
** \func Translate a stored exception log entry into a summary string
**
** \par Description:
** This function takes a stored exception-log entry and converts it into a summary string.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] ContextLogId - The stored exception log ID
** \param[out] ContextBuf - Pointer to the variable that stores the copied data
** \param[in] ContextSize - The maximum length of the buffer, ContextBuf
**
** \return The actual size of the copied data
** \return #CFE_PSP_NO_EXCEPTION_DATA
** \return #CFE_PSP_INVALID_POINTER
*/
int32  CFE_PSP_Exception_CopyContext(uint32 ContextLogId, void *ContextBuf, uint32 ContextSize);

/*
** I/O Port API
*/

/**
** \func Read one byte from memory
**
** \par Description:
** This function reads one byte from the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PortAddress - The port address to read from
** \param[out] ByteValue - Pointer to the variable that stores the one-byte value read
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_INVALID_MEM_ADDR
*/
int32  CFE_PSP_PortRead8(cpuaddr PortAddress, uint8 *ByteValue);

/**
** \func Write one byte to memory
**
** \par Description:
** This function writes one byte to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PortAddress - The port address to write to
** \param[in] ByteValue - One-byte value to be written
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_ADDR
*/
int32  CFE_PSP_PortWrite8(cpuaddr PortAddress, uint8 ByteValue);

/**
** \func Read two bytes from memory
**
** \par Description:
** This function reads two bytes from the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PortAddress - The port address to read from
** \param[out] uint16Value - Pointer to the variable that stores the two-byte value read
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED
** \return #CFE_PSP_INVALID_MEM_ADDR
*/
int32  CFE_PSP_PortRead16(cpuaddr PortAddress, uint16 *uint16Value);

/**
** \func Write two bytes to memory
**
** \par Description:
** This function writes two bytes to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PortAddress - The port address to write to
** \param[in] uint16Value - Two-byte value to be written
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_ADDR
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED
*/
int32  CFE_PSP_PortWrite16(cpuaddr PortAddress, uint16 uint16Value);

/**
** \func Read four bytes from memory
**
** \par Description:
** This function reads four bytes from the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PortAddress - The port address to read from
** \param[out] uint32Value - Pointer to the variable that stores the four-byte value read
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_POINTER
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED
*/
int32 CFE_PSP_PortRead32(cpuaddr PortAddress, uint32 *uint32Value);

/**
** \func Write four bytes to memory
**
** \par Description:
** This function writes four bytes to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PortAddress - The port address to write to
** \param[in] uint32Value - Four-byte value to be written
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_ADDR
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED
*/
int32  CFE_PSP_PortWrite32(cpuaddr PortAddress, uint32 uint32Value);

/*
** Memory API
*/

/**
** \func Read an 8-bit value from memory
**
** \par Description:
** This function reads an 8-bit value from the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to read from
** \param[out] ByteValue - Pointer to the variable that stores the 8-bit value read
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_MemRead8(cpuaddr MemoryAddress, uint8 *ByteValue);

/**
** \func Write an 8-bit value to memory
**
** \par Description:
** This function writes an 8-bit value to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to write to
** \param[in] ByteValue - An 8-bit value to be written
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_MemWrite8(cpuaddr MemoryAddress, uint8 ByteValue);

/**
** \func Read an 16-bit value from memory
**
** \par Description:
** This function reads a 16-bit value from the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to read from
** \param[out] uint16Value - Pointer to the variable that stores the 16-bit value read
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_MemRead16(cpuaddr MemoryAddress, uint16 *uint16Value);

/**
** \func Write 16-bit value to memory
**
** \par Description:
** This function writes a 16-bit value to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to write to
** \param[in] uint16Value - A 16-bit value to be written
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_MemWrite16(cpuaddr MemoryAddress, uint16 uint16Value);

/**
** \func Read a 32-bit value from memory
**
** \par Description:
** This function reads a 32-bit value from the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to read from
** \param[out] uint32Value - Pointer to the variable that stores the 32-bit value read
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_MemRead32(cpuaddr MemoryAddress, uint32 *uint32Value);

/**
** \func Write a 32-bit value to memory
**
** \par Description:
** This function writes a 32-bit value to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to write to
** \param[in] uint32Value - A 32-bit value to be written
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_MemWrite32(cpuaddr MemoryAddress, uint32 uint32Value);

/**
 ** \func Copy from one memory block to another memory block
 **
 ** \par Description:
 ** Copies 'size' byte from memory address pointed by 'src' to memory
 ** address pointed by ' dst' For now we are using the standard c library
 ** call 'memcpy' but if we find we need to make it more efficient then
 ** we'll implement it in assembly.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] dest - Pointer to an address to copy to
 ** \param[in] src - Pointer address to copy from
 ** \param[in] size - Number of bytes to copy
 **
 ** \return #CFE_PSP_SUCCESS 
 */
int32  CFE_PSP_MemCpy(void *dest, const void *src, uint32 size);

/**
 ** \func Initialize the specified memory block with the specified value
 **
 ** \par Description:
 ** Copies 'size' number of byte of value 'value' to memory address pointed
 ** by 'dst' .For now we are using the standard c library call 'memset'
 ** but if we find we need to make it more efficient then we'll implement
 ** it in assembly.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] dest - Pointer to destination address
 ** \param[in] value - An 8-bit value to fill in the memory
 ** \param[in] size - The number of values to write
 **
 ** \return #CFE_PSP_SUCCESS
 */
int32  CFE_PSP_MemSet(void *dest, uint8 value, uint32 size);

/**
** \func Validate memory range and type
**
** \par Description:
** This function validates the memory range and type using the global CFE_PSP_MemoryTable.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] Address - A 32-bit starting address of the memory range
** \param[in] Size - A 32-bit size of the memory range (Address+Size = End Address)
** \param[in] MemoryType - The memory type to validate, including but not limited to:
**                         CFE_PSP_MEM_RAM, CFE_PSP_MEM_EEPROM, or CFE_PSP_MEM_ANY.
**                         Any defined CFE_PSP_MEM_* enumeration can be specified
**
** \return #CFE_PSP_SUCCESS - Memory range and type information is valid and can be used.
** \return #CFE_PSP_INVALID_MEM_ADDR - Starting address is not valid
** \return #CFE_PSP_INVALID_MEM_TYPE - Memory type associated with the range does not 
**                                     match the passed in type.
** \return #CFE_PSP_INVALID_MEM_RANGE - The Memory range associated with the address is not
**                                      large enough to contain Address+Size.
*/
int32  CFE_PSP_MemValidateRange(cpuaddr Address, size_t Size, uint32 MemoryType);

/**
** \func Get the number of memory ranges
**
** \par Description:
** This function fetches the number of memory ranges from the global CFE_PSP_MemoryTable.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return The number of entries in the CFE_PSP_MemoryTable
*/
uint32  CFE_PSP_MemRanges(void);

/**
** \func Set an entry in the memory range table
**
** \par Description:
** This function populates an entry in the global CFE_PSP_MemoryTable.
** 
** \par Assumptions, External Events, and Notes:
** Because the table is fixed size, the entries are set by using the integer index.
** No validation is done with the address or size.
**
** \param[in] RangeNum - A 32-bit integer (starting with 0) specifying the MemoryTable entry.
** \param[in] MemoryType - The memory type to validate, including but not limited to:
**                         CFE_PSP_MEM_RAM, CFE_PSP_MEM_EEPROM, or CFE_PSP_MEM_ANY.
**                         Any defined CFE_PSP_MEM_* enumeration can be specified
** \param[in] StartAddr - A 32-bit starting address of the memory range
** \param[in] Size - A 32-bit size of the memory range (Address+Size = End Address)
** \param[in] WordSize - The minimum addressable size of the range:
**                       (CFE_PSP_MEM_SIZE_BYTE, CFE_PSP_MEM_SIZE_WORD, CFE_PSP_MEM_SIZE_DWORD)
** \param[in] Attributes - The attributes of the Memory Range:
**                         (CFE_PSP_MEM_ATTR_WRITE, CFE_PSP_MEM_ATTR_READ, 
**                          CFE_PSP_MEM_ATTR_READWRITE)
**
** \return #CFE_PSP_SUCCESS - Memory range set successfully
** \return #CFE_PSP_INVALID_MEM_RANGE - The index into the table is invalid
** \return #CFE_PSP_INVALID_MEM_TYPE - Memory type associated with the range does not match
**                                     the passed in type.
** \return #CFE_PSP_INVALID_MEM_WORDSIZE - The WordSize parameter is not one of the 
**                                         types.
** \return #CFE_PSP_INVALID_MEM_ATTR - The Attributes parameter is not one of the 
**                                     predefined types.
*/
int32  CFE_PSP_MemRangeSet(uint32 RangeNum, uint32 MemoryType, cpuaddr StartAddr,
                           size_t Size, size_t WordSize, uint32 Attributes);

/**
** \func Get an entry in the memory range table
**
** \par Description:
** This function retrieves an entry in the global CFE_PSP_MemoryTable.
** 
** \par Assumptions, External Events, and Notes:
** Because the table is fixed size, the entries are set by using the integer index.
**
** \param[in] RangeNum - A 32-bit integer (starting with 0) specifying the MemoryTable entry.
** \param[out] MemoryType - A pointer to the 32-bit integer where the Memory Type is stored.
**                          Any defined CFE_PSP_MEM_* enumeration can be specified
** \param[out] StartAddr - A pointer to the 32-bit integer where the 32-bit starting address
**                       of the memory range is stored.
** \param[out] Size - A pointer to the 32-bit integer where the 32-bit size of the memory
**                    range is stored.
** \param[out] WordSize - A pointer to the 32-bit integer where the the minimum addressable 
**                        size of the range: (CFE_PSP_MEM_SIZE_BYTE, CFE_PSP_MEM_SIZE_WORD,
**                        CFE_PSP_MEM_SIZE_DWORD) is stored.
** \param[out] Attributes - A pointer to the 32-bit integer where the attributes of the 
**                          memory range: (CFE_PSP_MEM_ATTR_WRITE, CFE_PSP_MEM_ATTR_READ,
**                          CFE_PSP_MEM_ATTR_READWRITE) are stored.
**
** \return #CFE_PSP_SUCCESS - Memory range returned successfully
** \return #CFE_PSP_INVALID_POINTER - Parameter error
** \return #CFE_PSP_INVALID_MEM_RANGE - The index into the table is invalid
*/
int32  CFE_PSP_MemRangeGet(uint32 RangeNum, uint32 *MemoryType, cpuaddr *StartAddr,
                           size_t *Size, size_t *WordSize, uint32 *Attributes);

/**
** \func Write an 8-bit value to memory
**
** \par Description:
** This function writes an 8-bit value to the specified memory.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to write to
** \param[in] ByteValue - An 8-bit value to be written
**
** \return #CFE_PSP_SUCCESS - Data wrote successfully
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED - The Address is not aligned to 16-bit addressing scheme.
*/
int32  CFE_PSP_EepromWrite8(cpuaddr MemoryAddress, uint8 ByteValue);

/**
** \func Write a 16-bit value to memory
**
** \par Description:
** This function writes a 16-bit value to the specified memory.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to write to
** \param[in] uint16Value - A 16-bit value to be written
** 
** \return #CFE_PSP_SUCCESS - Data wrote successfully
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED - The Address is not aligned to 16-bit addressing scheme.
*/
int32  CFE_PSP_EepromWrite16(cpuaddr MemoryAddress, uint16 uint16Value);

/**
** \func Write a 32-bit value to memory
**
** \par Description:
** This function writes a 32-bit value to the specified memory.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] MemoryAddress - The memory address to write to
** \param[in] uint32Value - A 32-bit value to be written 
**
** \return #CFE_PSP_SUCCESS - Data wrote successfully
** \return #CFE_PSP_ERROR_ADDRESS_MISALIGNED - The Address is not aligned to 16-bit addressing scheme.
*/
int32  CFE_PSP_EepromWrite32(cpuaddr MemoryAddress, uint32 uint32Value);

/**
** \func Enable EEPROM for write operations
**
** \par Description:
** This function enables the specified EEPROM bank for write operations.
**
** \par Assumptions, External Events, and Notes:
** This function is currently not implemented.
**
** \param[in] Bank - The EEPROM bank to enable
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_EepromWriteEnable(uint32 Bank);

/**
** \func Disable EEPROM from write operations
**
** \par Description:
** This function disables the specified EEPROM bank from write operations.
**
** \par Assumptions, External Events, and Notes:
** This function is currently not implemented.
**
** \param[in] Bank - The EEPROM bank to disable
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_EepromWriteDisable(uint32 Bank);

/**
** \func Power on the EEPROM
**
** \par Description:
** This function powers on the specified EEPROM bank.
**
** \par Assumptions, External Events, and Notes:
** This function is currently not implemented.
**
** \param[in] Bank - The EEPROM bank to power on
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_EepromPowerUp(uint32 Bank);

/**
** \func Power down the EEPROM
**
** \par Description:
** This function powers down the specified EEPROM bank.
**
** \par Assumptions, External Events, and Notes:
** This function is currently not implemented.
**
** \param[in] Bank - The EEPROM bank to power down
**
** \return #CFE_PSP_SUCCESS
*/
int32  CFE_PSP_EepromPowerDown(uint32 Bank);

/**
 ** \func Obtain the PSP version/baseline identifier string
 **
 ** \par Description:
 ** This retrieves the PSP version identifier string without extra info.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return Version string.  This is a fixed string and cannot be NULL.
 */
const char *CFE_PSP_GetVersionString(void);

/**
 ** \func Obtain the version code name
 **
 ** \par Description:
 ** This retrieves the PSP code name.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This is a compatibility indicator for the overall cFS ecosystem. 
 ** All modular components which are intended to interoperate should 
 ** report the same code name.
 **
 ** \param None
 **
 ** \return Code name. This is a fixed string and cannot be NULL.
 */
const char *CFE_PSP_GetVersionCodeName(void);

/**
 ** \func Obtain the PSP numeric version numbers as uint8 values
 **
 ** \par Description:
 ** This retrieves the numeric PSP version identifier as an array of 4 uint8 values.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The array of numeric values is in order of precedence:
 ** \li [0] = Major Number
 ** \li [1] = Minor Number
 ** \li [2] = Revision Number
 ** \li[3] = Mission Revision
 ** \n
 ** The "Mission Revision" (last output) also indicates whether this is an
 ** official release, a patched release, or a development version.
 ** \li 0 indicates an official release
 ** \li 1-254 local patch level (reserved for mission use)
 ** \li 255 indicates a development build
 **
 ** \param[out] VersionNumbers  A fixed-size array to be filled with the version numbers
 **
 ** \return None
 */
void CFE_PSP_GetVersionNumber(uint8 VersionNumbers[4]);

/**
 ** \func Obtain the PSP library numeric build number
 **
 ** \par Description:
 ** The build number is a monotonically increasing number that (coarsely)
 ** reflects the number of commits/changes that have been merged since the
 ** epoch release. 
 **
 ** \par Assumptions, External Events, and Notes:
 ** During development cycles this number should increase after each 
 ** subsequent merge/modification.  Like other version information, this 
 ** is a fixed number assigned at compile time.
 **
 ** \param None
 **
 ** \return The PSP library build number
 */
uint32 CFE_PSP_GetBuildNumber(void);

/**
** \} <!-- End of group "psp_public_api" -->
*/

#endif  /* _cfe_psp_ */
