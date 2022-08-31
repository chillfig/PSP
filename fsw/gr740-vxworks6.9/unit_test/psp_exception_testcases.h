/*=======================================================================================
**
** File:  psp_exception_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_exception.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
**=======================================================================================*/

#ifndef UT_PSP_EXCEPTION_TESTCASES_H
#define UT_PSP_EXCEPTION_TESTCASES_H

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
void Ut_CFE_PSP_edrPolicyHandlerHook(void);
void Ut_CFE_PSP_AttachExceptions(void);
void Ut_CFE_PSP_SetDefaultExceptionEnvironment(void);
void Ut_CFE_PSP_ExceptionGetSummary_Impl(void);

#endif  /* UT_PSP_EXCEPTION_TESTCASES_H */

/*=======================================================================================
** End of file psp_exception_testcases.h
**=======================================================================================*/