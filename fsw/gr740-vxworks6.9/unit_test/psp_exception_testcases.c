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
*   \file psp_exception_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_exception.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"

#include "psp_exception_testcases.h"
#include "../src/cfe_psp_exception.c"

extern int currentedrPolicyHandlerHook1(int type, void *pInfo_param, bool debug);

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_edrPolicyHandlerHook(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_edrPolicyHandlerHook(void)
{
    bool    iRetCode = false;
    int32   iType = 0;
    EDR_TASK_INFO InfoParam = {};
    bool    bDebug = false;
    char    cMsg_increase_buffer[] = {PSP_EXCEP_PRINT_SCOPE "No more storage available in exception buffer. Increase PSP_EXCEP_PRINT_SCOPE\n"};

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    g_ucOverRideDefaultedrPolicyHandlerHook = false;
    UT_SetDeferredRetcode(UT_KEY(currentedrPolicyHandlerHook1), 1, true);
    g_pDefaultedrPolicyHandlerHook = &currentedrPolicyHandlerHook1;
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
    g_pDefaultedrPolicyHandlerHook = NULL;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True((iRetCode == false), "_CFE_PSP_edrPolicyHandlerHook() - 4/4: Failed No context buffer received");
}

/*=======================================================================================
** Ut_CFE_PSP_AttachExceptions(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_AttachExceptions(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookGet), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%x\n", &currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_AttachExceptions - 1/4: Nominal doesn't call the policy hook removal function");
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 1/4: Nominal attached exception handler");

    /* ----- Test case #2 - Nominal but failed to remove the policy hook ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(edrErrorPolicyHookRemove), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%p\n", &currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 2/4: Nominal failed remove the policy hook");
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 2/4: Nominal attached exception handler");

    /* ----- Test case #3 - Failed attached exception handler ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookGet), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookAdd), 1, OS_ERROR);
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "edrPolicyHandlerHookAdd() failed for CFE_PSP_edrPolicyHandlerHook()\n");
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 3/4: Failed attached exception handler");

    /* ----- Test case #2 - Nominal but failed to remove the policy hook ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(edrErrorPolicyHookRemove), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%x\n", &currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_AttachExceptions - 4/4: Nominal policy hook removal returned error");
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 4/4: Nominal attached exception handler");
}

/*=======================================================================================
** Ut_CFE_PSP_SetDefaultExceptionEnvironment(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    CFE_PSP_SetDefaultExceptionEnvironment();
    /* Verify outputs */
    UtAssert_NA("_CFE_PSP_SetDefaultExceptionEnvironment - 1/1: N/A");
}

/*=======================================================================================
** Ut_CFE_PSP_ExceptionGetSummary_Impl(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_ExceptionGetSummary_Impl(void)
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
    UT_SetDeferredRetcode(UT_KEY(taskName), 1, (int32) &tName);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 1/5: Nominal output string parameter value was set");
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_ExceptionGetSummary_Impl() - 1/5: Nominal");

    /* ----- Test case #2 - task name not found ----- */
    /* Setup additional inputs */
    UT_SetDeferredRetcode(UT_KEY(taskName), 1, (int32) NULL);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 2/5: TASK ID not found");
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_ExceptionGetSummary_Impl() - 2/5: TASK ID not found");

    /* ----- Test case #3 - ReasonBuf is null ----- */
    /* Setup additional inputs */
    /* Execute tests*/
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, NULL, uiReasonSize);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 3/5: ReasonBuf is NULL");

    /* ----- Test case #4 - Buffer is null ----- */
    /* Setup additional inputs */
    /* Execute tests*/
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(NULL, cReasonBuf, uiReasonSize);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 4/5: Buffer is NULL");

    /* ----- Test case #5 - Nominal ReasonBuf too short ----- */
    /* Setup additional inputs */
    UT_SetDeferredRetcode(UT_KEY(taskName), 1, (int32) &tName_too_long);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ExceptionGetSummary_Impl() - 5/6: Nominal - ReasonBuf too short - Message");
    UtAssert_True(strlen(cReasonBuf) == (uiReasonSize - 1), "_CFE_PSP_ExceptionGetSummary_Impl() - 5/6: Nominal - ReasonBuf length same as ReasonSize");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 5/6: ReasonBuf too short - Return code error");

    /* ----- Test case #6 - snprintf error ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskName), (int32) &tName);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_snprintf_error, "_CFE_PSP_ExceptionGetSummary_Impl() - 6/6: Nominal - snprintf error - Message");
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_ExceptionGetSummary_Impl() - 6/6: snprintf error - Return code error");
}

/*=======================================================================================
** End of file psp_exception_testcases.c
**=======================================================================================*/
