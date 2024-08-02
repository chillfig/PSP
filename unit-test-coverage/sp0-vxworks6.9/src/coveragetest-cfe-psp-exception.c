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
 * \author   
 *
 */

#include "coveragetest-psp-sp0-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include "cfe_psp.h"

#include "PCS_stdlib.h"
#include "PCS_stdio.h"
#include "PCS_sys_stat.h"
#include "PCS_fcntl.h"
#include "PCS_unistd.h"
#include "PCS_cfe_configdata.h"

#include "PCS_arch_sparc_esfSparc.h"
#include "PCS_arch_ppc_vxPpcLib.h"
#include "PCS_edrLib.h"
#include "PCS_edrLibP.h"

#include "target_config.h"

#include "cfe_psp_memory.h"
#include "cfe_psp_flash.h"
#include "cfe_psp_exceptionstorage_api.h"
#include "cfe_psp_exceptionstorage_types.h"

#define PSP_EXCEP_PRINT_SCOPE         "PSP EXC: "

extern bool g_ucOverRideDefaultedrPolicyHandlerHook;
extern PCS_EDR_POLICY_HANDLER_HOOK g_pDefaultedrPolicyHandlerHook;
extern bool CFE_PSP_edrPolicyHandlerHook(int type, void *pInfo_param, bool debug);
extern void Ut_OS_printf_Setup(void);
extern bool PCS_currentedrPolicyHandlerHook1(int type, void *pInfo_param, bool debug);
extern CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap;
extern Target_CfeConfigData GLOBAL_CFE_CONFIGDATA;

static uint8 ucLocalExceptionBuffer[1024];
static uint8 ucLocalEsf[sizeof(PCS_ESFSPARC)];

/*=======================================================================================
** Ut_CFE_PSP_edrPolicyHandlerHook(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_edrPolicyHandlerHook(void)
{
    bool    iRetCode = false;
    int32   iType = 0;
    PCS_EDR_TASK_INFO InfoParam = { .pEsf = (char*)ucLocalEsf };
    bool    bDebug = false;
    char    cMsg_increase_buffer[] = {PSP_EXCEP_PRINT_SCOPE "No more storage available in exception buffer. Increase PSP_EXCEP_PRINT_SCOPE\n"};
    char    cMsg_hook_not_setup[] = {PSP_EXCEP_PRINT_SCOPE "Override enabled, but Handler Hook is not setup\n"};

    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr = (void*)ucLocalExceptionBuffer;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    g_ucOverRideDefaultedrPolicyHandlerHook = false;
    UT_SetDeferredRetcode(UT_KEY(PCS_currentedrPolicyHandlerHook1), 1, true);
    g_pDefaultedrPolicyHandlerHook = (void *)&PCS_currentedrPolicyHandlerHook1;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True((iRetCode == true), "_CFE_PSP_edrPolicyHandlerHook() - 1/4: Nominal");

    /* ----- Test case #2 - Nominal with Override Policy Enabled ----- */
    UT_ResetState(0);
    /* Setup additional inputs */
    g_ucOverRideDefaultedrPolicyHandlerHook = true;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True((iRetCode == false), "_CFE_PSP_edrPolicyHandlerHook() - 2/4: Nominal with Override Policy Enabled");

    /* ----- Test case #3 - Failed CFE_PSP_Exception_GetNextContextBuffer returned NULL ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Setup additional inputs */
    g_ucOverRideDefaultedrPolicyHandlerHook = false;
    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr->NumRead = 9;
    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr->NumWritten = 13;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 1);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_increase_buffer, "_CFE_PSP_edrPolicyHandlerHook() - 3/4: Failed to get context buffer - message");
    UtAssert_True((iRetCode == false), "_CFE_PSP_edrPolicyHandlerHook() - 3/4: Failed to get context buffer - return code error");

    /* ----- Test case #4 - Nominal - g_pDefaultedrPolicyHandlerHook False ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Setup additional inputs */
    iRetCode = true;
    g_ucOverRideDefaultedrPolicyHandlerHook = false;
    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr->NumRead = 0;
    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr->NumWritten = 0;
    g_pDefaultedrPolicyHandlerHook = NULL;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_hook_not_setup, "_CFE_PSP_edrPolicyHandlerHook() - 3/4: Failed to get context buffer - message");
    UtAssert_True((iRetCode == false), "_CFE_PSP_edrPolicyHandlerHook() - 4/4: Failed No context buffer received");
}

