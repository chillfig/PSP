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
*   \file cfe_psp_module.h
*
*   \brief Header file for the PSP public module data types and functions
*
*   \brief Header file containing the function declarations to initialize and manage PSP Modules
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

#ifndef CFE_PSP_MODULE_H_
#define CFE_PSP_MODULE_H_

#include "cfe_psp.h"
#include "target_config.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

/**
 ** \brief Maximum Module Name Length
 ** \par Note: Currently not enforced other than
 ** in cfe_psp_module.c -> CFE_PSP_Module_FindByName
 */
#define CFE_PSP_MODULE_NAME_MAX_LENGTH          30

/**
 ** \brief Enum Module Type
 ** \par Note:
 ** May be extended in the future
 */
typedef enum
{
    /** \brief Type Invalid */
    CFE_PSP_MODULE_TYPE_INVALID = 0,
    /** \brief Type Simple */
    CFE_PSP_MODULE_TYPE_SIMPLE,
    CFE_PSP_MODULE_TYPE_DEVICEDRIVER,
    /* May be extended in the future */
} CFE_PSP_ModuleType_t;

/**
 ** \brief Prototype for a PSP module initialization function
 */
typedef void (*CFE_PSP_ModuleInitFunc_t)(uint32 PspModuleId);

/**
 ** \brief Concrete version of the abstract API definition structure
 ** 
 ** \par Note:
 ** More API calls may be added for other module types
 */
typedef const struct
{
    /** \brief  Module Type */
    CFE_PSP_ModuleType_t     ModuleType;
    /** \brief  OperationFlags */
    uint32                   OperationFlags;
    /** \brief  Module Initialization Function */
    CFE_PSP_ModuleInitFunc_t Init;
    /* More API calls may be added for other module types */
    const void *ExtendedApi;
} CFE_PSP_ModuleApi_t;

/**
 ** \brief CFE_PSP_MODULE_DECLARE_SIMPLE
 ** 
 ** \par Description:
 ** Macro to simplify declaration of the IO Driver API structure according to
 ** the required naming convention.
 ** The "name" argument should match the name of the module object file
 */
#define CFE_PSP_MODULE_DECLARE_SIMPLE(name)              \
    static void         name##_Init(uint32 PspModuleId); \
    CFE_PSP_ModuleApi_t CFE_PSP_##name##_API = {         \
        .ModuleType     = CFE_PSP_MODULE_TYPE_SIMPLE,    \
        .OperationFlags = 0,                             \
        .Init           = name##_Init,                   \
    }

/**
 ** \func Initialize the included PSP modules
 ** 
 ** \par Description:
 ** This function initializes the include PSP modules.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is an optional part of the PSP and some PSP implementations 
 ** may not use it.
 ** \n\n
 ** Note 1: This function should only be called during PSP initialization before
 ** the system is operational. It is not intended to be called from application 
 ** code after cFE has started. The function is not necessarily be thread-safe 
 ** and should be called before any child threads are created.
 ** \n\n
 ** Note 2: This function does _not_ return any status.
 ** If a failure occurs during initialization that would make normal operation 
 ** impossible, then the module itself will call CFE_PSP_Panic() and this will 
 ** not return. Otherwise, benign/recoverable failures are expected to be just 
 ** that, and the calling code will not need to take any special action either 
 ** way. In short, if this function returns, then it means the system is good 
 ** enough to continue.
 **
 ** \param None
 **
 ** \return None
 */
void  CFE_PSP_ModuleInit(void);

/**
 ** \func Obtain the module ID by name
 **
 ** \par Description:
 ** This function retrieves the module ID of the given module name.
 **
 ** \par Assumptions, External Events, and Notes:
 ** Although this is currently prototyped as a function scoped to the PSP,
 ** this prototype could be moved to the public area so the cFS could use this.
 **
 ** \param[in] ModuleName - Name of the module to look up
 ** \param[out] PspModuleId - Pointer to the variable that stores the returned module ID
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_INVALID_MODULE_NAME
 */
int32  CFE_PSP_Module_FindByName(const char *ModuleName, uint32 *PspModuleId);

/**
 ** \func Obtain the API for a specific module
 **
 ** \par Description:
 ** This function retrieves the API structure for a given module ID.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] PspModuleId - The ID of the module (configuration-dependent)
 ** \param[out] API - Pointer to the variable that stores the returned API structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_INVALID_MODULE_ID
 */
int32  CFE_PSP_Module_GetAPIEntry(uint32 PspModuleId, CFE_PSP_ModuleApi_t **API);

/**
 ** \brief A list of fixed/base modules associated with the PSP
 ** 
 ** \par Description:
 ** This list should be generated by the build system based on the user-selected PSP
 */
extern CFE_StaticModuleLoadEntry_t CFE_PSP_BASE_MODULE_LIST[];

/**
** \} <!-- End of group "psp_public_api" -->
*/

#endif /* CFE_PSP_MODULE_H_ */

