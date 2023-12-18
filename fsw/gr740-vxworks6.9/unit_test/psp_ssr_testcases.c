/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
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
