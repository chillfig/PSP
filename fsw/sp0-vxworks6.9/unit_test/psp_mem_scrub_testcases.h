/*=======================================================================================
**
** File:  psp_mem_scrub_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_mem_scrub.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-09-22 | Matthew Rumbel   | Initial revision
**=======================================================================================*/
#ifndef UT_PSP_MEM_SCRUB_TESTCASES_H
#define UT_PSP_MEM_SCRUB_TESTCASES_H
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
void Ut_CFE_PSP_MEM_SCRUB_Init(void);
void Ut_CFE_PSP_MEM_SCRUB_Task(void);
void Ut_CFE_PSP_MEM_SCRUB_isRunning(void);
void Ut_CFE_PSP_MEM_SCRUB_Set(void);
void Ut_CFE_PSP_MEM_SCRUB_Status(void);
void Ut_CFE_PSP_MEM_SCRUB_Enable(void);
void Ut_CFE_PSP_MEM_SCRUB_Disable(void);
void Ut_CFE_PSP_MEM_SCRUB_Delete(void);
void Ut_CFE_PSP_MEM_SCRUB_ErrStats(void);
#endif  /* UT_PSP_MEM_SCRUB_TESTCASES_H */
/*=======================================================================================
** End of file psp_mem_scrub_testcases.h
**=======================================================================================*/