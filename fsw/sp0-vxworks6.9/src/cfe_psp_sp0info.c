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
*   \file cfe_psp_sp0info.c
*
*   \brief API for collecting SP0(s) hardware and software information
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

/*
**  Include Files
*/

#include <stdio.h>
#include <ioLib.h>
#include <errno.h>
#include <vxWorks.h>
#include <float.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

/* For supporting REALTIME clock */
#include <timers.h>

/* Aitech BSP Specific */
#include <aimonUtil.h>
#include <sys950Lib.h>
#include <sysApi.h>
#include <scratchRegMap.h>
#include <bflashCt.h>
#include <tempSensor.h>

/* SP0 Hardware Registers Definitions */
#include <cfe_psp_sp0.h>

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_sp0info.h"

/*
** Static Function Declarations
*/
int32 CFE_PSP_SP0PrintToBuffer(void);
int32 CFE_PSP_SP0ROMXCmd(uint32_t uiCode);

/*
** Static Variables
*/
/** \name SP0 Information String Buffer */
/** \{ */
/** \brief SP0 String Buffer */
char g_cSP0DataDump[SP0_TEXT_BUFFER_MAX_SIZE];
/** \brief Actual length of the string buffer */
int32 g_iSP0DataDumpLength;
/** \} */

/**
 ** \brief SP0 Static Info Table
 */
CFE_PSP_SP0StaticInfoTable_t g_SP0StaticInfoTable;

/**
 ** \brief SP0 Dynamic Data Table
 */
CFE_PSP_SP0DynamicInfoTable_t g_SP0DynamicInfoTable;

