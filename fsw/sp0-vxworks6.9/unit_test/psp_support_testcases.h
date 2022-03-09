/*=======================================================================================
**
** File:  psp_support_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_support.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2021-01-05 | Minh Luong       | Initial revision
**=======================================================================================*/

#ifndef UT_PSP_SUPPORT_TESTCASES_H
#define UT_PSP_SUPPORT_TESTCASES_H

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
void Ut_CFE_PSP_Restart(void);
void Ut_CFE_PSP_Panic(void);
void Ut_CFE_PSP_FlushCaches(void);
void Ut_CFE_PSP_GetProcessorId(void);
void Ut_CFE_PSP_GetSpacecraftId(void);
void Ut_CFE_PSP_GetProcessorName(void);
void Ut_CFE_PSP_ToggleCFSBootPartition(void);
void Ut_CFE_PSP_GetBootStartupString(void);
void Ut_CFE_PSP_SetBootStartupString(void);
void Ut_CFE_PSP_GetBootStructure(void);
void Ut_CFE_PSP_SetBootStructure(void);
void Ut_CFE_PSP_PrintBootParameters(void);

#endif  /* UT_PSP_SUPPORT_TESTCASES_H */

/*=======================================================================================
** End of file psp_support_testcases.h
**=======================================================================================*/