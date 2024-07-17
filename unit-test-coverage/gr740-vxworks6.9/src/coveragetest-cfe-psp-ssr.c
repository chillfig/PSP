/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
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

/*
 *
 *    Copyright (c) 2020, United States government as represented by the
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
 * \ingroup  vxworks
 * \author   
 *
 */

#include "coveragetest-psp-gr740-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include "cfe_psp.h"

#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Test_CFE_PSP_InitSSR(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_InitSSR(void)
{
    int32  iRetCode = CFE_PSP_SUCCESS;
    uint32 uiBus = 0;
    uint32 uiDevice = 0;
    char   cDeviceName[] = "ram0";

    /* ----- Test case #1 - Nominal no ATA DRIVER ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_InitSSR(uiBus, uiDevice, (char*)cDeviceName);
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