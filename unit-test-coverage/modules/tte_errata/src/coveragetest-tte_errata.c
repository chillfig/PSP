/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: 
 * Draco
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
************************************************************************/

/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/*
 *
 *    Copyright (c) 2023, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

/**
 * \file
 * \ingroup  modules
 *
 * Coverage test for TTE Erratas
 */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "cfe_psp.h"
#include "cfe_psp_module.h"
#include "PCS_drv_pci_pciConfigLib.h"

#include "tte_errata.h"

#include "coveragetest-tte_errata.h"
#include "coveragetest-psp-shared.h"

/*=======================================================================================
** PSP TTE Erratas test setup functions
**=======================================================================================*/

void ModuleTest_ResetState(void)
{
    
}

void Test_tte_errata_Init(void)
{
    char cMsg1[] = "TTE Errata: Device found, but unknown error getting DPE bit\n";
    char cMsg2[] = "TTE Errata: Initialized Successfully";
    char cMsg3[] = "TTE Errata: Device not found\n";
    char cMsg[256] = {0};

    uint32 uiPspModuleId = 0;
    uint32 uiDpeBit = 0x0280;

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Successfully Initialize and get PCI bus status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(PCS_pciFindDevice), 1, PCS_OK);
    UT_SetDeferredRetcode(UT_KEY(PCS_pciConfigInWord), 1, PCS_OK);
    UT_SetDataBuffer(UT_KEY(PCS_pciConfigInWord), &uiDpeBit, sizeof(uiDpeBit), true);
    /* Execute test */
    tte_errata_Init(uiPspModuleId);
    /* Verify result */
    snprintf(cMsg, sizeof(cMsg), "%s Bus #: %d Dev #: %d Func #: %d\n", cMsg2, g_iBusNum, g_iDevNum, g_iFuncNum);
    UtAssert_OS_print(cMsg, "_tte_errata_Init - 1/3: Nominal - Initialized and PCI Bus Status DPE bit read");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Successfully Initialize, but unable to get PCI bus status ----- */
    /* Set additional variables */
    //UT_SetDefaultReturnValue(UT_KEY(PCS_pciFindDevice), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_pciConfigInWord), PCS_ERROR);
    /* Execute test */
    tte_errata_Init(uiPspModuleId);
    /* Verify results */
    UtAssert_OS_print(cMsg1, "_tte_errata_Init - 2/3: Initialized, but PCI Bus status not read");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Unsuccessful initialization ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(PCS_pciFindDevice), PCS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_pciConfigInWord), PCS_ERROR);
    /* Execute test */
    tte_errata_Init(uiPspModuleId);
    /* Verify results */
    UtAssert_OS_print(cMsg3, "_tte_errata_Init - 3/3: Initialization error");
}

void Test_CFE_PSP_GetPCIDpeBit(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    uint32 uiDpeBit = 0;
    uint16 uiPciBusStatusBit0 = 0x0280;
    uint16 uiPciBusStatusBit1 = 0x8280;

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Successfully get PCI bus status - bit 0 ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(PCS_pciConfigInWord), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_pciConfigInWord), &uiPciBusStatusBit0, sizeof(uiPciBusStatusBit0), true);
    /* Execute test */
    iReturnCode = CFE_PSP_GetPCIDpeBit(&uiDpeBit);
    /* Verify result */
    UtAssert_True(uiDpeBit == 0, "_CFE_PSP_GetPCIDpeBit - 1/3: Nominal - PCI Bus Status DPE bit success - bit 0");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetPCIDpeBit - 1/3: Nominal - Returned Success");

    /* ----- Test case #2 - Successfully get PCI bus status - bit 1 ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(PCS_pciConfigInWord), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(PCS_pciConfigInWord), &uiPciBusStatusBit1, sizeof(uiPciBusStatusBit1), true);
    /* Execute test */
    iReturnCode = CFE_PSP_GetPCIDpeBit(&uiDpeBit);
    /* Verify result */
    UtAssert_True(uiDpeBit == 1, "_CFE_PSP_GetPCIDpeBit - 2/3: Nominal - PCI Bus Status DPE bit success - bit 1");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetPCIDpeBit - 2/3: Nominal - Returned Success");

    /* ----- Test case #3 - PSP Error, Unable to get PCI bus status ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(PCS_pciConfigInWord), CFE_PSP_ERROR);
    uiDpeBit = 0xFFFF;
    /* Execute test */
    iReturnCode = CFE_PSP_GetPCIDpeBit(&uiDpeBit);
    /* Verify results */
    UtAssert_True(uiDpeBit == 0xFFFF, "_CFE_PSP_GetPCIDpeBit - 3/3: Error - DPE bit untouched");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_GetPCIDpeBit - 3/3: Error returned");

}

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add(test, ModuleTest_ResetState, NULL, #test)

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(Test_tte_errata_Init);
    ADD_TEST(Test_CFE_PSP_GetPCIDpeBit);
}
