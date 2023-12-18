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
*   \file cfe_psp_exceptionstorage_api.h
*
*   \brief This file provides a generic storage buffer ring for exceptions and functions to manipulate it.
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

#ifndef CFE_PSP_EXCEPTIONSTORAGE_API_H_
#define CFE_PSP_EXCEPTIONSTORAGE_API_H_

#include "cfe_psp.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

/**
 **  \brief Abstract types for exception storage.
 **  These are made concrete depending on PSP-specific config information.
 */
struct CFE_PSP_Exception_LogData;

/* -------------------------------------------------------------
 *  Functions implemented in shared layer, invoked by impl layer.
 * ------------------------------------------------------------- */

/**
 ** \func Get the next buffer for exception buffer corresponding to sequence
 ** 
 ** \par Description:
 ** This function obtains a storage buffer corresponding to the given sequence number. 
 ** The pointer to storage memory is directly returned. 
 **
 ** \par Assumptions, External Events, and Notes:
 ** It is not cleared or modified, and no checks are performed to determine 
 ** if the sequence number is valid.
 **
 ** \param[in] seq - Sequence number
 **
 ** \return Pointer to buffer.
 */
struct CFE_PSP_Exception_LogData  *CFE_PSP_Exception_GetBuffer(uint32 seq);

/**
 ** \func Get the next buffer for exception context storage
 **
 ** \par Description:
 ** This function is invoked by the low level exception handler (typically an ISR/signal)
 ** to obtain a buffer for context capture.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The buffer is cleared (memset zero) before returning to the caller.
 **
 ** \param None
 **
 ** \return Pointer to buffer - If successful
 ** \return NULL - If storage is full
 */
struct CFE_PSP_Exception_LogData  *CFE_PSP_Exception_GetNextContextBuffer(void);

/**
 ** \func Wrap up the storage of exception data
 **
 ** \par Description:
 ** This function is invoked by the low level exception handler (typically an ISR/signal)
 ** once the exception context capture is complete.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This should be invoked after a successful call to CFE_PSP_Exception_GetNextContextBuffer()
 ** to commit the information to the log.
 **
 ** \param None
 **
 ** \return None
 */
void  CFE_PSP_Exception_WriteComplete(void);

/**
 ** \func Reset the exception storage buffer
 **
 ** \par Description:
 ** This function resets the state of exception processing.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return None
 */
void  CFE_PSP_Exception_Reset(void);

/* -------------------------------------------------------------
 * Functions implemented in impl layer, invoked by shared layer.
 * ------------------------------------------------------------- */

/**
 ** \func Translate the exception context data into a string
 **
 ** \par Description:
 ** This function translates the exception context data into a user-friendly "reason" string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This is called in an application context to determine the cause of the exception.
 **
 ** \param[in] Buffer - Pointer to the Buffer Context data previously stored by ISR/signal handler
 ** \param[out] ReasonBuf - Buffer to store string
 ** \param[in] ReasonSize - Size of string buffer
 **
 ** \return #CFE_PSP_SUCCESS on success
 ** \return #CFE_PSP_ERROR - When buffer is null, or when could not write the
 **                          whole message to reason buffer.
 */
int32 CFE_PSP_ExceptionGetSummary_Impl(const struct CFE_PSP_Exception_LogData *Buffer, char *ReasonBuf,
                                              uint32 ReasonSize);
/**
** \} <!-- End of group "psp_public_api" -->
*/

#endif /* CFE_PSP_EXCEPTIONSTORAGE_API_H_ */
