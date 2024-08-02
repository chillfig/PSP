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
#ifndef PCS_AIMONUTIL_H
#define PCS_AIMONUTIL_H

#include "common_types.h"

#include "PCS_basetypes.h"
#include "PCS_vxWorks.h"

/* ----------------------------------------- */
/* constants normally defined in aimonUtil.h */
/* ----------------------------------------- */
typedef enum {
    MTE_L      =   1,
    MTE_W      =   2,
    MTE_B      =   3,
    MTF_L      =   4,
    MTF_W      =   5,
    MTF_B      =   6,
    MTG_L      =   7,
    MTG_W      =   8,
    MTG_B      =   9,
    MTH_L      =  10,
    MTH_W      =  11,
    MTH_B      =  12,
    CLRTST_L   =  13,
    CLRTST_W   =  14,
    CLRTST_B   =  15,
    ND_MTF_L   =  16,
    ND_MTF_W   =  17,
    ND_MTF_B   =  18,
    ECC_RD     =  19,
    BFLASH     =  20,
    CRC_EEP    =  21,
    CRC_USF    =  22,
    RET_FLASH  =  23,
    CPU_TST    =  24,
    L1_CACHE   =  25,
    L2_CACHE   =  26,
    PCI_BRI    =  27,
    CPCI_BRI   =  28,
    WCH_DOG    =  29,
    INT_TST    =  30,
    TIMER      =  31,
    SIO_ETL    =  32,
    MEMIF_TST  =  33,
    CO_TIME    =  34,
    ECC_INJ    =  35,
    SIO_ITL    =  36,
    WCH_TMR    =  37,
    FWCH_DOG   =  38,
    FWCH_TMR   =  39,
    LAST_AIMON_TEST = 64
} PCS_AIMON_TEST_ID_ENUM;

#define PCS_TEST_NAME_STRING_SIZE 48

/* ----------------------------------------- */
/* types normally defined in aimonUtil.h */
/* ----------------------------------------- */
extern char PCS_AimonCompletionBlockTestIDStrings[][PCS_TEST_NAME_STRING_SIZE];

/* ----------------------------------------- */
/* prototypes normally declared in aimonUtil.h */
/* ----------------------------------------- */
PCS_STATUS PCS_aimonGetBITExecuted(uint64_t *pBitResult, bool talkative);
PCS_STATUS PCS_aimonGetBITResults(uint64_t *pBitResult, bool talkative);

#endif
