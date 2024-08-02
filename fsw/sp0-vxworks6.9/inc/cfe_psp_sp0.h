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
*   \file cfe_psp_sp0.h
*
*   \brief API header for collecting SP0(s) hardware and software information
*
*   \brief Functions here allow CFS to provide a method to probe SP0 hardware for information from POST,
*   Temperatures, Voltages, Active Boot EEPROM, etc. In addition, this module has a function to save a 
*   dump_core text file before aborting CFS execution.  
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef CFE_PSP_SP0_H
#define CFE_PSP_SP0_H

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \name Max number of Voltage and Temperature sensors per target generation
 **
 ** \par Description:
 ** Aitech consider SP0 and SP0s as Original and Upgrade respectively.
 **/
/** \{ */
/** \brief SP0s Maximum Number of Voltage Sensors */
#define SP0_UPGRADE_MAX_VOLT_SENSORS    6
/** \brief SP0 Maximum Number of Voltage Sensors */
#define SP0_ORIGINAL_MAX_VOLT_SENSORS   0
/** \brief SP0s Maximum Number of Temperature Sensors */
#define SP0_UPGRADE_MAX_TEMP_SENSORS    4
/** \brief SP0 Maximum Number of Temperature Sensors */
#define SP0_ORIGINAL_MAX_TEMP_SENSORS   3
/** \} */

/** \brief ROM1 LOCK Code */
#define SP0_ROM1_CODE_LOCK                  0x000000A1
/** \brief ROM1 UNLOCK Code */
#define SP0_ROM1_CODE_UNLOCK                0x000000A3
/** \brief ROM2 LOCK Code */
#define SP0_ROM2_CODE_LOCK                  0x000000B1
/** \brief ROM2 UNLOCK Code */
#define SP0_ROM2_CODE_UNLOCK                0x000000B3
/** \brief SP0 Boot ROM Status Address */
#define SP0_BOOT_ROM_STATUS_ADDR            0xE8000040
/** \brief SP0 ROM1 Bit Mask */
#define SP0_ROM1_MASK                       0x00000100
/** \brief SP0 ROM2 Bit Mask */
#define SP0_ROM2_MASK                       0x00000200
/** \brief SP0 ROM1 Status Shift */
#define SP0_ROM1_STATUS_SHIFT               8
/** \brief SP0 ROM2 Status Shift */
#define SP0_ROM2_STATUS_SHIFT               9

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_SP0_INFO_H_ */