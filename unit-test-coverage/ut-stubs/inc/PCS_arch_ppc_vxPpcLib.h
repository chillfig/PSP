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

/* PSP coverage stub replacement for vxPpcLib.h */
#ifndef PCS_ARCH_PPC_VXPPCLIB_H
#define PCS_ARCH_PPC_VXPPCLIB_H

#include "PCS_basetypes.h"

/*
 * When building the coverage test on an actual VxWorks target,
 * these symbols are indirectly provided via the system limits.h file.
 * This results in a redefinition issue if the custom PCS definitions are used.
 */
#if defined(_VXWORKS_OS_) && defined(__PPC__)

#define PCS_PPC_MSR_EE  _PPC_MSR_EE
#define PCS_PPC_MSR_FP  _PPC_MSR_FP
#define PCS_PPC_MSR_ME  _PPC_MSR_ME
#define PCS_PPC_MSR_FE0 _PPC_MSR_FE0
#define PCS_PPC_MSR_FE1 _PPC_MSR_FE1
#define PCS_PPC_MSR_DR  _PPC_MSR_DR

#define PCS_PPC_FPSCR_VE _PPC_FPSCR_VE
#define PCS_PPC_FPSCR_OE _PPC_FPSCR_OE
#define PCS_PPC_FPSCR_NI _PPC_FPSCR_NI
#define PCS_PPC_FPSCR_ZE _PPC_FPSCR_ZE
#define PCS_PPC_FPSCR_XE _PPC_FPSCR_XE
#define PCS_PPC_FPSCR_UE _PPC_FPSCR_UE

#else

/* ----------------------------------------- */
/* constants normally defined in vxPpcLib.h */
/* ----------------------------------------- */

#define PCS_PPC_MSR_EE  0x2101
#define PCS_PPC_MSR_FP  0x2102
#define PCS_PPC_MSR_ME  0x2104
#define PCS_PPC_MSR_FE0 0x2108
#define PCS_PPC_MSR_FE1 0x2110
#define PCS_PPC_MSR_DR  0x2120

#define PCS_PPC_FPSCR_VE 0x2201
#define PCS_PPC_FPSCR_OE 0x2202
#define PCS_PPC_FPSCR_NI 0x2204
#define PCS_PPC_FPSCR_ZE 0x2208
#define PCS_PPC_FPSCR_XE 0x2210
#define PCS_PPC_FPSCR_UE 0x2220

#define PCS_PPC_MSR_SPE         0x02000000

#define PCS_PPC_SPEFSCR_FINXE   0x00000040
#define PCS_PPC_SPEFSCR_FINVE   0x00000020
#define PCS_PPC_SPEFSCR_FDBZE   0x00000010
#define PCS_PPC_SPEFSCR_FUNFE   0x00000008
#define PCS_PPC_SPEFSCR_FOVFE   0x00000004

#define PCS_PPC_SPEFSCR_SOV     0x00008000
#define PCS_PPC_SPEFSCR_OV      0x00004000
#define PCS_PPC_SPEFSCR_FG      0x00002000
#define PCS_PPC_SPEFSCR_FX      0x00001000
#define PCS_PPC_SPEFSCR_FINV    0x00000800
#define PCS_PPC_SPEFSCR_FDBZ    0x00000400
#define PCS_PPC_SPEFSCR_FUNF    0x00000200
#define PCS_PPC_SPEFSCR_FOVF    0x00000100

#endif

/* ----------------------------------------- */
/* types normally defined in vxPpcLib.h */
/* ----------------------------------------- */

/* ----------------------------------------- */
/* prototypes normally declared in vxPpcLib.h */
/* ----------------------------------------- */
extern void     PCS_vxTimeBaseGet(uint32_t *u, uint32_t *l);
extern void     PCS_vxMsrSet(uint32_t);
extern uint32_t PCS_vxMsrGet(void);
extern void     PCS_vxFpscrSet(uint32_t);
extern uint32_t PCS_vxFpscrGet(void);
extern uint32_t PCS_vxDecGet(void);

extern uint32_t PCS_vxSpefscrGet(void);
extern void     PCS_vxSpefscrSet(uint32_t);

#endif
