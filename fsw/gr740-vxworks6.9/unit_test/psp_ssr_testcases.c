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
*   \file psp_srr_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_srr.c.
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
