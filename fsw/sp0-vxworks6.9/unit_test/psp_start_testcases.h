/*=======================================================================================
**
** File:  psp_start_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_start.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-18 | Minh Luong       | Initial revision
**=======================================================================================*/

#ifndef UT_PSP_START_TESTCASES_H
#define UT_PSP_START_TESTCASES_H

/*=======================================================================================
** Includes
**=======================================================================================*/

#include <errno.h>

#include "utassert.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function Declarations
**=======================================================================================*/

void Ut_CFE_PSP_ProcessPOSTResults(void);
void Ut_CFE_PSP_ProcessResetType(void);
void Ut_CFE_PSP_GetRestartType(void);
void Ut_CFE_PSP_LogSoftwareResetType(void);
void Ut_OS_Application_Startup(void);
void Ut_CFE_PSP_LogSoftwareResetType(void);
void Ut_CFE_PSP_SetTaskPrio(void);
void Ut_CFE_PSP_SetSysTasksPrio(void);
void Ut_OS_Application_Run(void);
void Ut_CFE_PSP_Main(void);
void Ut_OS_Application_Run(void);
void Ut_CFE_PSP_SuspendConsoleShellTask(void);
void Ut_CFE_PSP_InitSSR(void);

#endif  /* UT_PSP_START_TESTCASES_H */

/*=======================================================================================
** End of file psp_start_testcases.h
**=======================================================================================*/