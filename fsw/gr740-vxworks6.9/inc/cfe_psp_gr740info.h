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

#include <time.h>

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
    /** \brief Pointer to the string identifying the System Model */
    char * systemModel;
    /** \brief Pointer to the string identifying the system BSP Revision */
    char * systemBspRev;
    /** \brief Top of the System Physical Memory */
    uint32 systemPhysMemTop;
    /** \brief Number of Processors */
    int32 systemProcNum;
    /** \brief System Clock Rate */
    int32 systemClkRateGet;
    /** \brief System Aux Clock Rate */
    int32 systemAuxClkRateGet;
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
 ** \func Print the GR740 data to string buffer
 ** 
 ** \par Description:
 ** Internal function to print the gathered data from GR740 to a string buffer.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GR740PrintToBuffer(void);

/**
 ** \func Get disk free disk space
 ** 
 ** \par Description:
 ** Function uses the statfs64 to gather statistics about the file system.
 ** It works with both RAM and FLASH file systems such as "/ram0" and "/ffx0"
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] disk_root_path 
 **
 ** \return int64_t - Size of free space in disk in bytes
 ** \return CFE_PSP_ERROR - If statfs returned error
 */
int64_t CFE_PSP_GR740GetDiskFreeSize(char *disk_root_path);

/**
** \} <!-- End of group "psp_public_api_gr740vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* PSP_GR740_INFO_H */

