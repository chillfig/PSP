/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file cfe_psp_exceptionstorage_types.h
*
*   \brief The "MetaData" stores ephemeral exception information which only has meaning within the 
*   currently-running process.
*
*   \brief This data is important for diagnosing the exception, but it is NOT saved to any persistent log 
*   because it will not be relevant once the process ends.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was cloned-n-owned at the NASA Johnson Space Center from the NASA cFE open source 
*   with license below.
**********************************************************************************************************/

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

#ifndef CFE_PSP_EXCEPTIONSTORAGE_TYPES_H_
#define CFE_PSP_EXCEPTIONSTORAGE_TYPES_H_

#include "cfe_psp.h"
#include "cfe_psp_config.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

/** \brief Exception Log Data Struct */
struct CFE_PSP_Exception_LogData
{
    /** \brief a unique ID assigned to this exception entry */
    uint32                               context_id;
    /** \brief actual size of the "context_info" data */
    uint32                               context_size;
    /** \brief the BSP-specific task info (not osal abstracted id) */
    CFE_PSP_Exception_SysTaskId_t        sys_task_id;
    /** \brief Context Info */
    CFE_PSP_Exception_ContextDataEntry_t context_info;
};

/** \brief Exception Storage Struct */
struct CFE_PSP_ExceptionStorage
{
    /** \brief Num Written*/
    volatile uint32                  NumWritten;
    /** \brief Num Read*/
    volatile uint32                  NumRead;
    /** \brief Entries */
    struct CFE_PSP_Exception_LogData Entries[CFE_PSP_MAX_EXCEPTION_ENTRIES];
};

/** \brief Exception Log Data Type */
typedef struct CFE_PSP_Exception_LogData CFE_PSP_Exception_LogData_t;

/** \brief Exception Storage Type */
typedef struct CFE_PSP_ExceptionStorage CFE_PSP_ExceptionStorage_t;

/**
** \} <!-- End of group "psp_public_api" -->
*/

#endif /* CFE_PSP_EXCEPTIONSTORAGE_TYPES_H_ */
