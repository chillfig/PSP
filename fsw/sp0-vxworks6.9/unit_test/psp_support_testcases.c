/*=======================================================================================
**
** File:  psp_support_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_support.c.
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

#include "psp_support_testcases.h"
#include "../src/cfe_psp_support.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_Restart(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Restart(void)
{
    uint32  uiResetType = CFE_PSP_RST_TYPE_POWERON;
    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord[200];
    CFE_PSP_ReservedMemoryMap.BootPtr = (CFE_PSP_ReservedMemoryBootRecord_t *) localBootRecord;

    /* ----- Test case #1 - Nominal Power on reboot ----- */
    /* Setup additional inputs */
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_POWERON, "_CFE_PSP_Restart() - 1/2: Nominal PowerOn reboot");

    /* ----- Test case #2 - Nominal Processor reboot ----- */
    /* Setup additional inputs */
    uiResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_PROCESSOR, "_CFE_PSP_Restart() - 2/2: Nominal Processor reboot");
}

/*=======================================================================================
** Ut_CFE_PSP_Panic(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Panic(void)
{
    int32 iErrorCode = -1;
    char cMsg[256] = {};
    sprintf(cMsg, "CFE_PSP_Panic Called with error code = 0x%08X. Exiting.\n", iErrorCode);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_logMsg_Setup();
    UT_SetDefaultReturnValue(UT_KEY(remove), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(write), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(close), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_SUCCESS);
    CFE_PSP_MEM_SCRUB_Init();
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    // Ut_OS_printfPrint();
    UtAssert_logMsg(cMsg, "_CFE_PSP_Panic - 1/1: Nominal Panic log string found");
}

/*=======================================================================================
** Ut_CFE_PSP_FlushCaches(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FlushCaches(void)
{
    uint32 ftype = 0;
    void *faddress = NULL;
    uint32 fsize = 1;
    /* Nothing to test because the empty function */
    CFE_PSP_FlushCaches(ftype, faddress, fsize);
    UtAssert_NA("_CFE_PSP_FlushCaches() - 1/1: N/A");
}

/*=======================================================================================
** Ut_CFE_PSP_GetProcessorId(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetProcessorId(void)
{
    uint32   uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetCode = CFE_PSP_GetProcessorId();
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_CPU_ID, "_CFE_PSP_GetProcessorId() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetSpacecraftId(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetSpacecraftId(void)
{
    uint32   uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetCode = CFE_PSP_GetSpacecraftId();
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SPACECRAFT_ID, "_CFE_PSP_GetSpacecraftId() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetProcessorName(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetProcessorName(void)
{
    const char *pRetString;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    pRetString = CFE_PSP_GetProcessorName();
    /* Verify outputs */
    UtAssert_StrCmp(pRetString, CFE_PSP_CPU_NAME, "_CFE_PSP_GetProcessorName() - 1/1: Nominal");
}

/*=======================================================================================
** End of file psp_support_testcases.c
**=======================================================================================*/
