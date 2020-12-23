/*=======================================================================================
**
** File:  psp_memory_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_memory.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-18 | Minh Luong       | Initial revision
**=======================================================================================*/

#ifndef ut_psp_memory_testcases_h
#define ut_psp_memory_testcases_h

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
void Ut_CFE_PSP_GetCDSSize(void);
void Ut_CFE_PSP_SetReadCDSMethod(void);
void Ut_CFE_PSP_GetReadCDSMethod(void);
void Ut_CFE_PSP_SetStaticCRC(void);
void Ut_CFE_PSP_GetStaticCRC(void);
void Ut_CFE_PSP_CalculateCRC(void);
void Ut_CFE_PSP_ReadCDSFromFlash(void);
void Ut_CFE_PSP_WriteCDSToFlash(void);


#endif  /* ut_psp_memory_testcases_h */

/*=======================================================================================
** End of file psp_memory_testcases.h
**=======================================================================================*/