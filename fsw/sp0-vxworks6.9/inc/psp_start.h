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

/**
 ** \brief List of MCHK Errors Messages
 */
const char * g_pMachineCheckCause_msg[10];

/**
** \func Print Power On Self Test (POST) results to the console
**
** \par Description:
** None
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
** \func Determines the reset type and subtype
**
** \par Description:
** Reset Types are defined in Aitech headers
** Function will save reset types to the respective global static variables:
** - g_uiResetType
** - g_uiResetSubtype
** Finally, function will print to console the reset type
**
** \par Assumptions, External Events, and Notes:
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
** \func Determines if started in safe mode and logs off nominal resets.
**
** \par Description:
** None
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
** \func Application startup entry point from OSAL BSP.
**
** \par Description:
** SP0 Implementation Specific
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void OS_Application_Startup(void);

/**
** \func Application Run entry point from OSAL BSP.
**
** \par Description:
** SP0 Implementation Specific
**
** \par Assumptions, External Events, and Notes:
** This function is declared but empty so that we don't run the default OSAL
** equivalent function. The latter will actively suspend the console shell.
**
** \param None
**
** \return None
*/
void OS_Application_Run(void);

/**
 ** \func Function Suspend/Resume the Console Shell Task.
 **
 ** \par Description:
 ** None
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
** This function returns the last reset type.  If a pointer to a valid
** memory space is passed in, it returns the reset sub-type in that memory.
** Right now the reset types are application-specific. For the cFE, they
** are defined in the cfe_es.h file.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] resetSubType - Pointer to the variable that stores the returned reset sub-type
**
** \return Last reset type
*/
uint32 CFE_PSP_GetRestartType(uint32 *resetSubType);

/**
 ** \func Changes default task priority to a given priority
 **
 ** \par Description:
 ** None
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
 ** \func Changes system task priorities so that they are lower than CFS system
 ** task priorities
 **
 ** \par Description:
 ** None
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

#ifdef __cplusplus
}
#endif

#endif /* _PSP_START_H_ */
