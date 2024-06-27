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
*   \file cfe_psp_exception.h
*
*   \brief API header for collecting SP0(s) hardware and software information
*
*   \brief Functions here allow PSP to load and save exceptions from flash.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_EXCEPTIONS_H
#define PSP_EXCEPTIONS_H


#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \func Load data from flash
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will recover the flash user data.
 ** Data is saved using #CFE_PSP_SaveToNVRAM
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is preserved for backwards compatibility, and simply calls 
 ** CFE_PSP_LoadExceptionData which is the new preferred API.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_LoadFromNVRAM(void);

/**
 ** \func Save data to flash
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will save the ED&R and Memory Boot data to persistent memory.
 ** 
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is preserved for backwards compatibility, and simply calls
 ** CFE_PSP_SaveExceptionData which is the new preferred API.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SaveToNVRAM(void);

/**
 ** \func Clear flash
 ** 
 ** \par Description:
 ** Function reset to \0 the signature in flash.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is preserved for backwards compatibility, and simply calls
 ** CFE_PSP_ClearExceptionData which is the new preferred API.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_ClearNVRAM(void);

/**
 ** \func Load exception data from flash
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will recover the flash user data.
 ** Data is saved to FLASH using #CFE_PSP_SaveExceptionData.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the user FLASH to minimize reading and writing to EEPROM.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_LoadExceptionData(void);

/**
 ** \func Save exception data to flash
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will save the ED&R and Memory Boot data to flash.
 ** 
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the user FLASH to minimize reading and writing to EEPROM.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SaveExceptionData(void);

/**
 ** \func Clear exception data
 ** 
 ** \par Description:
 ** Function reset to \0 the signature in flash.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_ClearExceptionData(void);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_EXCEPTIONS_H_ */

