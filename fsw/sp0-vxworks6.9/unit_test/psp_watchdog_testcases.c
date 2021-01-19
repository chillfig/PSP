/*=======================================================================================
**
** File:  psp_watchdog_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_watchdog.c.
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
#include <target_config.h>
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_watchdog_testcases.h"
#include "../src/cfe_psp_watchdog.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_WatchdogInit(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogInit(void)
{
    /* ----- Test case #1 - Failed input pointer is NULL ----- */
    /* Setup additional inputs */
    CFE_PSP_WatchdogValue = 20000;
    /* Execute test */
    CFE_PSP_WatchdogInit();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(CFE_PSP_WatchdogValue, 20, 0, "_CFE_PSP_WatchdogInit() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogEnable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogEnable(void)
{
    /* No content for implementing unit test */
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogDisable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogDisable(void)
{
    /* No content for implementing unit test */
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogService(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogService(void)
{
    /* No content for implementing unit test */
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogGet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogGet(void)
{
    uint32  uiRetVal = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetVal = CFE_PSP_WatchdogGet();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, 20, 0, "_CFE_PSP_WatchdogGet() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogSet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogSet(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    CFE_PSP_WatchdogSet(10000);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(CFE_PSP_WatchdogValue, 10, 0, "_CFE_PSP_WatchdogSet() - 1/1: Nominal");
}

/*=======================================================================================
** End of file psp_watchdog_testcases.c
**=======================================================================================*/