/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/
 
/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/*
 *
 *    Copyright (c) 2020, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

/**
 * \file
 * \ingroup  vxworks
 * \author   joseph.p.hickey@nasa.gov
 *
 */

#ifndef COVERAGETEST_PSP_SHARED_H
#define COVERAGETEST_PSP_SHARED_H

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

/*=======================================================================================
** Macro Definitions
**=======================================================================================*/

#define URM_SIZE 10000

/****************** Helper macros for PSP asserts ******************/
#define     UtAssert_OS_print(cText, Description) \
                UtAssert(Ut_OS_printfHistoryWithText(cText), Description, __FILE__, __LINE__)
#define     UtAssert_NoOS_print(cText, Description) \
                UtAssert(Ut_OS_printfHistoryWithText(cText) == false, Description, __FILE__, __LINE__)
#define     UtAssert_ES_WriteToSysLog(cText, Description) \
                UtAssert(Ut_ES_WriteToSysLogWithText(cText), Description, __FILE__, __LINE__)
#define     UtAssert_NoES_WriteToSysLog(cText, Description) \
                UtAssert(Ut_ES_WriteToSysLogWithText(cText) == false, Description, __FILE__, __LINE__)
#define     UtAssert_logMsg(cText, Description) \
                UtAssert(Ut_logMsgHistoryWithText(cText), Description, __FILE__, __LINE__)
#define     UtAssert_NologMsg(cText, Description) \
                UtAssert(Ut_logMsgHistoryWithText(cText) == false, Description, __FILE__, __LINE__)

/*=======================================================================================
** Data Structures
**=======================================================================================*/

/*=======================================================================================
** Function Declarations
**=======================================================================================*/
int32   Ut_OS_printf_Hook(void *UserObj, int32 StubRetcode,
                          uint32 CallCount, const UT_StubContext_t *Context, va_list va);
void    Ut_OS_printf_Setup(void);
bool    Ut_OS_printfHistoryWithText(const char *cText);
int32   Ut_ES_WriteToSysLog_Hook(void *UserObj, int32 StubRetcode,
                                 uint32 CallCount, const UT_StubContext_t *Context, va_list va);
void    Ut_ES_WriteToSysLog_Setup(void);
bool    Ut_ES_WriteToSysLogWithText(const char *cText);
int32   Ut_logMsg_hook(void *UserObj, int32 StubRetcode,
                       uint32 CallCount, const UT_StubContext_t *Context, va_list va);
void    Ut_logMsg_Setup(void);
bool    Ut_logMsgHistoryWithText(const char *cText);
uint8   Ut_OS_printf_MsgCount(void);

void Test_CFE_PSP_Exception_GetBuffer(void);
void Test_CFE_PSP_Exception_GetNextContextBuffer(void);
void Test_CFE_PSP_Exception_GetSummary(void);
void Test_CFE_PSP_Exception_CopyContext(void);

#endif
