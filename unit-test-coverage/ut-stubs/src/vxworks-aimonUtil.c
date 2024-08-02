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

/* PSP coverage stub replacement for aimonUtil.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_aimonUtil.h"

char PCS_AimonCompletionBlockTestIDStrings[128][PCS_TEST_NAME_STRING_SIZE];

PCS_STATUS PCS_aimonGetBITExecuted(uint64_t *pBitResult, bool talkative)
{
    UT_Stub_RegisterContext(UT_KEY(PCS_aimonGetBITExecuted), pBitResult);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_aimonGetBITExecuted);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(PCS_aimonGetBITExecuted), (uint64 *)pBitResult, sizeof(*pBitResult));
    }

    return iStatus;
}

PCS_STATUS PCS_aimonGetBITResults(uint64_t *pBitResult, bool talkative)
{
    UT_Stub_RegisterContext(UT_KEY(PCS_aimonGetBITResults), pBitResult);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_aimonGetBITResults);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(PCS_aimonGetBITResults), (uint64 *)pBitResult, sizeof(*pBitResult));
    }

    return iStatus;
}
