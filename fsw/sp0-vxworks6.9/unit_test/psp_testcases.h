/*=======================================================================================
**
** File:  psp_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the PSP module.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-18 | Minh Luong       | Initial revision
**    2021-01-05 | Minh Luong       | Added test cases header file for psp_exception_testcase.c, 
**               |                  | psp_support_testcases.c, psp_timer_testcases.c, and 
**               |                  | psp_watchdog_testcases.c
**=======================================================================================*/

#ifndef UT_PSP_TESTCASES_H
#define UT_PSP_TESTCASES_H

/*=======================================================================================
** Includes
**=======================================================================================*/

#include <errno.h>
#include "utassert.h"
#include "psp_start_testcases.h"
#include "psp_memory_testcases.h"
#include "psp_exception_testcases.h"
#include "psp_support_testcases.h"
/* #include "psp_timer_testcases.h" */
#include "psp_watchdog_testcases.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function Declarations
**=======================================================================================*/

void Ut_CFE_PSP_Setup(void);
void Ut_CFE_PSP_Teardown(void);
void Ut_CFE_PSP_InitEvent(void);

#endif  /* UT_PSP_TESTCASES_H */

/*=======================================================================================
** End of file psp_testcases.h
**=======================================================================================*/


