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
void Ut_CFE_PSP_WriteToRESET(void);
void Ut_CFE_PSP_ReadFromRESET(void);
void Ut_CFE_PSP_WriteToVOLATILEDISK(void);
void Ut_CFE_PSP_ReadFromVOLATILEDISK(void);
void Ut_CFE_PSP_WriteToUSERRESERVED(void);
void Ut_CFE_PSP_ReadFromUSERRESERVED(void);
void Ut_CFE_PSP_GetCDSArea(void);
void Ut_CFE_PSP_GetResetArea(void);
void Ut_CFE_PSP_GetUserReservedArea(void);
void Ut_CFE_PSP_GetVolatileDiskMem(void);
void Ut_CFE_PSP_InitProcessorReservedMemory(void);
void Ut_CFE_PSP_SetupReservedMemoryMap(void);
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void);
void Ut_CFE_PSP_GetKernelTextSegmentInfo(void);
void Ut_CFE_PSP_GetCFETextSegmentInfo(void);
void Ut_CFE_PSP_RestoreReset(void);
void Ut_CFE_PSP_RestoreCDS(void);
void Ut_CFE_PSP_RestoreVolatileDisk(void);
void Ut_CFE_PSP_RestoreUserReserved(void);
void Ut_CFE_PSP_WriteToRAM(void);
void Ut_CFE_PSP_ReadFromRAM(void);
void Ut_CFE_PSP_RestoreData(void);
void Ut_CFE_PSP_MemSyncInit(void);
void Ut_CFE_PSP_MemSyncDestroy(void);
void Ut_CFE_PSP_MemSyncStart(void);
void Ut_CFE_PSP_MemSyncStop(void);
void Ut_CFE_PSP_MemSyncIsRunning(void);
void Ut_CFE_PSP_MemSyncSetPriority(void);
void Ut_CFE_PSP_MemSyncGetPriority(void);
void Ut_CFE_PSP_MemSyncSetFrequency(void);
void Ut_CFE_PSP_MemSyncGetFrequency(void);
void Ut_CFE_PSP_MemSyncgetStatus(void);
void Ut_CFE_PSP_MemSyncTask(void);
void Ut_CFE_PSP_CDSFilepath(void);
void Ut_CFE_PSP_ResetFilepath(void);
void Ut_CFE_PSP_VolatileDiskFilepath(void);
void Ut_CFE_PSP_UserReservedFilepath(void);
void Ut_CFE_PSP_GenerateFilepath(void);
void Ut_CFE_PSP_CheckURMFilesExists(void);
void Ut_CFE_PSP_FlushToFLASH(void);

#endif  /* UT_PSP_MEMORY_TESTCASES_H */
/*=======================================================================================
** End of file psp_memory_testcases.h
**=======================================================================================*/
