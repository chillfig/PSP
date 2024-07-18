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
*   \file psp_tte_errata_pci7_testcases.c
*
*   \brief This file contains test cases for the tte_errata PSP module.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_tte_errata_testcases.h"
#include "../../modules/tte_errata/src/tte_errata_pci7.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_GetPCIDpeBit(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetPCIDpeBit(void)
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
    UT_SetDefaultReturnValue(UT_KEY(pciConfigInWord), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(pciConfigInWord), &uiPciBusStatusBit0, sizeof(uiPciBusStatusBit0), true);
    /* Execute test */
    iReturnCode = CFE_PSP_GetPCIDpeBit(&uiDpeBit);
    /* Verify result */
    UtAssert_True(uiDpeBit == 0, "_CFE_PSP_GetPCIDpeBit - 1/3: Nominal - PCI Bus Status DPE bit success - bit 0");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetPCIDpeBit - 1/3: Nominal - Returned Success");

    /* ----- Test case #2 - Successfully get PCI bus status - bit 1 ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(pciConfigInWord), CFE_PSP_SUCCESS);
    UT_SetDataBuffer(UT_KEY(pciConfigInWord), &uiPciBusStatusBit1, sizeof(uiPciBusStatusBit1), true);
    /* Execute test */
    iReturnCode = CFE_PSP_GetPCIDpeBit(&uiDpeBit);
    /* Verify result */
    UtAssert_True(uiDpeBit == 1, "_CFE_PSP_GetPCIDpeBit - 2/3: Nominal - PCI Bus Status DPE bit success - bit 1");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetPCIDpeBit - 2/3: Nominal - Returned Success");

    /* ----- Test case #3 - PSP Error, Unable to get PCI bus status ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(pciConfigInWord), CFE_PSP_ERROR);
    uiDpeBit = 0xFFFF;
    /* Execute test */
    iReturnCode = CFE_PSP_GetPCIDpeBit(&uiDpeBit);
    /* Verify results */
    UtAssert_True(uiDpeBit == 0xFFFF, "_CFE_PSP_GetPCIDpeBit - 3/3: Error - DPE bit untouched");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_GetPCIDpeBit - 3/3: Error returned");

}

/*=======================================================================================
** End of file psp_tte_errata_pci7_testcases.c
**=======================================================================================*/
