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
    g_uiCFE_PSP_WatchdogValue_ms = 20000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = g_uiCFE_PSP_WatchdogValue_ms;
    /* Execute test */
    CFE_PSP_WatchdogInit();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(g_uiCFE_PSP_WatchdogValue_ms, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogInit() - 1/1: Nominal");
    UtAssert_True(g_uiCFE_PSP_WatchdogValue_ms == BASE_g_uiCFE_PSP_WatchdogValue_ms, "_CFE_PSP_WatchdogInit() - 1/1: Nominal - Reduntant test");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogEnable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogEnable(void)
{
    char cMsg[256] = {};
    sprintf(cMsg, "PSP Watchdog: Could not enable FPGA Watchdog\n");

    Ut_OS_printf_Setup();

    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), OK);
    CFE_PSP_WatchdogEnable();
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_WatchdogEnable() - 1/2: Nominal");

    UT_SetDefaultReturnValue(UT_KEY(sysEnableFpgaWdt), ERROR);
    CFE_PSP_WatchdogEnable();
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogEnable() - 2/2: Could not enable Watchdog");
    
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogDisable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogDisable(void)
{
    char cMsg[256] = {};
    sprintf(cMsg, "PSP Watchdog: Could not disable FPGA Watchdog\n");

    Ut_OS_printf_Setup();

    UT_SetDefaultReturnValue(UT_KEY(sysDisableFpgaWdt), OK);
    CFE_PSP_WatchdogDisable();
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_WatchdogDisable() - 1/2: NA");

    UT_SetDefaultReturnValue(UT_KEY(sysDisableFpgaWdt), ERROR);
    CFE_PSP_WatchdogDisable();
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogDisable() - 2/2: NA");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogService(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogService(void)
{
    /* No content for implementing unit test */
    CFE_PSP_WatchdogService();
    UtAssert_NA("_CFE_PSP_WatchdogService() - 1/1: NA");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogGet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogGet(void)
{
    uint32  uiRetVal = 0;
    
    g_uiCFE_PSP_WatchdogValue_ms = 20000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = g_uiCFE_PSP_WatchdogValue_ms;
    

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetVal = CFE_PSP_WatchdogGet();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogGet() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WatchdogSet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WatchdogSet(void)
{
    char cMsg[256] = {};
    sprintf(cMsg, "PSP Watchdog: Could not set FPGA Watchdog rate\n");

    Ut_OS_printf_Setup();

    g_uiCFE_PSP_WatchdogValue_ms = 10000;
    uint32 BASE_g_uiCFE_PSP_WatchdogValue_ms = g_uiCFE_PSP_WatchdogValue_ms;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), OK);
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(g_uiCFE_PSP_WatchdogValue_ms, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogSet() - 1/2: Nominal");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_WatchdogSet() - 1/2: Nominal No Print");

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysSetFpgaWdt), ERROR);
    /* Execute test */
    CFE_PSP_WatchdogSet(BASE_g_uiCFE_PSP_WatchdogValue_ms);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(g_uiCFE_PSP_WatchdogValue_ms, BASE_g_uiCFE_PSP_WatchdogValue_ms, 0, "_CFE_PSP_WatchdogSet() - 2/2: Nominal");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WatchdogSet() - 2/2: Nominal Error Print");
}

/*=======================================================================================
** End of file psp_watchdog_testcases.c
**=======================================================================================*/