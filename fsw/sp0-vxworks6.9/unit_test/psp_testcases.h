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
*   \file psp_testcases.h
*
*   \brief This file contains function declarations of test cases for the PSP module.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef UT_PSP_TESTCASES_H
#define UT_PSP_TESTCASES_H

/*=======================================================================================
** Includes
**=======================================================================================*/

#include <errno.h>
#include "utassert.h"
#include "psp_start_testcases.h"
#include "psp_memory_testcases.h"
#include "psp_mem_scrub_testcases.h"
#include "psp_exception_testcases.h"
#include "psp_support_testcases.h"
#include "psp_watchdog_testcases.h"
#include "psp_sp0_info_testcases.h"
#include "psp_ntp_testcases.h"
#include "psp_flash_testcases.h"
#include "psp_vxworks_sysmon_testcases.h"
#include "psp_ping_testcases.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function Declarations
**=======================================================================================*/

void Ut_CFE_PSP_Setup(void);
void Ut_CFE_PSP_Teardown(void);
void Ut_CFE_PSP_InitEvent(void);
void Ut_CFE_PSP_EndTests(void);

#endif  /* UT_PSP_TESTCASES_H */

/*=======================================================================================
** End of file psp_testcases.h
**=======================================================================================*/


