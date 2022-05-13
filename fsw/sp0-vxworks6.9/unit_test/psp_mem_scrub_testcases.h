/*=======================================================================================
**
** File:  psp_mem_scrub_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_memscrub.c.
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
void Ut_CFE_PSP_MemScrubInit(void);
void Ut_CFE_PSP_MemScrubTask(void);
void Ut_CFE_PSP_MemScrubIsRunning(void);
void Ut_CFE_PSP_MemScrubSet(void);
void Ut_CFE_PSP_MemScrubValidate(void);
void Ut_CFE_PSP_MemScrubGet(void);
void Ut_CFE_PSP_MemScrubEnable(void);
void Ut_CFE_PSP_MemScrubDisable(void);
void Ut_CFE_PSP_MemScrubDelete(void);
void Ut_CFE_PSP_MemScrubTrigger(void);
void Ut_CFE_PSP_MemScrubErrStats(void);
#endif  /* UT_PSP_MEM_SCRUB_TESTCASES_H */
/*=======================================================================================
** End of file psp_mem_scrub_testcases.h
**=======================================================================================*/