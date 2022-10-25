/*=======================================================================================
**
** File:  psp_sp0_info_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_sp0info.c.
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
void Ut_CFE_PSP_SP0CollectStaticInfo(void);
void Ut_CFE_PSP_SP0GetStaticInfoTable(void);
void Ut_CFE_PSP_SP0CollectDynamicInfo(void);
void Ut_CFE_PSP_SP0GetDynamicInfoTable(void);
void Ut_CFE_PSP_SP0PrintToBuffer(void);
void Ut_CFE_PSP_SP0DumpData(void);
void Ut_CFE_PSP_SP0GetDiskFreeSize(void);
void Ut_CFE_PSP_SP0ROM1Lock(void);
void Ut_CFE_PSP_SP0ROM1Unlock(void);
void Ut_CFE_PSP_SP0ROM2Lock(void);
void Ut_CFE_PSP_SP0ROM2Unlock(void);
void Ut_CFE_PSP_SP0ROMXCmd(void);
void Ut_CFE_PSP_SP0ROM1Status(void);
void Ut_CFE_PSP_SP0ROM2Status(void);

#endif  /* UT_PSP_SP0_INFO_TESTCASES_H */

/*=======================================================================================
** End of file psp_sp0_info_testcases.h
**=======================================================================================*/