/*=======================================================================================
** Ut_CFE_PSP_AttachExceptions(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_AttachExceptions(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookGet), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%x\n", &PCS_currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_AttachExceptions - 1/4: Nominal doesn't call the policy hook removal function");
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 1/4: Nominal attached exception handler");

    /* ----- Test case #2 - Nominal but failed to remove the policy hook ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_edrErrorPolicyHookRemove), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%p\n", &PCS_currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 2/4: Nominal failed remove the policy hook");
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 2/4: Nominal attached exception handler");

    /* ----- Test case #3 - Failed attached exception handler ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookGet), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookAdd), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "edrPolicyHandlerHookAdd() failed for CFE_PSP_edrPolicyHandlerHook()\n");
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 3/4: Failed attached exception handler");

    /* ----- Test case #4 - Nominal but failed to remove the policy hook ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_edrErrorPolicyHookRemove), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%x\n", &PCS_currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_AttachExceptions - 4/4: Nominal policy hook removal returned error");
    snprintf(cMsg, sizeof(cMsg), PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 4/4: Nominal attached exception handler");
}

/*=======================================================================================
** Ut_CFE_PSP_SetDefaultExceptionEnvironment(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    CFE_PSP_SetDefaultExceptionEnvironment();
    /* Verify outputs */
    UtAssert_STUB_COUNT(PCS_vxMsrSet, 1);
    UtAssert_STUB_COUNT(PCS_vxMsrGet, 1);
    UtAssert_STUB_COUNT(PCS_vxSpefscrSet, 1);
    UtAssert_STUB_COUNT(PCS_vxSpefscrGet, 1);
}

/*=======================================================================================
** Ut_CFE_PSP_ExceptionGetSummary_Impl(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_ExceptionGetSummary_Impl(void)
{
    int32   iRetCode = 0;
    CFE_PSP_Exception_LogData_t buffer = {};
    char    cReasonBuf[100] = {'\0'};
    char    tName[] = "NULL";
    char    tName_too_long[] = "NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL_NULL";
    char    cMsg[] = PSP_EXCEP_PRINT_SCOPE "Could not save the whole exception reason string on buffer\n";
    char    cMsg_snprintf_error[] = PSP_EXCEP_PRINT_SCOPE "Could not save exception reason on buffer\n";
    uint32  uiReasonSize = 100;

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskName), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_taskName), (void *)tName, sizeof(tName), false);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 1/6: Nominal output string parameter value was set");
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_ExceptionGetSummary_Impl() - 1/6: Nominal");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - task name not found ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskName), CFE_PSP_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 2/6: TASK ID not found");
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_ExceptionGetSummary_Impl() - 2/6: TASK ID not found");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - ReasonBuf is null ----- */
    /* Setup additional inputs */
    /* Execute tests*/
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, NULL, uiReasonSize);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 3/6: ReasonBuf is NULL");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Buffer is null ----- */
    /* Setup additional inputs */
    /* Execute tests*/
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(NULL, cReasonBuf, uiReasonSize);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 4/6: Buffer is NULL");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #5 - Nominal ReasonBuf too short ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskName), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_taskName), (void *)tName_too_long, sizeof(tName_too_long), false);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ExceptionGetSummary_Impl() - 5/6: Nominal - ReasonBuf too short - Message");
    UtAssert_True(strlen(cReasonBuf) == (uiReasonSize - 1), "_CFE_PSP_ExceptionGetSummary_Impl() - 5/6: Nominal - ReasonBuf length same as ReasonSize");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 5/6: ReasonBuf too short - Return code error");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #6 - snprintf error ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskName), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_taskName), (void *)tName, sizeof(tName), false); 
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_snprintf_error, "_CFE_PSP_ExceptionGetSummary_Impl() - 6/6: Nominal - snprintf error - Message");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 6/6: snprintf error - Return code error");
}
