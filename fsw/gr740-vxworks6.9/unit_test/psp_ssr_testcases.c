/*=======================================================================================
**
** File:  psp_ssr_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_ssr.c.
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
#include "osapi.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_ssr_testcases.h"
#include "../src/cfe_psp_ssr.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_InitSSR(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_InitSSR(void)
{
    int32  iRetCode = CFE_PSP_SUCCESS;
    uint32 uiBus = 0;
    uint32 uiDevice = 0;
    char   cDeviceName[] = "ram0";

    /* ----- Test case #1 - Nominal no ATA DRIVER ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_InitSSR(uiBus, uiDevice, &cDeviceName);
    /* Verify outputs */
#ifdef USE_VXWORKS_ATA_DRIVER
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitSSR - 1/1: Nominal ATA DRIVER return success");
#else
    UtAssert_True(iRetCode == CFE_PSP_ERROR_NOT_IMPLEMENTED, "_CFE_PSP_InitSSR - 1/1: Nominal NO ATA DRIVER return not implemented");
#endif
}


/*=======================================================================================
** End of file psp_ssr_testcases.c
**=======================================================================================*/
