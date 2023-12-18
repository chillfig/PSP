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
*   \file cfe_psp_start.h
*
*   \brief Header file for the PSP function prototypes in cfe_psp_start.c
*
*   \brief PSP Startup API for GR740. Functions implemented in this file are used to configure the GR740 
*   target and the VxWorks OS, and gather information on how the system is setup.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_START_H
#define PSP_START_H

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <taskLib.h>



#include "cfe_psp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_gr740vx69 PSP Public APIs - GR740-VxWorks6.9 Platform
** \{
*/

/* MACRO to perform a bitwise AND operation on a specific bit position */
#define CHECK_BIT(x, pos)       ((x) & (1U << (pos)))

/* MACRO to perform a bitwise AND operation */
#define BITWISE_AND(x, y)       ((x) & (y))

/**
 ** \brief Default PSP StartupTimer pre-print string 
 ** \par Description:
 ** This string is printed before every print related to StartupTimer API.
*/
#define PSP_STARTUP_TIMER_PRINT_SCOPE                     "PSP Startup: "

/**
** \func Output POST results
**
** \par Description:
** This function prints the Power-On Self-Test (POST) results to the console.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void CFE_PSP_ProcessPOSTResults(void);

/**
 ** \func Gracefully Restart CFS
 ** 
 ** \par Description:
 ** If we reached this point, something failed or it is taking too long to
 ** complete. If that is the case, function will log the attempt and restart the
 ** SP0 target with the appropriate restart type.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** If for some reason the file containing the restart attempts located in the 
 ** currently active CFS flash partition is corrupted, the file will be deleted
 ** and new data will be added with counters starting from zero.
 ** This function requires the execution of #CFE_PSP_GetActiveCFSPartition and
 ** #CFE_PSP_GetBootStartupString to gather the active CFS flash partition and
 ** the boot string respectively.
 **
 ** \param[in] timer_id - is the ID of the timer that triggered the call
 **
 ** \return None
 */
void CFE_PSP_StartupFailedRestartSP0_hook(osal_id_t timer_id);

/**
 ** \func Delete startup timer and status file
 ** 
 ** \par Description:
 ** This function is called at the end of #OS_Application_Startup function which
 ** identifies the end of PSP startup and when CFE can restart logic is ready.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 **
 */
int32 CFE_PSP_StartupClear(void);

/**
 ** \func  Retrieve the currently active CFS partition
 ** 
 ** \par Description: 
 ** The Flash memory is split into 2 partitions each containing 1 CFS instance.
 ** This function will return the device name such as: "/ffx0" or "/ffx1"
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** To figure out which one is loaded in RAM, this function will check if the
 ** kernel is exposing a specific CFS support variable that contains the 
 ** currently active CFS partition.
 ** If variable is not available, function will assume a single Flash partition 
 ** and fallback to "/ffx0".
 ** Return code only refers to the availability of the kernel variable.
 ** 
 ** \param[out] pBuffer - Pointer to the buffer that will receive the string
 ** \param[in] uBuffer_size - Maximum size of the buffer
 ** 
 ** \return #CFE_PSP_SUCCESS - when Kernel has symbol
 ** \return #CFE_PSP_ERROR - when Kernel does not have support
 */
int32 CFE_PSP_GetActiveCFSPartition(char *pBuffer, uint32 uBuffer_size);

/**
 **
 ** \func Setup and Starts the Startup Timer
 **
 ** \par Description:
 ** During CFS startup, and before CFE is ready to start the apps, CFS cannot
 ** catch errors and restart. This function sets up a callback function
 ** triggered by a timer. When CFS loads within a configurable time, the
 ** #OS_Application_Startup function will complete and the timer reset. If the
 ** end of the function is not reached before the timer expires, the callback
 ** function will restart the target and log the attempt on the currently active
 ** partition.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_StartupTimer(void);

/**
** \func OSAL startup entry point
**
** \par Description:
** This function serves as the OSAL startup entry point.
**
** \par Assumptions, External Events, and Notes:
** This is an GR740-specific implementation so that we don't run the default
** OSAL-equivalent function.
**
** \param None
**
** \return None
*/
void OS_Application_Startup(void);

/**
** \func OSAL run entry point
**
** \par Description:
** This function serves as the PSP run entry point.
**
** \par Assumptions, External Events, and Notes:
** This function is declared but empty so that we don't run the default
** OSAL-equivalent function. The latter will actively suspend the console shell.
**
** \param None
**
** \return None
*/
void OS_Application_Run(void);

/**
 ** \func Suspend/Resume the Console Shell Task
 **
 ** \par Description:
 ** This function suspends/resumes the Console Shell task.
 **
 ** \par Assumptions, External Events, and Notes:
 ** Implementation is BSP independent
 **
 ** \param[in] suspend - True to suspend task, False to resume task
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SuspendConsoleShellTask(bool suspend);

/**
** \func Get restart type
**
** \par Description:
** This function returns the last reset type.
**
** \par Assumptions, External Events, and Notes:
** If a pointer to a valid memory space is passed in, it returns the reset 
** sub-type in that memory. Right now the reset types are application-specific. 
** For the cFE, they are defined in the cfe_es.h file.
**
** \param[out] resetSubType - Pointer to the variable that stores the returned reset sub-type
**
** \return Last reset type
*/
uint32 CFE_PSP_GetRestartType(uint32 *resetSubType);

/**
 ** \func Set task priority
 **
 ** \par Description:
 ** This function sets the new task priority for a given task name.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] tName - Task name
 ** \param[in] tgtPrio - New task priority
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetTaskPrio(const char* tName, uint8 tgtPrio);

/**
 ** \func Add a list of symbolic link mappings 
 **
 ** \par Description:
 ** This function simply calls the OS_FileSysAddFixedMap multiple times
 **
 ** \par Assumptions, External Events, and Notes:
 ** Implementation is BSP independent
 **
 ** \param[out] fs_id - Buffer to store the ID of the file system mapping
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetFileSysAddFixedMap(osal_id_t *fs_id);


/**
** \} <!-- End of group "psp_public_api_gr740vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_START_H_ */
