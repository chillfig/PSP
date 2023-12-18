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
*   \file psp_gr740_info_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_gr740info.c.
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
}

/*=======================================================================================
** End of file psp_gr740info_testcases.c
**=======================================================================================*/
