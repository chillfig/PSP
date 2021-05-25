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

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    currentedrPolicyHandlerHook = &currentedrPolicyHandlerHook1;
    UT_SetDeferredRetcode(UT_KEY(currentedrPolicyHandlerHook1), 1, 1);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == true, "_CFE_PSP_edrPolicyHandlerHook() - 1/2: Nominal");

    /* ----- Test case #2 - Failed CFE_PSP_Exception_GetNextContextBuffer returned NULL ----- */
    /* Setup additional inputs */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_GetNextContextBuffer), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_edrPolicyHandlerHook(iType, &InfoParam, bDebug);
    /* Verify outputs */
    UtAssert_True(iRetCode == false, "_CFE_PSP_edrPolicyHandlerHook() - 2/2: Failed No context buffer received");
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
    sprintf(cMsg, "CFE_PSP_AttachException() - edrErrorPolicyHookRemove() failed for address 0x%x ", &currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_AttachExceptions - 1/3: Nominal don't call the removing the policy hook");
    sprintf(cMsg, "CFE_PSP_AttacheException() - Attached cFE Exception Handler. ");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 1/3: Nominal attached exception handler");

    /* ----- Test case #2 - Nominal but failed to remove the policy hook ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(edrErrorPolicyHookRemove), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookAdd), 1, OS_SUCCESS);
    sprintf(cMsg, "CFE_PSP_AttachException() - edrErrorPolicyHookRemove() failed for address 0x%x ", &currentedrPolicyHandlerHook1);
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 2/3: Nominal failed remove the policy hook");
    sprintf(cMsg, "CFE_PSP_AttacheException() - Attached cFE Exception Handler. ");
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 2/3: Nominal attached exception handler");

    /* ----- Test case #3 - Failed attached exception handler ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookGet), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(edrPolicyHandlerHookAdd), 1, OS_ERROR);
    sprintf(cMsg, "CFE_PSP_AttachException() - edrPolicyHandlerHookAdd() failed for CFE_PSP_edrPolicyHandlerHook()");
    /* Execute test */
    CFE_PSP_AttachExceptions();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_AttachExceptions - 3/3: Failed attached exception handler");
}

/*=======================================================================================
** Ut_CFE_PSP_SetDefaultExceptionEnvironment(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /* Nothing to test because the empty function */
}

/*=======================================================================================
** Ut_CFE_PSP_ExceptionGetSummary_Impl(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_ExceptionGetSummary_Impl(void)
{
    int32   iRetCode = 0;
    CFE_PSP_Exception_LogData_t buffer;
    char cReasonBuf[100] = {'\0'};
    uint32 uiReasonSize = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    iRetCode = CFE_PSP_ExceptionGetSummary_Impl(&buffer, cReasonBuf, uiReasonSize);
    /* Verify outputs */
    UtAssert_True(strlen(cReasonBuf) != 0, "_CFE_PSP_ExceptionGetSummary_Impl() - 1/1: Nominal output string parameter value was set");
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ExceptionGetSummary_Impl() - 1/1: Nominal");
}

/*=======================================================================================
** End of file psp_exception_testcases.c
**=======================================================================================*/
