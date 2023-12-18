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
*   \file cfe_psp_sp0info.h
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

#ifndef PSP_SP0_INFO_H
#define PSP_SP0_INFO_H

#ifdef __cplusplus
extern "C" {
#endif

/* For supporting REALTIME clock */
#include <timers.h>

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \brief SP0_TEXT_BUFFER_MAX_SIZE
 ** \par Description:
 ** This is the maximum size of the SP0 char array table.
 */
#define SP0_TEXT_BUFFER_MAX_SIZE            1000

/**
 ** \brief SP0_SAFEMODEUSERDATA_BUFFER_SIZE
 ** \par Description:
 ** This is the maximum size of the safeModeUserData char array.
 */
#define SP0_SAFEMODEUSERDATA_BUFFER_SIZE    256

/**
 ** \brief Default SP0 Info pre-print string 
 ** \par Description:
 ** This string is printed before every print related to SP0 Info API.
*/
#define SP0_PRINT_SCOPE                     "PSP SP0: "

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
** \brief SP0 static info structure
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
    /** \brief Slot ID in the chassis */
    int32 systemSlotId;
    /** \brief Identifies if the SP0 is the cPCI main system controller */
    bool systemCpciSysCtrl;
    /** \brief System Core Clock Speed in MHz */
    uint32 systemCoreClockSpeed;
    /** \brief Reason for last SP0 computer reset */
    uint8 systemLastResetReason;
    /** \brief Identifies the EEPROM to successfully booted the kernel */
    uint8 active_boot;
    /** \brief System Clock Rate */
    int32 systemClkRateGet;
    /** \brief System Aux Clock Rate */
    int32 systemAuxClkRateGet;
    /** \brief Identifies the POST Test Bit Executed */
    uint64 bitExecuted;
    /** \brief Identifies the POST Test Results */
    uint64 bitResult;
    /** \brief Safe Mode User Data */
    char safeModeUserData[SP0_SAFEMODEUSERDATA_BUFFER_SIZE];
} CFE_PSP_SP0StaticInfoTable_t;

/**
** \brief SP0 dynamic info structure
** \par Description:
** The table includes values that may change continuously.
*/
typedef struct
{
    /** \brief UTC date time when the data was collected */
    struct timespec lastUpdatedUTC;
    /** \brief Number of usec since startup */
    float systemStartupUsecTime;
    /** \brief Array of 4 temperatures on the SP0 computer */
    float temperatures[4];
    /** \brief Array of 6 voltages powering the SP0 */
    float voltages[6];  
} CFE_PSP_SP0DynamicInfoTable_t;

/**
 ** \func Collect SP0 Hardware and Firmware information
 **
 ** \par Description:
 ** This function collects the SP0 hardware and firmware information and saves it
 ** in the #CFE_PSP_SP0StaticInfoTable_t structure.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32  CFE_PSP_SP0CollectStaticInfo(void);

/**
 ** \func Collect SP0 Hardware and Firmware data
 **
 ** \par Description:
 ** This function collects the SP0 temperatures, voltages, and uptime values and
 ** saves them in the #CFE_PSP_SP0DynamicInfoTable_t structure.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32  CFE_PSP_SP0CollectDynamicInfo(void);

/**
 ** \func Get the structure containing the SP0 static info data table
 **
 ** \par Description:
 ** This function returns and/or print the structure containing the SP0 Hardware and Firmware
 ** data.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Printing to console will print the both static and dynamic data tables
 **
 ** \param pStatic Pointer to a #CFE_PSP_SP0StaticInfoTable_t structure
 ** \param iStaticSize - Size of the memory array pointed by pStatic
 ** \param print_to_console Print string buffer to console if larger than 0
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0GetStaticInfoTable(CFE_PSP_SP0StaticInfoTable_t *pStatic, size_t iStaticSize, uint8_t print_to_console);

/**
 ** \func Get the structure containing the SP0 dynamic data table
 **
 ** \par Description:
 ** This function returns and/or print the structure containing the temperatures,
 ** voltages and uptime values obtained from the SP0 hardware.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Printing to console will print the both static and dynamic data tables
 **
 ** \param pDynamic Pointer to a #CFE_PSP_SP0DynamicInfoTable_t structure
 ** \param iDynamicSize - Size of the memory array pointed by pDynamic
 ** \param print_to_console Print string buffer to console if larger than 0
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0GetDynamicInfoTable(CFE_PSP_SP0DynamicInfoTable_t *pDynamic, size_t iDynamicSize, uint8_t print_to_console);

/**
 ** \func Function dumps the collected data to file
 **
 ** \par Description:
 ** Saves data dump to location defined by #SP0_DATA_DUMP_FILEPATH
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0DumpData(void);

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
 ** \param[in] ram_disk_root_path 
 **
 ** \return int64_t - Size of free space in disk in bytes
 ** \return CFE_PSP_ERROR - If statfs returned error
 */
int64_t CFE_PSP_SP0GetDiskFreeSize(char *ram_disk_root_path);

/**
 ** \func Lock ROM1
 **
 ** \par Description:
 ** Function that will lock ROM1
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will still attempt to lock ROM1
 ** even if it is currently locked
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM1Lock(void);

/**
 ** \func Unlock ROM1
 **
 ** \par Description:
 ** Function that will unlock ROM1
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will still attempt to unlock ROM1
 ** even if it is currently unlocked
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM1Unlock(void);

/**
 ** \func Lock ROM2
 **
 ** \par Description:
 ** Function that will lock ROM2
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will still attempt to lock ROM2
 ** even if it is currently locked
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM2Lock(void);

/**
 ** \func Unlock ROM2
 **
 ** \par Description:
 ** Function that will unlock ROM2
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function will still attempt to unlock ROM2
 ** even if it is currently unlocked
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0ROM2Unlock(void);

/**
 ** \func Get ROM1 Lock/Unlock Status
 **
 ** \par Description:
 ** Function will return LOCK/UNLOCK status of ROM1
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return false - ROM1 is UNLOCKED
 ** \return true - ROM1 is LOCKED
 */
bool CFE_PSP_SP0ROM1Status(void);

/**
 ** \func Get ROM2 Lock/Unlock Status
 **
 ** \par Description:
 ** Function will return LOCK/UNLOCK status of ROM2
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return false - ROM2 is UNLOCKED
 ** \return true - ROM2 is LOCKED
 */
bool CFE_PSP_SP0ROM2Status(void);


/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_SP0_INFO_H_ */

