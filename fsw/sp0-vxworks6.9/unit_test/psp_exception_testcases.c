/*=======================================================================================
**
** File:  psp_exception_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_exception.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2021-01-05 | Minh Luong       | Initial revision
**=======================================================================================*/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
/* #include <target_config.h> */
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
/* #include "cfe_psp.h" */

#include "psp_exception_testcases.h"
#include "../src/cfe_psp_exception.c"

extern int currentedrPolicyHandlerHook1(int type, void *pInfo_param, BOOL debug);
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
    int32   iRetCode = 0;
    int32   iType = 0;
    EDR_TASK_INFO   InfoParam = {};
    bool    bDebug = false;

    /*
    GLOBAL_CFE_CONFIGDATA.SystemNotify is always pointing to SystemNotify 
    function defined in psp_shared_stubs.c
    */

    /* ----- Test case #1 - Nominal ----- */
    UT_ResetState(0);
    /* Setup additional inputs */
    g_ucOverRideDefaultedrPolicyHandlerHook = FALSE;
    UT_SetDeferredRetcode(UT_KEY(currentedrPolicyHandlerHook1), 1, true);
    g_pDefaultedrPolicyHandlerHook = &currentedrPolicyHandlerHook1;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == true, "_CFE_PSP_edrPolicyHandlerHook() - 1/5: Nominal");

    /* ----- Test case #2 - Nominal with Override Policy Enabled ----- */
    UT_ResetState(0);
    /* Setup additional inputs */
    iRetCode = true;
    g_ucOverRideDefaultedrPolicyHandlerHook = TRUE;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == false, "_CFE_PSP_edrPolicyHandlerHook() - 2/5: Nominal with Override Policy Enabled");

    /* ----- Test case #4 - Failed CFE_PSP_Exception_GetNextContextBuffer returned NULL ----- */
    UT_ResetState(0);
    /* Setup additional inputs */
    iRetCode = true;
    g_ucOverRideDefaultedrPolicyHandlerHook = FALSE;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 1);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == false, "_CFE_PSP_edrPolicyHandlerHook() - 3/5: Failed No context buffer received");

    /* ----- Test case #4 - Nominal - SystemNotify NULL ----- */
    UT_ResetState(0);
    /* Setup additional inputs */
    g_ucOverRideDefaultedrPolicyHandlerHook = FALSE;
    UT_SetDeferredRetcode(UT_KEY(currentedrPolicyHandlerHook1), 1, true);
    g_pDefaultedrPolicyHandlerHook = &currentedrPolicyHandlerHook1;
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == true, "_CFE_PSP_edrPolicyHandlerHook() - 4/5: Nominal");

    /* ----- Test case #5 - Nominal - g_pDefaultedrPolicyHandlerHook False ----- */
    /* Decision "g_pDefaultedrPolicyHandlerHook != ( ( void * ) 0 )" never evaluated to false is an Impossible Condition */
    /* Add to increase code coverage? If it is impossible, then should we remove? 
     * If not remove, then let's test for code coverage sake 
     */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    char cMsg[256] = {};
    /* Setup additional inputs */
    iRetCode = true;
    g_ucOverRideDefaultedrPolicyHandlerHook = FALSE;
    g_pDefaultedrPolicyHandlerHook = NULL;
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "Override enabled, but Handler Hook is not setup\n");
    /* 0 will return a valid address, 1 will return NULL */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, 0);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == false, "_CFE_PSP_edrPolicyHandlerHook() - 5/5: Failed No context buffer received");
    UtAssert_OS_print(cMsg, "_CFE_PSP_edrPolicyHandlerHook() - 5/5: Override policy enabled, no policy handler hook set");
    
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
    sprintf(cMsg, PSP_EXCEP_PRINT_SCOPE "edrErrorPolicyHookRemove() failed for address 0x%x\n", &currentedrPolicyHandlerHook1);
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
    /* Nothing to test because the empty function */
    CFE_PSP_SetDefaultExceptionEnvironment();
    UtAssert_NA("_CFE_PSP_SetDefaultExceptionEnvironment - 1/1: Nominal - Function is empty");
}

/*=======================================================================================
** Ut_CFE_PSP_ExceptionGetSummary_Impl(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_ExceptionGetSummary_Impl(void)
{
    int32   iRetCode = 0;
    CFE_PSP_Exception_LogData_t buffer;
    char    cReasonBuf[100] = {'\0'};
    char    tName[6] = "NULL";
    uint32  uiReasonSize = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(taskName), 1, (int32) &tName);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 1/3: Nominal output string parameter value was set");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ExceptionGetSummary_Impl() - 1/3: Nominal");

    /* ----- Test case #2 - task name not found ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(taskName), 1, (int32) NULL);
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 2/3: TASK ID not found");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ExceptionGetSummary_Impl() - 2/3: TASK ID not found");

    /* ----- Test case #3 - Buffer and/or ReasonBuf are null ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    uiReasonSize = 100;
    /* Execute tests*/
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, NULL, uiReasonSize);
    /* Verify results */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ExceptionGetSummary_Impl() - 3/3: Buffer or ReasonBuf are NULL");
}

/*=======================================================================================
** End of file psp_exception_testcases.c
**=======================================================================================*/
