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

/* PSP coverage stub replacement for bflashCt.h */
#ifndef PCS_BFLASHCT_H
#define PCS_BFLASHCT_H

#include "PCS_basetypes.h"

/* ----------------------------------------- */
/* constants normally defined in bflashCt.h */
/* ----------------------------------------- */
#define PCS_FLA_MAX_CATALOG_ENTRIES            100
#define PCS_MAX_CATALOG_ENTRIES                (PCS_FLA_MAX_CATALOG_ENTRIES)

/* ----------------------------------------- */
/* types normally defined in bflashCt.h */
/* ----------------------------------------- */
typedef struct PCS_f_catalog_s
{                               /*     */
    char  id[8];                /* 0-1 */
    uint32_t * runAddr;           /* 2 */
    uint32_t * startInFlashAddr;  /* 3 */
    uint32_t * endInFlashAddr;    /* 4 */
    uint32_t * startInRamAddr;    /* 5 */
    uint32_t * copyAddr;          /* 6  - unused on SP0 */
    uint32_t crc;                 /* 7 */
} PCS_FCatalog_S;       /* Total 32-Bytes * 100 = 3200 Bytes per catalog */

typedef struct
{
    uint32_t     catalog_crc;       /* Crc of catalog */
    PCS_FCatalog_S fcatalog_s[PCS_MAX_CATALOG_ENTRIES];
} PCS_cat_struct_type;

/* ----------------------------------------- */
/* prototypes normally declared in bflashCt.h */
/* ----------------------------------------- */
extern PCS_cat_struct_type *PCS_getCatalogPointer(bool first_catalog);
extern int PCS_getCatalogEntryCount(PCS_cat_struct_type *catPtr);
extern int PCS_returnSelectedBootFlash(void);

#endif
