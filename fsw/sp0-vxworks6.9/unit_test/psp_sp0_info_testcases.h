/*=======================================================================================
**
** File:  psp_sp0_info_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_sp0_info.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2021-08-05 | Claudio Olmi     | Initial revision
**=======================================================================================*/

#ifndef UT_PSP_SP0_INFO_TESTCASES_H
#define UT_PSP_SP0_INFO_TESTCASES_H

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
void Ut_PSP_SP0_GetInfo(void);
void Ut_PSP_SP0_GetInfoTable(void);
void Ut_PSP_SP0_PrintToBuffer(void);
void Ut_PSP_SP0_DumpData(void);
void Ut_PSP_SP0_GetDiskFreeSize(void);
void Ut_PSP_SP0_ROM1_LOCK(void);
void Ut_PSP_SP0_ROM1_UNLOCK(void);
void Ut_PSP_SP0_ROM2_LOCK(void);
void Ut_PSP_SP0_ROM2_UNLOCK(void);
void Ut_PSP_SP0_ROMX_COMMAND(void);
void Ut_PSP_SP0_ROM1_Status(void);
void Ut_PSP_SP0_ROM2_Status(void);

#endif  /* UT_PSP_SP0_INFO_TESTCASES_H */

/*=======================================================================================
** End of file psp_sp0_info_testcases.h
**=======================================================================================*/