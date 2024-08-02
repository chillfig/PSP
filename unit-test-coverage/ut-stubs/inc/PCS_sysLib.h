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
#ifndef PCS_SYSLIB_H
#define PCS_SYSLIB_H

#include "PCS_basetypes.h"
#include "PCS_vxWorks.h"

/* ----------------------------------------- */
/* constants normally defined in sysLib.h */
/* ----------------------------------------- */
extern uint32_t PCS_l2errTotal;
extern uint32_t PCS_l2errMult;
extern uint32_t PCS_l2errTagPar;
extern uint32_t PCS_l2errMBECC;
extern uint32_t PCS_l2errSBECC;
extern uint32_t PCS_l2errCfg;
extern uint32_t PCS_mchCause;
extern uint32_t PCS_mchkHook;

/* ----------------------------------------- */
/* types normally defined in sysLib.h */
/* ----------------------------------------- */

/* ----------------------------------------- */
/* prototypes normally declared in sysLib.h */
/* ----------------------------------------- */
extern int   PCS_sysClkRateGet(void);
extern char *PCS_sysMemTop(void);
extern char *PCS_sysPhysMemTop(void);
extern char *PCS_sysModel(void);
extern char *PCS_sysBspRev(void);
extern int PCS_sysProcNumGet(void);
extern int PCS_sysAuxClkRateGet(void);

extern void PCS_PciOutByte(uint32_t address, uint8_t data);
extern void PCS_PciOutLong(uint32_t address, uint32_t data);
extern void PCS_sysPciWrite32(uint32_t address, uint32_t data);
extern void PCS_sysPciRead32(uint32_t address, uint32_t *data);

extern unsigned int PCS_GetWrsKernelTextStart(void);
extern unsigned int PCS_GetWrsKernelTextEnd(void);

extern char *PCS_sysModel(void);
extern char *PCS_sysBspRev(void);
extern int PCS_sysProcNumGet(void);

extern void PCS_ckCtrs(void);

#endif
