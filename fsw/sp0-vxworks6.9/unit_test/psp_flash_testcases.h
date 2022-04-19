/*=======================================================================================
**
** File:  psp_flash_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_flash.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2022-02-01 | Matthew Rumbel   | Initial revision
**=======================================================================================*/
#ifndef UT_PSP_FLASH_TESTCASES_H
#define UT_PSP_FLASH_TESTCASES_H
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
void Ut_CFE_PSP_ReadFromFlash(void);
void Ut_CFE_PSP_WriteToFlash(void);
void Ut_CFE_PSP_DeleteFile(void);
void Ut_CFE_PSP_CreateFile(void);
void Ut_CFE_PSP_CheckFile(void);
void Ut_CFE_PSP_CreateDirectory(void);
#endif  /* UT_PSP_FLASH_TESTCASES_H */
/*=======================================================================================
** End of file psp_flash_testcases.h
**=======================================================================================*/