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

/* PSP coverage stub replacement for scratchRegMap.h */
#ifndef PCS_SCRATCHREGMAP_H
#define PCS_SCRATCHREGMAP_H

#include "common_types.h"

#include "PCS_basetypes.h"
#include "PCS_vxWorks.h"

/* ----------------------------------------- */
/* constants normally defined in scratchRegMap.h */
/* ----------------------------------------- */
typedef enum {
    SM_LOCAL_SBC  = 0, /* safe mode reset occurred by local SBC */
    SM_REMOTE_SBC = 1  /* provisional: safe mode reset occurred by remote SBC */
} PCS_SAFE_MODE_SBC_LOC;

typedef enum {
    SM_REASON_FWDT_FAIL           = 0, /* safe mode reset occurred by FWDT fail */
    SM_REASON_UNRECOVERABLE_FAULT = 1, /* safe mode reset occurred by SDRAM/L1/L2 unrecoverable fault */
    SM_REASON_SAFE_MODE_API       = 2, /* provisional: safe mode reset occurred by safe mode API */
    SM_REASON_SHUTDOWN_MODE_API   = 3, /* provisional: safe mode reset occurred by shutdown mode API */
    SM_REASON_EDR_FATAL_ERROR     = 4, /* provisional: safe mode reset occurred by fatal error captured by EDR facility */
    SM_REASON_NO_OFP              = 5  /* provisional: safe mode reset occurred for no OFP to load */
} PCS_SAFE_MODE_RESET_REASON;

typedef enum {
    RESET_SRC_POR  = 0x08,    /* Power on Reset */
    RESET_SRC_WDT  = 0x10,    /* External FPGA Watchdog timer - detects primary EEPROM boot failure */
    RESET_SRC_SWR  = 0x20,    /* Software Reset - detects user (including machine check) requested reset */
    RESET_SRC_CPCI = 0x40,    /* cPCI Reset - detects cPCI reset initiated by FPGA from remote SBC */
    RESET_SRC_FWDT = 0x80     /* Internal FPGA Watchdog timer - detects application SW failure */
} PCS_RESET_SRC_REG_ENUM;

/* ----------------------------------------- */
/* types normally defined in scratchRegMap.h */
/* ----------------------------------------- */


typedef struct
{
    bool safeMode;                  /* 1 = current reset is due to safe mode reset, 0 = normal POR */
    PCS_SAFE_MODE_SBC_LOC sbc;          /* location of SBC caused safe mode reset if occurred */
    PCS_SAFE_MODE_RESET_REASON reason;  /* reson of safe mode reset if occurred */
    uint32 mckCause;                /* reason for unrecoverable machine check fault, see bit definition above */
    uint32 data0;                   /* additional data0, if it is saved by machine check exception, DEAR (Exception Data Address) */
    uint32 data1;                   /* additional data1, if it is saved by machine check exception, PC (Exception Instruction Address) */
} PCS_USER_SAFE_MODE_DATA_STRUCT;

/* ----------------------------------------- */
/* prototypes normally declared in scratchRegMap.h */
/* ----------------------------------------- */
PCS_STATUS PCS_ReadResetSourceReg(PCS_RESET_SRC_REG_ENUM *pResetSrc, bool talkative);
PCS_STATUS PCS_ReadSafeModeUserData(PCS_USER_SAFE_MODE_DATA_STRUCT *pSafeModeUserData, bool talkative);

#endif
