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
**
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

void Ut_CFE_PSP_SetSysTasksPrio(void);
void Ut_CFE_PSP_FindProcessor(void);
void Ut_CFE_PSP_OS_EventHandler(void);
void Ut_OS_Application_Startup(void);
void Ut_OS_Application_Run(void);
void Ut_CFE_PSP_SetTaskPrio(void);
void Ut_CFE_PSP_SuspendConsoleShellTask(void);
void Ut_CFE_PSP_SetFileSysAddFixedMap(void);

#endif  /* UT_PSP_START_TESTCASES_H */

/*=======================================================================================
** End of file psp_start_testcases.h
**=======================================================================================*/