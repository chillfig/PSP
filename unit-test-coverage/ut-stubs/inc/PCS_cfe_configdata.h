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

#ifndef PCS_CFE_CONFIGDATA_H
#define PCS_CFE_CONFIGDATA_H

#include "PCS_basetypes.h"

#define PCS_CONFIG_MISSIONVERSION "UT-mission"
#define PCS_CONFIG_CFEVERSION     "UT-CFE"
#define PCS_CONFIG_OSALVERSION    "UT-OSAL"
#define PCS_CONFIG_CONFIGSTR      "UT-config"
#define PCS_CONFIG_DATESTR        "UT-date"
#define PCS_CONFIG_USERSTR        "UT-user"
#define PCS_CONFIG_CPUNAME        "UT-cpuname"
#define PCS_CONFIG_CPUNUMBER      111
#define PCS_CONFIG_SPACECRAFT     222

/**
 * Stub for the main system entry function implemented in CFE ES
 */
void PCS_SystemMain(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath);

/**
 * Stub for 1Hz ISR function implemented in CFE TIME
 */
void PCS_System1HzISR(void);

/**
 * Stub for notification function implemented in CFE ES
 */
void PCS_SystemNotify(void);

#endif
