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
 * \ingroup  adaptors
 * \author   joseph.p.hickey@nasa.gov
 *
 */

#include "ut-adaptor-exceptions.h"
#include "cfe_psp_config.h"
#include "cfe_psp_memory.h"

#define CFE_PSP_MAX_EXCEPTION_ENTRIES        4
#define CFE_PSP_MAX_EXCEPTION_BACKTRACE_SIZE 16

uint32 UT_Get_Exception_MaxEntries(void)
{
    return CFE_PSP_MAX_EXCEPTION_ENTRIES;
}

size_t UT_Get_Exception_Size(void)
{
    return sizeof(CFE_PSP_Exception_ContextDataEntry_t);
}

void UT_Generate_Exception_Context(struct CFE_PSP_Exception_LogData *Buffer, size_t Size)
{
    size_t i;
    uint8 *Dest = (uint8 *)&Buffer->context_info;
    for (i = 0; i < Size && i < sizeof(Buffer->context_info); ++i)
    {
        *Dest = i & 0xFF;
    }
    Buffer->context_size = i;
}

uint32 UT_Get_Exception_Id(struct CFE_PSP_Exception_LogData *Buffer)
{
    return Buffer->context_id;
}
