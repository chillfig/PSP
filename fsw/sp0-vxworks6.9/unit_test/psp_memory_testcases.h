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
#ifndef UT_PSP_MEMORY_TESTCASES_H
#define UT_PSP_MEMORY_TESTCASES_H
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
void Ut_CFE_PSP_CalculateCRC(void);
void Ut_CFE_PSP_ReadCDSFromFlash(void);
void Ut_CFE_PSP_WriteCDSToFlash(void);
void Ut_CFE_PSP_WriteToCDS(void);
void Ut_CFE_PSP_ReadFromCDS(void);
void Ut_CFE_PSP_GetResetArea(void);
void Ut_CFE_PSP_GetUserReservedArea(void);
void Ut_CFE_PSP_GetVolatileDiskMem(void);
void Ut_CFE_PSP_InitProcessorReservedMemory(void);
void Ut_CFE_PSP_SetupReservedMemoryMap(void);
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void);
void Ut_CFE_PSP_GetKernelTextSegmentInfo(void);
void Ut_CFE_PSP_GetCFETextSegmentInfo(void);
void Ut_CFE_PSP_MEMORY_SYNC_Enable(void);
void Ut_CFE_PSP_MEMORY_SYNC_Disable(void);
#endif  /* UT_PSP_MEMORY_TESTCASES_H */
/*=======================================================================================
** End of file psp_memory_testcases.h
**=======================================================================================*/
