/*=======================================================================================
**
** File:  psp_watchdog_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_watchdog.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
**=======================================================================================*/

#ifndef UT_PSP_WATCHDOG_TESTCASES_H
#define UT_PSP_WATCHDOG_TESTCASES_H

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
void Ut_CFE_PSP_WatchdogInit(void);
void Ut_CFE_PSP_WatchdogEnable(void);
void Ut_CFE_PSP_WatchdogDisable(void);
void Ut_CFE_PSP_WatchdogService(void);
void Ut_CFE_PSP_WatchdogGet(void);
void Ut_CFE_PSP_WatchdogSet(void);
void Ut_CFE_PSP_WatchdogStatus(void);

#endif  /* UT_PSP_WATCHDOG_TESTCASES_H */

/*=======================================================================================
** End of file psp_watchdog_testcases.h
**=======================================================================================*/