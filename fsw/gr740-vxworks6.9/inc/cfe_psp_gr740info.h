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
*   \file cfe_psp_gr740info.h
*
*   \brief API header for collecting GR740 hardware and software information
*
*   \brief Functions here allow CFS to provide a method to probe various board hardware and software 
*   information from POST, Temperatures, Voltages, Active Boot EEPROM, etc. In addition, this module has 
*   a function to save a dump_core text file before aborting CFS execution. 
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_GR740_INFO_H
#define PSP_GR740_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_gr740vx69 PSP Public APIs - GR740-VxWorks6.9 Platform
** \{
*/

/**
 ** \brief GR740_TEXT_BUFFER_MAX_SIZE
 ** \par Description:
 ** This is the maximum size of the GR740 char array table.
 */
#define GR740_TEXT_BUFFER_MAX_SIZE            1000

/**
 ** \brief Default GR740 Info pre-print string 
 ** \par Description:
 ** This string is printed before every print related to GR740 Info API.
*/
#define GR740_PRINT_SCOPE                     "PSP GR740: "

/**
** \brief GR740 static info structure
** \par Description:
** The table includes values that changes only once during boot.
*/
typedef struct
{
    /** \brief UTC date time when the data was collected */
    struct timespec lastUpdatedUTC;
} CFE_PSP_GR740StaticInfoTable_t;

/**
** \brief GR740 dynamic info structure
** \par Description:
** The table includes values that may change continuously.
*/
typedef struct
{
    /** \brief UTC date time when the data was collected */
    struct timespec lastUpdatedUTC;
    /** \brief Temperature at one point on the GR740 computer in Celsius, ranges from 0-127 */
    uint32 temperature;
} CFE_PSP_GR740DynamicInfoTable_t;

int32 CFE_PSP_TempSensorInit(void);
int32 CFE_PSP_GetTemperatureAwait(uint32 * uiTempPtr);

/**
 ** \func Collect GR740 Hardware and Firmware information
 **
 ** \par Description:
 ** This function collects the GR740 hardware and firmware information and saves it
 ** in the #CFE_PSP_GR740StaticInfoTable_t structure.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GR740CollectStaticInfo(void);

/**
 ** \func Collect GR740 Hardware and Firmware data
 **
 ** \par Description:
 ** This function collects dynamic GR740 information and
 ** saves it in the #CFE_PSP_GR740DynamicInfoTable_t structure.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GR740CollectDynamicInfo(void);

/**
 ** \func Get the structure containing the GR740 static info data table
 **
 ** \par Description:
 ** This function returns and/or print the structure containing the GR740 Hardware and Firmware
 ** data.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Printing to console will print the both static and dynamic data tables
 **
 ** \param pStatic Pointer to a #CFE_PSP_GR740StaticInfoTable_t structure
 ** \param iStaticSize - Size of the memory array pointed by pStatic
 ** \param print_to_console Print string buffer to console if larger than 0
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GR740GetStaticInfoTable(CFE_PSP_GR740StaticInfoTable_t *pStatic, size_t iStaticSize, uint8_t print_to_console);

/**
 ** \func Get the structure containing the GR740 dynamic data table
 **
 ** \par Description:
 ** This function returns and/or print the structure containing the temperatures,
 ** voltages and uptime values obtained from the GR740 hardware.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Printing to console will print the both static and dynamic data tables
 **
 ** \param pDynamic Pointer to a #CFE_PSP_GR740DynamicInfoTable_t structure
 ** \param iDynamicSize - Size of the memory array pointed by pDynamic
 ** \param print_to_console Print string buffer to console if larger than 0
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GR740GetDynamicInfoTable(CFE_PSP_GR740DynamicInfoTable_t *pDynamic, size_t iDynamicSize, uint8_t print_to_console);


/**
** \} <!-- End of group "psp_public_api_gr740vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* PSP_GR740_INFO_H */

