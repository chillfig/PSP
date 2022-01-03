/**
 ** \file psp_start.h
 **
 ** \brief Header file for the PSP function prototypes in cfe_psp_start.c
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
 ** PSP Startup API for Aitech SP0. Functions implemented in this file are
 ** used to configure the SP0 target and the VxWorks OS, and gather information
 ** on how the system is setup.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

#ifndef _PSP_START_H_
#define _PSP_START_H_

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <taskLib.h>

/* Aitech BSP Specific */
#include <scratchRegMap.h>
#include <aimonUtil.h>

#include "cfe_psp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* MACRO to perform a bitwise AND operation */
#define CHECK_BIT(x, pos)       ((x) & (1U << (pos)))

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \brief List of MCHK Errors Messages
 */
const char * g_pMachineCheckCause_msg[10];

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
** \func Logs software reset type
**
** \par Description:
** This function determines if started in safe mode and logs off software reset type.
**
** \par Assumptions, External Events, and Notes:
** RESET_SRC_REG_ENUM is defined in Aitech file scratchRegMap.h
**
** \param resetSrc - Reset Type RESET_SRC_REG_ENUM
**
** \return None
*/
void CFE_PSP_LogSoftwareResetType(RESET_SRC_REG_ENUM resetSrc);

/**
** \func OSAL startup entry point
**
** \par Description:
** This function serves as the OSAL startup entry point.
**
** \par Assumptions, External Events, and Notes:
** This is an SP0-specific implementation so that we don't run the default
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
** This is an SP0-specific implementation.
** \n\n
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
 ** None
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
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

/**
** \brief Get the reset type and subtype
**
** \par Description:
** This function determines the reset type and subtype.
**
** \par Assumptions, External Events, and Notes:
** Reset Types are defined in Aitech headers.
** \n\n
** Function will save reset types to the respective global static variables:
** \li g_uiResetType
** \li g_uiResetSubtype
** \n\n
** Finally, function will print to console the reset type.
** \n\n
** Output defines are defined in Aitech file scratchRegMap.h
**
** \param None
**
** \return RESET_SRC_POR 
** \return RESET_SRC_WDT 
** \return RESET_SRC_FWDT 
** \return RESET_SRC_CPCI 
** \return RESET_SRC_SWR 
*/
static RESET_SRC_REG_ENUM CFE_PSP_ProcessResetType(void);

/**
 ** \brief Change system task priorities
 **
 ** \par Description:
 ** This function changes the system task priorities so that they are 
 ** lower than CFS system task priorities.
 **
 ** \par Assumptions, External Events, and Notes:
 ** tNet0 priority should be adjusted to be right below what ever
 ** gets defined for CI/TO apps in your system if using the network
 ** interface CCSDS/UDP for CI/TO apps.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
static int32 CFE_PSP_SetSysTasksPrio(void);

/**
 ** \func Add a list of symbolic link mappings 
 **
 ** \par Description:
 ** This function simply calls the OS_FileSysAddFixedMap multiple times
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
static int32 CFE_PSP_SetFileSysAddFixedMap(osal_id_t *fs_id);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_START_H_ */
