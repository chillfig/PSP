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

void Ut_CFE_PSP_GetRESETSize(void);
void Ut_CFE_PSP_GetVOLATILEDISKSize(void);
void Ut_CFE_PSP_GetUSERRESERVEDSize(void);
void Ut_CFE_PSP_GetMemSize(void);
void Ut_CFE_PSP_GetMemArea(void);
void Ut_CFE_PSP_CalculateCRC(void);
void Ut_CFE_PSP_WriteToCDS(void);
void Ut_CFE_PSP_ReadFromCDS(void);
void Ut_CFE_PSP_MEMORY_WriteToRESET(void);
void Ut_CFE_PSP_MEMORY_ReadFromRESET(void);
void Ut_CFE_PSP_MEMORY_WriteToVOLATILEDISK(void);
void Ut_CFE_PSP_MEMORY_ReadFromVOLATILEDISK(void);
void Ut_CFE_PSP_MEMORY_WriteToUSERRESERVED(void);
void Ut_CFE_PSP_MEMORY_ReadFromUSERRESERVED(void);
void Ut_CFE_PSP_GetCDSArea(void);
void Ut_CFE_PSP_GetResetArea(void);
void Ut_CFE_PSP_GetUserReservedArea(void);
void Ut_CFE_PSP_GetVolatileDiskMem(void);
void Ut_CFE_PSP_InitProcessorReservedMemory(void);
void Ut_CFE_PSP_SetupReservedMemoryMap(void);
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void);
void Ut_CFE_PSP_GetKernelTextSegmentInfo(void);
void Ut_CFE_PSP_GetCFETextSegmentInfo(void);
void Ut_CFE_PSP_MEMORY_RestoreRESET(void);
void Ut_CFE_PSP_MEMORY_RestoreCDS(void);
void Ut_CFE_PSP_MEMORY_RestoreVOLATILEDISK(void);
void Ut_CFE_PSP_MEMORY_RestoreUSERRESERVED(void);
void Ut_CFE_PSP_MEMORY_WriteToRAM(void);
void Ut_CFE_PSP_MEMORY_ReadFromRAM(void);
void Ut_CFE_PSP_MEMORY_RestoreDATA(void);
void Ut_CFE_PSP_MEMORY_SYNC_Init(void);
void Ut_CFE_PSP_MEMORY_SYNC_Destroy(void);
void Ut_CFE_PSP_MEMORY_SYNC_Start(void);
void Ut_CFE_PSP_MEMORY_SYNC_Stop(void);
void Ut_CFE_PSP_MEMORY_SYNC_isRunning(void);
void Ut_CFE_PSP_MEMORY_SYNC_setPriority(void);
void Ut_CFE_PSP_MEMORY_SYNC_getPriority(void);
void Ut_CFE_PSP_MEMORY_SYNC_setFrequency(void);
void Ut_CFE_PSP_MEMORY_SYNC_getFrequency(void);
void Ut_CFE_PSP_MEMORY_SYNC_getStatus(void);
void Ut_CFE_PSP_MEMORY_SYNC_Task(void);

#endif  /* UT_PSP_MEMORY_TESTCASES_H */
/*=======================================================================================
** End of file psp_memory_testcases.h
**=======================================================================================*/
