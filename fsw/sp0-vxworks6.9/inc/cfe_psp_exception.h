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
*   \file cfe_psp_exception.h
*
*   \brief API header for collecting SP0(s) hardware and software information
*
*   \brief Functions here allow PSP to load and save exceptions from EEPROM.
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
 ** \func Load data from NVRAM
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will first verify that the EEPROM user area contains a valid,
 ** signature, then recover it.
 ** Data is saved to NVRAM (EEPROM) using #CFE_PSP_SaveToNVRAM.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the onboard EEPROM user area with maximum usable
 ** space of 20 kB. Read and Write access is slow.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_LoadFromNVRAM(void);

/**
 ** \func Save data to NVRAM
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will save the ED&R and Memory Boot data to the NVRAM user area.
 ** It will also save a simple signature to allow the #CFE_PSP_LoadFromNVRAM to
 ** validate the presence of data.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the onboard EEPROM user area with maximum usable
 ** space of 20 kB. Read and Write access is slow.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_SaveToNVRAM(void);

/**
 ** \func Clear NVRAM
 ** 
 ** \par Description:
 ** Function reset to \0 the signature in NVRAM.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_ClearNVRAM(void);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_EXCEPTIONS_H_ */

