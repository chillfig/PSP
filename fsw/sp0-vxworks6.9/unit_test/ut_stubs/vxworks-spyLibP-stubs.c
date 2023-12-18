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
*   \file vxworks-spyLibP-stubs.c
*
*   \brief PSP coverage stub replacement for spyLibP.h
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was cloned-n-owned at the NASA Johnson Space Center from the NASA cFE open source 
*   with license below.
**********************************************************************************************************/

/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
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

#include <string.h>
#include <stdlib.h>
#include "utstubs.h"
#include "utgenstub.h"

#include "PCS_spyLibP.h"

void PCS_spyReportCommon(PCS_FUNCPTR print)
{
    /* Not Implement*/
}

void PCS_spyClkStopCommon(void)
{
    /* Not Implement */
}

int  PCS_spyClkStartCommon(int intsPerSec, PCS_FUNCPTR print)
{

    UT_GenStub_SetupReturnBuffer(PCS_spyClkStartCommon, int32);

    UT_GenStub_AddParam(PCS_spyClkStartCommon, int, intsPerSec);
    UT_GenStub_AddParam(PCS_spyClkStartCommon, PCS_FUNCPTR, print);

    UT_GenStub_Execute(PCS_spyClkStartCommon, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_spyClkStartCommon, int32);;
}

