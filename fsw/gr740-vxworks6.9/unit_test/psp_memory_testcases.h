/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file psp_memory_testcases.h
*
*   \brief This file contains function declarations of test cases for the cfe_psp_memory.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
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

#endif  /* UT_PSP_MEMORY_TESTCASES_H */
/*=======================================================================================
** End of file psp_memory_testcases.h
**=======================================================================================*/
