/**
 ** \file  psp_start.h
 **
 ** \brief Header file for the PSP function prototypes in cfe_psp_start.c
 **
 ** \copyright
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description:
 ** None
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

/**
** \func Log the Power On Self Test (POST) results to the system log.
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
** \func Determines the reset type and logs off nominal resets.
**
** \par Description:
** None
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
static int32 SetTaskPrio(const char* tName, int32 tgtPrio);

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
static int32 SetSysTasksPrio(void);

/**
 ** \func Provides stub function for FPU exception handler, vxFpscrGet()
 **
 ** \par Description:
 **    Added this function here so that the code can compile & run without error.
 **
 **    If there's code that calls these functions, we will get a message like so,
 **       > ld < cfe-core.o
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrGet.
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrSet.
 **
 **    These do not seem to be included in 85xx build, but are defined as
 **       "defined(_PPC_) && CPU != PPC440" in vxWorks osapi.c, line 2707, v4.2.1a
 **
 **    If this function is not used, stub it out like below. Otherwise, define it.
 **
 ** \par Assumptions, External Events, and Notes:
 ** If still relevant, have OSAL add conditional compile when SPE preset instead of FPU
 ** Once that has occurred we can remove vxFpscrGet and vxFpscrSet
 **
 ** \param None
 **
 ** \return 0 - Integer Zero
 */
unsigned int vxFpscrGet(void);

/**
 ** \func Provides stub function for FPU exception handler, vxFpscrSet()
 **
 ** \par Description:
 **    Added this function here so that the code can compile & run without error.
 **
 **    If there's code that calls these functions, we will get a message like so,
 **       > ld < cfe-core.o
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrGet.
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrSet.
 **
 **    These do not seem to be included in 85xx build, but are defined as
 **       "defined(_PPC_) && CPU != PPC440" in vxWorks osapi.c, line 2707, v4.2.1a
 **
 **    If this function is not used, stub it out like below. Otherwise, define it.
 **
 ** \par Assumptions, External Events, and Notes:
 ** If still relevant, have OSAL add conditional compile when SPE preset instead of FPU
 ** Once that has occurred we can remove vxFpscrGet and vxFpscrSet
 **
 ** \param x - Unused
 **
 ** \return None
 */
void vxFpscrSet(unsigned int x);
