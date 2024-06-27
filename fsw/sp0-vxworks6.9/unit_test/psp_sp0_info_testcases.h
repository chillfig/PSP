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
*   \file psp_sp0_info_testcases.h
*
*   \brief This header file contains function declarations for test cases for the cfe_psp_sp0info.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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
