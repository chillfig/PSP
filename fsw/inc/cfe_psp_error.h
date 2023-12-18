/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file cfe_psp_error.h
*
*   \brief Header file for cFE PSP Error Handler
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

#ifndef CFE_PSP_ERROR_H
#define CFE_PSP_ERROR_H

#include "common_types.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/


/**
 * \brief PSP Status type for readability and potentially type safety
 */
typedef int32 CFE_PSP_Status_t;

/**
 * \brief PSP Status macro for literal
 */
#define CFE_PSP_STATUS_C(X) ((CFE_PSP_Status_t)(X))

/**
 * \brief PSP Status converted to string length limit
 *
 * Used for sizing CFE_PSP_StatusString_t intended for use in printing CFE_PSP_Status_t values
 * Sized for %ld (LONG_MIN) including NULL
 */
#define CFE_PSP_STATUS_STRING_LENGTH 12

/**
 * @brief For the @ref CFE_PSP_StatusToString() function, to ensure
 * everyone is making an array of the same length.
 */
typedef char CFE_PSP_StatusString_t[CFE_PSP_STATUS_STRING_LENGTH];

/**
 * @brief Convert status to a string
 *
 * @param[in] status Status value to convert
 * @param[out] status_string Buffer to store status converted to string
 *
 * @return Passed in string pointer
 */
char *CFE_PSP_StatusToString(CFE_PSP_Status_t status, CFE_PSP_StatusString_t *status_string);

/** \name Error and return codes */
/** \{ */
#define CFE_PSP_SUCCESS                  (CFE_PSP_STATUS_C(0))   /**< \brief  Success */
#define CFE_PSP_ERROR                    (CFE_PSP_STATUS_C(-1))  /**< \brief  Generic Error */
#define CFE_PSP_INVALID_POINTER          (CFE_PSP_STATUS_C(-2))  /**< \brief  Invalid Pointer */
#define CFE_PSP_ERROR_ADDRESS_MISALIGNED (CFE_PSP_STATUS_C(-3))  /**< \brief  Misaligned Address */
#define CFE_PSP_ERROR_TIMEOUT            (CFE_PSP_STATUS_C(-4))  /**< \brief  Timeout Error */
#define CFE_PSP_INVALID_INT_NUM          (CFE_PSP_STATUS_C(-5))  /**< \brief  Invalid Integer Number */
#define CFE_PSP_INVALID_MEM_ADDR         (CFE_PSP_STATUS_C(-21)) /**< \brief  Invalid Memory Address */
#define CFE_PSP_INVALID_MEM_TYPE         (CFE_PSP_STATUS_C(-22)) /**< \brief  Invalid Memory Type */
#define CFE_PSP_INVALID_MEM_RANGE        (CFE_PSP_STATUS_C(-23)) /**< \brief  Invalid Memory Range */
#define CFE_PSP_INVALID_MEM_WORDSIZE     (CFE_PSP_STATUS_C(-24)) /**< \brief  Invalid Memory Word Size */
#define CFE_PSP_INVALID_MEM_SIZE         (CFE_PSP_STATUS_C(-25)) /**< \brief  Invalid Memory Size */
#define CFE_PSP_INVALID_MEM_ATTR         (CFE_PSP_STATUS_C(-26)) /**< \brief  Invalid Memory Attribute */
#define CFE_PSP_ERROR_NOT_IMPLEMENTED    (CFE_PSP_STATUS_C(-27)) /**< \brief  Not Implemented */
#define CFE_PSP_INVALID_MODULE_NAME      (CFE_PSP_STATUS_C(-28)) /**< \brief  Invalid Module Name */
#define CFE_PSP_INVALID_MODULE_ID        (CFE_PSP_STATUS_C(-29)) /**< \brief  Invalid Module ID */
#define CFE_PSP_NO_EXCEPTION_DATA        (CFE_PSP_STATUS_C(-30)) /**< \brief  No Exception Data */
#define CFE_PSP_ERROR_LEVEL_0            (CFE_PSP_STATUS_C(-31)) /**< \brief  Generic Error, but returned data is valid */
/** \} */

/**
** \} <!-- End of group "psp_public_api" -->
*/

#endif /* CFE_PSP_ERROR_H */
