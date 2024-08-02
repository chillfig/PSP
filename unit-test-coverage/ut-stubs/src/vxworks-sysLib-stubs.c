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

/* PSP coverage stub replacement for sysLib.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_sysLib.h"

uint32_t PCS_l2errTotal;
uint32_t PCS_l2errMult;
uint32_t PCS_l2errTagPar;
uint32_t PCS_l2errMBECC;
uint32_t PCS_l2errSBECC;
uint32_t PCS_l2errCfg;
uint32_t PCS_mchCause;
uint32_t PCS_mchkHook;

int PCS_sysClkRateGet(void)
{
    return UT_DEFAULT_IMPL_RC(PCS_sysClkRateGet, 10000);
}

char *PCS_sysMemTop(void)
{
    int32 Status;
    char *BufPtr;

    BufPtr = NULL;
    Status = UT_DEFAULT_IMPL(PCS_sysMemTop);
    if (Status == 0)
    {
        UT_GetDataBuffer(UT_KEY(PCS_sysMemTop), (void **)&BufPtr, NULL, NULL);
    }

    return BufPtr;
}

char *PCS_sysPhysMemTop(void)
{
    int32 Status;
    cpuaddr BufPtr;

    BufPtr = (cpuaddr)NULL;
    Status = UT_DEFAULT_IMPL(PCS_sysPhysMemTop);
    if (Status == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(PCS_sysPhysMemTop), &BufPtr, sizeof(BufPtr));
    }

    return (char *)BufPtr;
}

int PCS_sysAuxClkRateGet(void)
{
    return UT_DEFAULT_IMPL(PCS_sysAuxClkRateGet);
}

void PCS_PciOutByte(uint32_t address, uint8_t data)
{
    UT_DEFAULT_IMPL(PCS_PciOutByte);
}

void PCS_PciOutLong(uint32_t address, uint32_t data)
{
    UT_DEFAULT_IMPL(PCS_PciOutLong);
}

void PCS_sysPciWrite32(uint32_t address, uint32_t data)
{
    UT_DEFAULT_IMPL(PCS_sysPciWrite32);
}

void PCS_sysPciRead32(uint32_t address, uint32_t *data)
{
    UT_DEFAULT_IMPL(PCS_sysPciRead32);
}

unsigned int PCS_GetWrsKernelTextStart(void)
{
    return UT_DEFAULT_IMPL(PCS_GetWrsKernelTextStart);
}

unsigned int PCS_GetWrsKernelTextEnd(void)
{
    return UT_DEFAULT_IMPL(PCS_GetWrsKernelTextEnd);
}

char *PCS_sysModel(void)
{
    int iStatus;
    static char cLongMessage[] = "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE";
    static char cShortMessage[] = "Typical Message";

    iStatus = UT_DEFAULT_IMPL(PCS_sysModel);
    if (iStatus == 1)
    {
        return cLongMessage;
    }
    return cShortMessage;
}

char *PCS_sysBspRev(void)
{
    int iStatus;
    static char cLongMessage[] = "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE";
    static char cShortMessage[] = "Typical Message";

    iStatus = UT_DEFAULT_IMPL(PCS_sysBspRev);
    if (iStatus == 1)
    {
        return cLongMessage;
    }
    return cShortMessage;
}

int PCS_sysProcNumGet(void)
{
    return UT_DEFAULT_IMPL(PCS_sysProcNumGet);
}

void PCS_ckCtrs(void)
{
    UT_DEFAULT_IMPL(PCS_ckCtrs);
}