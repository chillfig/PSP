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


