/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file psp_memory_testcases.h
*
*   \brief This header file contains function declarations for test cases for the cfe_psp_memory.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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
void Ut_CFE_PSP_BootRecordFilepath(void);
void Ut_CFE_PSP_ExceptionStorageFilepath(void);
void Ut_CFE_PSP_GenerateFilepath(void);
void Ut_CFE_PSP_CheckURMFilesExists(void);
void Ut_CFE_PSP_FlushToFLASH(void);

#endif  /* UT_PSP_MEMORY_TESTCASES_H */
/*=======================================================================================
** End of file psp_memory_testcases.h
**=======================================================================================*/
