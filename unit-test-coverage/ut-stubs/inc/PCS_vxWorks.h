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
 * \ingroup  ut-stubs
 * \author   joseph.p.hickey@nasa.gov
 * PSP coverage stub replacement for vxWorks.h
 */

#ifndef PCS_VXWORKS_H
#define PCS_VXWORKS_H

#include "PCS_basetypes.h"

/* ----------------------------------------- */
/* constants normally defined in vxWorks.h */
/* ----------------------------------------- */
enum
{
    PCS_ERROR = -1,
    PCS_OK    = 0
};

enum
{
    PCS_WAIT_FOREVER = -1,
    PCS_NO_WAIT      = 0
};

#define PCS_EOS '\0'

/* ----------------------------------------- */
/* types normally defined in vxWorks.h */
/* ----------------------------------------- */
typedef int          PCS_STATUS;
typedef bool         PCS_BOOL;
typedef unsigned int PCS_UINT;
typedef int8_t       PCS_INT8;
typedef uint8_t      PCS_UINT8;
typedef int16_t      PCS_INT16;
typedef uint16_t     PCS_UINT16;
typedef int32_t      PCS_INT32;
typedef uint32_t     PCS_UINT32;

typedef long PCS_Vx_usr_arg_t;
typedef long PCS_Vx_exit_code_t;

/* Function pointers are used in many VxWorks modules. */
/*
 * NOTE: The FUNCPTR type in the actual library may be defined
 * without arguments, e.g. "int (*FUNCPTR)()".  This is acceptable
 * by some compilers but generally incompatible with the
 * "-Wstrict-prototype" gcc warning option.  So in this override it
 * is defined as an int argument.  This means that application code
 * may need to cast it at the time of use (which is generally done anyway).
 */
typedef int (*PCS_FUNCPTR)(int);
typedef void (*PCS_VOIDFUNCPTR)(void);

/* ----------------------------------------- */
/* prototypes normally declared in vxWorks.h */
/* ----------------------------------------- */


#endif
