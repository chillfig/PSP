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
*   \file psp_support_testcases.h
*
*   \brief This file contains function declarations of test cases for the cfe_psp_support.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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
void Ut_CFE_PSP_ValidatePath(void);
void Ut_CFE_PSP_GetBootStartupString(void);
void Ut_CFE_PSP_SetBootStartupString(void);
void Ut_CFE_PSP_GetBootStructure(void);
void Ut_CFE_PSP_SetBootStructure(void);
void Ut_CFE_PSP_PrintBootParameters(void);
void Ut_CFE_PSP_KernelGetCRC(void);
void Ut_CFE_PSP_KernelLoadNew(void);

#endif  /* UT_PSP_SUPPORT_TESTCASES_H */

/*=======================================================================================
** End of file psp_support_testcases.h
**=======================================================================================*/
