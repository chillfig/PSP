/**
 ** \file  psp_sp0_info.h
 **
 ** \brief API to collect and dump SP0 Hardware/Software information
 **
 ** \copyright
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description
 ** This file contains the function prototypes to access the SP0 hardware and 
 ** software data. It also dumps the same data to FLASH memory in the case when 
 ** cFS calls Panic.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

/**
 ** \brief SP0_TEXT_BUFFER_MAX_SIZE
 ** \par Description:
 ** This is the maximum size of the SP0 char array table.
 */
#define SP0_TEXT_BUFFER_MAX_SIZE    1000

/** \name SP0 Information String Buffer */
/** \{ */
/** \brief SP0 String Buffer */
char sp0_data_dump[SP0_TEXT_BUFFER_MAX_SIZE];
/** \brief Actual length of the string buffer */
int sp0_data_dump_length;
/** \} */

/**
** \name SP0 info structure
** \par Description:
** The table includes values that changes only once during boot and others
** that changes at a regular interval.
** 
** Variables that changes at regular intervals are:
** - systemStartupUsecTime
** - temperatures
** - voltages
*/
struct
{
    /** \brief Pointer to the string identifing the System Model */
    char * systemModel;
    /** \brief Pointer to the string identifing the system BSP Revision */
    char * systemBspRev;
    /** \brief Top of the System Physical Memmory */
    uint32 systemPhysMemTop;
    /** \brief Number of Processors */
    int systemProcNum;
    /** \brief Slod ID in the chassis */
    int systemSlotId;
    /** \brief Identifies if the SP0 is the cPCI main system controller */
    bool systemCpciSysCtrl;
    /** \brief System Core Clock Speed in MHz */
    uint32 systemCoreClockSpeed;
    /** \brief Reason for last SP0 computer reset */
    uint8 systemLastResetReason;
    /** \brief Identifies the EEPROM to successfully booted the kernel */
    uint8 active_boot;
    /** \brief System Clock Rate */
    int systemClkRateGet;
    /** \brief System Aux Clock Rate */
    int systemAuxClkRateGet;
    /** \brief Identifies the POST Test Bit Executed */
    uint64 bitExecuted;
    /** \brief Identifies the POST Test Results */
    uint64 bitResult;
    /** \brief Safe Mode User Data */
    char safeModeUserData[256];
    
    /** \brief Number of usec since startup */
    double systemStartupUsecTime;
    /** \brief Array of 4 temperatures on the SP0 computer */
    float temperatures[4];
    /** \brief Array of 6 voltages powering the SP0 */
    float voltages[6];
} sp0_info_table;


/**
** \func Collect SP0 Hardware and Firmware data
**
** \par Description:
** This function collects the SP0 hardware and firmware data and saves it
** in the sp0_info_table object, as well as a string in the sp0_data_dump object.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return #CFE_PSP_SUCCESS
*/
int32  getSP0Info(void);

/**
** \func Collect SP0 Hardware and Firmware data
**
** \par Description:
** This function prints the SP0 data to the output console.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void  printSP0_info_table(void);

/**
 ** \func Function dumps the collected data to file
 **
 ** \par Description:
 ** This function prints the SP0 data to the output console.
 ** Data is saved at #SP0_DATA_DUMP_FILEPATH
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return None
 **
 */
void psp_dump_data(void);
