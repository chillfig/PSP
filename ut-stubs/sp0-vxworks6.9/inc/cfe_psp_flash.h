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
*   \file cfe_psp_flash.h
*
*   \brief API header to control FLASH interaction
*
*   \copyright
*   Copyright (c) 2019-2022 United States Government as represented by
*   the Administrator of the National Aeronautics and Space Administration.
*   All Rights Reserved.
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   \par Description:
*   This file contains the function prototypes relating to FLASH interactions.
*
*   \par Limitations, Assumptions, External Events, and Notes:
*   None
**********************************************************************************************************/

#ifndef PSP_FLASH_H
#define PSP_FLASH_H

#include "common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \func Read data from flash
 **
 ** \par Description:
 ** Read a specific amount of data from flash
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] p_dest - destination buffer
 ** \param[in] size - Amount of data to read
 ** \param[in] fname - file name
 **
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_ReadFromFlash(uint32 *p_dest, size_t size, char *fname);

/**
 ** \func  Write data to flash
 **
 ** \par Description:
 ** Write a specific amount of data to flash
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] p_src - Data source
 ** \param[in] size - Amount of data to write
 ** \param[in] fname - file name
 **
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_WriteToFlash(uint32 *p_src, size_t size, char *fname);

/**
 ** \func Delete a file
 **
 ** \par Description:
 ** Delete a file
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] fname - file name
 **
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_DeleteFile(char *fname);

/**
 ** \func Create a file
 **
 ** \par Description:
 ** Create a file
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] fname - file name
 **
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_CreateFile(char *fname);

/**
 ** \func Check if a file exists
 **
 ** \par Description:
 ** Check if a file exists
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will only check if file exists. Will not send file
 ** send/fill any file information
 **
 ** \param[in] fname - file name
 **
 ** \return false - file does not exist or invalid pointer
 ** \return true - file does exist
 */
bool CFE_PSP_CheckFile(char *fname);

/**
 ** \brief Create a directory
 **
 ** \par Description:
 ** Create a directory
 **
 ** \par Assumptions, External Events, and Notes:
 ** If the directory already exists, this will NOT
 ** 'reset' the directory.
 **
 ** \param[in] p_dir - Null terminated character array containing dir path
 **
 ** \return #CFE_PSP_SUCCESS - Successfully created directory
 ** \return #CFE_PSP_ERROR - Unsuccessfully created directory
 ** \return #CFE_PSP_INVALID_POINTER - Invalid pointer
 */
int32 CFE_PSP_CreateDirectory(const char *p_dir);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_FLASH_H_ */