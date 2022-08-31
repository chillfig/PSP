/*=======================================================================================
**
** File:  psp_timer_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_timer.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
**=======================================================================================*/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "psp_timer_testcases.h"
#include "../src/cfe_psp_timer.c"

/*=======================================================================================
** Preprocessor Directives
**=======================================================================================*/


/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_InitLocalTime(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_InitLocalTime(void)
{
    UT_SetDefaultReturnValue(UT_KEY(sysAuxClkDisable), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysAuxClkRateGet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysClkRateSet), OS_SUCCESS);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    CFE_PSP_InitLocalTime();
    /* Verify outputs */
    UtAssert_NA("Ut_CFE_PSP_InitLocalTime() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_vxTimeBaseGet(void) test cases
**=======================================================================================*/
void Ut_vxTimeBaseGet(void)
{
    uint32 uiU = 0;
    uint32 uiL = 0;

    char cMsg_input_null[] = TIMER_PRINT_SCOPE "Input variables are NULL\n";
    char cMsg_timer_not_init[] = TIMER_PRINT_SCOPE "Timer is not initialized\n";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = true;
    /* Execute test */
    vxTimeBaseGet(&uiU, &uiL);
    /* Verify outputs */
    UtAssert_True(((uiU > 0) && (uiL > 0)), "Ut_vxTimeBaseGet() - 1/3: Nominal");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Timer not initialized ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = false;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(&uiU, &uiL);
    /* Verify outputs */
    UtAssert_True(((uiU == 0) && (uiL == 0)), "Ut_vxTimeBaseGet() - 2/3: Timer not initialized - unchanged inputs");
    UtAssert_OS_print(cMsg_timer_not_init, "Ut_vxTimeBaseGet - 2/32: Timer not initialized - message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - NULL Inputs ----- */
    /* Setup additional inputs */
    g_bTimerInitialized = true;
    uiU = 0;
    uiL = 0;
    /* Execute test */
    vxTimeBaseGet(NULL, NULL);
    /* Verify outputs */
    UtAssert_OS_print(cMsg_input_null, "Ut_vxTimeBaseGet - 3/3: NULL Inputs - message");
}
