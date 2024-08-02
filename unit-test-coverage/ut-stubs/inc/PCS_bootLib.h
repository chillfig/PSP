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

/* PSP coverage stub replacement for bootLib.h */
#ifndef PCS_BOOTLIB_H
#define PCS_BOOTLIB_H

#include "PCS_basetypes.h"

/* ----------------------------------------- */
/* constants normally defined in bootLib.h */
/* ----------------------------------------- */
#define PCS_BOOT_FILE_LEN 160
#define PCS_MAX_BOOT_LINE_SIZE 255

#define PCS_BOOT_DEV_LEN            40      /* max chars in device name */
#define PCS_BOOT_HOST_LEN           20      /* max chars in host name */
#define PCS_BOOT_ADDR_LEN           30      /* max chars in net addr */
#define PCS_BOOT_TARGET_ADDR_LEN    50      /* IP address + mask + lease times */
#define PCS_BOOT_FILE_LEN           160     /* max chars in file name */
#define PCS_BOOT_USR_LEN            20      /* max chars in user name */
#define PCS_BOOT_PASSWORD_LEN       20      /* max chars in password */
#define PCS_BOOT_OTHER_LEN          80      /* max chars in "other" field */

/* ----------------------------------------- */
/* types normally defined in bootLib.h */
/* ----------------------------------------- */
typedef struct                          /* BOOT_PARAMS */
{
    char bootDev [PCS_BOOT_DEV_LEN];        /* boot device code */
    char hostName [PCS_BOOT_HOST_LEN];      /* name of host */
    char targetName [PCS_BOOT_HOST_LEN];    /* name of target */
    char ead [PCS_BOOT_TARGET_ADDR_LEN];    /* ethernet internet addr */
    char bad [PCS_BOOT_TARGET_ADDR_LEN];    /* backplane internet addr */
    char had [PCS_BOOT_ADDR_LEN];           /* host internet addr */
    char gad [PCS_BOOT_ADDR_LEN];           /* gateway internet addr */
    char bootFile [PCS_BOOT_FILE_LEN];      /* name of boot file */
    char startupScript [PCS_BOOT_FILE_LEN]; /* name of startup script file */
    char usr [PCS_BOOT_USR_LEN];            /* user name */
    char passwd [PCS_BOOT_PASSWORD_LEN];    /* password */
    char other [PCS_BOOT_OTHER_LEN];        /* available for applications */
    int  procNum;                       /* processor number */
    int  flags;                         /* configuration flags */
    int  unitNum;                       /* network device unit number */
} PCS_BOOT_PARAMS;

/* ----------------------------------------- */
/* prototypes normally declared in bootLib.h */
/* ----------------------------------------- */
char *PCS_bootStringToStruct(char * bootString, PCS_BOOT_PARAMS * pBootParams);
int PCS_bootStructToString(char * bootString, PCS_BOOT_PARAMS *pBootParams);

#endif