/**********************************************************
 * 
 * Function: CFE_PSP_SP0GetStaticInfo
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SP0CollectStaticInfo(void)
{
    RESET_SRC_REG_ENUM          resetSrc = 0;
    USER_SAFE_MODE_DATA_STRUCT  safeModeUserData = {};
    int32                       iStatus = 0;
    uint64                      ulBitExecuted = 0ULL;
    uint64                      ulBitResult   = 0ULL;
    int32                       iRetChar = 0;
    int32                       iRet_code = CFE_PSP_SUCCESS;
    
    OS_printf(SP0_PRINT_SCOPE "Collecting Data\n");

    /* Reset the structure to remove stale data */
    memset(&g_SP0StaticInfoTable, 0x00, sizeof(g_SP0StaticInfoTable));
    g_iSP0DataDumpLength = 0;

    /*
    This routine returns the model name of the CPU board. The returned string 
    depends on the board model and CPU version being used, for example, "PPC Board".
    */
    g_SP0StaticInfoTable.systemModel = sysModel();

    /*
    This routine returns a pointer to a BSP version and revision number, for
    example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
    */
    g_SP0StaticInfoTable.systemBspRev = sysBspRev();

    /*
    This function returns the address of the first missing byte of memory, which
    indicates the top of memory. Normally, the user specifies the amount of 
    physical memory with the macro LOCAL_MEM_SIZE in config.h.
    */
    g_SP0StaticInfoTable.systemPhysMemTop = (cpuaddr) sysPhysMemTop();
    
    /*
    This routine returns the processor number for the CPU board, which is set 
    with sysProcNumSet( ).
    */
    g_SP0StaticInfoTable.systemProcNum = sysProcNumGet();

    /*
    Returns slot number of this card in the cPCI backplane
    */
    g_SP0StaticInfoTable.systemSlotId = sysGetSlotId();
    
    /*
    Returns true if the SP0 is the CPCI system controller. The hardware register
    read is done only once. This is so that after operation, in case the hardware 
    fails, we do not suddenly change our configuration.
    */
    g_SP0StaticInfoTable.systemCpciSysCtrl = isCpciSysController();
    
    /*
    Gets core clock speed in MHz
    Note: The Aitech defined setCoreClockSpeed() function actually does not
    work for SP0, so there is no way to change the core clock speed via software
    with the Aitech provided API.
    */
    g_SP0StaticInfoTable.systemCoreClockSpeed = getCoreClockSpeed();
    
    /*
    Gets last reset reason
    */
    iStatus = ReadResetSourceReg(&resetSrc,0);
    if (iStatus == OS_SUCCESS)
    {
        g_SP0StaticInfoTable.systemLastResetReason = (uint8) resetSrc;
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from ReadResetSourceReg()\n");
        iRet_code = CFE_PSP_ERROR;
    }
    
    /* This function reads and returns the user decodable safe mode reset data. */
    iStatus = ReadSafeModeUserData(&safeModeUserData,0);
    if (iStatus == OS_SUCCESS)
    {
        if (safeModeUserData.sbc == SM_LOCAL_SBC)
        {
            iRetChar = snprintf(g_SP0StaticInfoTable.safeModeUserData, 
                                SP0_SAFEMODEUSERDATA_BUFFER_SIZE, 
                                "{\"Safe mode\": %d, \"sbc\": \"%s\", \"reason\": %d, \"cause\": \"0x%08x\"}",
                                safeModeUserData.safeMode,
                                "LOCAL",
                                safeModeUserData.reason,
                                safeModeUserData.mckCause);
        }
        else
        {
            iRetChar = snprintf(g_SP0StaticInfoTable.safeModeUserData, 
                                SP0_SAFEMODEUSERDATA_BUFFER_SIZE, 
                                "{\"Safe mode\": %d, \"sbc\": \"%s\", \"reason\": %d, \"cause\": \"0x%08x\"}",
                                safeModeUserData.safeMode,
                                "REMOTE",
                                safeModeUserData.reason,
                                safeModeUserData.mckCause);
        }
        if ((iRetChar < 0) || (iRetChar >= SP0_SAFEMODEUSERDATA_BUFFER_SIZE))
        {
            OS_printf(SP0_PRINT_SCOPE "Could not save data in sp0 info table safeModeUserData\n");
            iRet_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        iRetChar = snprintf(g_SP0StaticInfoTable.safeModeUserData, 
                            SP0_SAFEMODEUSERDATA_BUFFER_SIZE, 
                            "OS_Error Retrieving SafeModeUserData");
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from ReadSafeModeUserData()\n");
        iRet_code = CFE_PSP_ERROR;
    }

    /*
    Returns the active boot EEPROM at the time of startup
    Boot device = 2 is returned when the board fails to boot at the first attempt
    and the (external) watchdog timer reboots the board successfully using the 
    alternative boot device.
    */
    g_SP0StaticInfoTable.active_boot = (uint8) returnSelectedBootFlash();

    /*
    This routine returns the system clock rate. The number of ticks per second 
    of the system tick timer.
    */
    g_SP0StaticInfoTable.systemClkRateGet = sysClkRateGet();

    /*
    This routine returns the interrupt rate of the auxiliary clock.
    The number of ticks per second of the auxiliary clock.
    */
    g_SP0StaticInfoTable.systemAuxClkRateGet = sysAuxClkRateGet();

    /* This function returns the list of the test executed by AIMON in a 64 bit packed word. */
    iStatus = aimonGetBITExecuted(&ulBitExecuted, 0);
    if (iStatus == OS_SUCCESS)
    {
        g_SP0StaticInfoTable.bitExecuted = ulBitExecuted;
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from aimonGetBITExecuted()\n");
        iRet_code = CFE_PSP_ERROR;
    }

    /* This function returns the summary of the test results in a 64 bit packed word */
    iStatus = aimonGetBITResults(&ulBitResult, 0);
    if (iStatus == OS_SUCCESS)
    {
        g_SP0StaticInfoTable.bitResult = ulBitResult;
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from aimonGetBITResults()\n");
        iRet_code = CFE_PSP_ERROR;
    }

    /* Get real time clock from OS */
    iStatus = clock_gettime(CLOCK_REALTIME, &g_SP0StaticInfoTable.lastUpdatedUTC);
    if (iStatus != OK)
    {
        OS_printf(SP0_PRINT_SCOPE "Error getting local time\n");
        iRet_code = CFE_PSP_ERROR;
    }

    /* Print data to string */
    if(CFE_PSP_SP0PrintToBuffer())
    {
        OS_printf(SP0_PRINT_SCOPE "Could not print to buffer. Data is left in the structure.");
        iRet_code = CFE_PSP_ERROR_LEVEL_0;
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0GetDynamicInfo
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SP0CollectDynamicInfo(void)
{
    int32   iStatus = 0;
    float   fTemperature = 0.0f;
    float   fVoltage = 0.0f;
    uint32  uiIndex = 0;
    uint8   ucMaxTempSensors = SP0_UPGRADE_MAX_TEMP_SENSORS;
    uint8   ucMaxVoltageSensors = SP0_UPGRADE_MAX_VOLT_SENSORS;
    int32   iRet_code = CFE_PSP_SUCCESS;

    /*
    Returns the time in micro-seconds since system startup. Rolls over after 3 years
    */
    g_SP0DynamicInfoTable.systemStartupUsecTime = (float) GetUsecTime();

    /*
    uiMaxTempSensors is set by default to the SP0_UPGRADE target 
    If it is the original target, update number of onboard temperature sensors
    */
    if (sysGetBoardGeneration(false) == SP0_ORIGINAL)
    {
        ucMaxTempSensors = SP0_ORIGINAL_MAX_TEMP_SENSORS;
    }
    /* Read temperature sensors */
    for (uiIndex = 0; uiIndex < ucMaxTempSensors; uiIndex++)
    {
        /* This function takes about 3msec each time it is called */
        iStatus = tempSensorRead(uiIndex, CURRENT_TEMP, &fTemperature, false);
        /* If temperature is read successfully, save on table */
        if (iStatus == OS_SUCCESS)
        {
            g_SP0DynamicInfoTable.temperatures[uiIndex] = fTemperature;
        }
        /* If temperature reading is unsuccessful, save lowest possible number to show error */
        else
        {
            OS_printf(SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n");
            g_SP0DynamicInfoTable.temperatures[uiIndex] = (float)FLT_MIN;
            iRet_code = CFE_PSP_ERROR;
        }
    }
    
    /**** Read SP0 Voltages ****/
    if (sysGetBoardGeneration(false) == SP0_UPGRADE)
    {
        /* Only SP0s DDR2 has Voltage Monitoring */
        for (uiIndex = 0; uiIndex < ucMaxVoltageSensors; uiIndex++)
        {
            /*
            Voltage sensor index is +1 since 0 is a special condition used only for 
            reading voltage statuses against the alarm. This is not implemented.
            */
           /* This function takes about 3msec each time it is called */
            iStatus = volSensorRead(uiIndex + 1, CURRENT_VOL, &fVoltage, false);
            /* If voltage is read successfully, save on table */
            if (iStatus == OS_SUCCESS)
            {
                g_SP0DynamicInfoTable.voltages[uiIndex] = fVoltage;
            }
            /* If voltage reading is unsuccessful, save lowest possible number to show error */
            else
            {
                OS_printf(SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n");
                g_SP0DynamicInfoTable.voltages[uiIndex] = (float)FLT_MIN;
                iRet_code = CFE_PSP_ERROR;
            }
        }
    }

    /* Get real time clock from OS */
    iStatus = clock_gettime(CLOCK_REALTIME, &g_SP0DynamicInfoTable.lastUpdatedUTC);
    if (iStatus != OK)
    {
        OS_printf(SP0_PRINT_SCOPE "Error getting local time\n");
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}

/**
 ** \func Print the SP0 data to string buffer
 ** 
 ** \par Description:
 ** Internal function to print the gathered data from SP0 to a string buffer.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SP0PrintToBuffer(void)
{
    uint32     uiTotalMemory_MiB = 0;
    char       *pActiveBootString = NULL;
    char       cActive_boot_primary[] = "PRIMARY";
    char       cActive_boot_secondary[] = "SECONDARY";
    int32      iRet_code = CFE_PSP_SUCCESS;
    
    /* Coverts bytes to Mibytes */
    uiTotalMemory_MiB = (g_SP0StaticInfoTable.systemPhysMemTop >> 20);

    if (g_SP0StaticInfoTable.active_boot == 1)
    {
        pActiveBootString = cActive_boot_primary;
    }
    else
    {
        pActiveBootString = cActive_boot_secondary;
    }

    /* Output to local string buffer */
    g_iSP0DataDumpLength = snprintf(
        g_cSP0DataDump, 
        SP0_TEXT_BUFFER_MAX_SIZE,
        "UTC sec: %u\n"
        "SysModel: %s\nSysBspRev: %s\n"
        "SysPhysMemTop: 0x%08X (%u MiB)\n"
        "sysProcNum: %d\n"
        "sysSlotId: %d\n"
        "sysStartupUsecTime: %0.3f\n"
        "sysCpciSysCtrl: %d\n"
        "sysCoreClockSpeed: %u [MHz]\n"
        "sysLastResetReason: %u\n"
        "active_boot: %u '%s'\n"
        "sysClkRateGet: %d [Ticks/sec]\n"
        "sysAuxClkRateGet: %d [Ticks/sec]\n"
        "bitExecuted: 0x%016llX\n"
        "bitResult: 0x%016llX\n"
        "TempLeft:  %.3f [C]\n"
        "TempRight: %.3f [C]\n"
        "TempCPU:   %.3f [C]\n"
        "TempCore:  %.3f [C]\n"
        "V0P9: %.3f [Volts]\n"
        "V1P1: %.3f [Volts]\n"
        "V1P5: %.3f [Volts]\n"
        "V1P8: %.3f [Volts]\n"
        "V2P5: %.3f [Volts]\n"
        "V3P3: %.3f [Volts]\n",
        g_SP0DynamicInfoTable.lastUpdatedUTC.tv_sec,
        g_SP0StaticInfoTable.systemModel,
        g_SP0StaticInfoTable.systemBspRev,
        g_SP0StaticInfoTable.systemPhysMemTop,
        uiTotalMemory_MiB,
        g_SP0StaticInfoTable.systemProcNum,
        g_SP0StaticInfoTable.systemSlotId,
        g_SP0DynamicInfoTable.systemStartupUsecTime,
        g_SP0StaticInfoTable.systemCpciSysCtrl,
        g_SP0StaticInfoTable.systemCoreClockSpeed,
        g_SP0StaticInfoTable.systemLastResetReason,
        g_SP0StaticInfoTable.active_boot,
        pActiveBootString,
        g_SP0StaticInfoTable.systemClkRateGet,
        g_SP0StaticInfoTable.systemAuxClkRateGet,
        g_SP0StaticInfoTable.bitExecuted,
        g_SP0StaticInfoTable.bitResult,
        g_SP0DynamicInfoTable.temperatures[0],
        g_SP0DynamicInfoTable.temperatures[1],
        g_SP0DynamicInfoTable.temperatures[2],
        g_SP0DynamicInfoTable.temperatures[3],
        g_SP0DynamicInfoTable.voltages[0],
        g_SP0DynamicInfoTable.voltages[1],
        g_SP0DynamicInfoTable.voltages[2],
        g_SP0DynamicInfoTable.voltages[3],
        g_SP0DynamicInfoTable.voltages[4],
        g_SP0DynamicInfoTable.voltages[5]
    );

    /*
    If there was an error in writing using snprintf, then what we have in 
    g_cSP0DataDump is garbage 
    */
    if (g_iSP0DataDumpLength < 0)
    {
        /* This will make sure that we don't write garbage to file */
        g_iSP0DataDumpLength = -1;
        iRet_code = CFE_PSP_ERROR;
    }
    /* Check if it was truncated */
    if (g_iSP0DataDumpLength >= SP0_TEXT_BUFFER_MAX_SIZE)
    {
        /* Return error but don't do anything else. */
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0GetDynamicInfoTable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SP0GetDynamicInfoTable(
                                    CFE_PSP_SP0DynamicInfoTable_t *pDynamic,
                                    size_t iDynamicSize,
                                    uint8_t print_to_console
                                    )
{
    int32   iRetCode = CFE_PSP_SUCCESS;

    /* Return error if the pointer is NULL */
    if (pDynamic == NULL)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    /* Return error if the size of the memory pointed by pDynamic is not large enough */
    if (sizeof(g_SP0DynamicInfoTable) > iDynamicSize)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    if (iRetCode == CFE_PSP_SUCCESS)
    {
        /* Copy internal structure */
        memcpy(pDynamic, &g_SP0DynamicInfoTable, sizeof(g_SP0DynamicInfoTable));

        /*
        OS_printf function cannot print more than OS_BUFFER_SIZE and g_cSP0DataDump
        is usually much longer.
        */
        if (g_iSP0DataDumpLength > 0)
        {
            if (print_to_console > 0)
            {
                // UndCC_NextLine(SSET134)
                printf("\n\n%s\n\n", &g_cSP0DataDump);
            }
        }
    }

    return iRetCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0GetStaticInfoTable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SP0GetStaticInfoTable(CFE_PSP_SP0StaticInfoTable_t *pStatic, size_t iStaticSize, uint8_t print_to_console)
{
    int32   iRetCode = CFE_PSP_SUCCESS;

    /* Return error if the pointer is NULL */
    if (pStatic == NULL)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    /* Return error if the size of the memory pointed by pStatic is not large enough */
    if (sizeof(CFE_PSP_SP0StaticInfoTable_t) > iStaticSize)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    if (iRetCode == CFE_PSP_SUCCESS)
    {
        /* Copy internal structure */
        memcpy(pStatic, &g_SP0StaticInfoTable, sizeof(g_SP0StaticInfoTable));
    
        /*
        OS_printf function cannot print more than OS_BUFFER_SIZE and g_cSP0DataDump
        is usually much longer.
        */
        if (g_iSP0DataDumpLength > 0)
        {
            if (print_to_console > 0)
            {
                // UndCC_NextLine(SSET134)
                printf("\n\n%s\n\n", &g_cSP0DataDump);
            }
        }
    }
    return iRetCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0DumpData
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SP0DumpData(void)
{
    char    cFilename[] = CFE_PSP_SP0_DATA_DUMP_FILEPATH;
    int32   iFD = -1;
    ssize_t iBytes = 0;
    int32   iStatus = OK;
    int32   iRet_code = CFE_PSP_SUCCESS;

    /* Save the SP0 info dump only if the data available */
    if (g_iSP0DataDumpLength > 0)
    {
        /* Delete previous dump file if exists */
        iStatus = remove(cFilename);
        errno = 0;
        /* Create a new file for writing */
        iFD = open(cFilename, O_CREAT | O_RDWR, 0644);
        /* If there are no errors */
        if (iFD > -1)
        {
            /* Write data to file */
            iBytes = write(iFD, g_cSP0DataDump, (size_t) g_iSP0DataDumpLength);
            /* If there was an error during writing, let the user know and continue */
            if (iBytes != g_iSP0DataDumpLength)
            {
                OS_printf(SP0_PRINT_SCOPE "Error while writing SP0 info data dump to file. errno=`%d`\n",errno);
                iRet_code = CFE_PSP_ERROR;
            }
            /* Close file */
            iStatus = close(iFD);
            /* If there was an error during closing, let the user know and continue */
            if (iStatus != OK)
            {
                OS_printf(SP0_PRINT_SCOPE "Could not close file SP0 info data dump. errno=`%d`\n",errno);
                iRet_code = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf(SP0_PRINT_SCOPE "Could not create the SP0 info file dump. errno=`%d`\n",errno);
            iRet_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Data Dump has not been initialized or error occurred\n");
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0GetDiskFreeSize
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int64_t CFE_PSP_SP0GetDiskFreeSize(char *ram_disk_root_path)
{
    int64_t lFree_size_bytes = CFE_PSP_ERROR;
    long block_size = 0;
    int64_t lBlocks_available = 0;
    struct statfs ram_disk_stats = {'\0'};

    if ((ram_disk_root_path != NULL) && memchr(ram_disk_root_path, (cpuaddr) NULL, CFE_PSP_MAXIMUM_TASK_LENGTH))
    {
        if (statfs(ram_disk_root_path, &ram_disk_stats) == OK)
        {
            block_size = ram_disk_stats.f_bsize;
            lBlocks_available = ram_disk_stats.f_bavail;
            lFree_size_bytes = block_size * lBlocks_available;
        }
    }
    return lFree_size_bytes;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0ROM1Lock
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_SP0ROM1Lock(void)
{
    return CFE_PSP_SP0ROMXCmd((uint32_t)SP0_ROM1_CODE_LOCK);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0ROM1Unlock
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_SP0ROM1Unlock(void)
{
    return CFE_PSP_SP0ROMXCmd((uint32_t)SP0_ROM1_CODE_UNLOCK);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0ROM2Lock
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_SP0ROM2Lock(void)
{
    return CFE_PSP_SP0ROMXCmd((uint32_t)SP0_ROM2_CODE_LOCK);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0ROM2Unlock
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_SP0ROM2Unlock(void)
{
    return CFE_PSP_SP0ROMXCmd((uint32_t)SP0_ROM2_CODE_UNLOCK);
}

/**
 ** \brief Executes the base commands to lock/unlock ROM device
 **
 ** \par Description:
 ** Executes commands to lock/unlock ROM device
 **
 ** \par Assumptions, Notes, and External Events:
 ** We first must write a series of 8-bit values in sequence
 ** to unlock/lock the BOOT ROM Write Protect. The first 
 ** five writes in the sequence are executed, then the last remaining
 ** write (handled by switch/case) will complete the sequence and
 ** lock/unlock the BOOT ROM as intended. See attachment on
 ** GWCFS-1226 Jira Story
 **
 ** \par Code coverage rationale: Can not stub out hardware register to
 ** to test off nominal cases.    
 **
 ** \param[in] uiCode - Code to indicate lock/unlock ROM1/2
 **
 ** \return #CFE_PSP_SUCCESS - Successfully executed intended sequence
 ** \return #CFE_PSP_ERROR - Unsuccessfully executed intended sequence
 */
int32 CFE_PSP_SP0ROMXCmd(uint32_t uiCode)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000000;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x000000AA;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000055;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000080;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x000000A5;
    
    switch (uiCode)
    {
        case SP0_ROM1_CODE_LOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM1_CODE_LOCK;
            if (CFE_PSP_SP0ROM1Status() == true)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;
        
        case SP0_ROM1_CODE_UNLOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM1_CODE_UNLOCK;
            if (CFE_PSP_SP0ROM1Status() == false)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;

        case SP0_ROM2_CODE_LOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM2_CODE_LOCK;
            if (CFE_PSP_SP0ROM2Status() == true)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;
        
        case SP0_ROM2_CODE_UNLOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM2_CODE_UNLOCK;
            if (CFE_PSP_SP0ROM2Status() == false)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;
        
        default:
            /* Execute Reset */
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000000;
            break;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0ROM1Status
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
bool CFE_PSP_SP0ROM1Status(void)
{
    return (bool) (((*(uint32 *)SP0_BOOT_ROM_STATUS_ADDR) & SP0_ROM1_MASK) >> SP0_ROM1_STATUS_SHIFT);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0ROM2Status
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
bool CFE_PSP_SP0ROM2Status(void)
{
    return (bool) (((*(uint32 *)SP0_BOOT_ROM_STATUS_ADDR) & SP0_ROM2_MASK) >> SP0_ROM2_STATUS_SHIFT);
}
