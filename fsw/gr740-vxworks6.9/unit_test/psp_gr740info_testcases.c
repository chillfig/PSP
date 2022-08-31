/*=======================================================================================
**
** File:  psp_gr740_info_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_gr740info.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
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

#include "psp_gr740info_testcases.h"
#include "../src/cfe_psp_gr740info.c"

/**********************************************************
 * PREPROCESSOR DIRECTIVES
 *********************************************************/
#define UT_GR740_PRINT_SCOPE "_PSP_GR740_"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/


/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_TempSensorInit(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TempSensorInit(void)
{
    int32   iRetCode = CFE_PSP_ERROR;
    
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_TempSensorInit();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_TempSensorInit() - 1/1: Nominal");

}

/*=======================================================================================
** Ut_CFE_PSP_GetTemperatureAwait(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetTemperatureAwait(void)
{
    int32   iRetCode = CFE_PSP_SUCCESS;
    uint32  uiTemp = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_GetTemperatureAwait(&uiTemp);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetTemperatureAwait() - 1/2: Nominal - Successful return code");
    UtAssert_True(&uiTemp > 0, "_CFE_PSP_GetTemperatureAwait() - 1/2: Nominal - Temperature %u", uiTemp);

    /* ----- Test case #2 - Could not get temperature - Error ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_GetTemperatureAwait(&uiTemp);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetTemperatureAwait() - 2/2: Could not get temperature - error return code");

}

/*=======================================================================================
** End of file psp_gr740info_testcases.c
**=======================================================================================*/
