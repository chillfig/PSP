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
*   \file psp_support_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_support.c.
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
#include <target_config.h>

#include "uttest.h"
#include "utstubs.h"
#include "osapi.h"
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
    char cMsg[256] = "";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal POWERON reboot ----- */
    /* Setup additional inputs */
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", uiResetType);
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_POWERON,
                  "_CFE_PSP_Restart() - 1/2: Nominal POWERON reboot");
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal PROCESSOR reboot ----- */
    /* Setup additional inputs */
    uiResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    sprintf(cMsg, "WARNING: PSP Restart called with %d\n", uiResetType);
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_PROCESSOR,
                  "_CFE_PSP_Restart() - 2/2: Nominal PROCESSOR reboot");
    UtAssert_STUB_COUNT(reboot,1);
}

/*=======================================================================================
** Ut_CFE_PSP_Panic(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Panic(void)
{
    int32 iErrorCode = -1;
    char cMsg[256] = "";
    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal CFE_PSP_PANIC_MEMORY_ALLOC ----- */
    /* Setup additional inputs */
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_MEMORY_ALLOC;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal CFE_PSP_PANIC_STARTUP ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_STARTUP;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Nominal CFE_PSP_PANIC_VOLATILE_DISK ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_VOLATILE_DISK;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Nominal CFE_PSP_PANIC_NONVOL_DISK ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_NONVOL_DISK;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);
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
