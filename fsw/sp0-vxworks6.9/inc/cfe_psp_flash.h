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
*   \file cfe_psp_flash.h
*
*   \brief API header for controlling FLASH interaction
*
*   \brief This file contains the function prototypes relating to FLASH interactions.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_FLASH_H
#define PSP_FLASH_H

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
 ** \func Create a directory
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
