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
*   \file psp_mem_scrub_testcases.h
*
*   \brief This header file contains function declarations for test cases for the cfe_psp_memscrub.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef UT_PSP_MEM_SCRUB_TESTCASES_H
#define UT_PSP_MEM_SCRUB_TESTCASES_H

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
void Ut_CFE_PSP_MemScrubInit(void);
void Ut_CFE_PSP_MemScrubTask(void);
void Ut_CFE_PSP_MemScrubIsRunning(void);
void Ut_CFE_PSP_MemScrubSet(void);
void Ut_CFE_PSP_MemScrubValidate(void);
void Ut_CFE_PSP_MemScrubGet(void);
void Ut_CFE_PSP_MemScrubEnable(void);
void Ut_CFE_PSP_MemScrubDisable(void);
void Ut_CFE_PSP_MemScrubDelete(void);
void Ut_CFE_PSP_MemScrubTrigger(void);
void Ut_CFE_PSP_MemScrubErrStats(void);
#endif  /* UT_PSP_MEM_SCRUB_TESTCASES_H */
/*=======================================================================================
** End of file psp_mem_scrub_testcases.h
**=======================================================================================*/