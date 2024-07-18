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
*   \file psp_tte_errata_testcases.c
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
#include "../../modules/tte_errata/src/tte_errata.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_tte_errata_Init(void) test cases
**=======================================================================================*/
void Ut_tte_errata_Init(void)
{
    char cMsg1[] = "TTE Errata: Device found, but unknown error getting DPE bit\n";
    char cMsg2[] = "TTE Errata: Initialized Successfully\n";
    char cMsg3[] = "TTE Errata: Device not found\n";

    uint32 uiPspModuleId = 0;
    uint32 uiDpeBit = 0x0280;

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Successfully Initialize and get PCI bus status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(pciFindDevice), 1, OK);
    UT_SetDeferredRetcode(UT_KEY(pciConfigInWord), 1, OK);
    UT_SetDataBuffer(UT_KEY(pciConfigInWord), &uiDpeBit, sizeof(uiDpeBit), true);
    /* Execute test */
    tte_errata_Init(uiPspModuleId);
    /* Verify result */
    UtAssert_OS_print(cMsg2, "_tte_errata_Init - 1/3: Nominal - Initialized and PCI Bus Status DPE bit read");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Successfully Initialize, but unable to get PCI bus status ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(pciFindDevice), OK);
    UT_SetDefaultReturnValue(UT_KEY(pciConfigInWord), ERROR);
    /* Execute test */
    tte_errata_Init(uiPspModuleId);
    /* Verify results */
    UtAssert_OS_print(cMsg1, "_tte_errata_Init - 2/3: Initialized, but PCI Bus status not read");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Unsuccessful initialization ----- */
    /* Set additional variables */
    UT_SetDefaultReturnValue(UT_KEY(pciFindDevice), ERROR);
    UT_SetDefaultReturnValue(UT_KEY(pciConfigInWord), ERROR);
    /* Execute test */
    tte_errata_Init(uiPspModuleId);
    /* Verify results */
    UtAssert_OS_print(cMsg3, "_tte_errata_Init - 3/3: Initialization error");
}

/*=======================================================================================
** End of file psp_tte_errata_testcases.c
**=======================================================================================*/
