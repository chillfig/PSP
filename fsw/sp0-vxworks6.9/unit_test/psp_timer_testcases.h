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
**    2021-01-05 | Minh Luong       | Initial revision
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
void Ut_CFE_PSP_GetTime(void);
void Ut_CFE_PSP_Get_Timer_Tick(void);
void Ut_CFE_PSP_GetTimerTicksPerSecond(void);
void Ut_CFE_PSP_GetTimerLow32Rollover(void);
void Ut_CFE_PSP_Get_Timebase(void);
void Ut_CFE_PSP_Get_Dec(void);

#endif  /* UT_PSP_TIMER_TESTCASES_H */

/*=======================================================================================
** End of file psp_timer_testcases.h
**=======================================================================================*/