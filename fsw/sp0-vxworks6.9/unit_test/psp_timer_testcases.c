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

#include "psp_timer_testcases.h"
#include "../src/cfe_psp_timer.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_GetTime(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetTime(void)
{
    OS_time_t localTime = {};
    OS_time_t *pLocalTime = NULL;
    uint32  uiTickPerSecond = 10000;
    uint32  uiSeconds = 0;
    uint64  ulTb = 0;
    char cMsg[100] = {};

    /* ----- Test case #1 - Nominal Power on reboot ----- */
    /* Setup additional inputs */
    pLocalTime = &localTime;
    ulTb = ((uint64)1 << 32);
    uiSeconds =  (ulTb / uiTickPerSecond);
    /* Set the return value for sysTimestampFreq() return to uiTickPerSecond */
    UT_SetDeferredRetcode(UT_KEY(sysTimestampFreq), 1, uiTickPerSecond);
    /* Execute test */
    CFE_PSP_GetTime(pLocalTime);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(localTime.seconds, uiSeconds, 0, "_CFE_PSP_Restart() - 1/3: Nominal");
    
    /* ----- Test case #2 - Failed input pointer is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pLocalTime = NULL;
    /* Execute test */
    CFE_PSP_GetTime(pLocalTime);
    /* Verify outputs */
    UtAssert_True(UT_GetStubCount(UT_KEY(vxTimeBaseGet)) == 0, "_CFE_PSP_Restart() - 2/3: Failed vxTimeBaseGet was not get called");

    /* ----- Test case #3 - Failed tickPerSecond is zero ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Reset local time */
    localTime.seconds = 0;
    localTime.microsecs = 0;
    pLocalTime = &localTime;
    /* Set tickPerSecond in cfe_psp_timer.c to 0 */
    tickPerSecond = 0;
    uiTickPerSecond = 0;
    /* Set the return value for sysTimestampFreq() return to uiTickPerSecond */
    UT_SetDeferredRetcode(UT_KEY(sysTimestampFreq), 1, uiTickPerSecond);
    sprintf(cMsg, "CFE_PSP: CFE_PSP_GetTime() - tickPerSecond equals zero.\n");
    /* Execute test */
    CFE_PSP_GetTime(pLocalTime);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_ProcessResetType() - 3/3: Failed tickPerSecond equals zero");
}

/*=======================================================================================
** Ut_CFE_PSP_Get_Timer_Tick(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Get_Timer_Tick(void)
{
    uint32  uiRetVal = 0;
    uint32  uiTickPerSecond = 10000;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Set the return value for sysClkRateGet() return to uiTickPerSecond */
    UT_SetDeferredRetcode(UT_KEY(sysClkRateGet), 1, uiTickPerSecond);
    /* Execute test */
    uiRetVal = CFE_PSP_Get_Timer_Tick();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, uiTickPerSecond, 0, "_CFE_PSP_Get_Timer_Tick() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetTimerTicksPerSecond(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetTimerTicksPerSecond(void)
{
    uint32  uiRetVal = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Set tickPerSecond in cfe_psp_timer.c to 1000 */
    tickPerSecond = 1000;
    /* Execute test */
    uiRetVal = CFE_PSP_GetTimerTicksPerSecond();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, tickPerSecond, 0, "_CFE_PSP_GetTimerTicksPerSecond() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetTimerLow32Rollover(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetTimerLow32Rollover(void)
{
    uint32  uiRetVal = 1;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetVal = CFE_PSP_GetTimerLow32Rollover();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, 0, 0, "_CFE_PSP_GetTimerLow32Rollover() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_Get_Timebase(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Get_Timebase(void)
{
    uint32 uiTbu = 0;
    uint32 uiTbl = 1;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    CFE_PSP_Get_Timebase(&uiTbu, &uiTbl);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiTbu, 1, 0, "_CFE_PSP_Get_Timebase() - 1/2: Nominal");
    UtAssert_IntegerCmpAbs(uiTbl, 0, 0, "_CFE_PSP_Get_Timebase() - 1/2: Nominal");

    /* ----- Test case #2 - Failed input parameter is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    /* Execute test */
    CFE_PSP_Get_Timebase(NULL, &uiTbl);
    /* Verify outputs */
    UtAssert_True(UT_GetStubCount(UT_KEY(vxTimeBaseGet)) == 0, "_CFE_PSP_Get_Timebase() - 2/2: Failed vxTimeBaseGet was not get called");
}

/*=======================================================================================
** Ut_CFE_PSP_Get_Dec(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Get_Dec(void)
{
    uint32  uiRetVal = 0;
    uint32  uiDec = 10000;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Set the return value for vxDecGet() return to uiDec */
    UT_SetDeferredRetcode(UT_KEY(vxDecGet), 1, uiDec);
    /* Execute test */
    uiRetVal = CFE_PSP_Get_Dec();
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiRetVal, uiDec, 0, "_CFE_PSP_Get_Dec() - 1/1: Nominal");
}

/*=======================================================================================
** End of file psp_timer_testcases.c
**=======================================================================================*/
