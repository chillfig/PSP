/*=======================================================================================
**
** File:  psp_timer_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_timer.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
**=======================================================================================*/
#ifndef UT_PSP_TIMER_TESTCASES_H
#define UT_PSP_TIMER_TESTCASES_H
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

void Ut_CFE_PSP_InitLocalTime(void);
void Ut_vxTimeBaseGet(void);

#endif  /* UT_PSP_TIMER_TESTCASES_H */

/*=======================================================================================
** End of file psp_timer_testcases.h
**=======================================================================================*/
