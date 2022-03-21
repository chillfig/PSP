/**
 ** \file cfe_psp_sp0_info.c
 **
 ** \brief API for collecting SP0(s) hardware and software information
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** Functions here allow CFS to provide a method to probe 
 ** SP0 hardware for information from POST, Temperatures, Voltages, Active 
 ** Boot EEPROM, etc. In addition, this module has a function to save a 
 ** dump_core text file before aborting CFS execution. 
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

/*
**  Include Files
*/

#include <fcntl.h>
#include <stdio.h>
#include <ioLib.h>
#include <errno.h>
#include <vxWorks.h>
#include <float.h>
#include <stat.h>

/* For supporting REALTIME clock */
#include <timers.h>

/* Aitech BSP Specific */
#include <aimonUtil.h>
#include <sys950Lib.h>
#include <sysApi.h>
#include <scratchRegMap.h>
#include <bflashCt.h>
#include <tempSensor.h>

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "psp_sp0_info.h"

/**
 ** \brief Max number of Voltage and Temperature sensors per target generation
 ** 
 */
#define SP0_UPGRADE_MAX_VOLT_SENSORS    6
#define SP0_ORIGINAL_MAX_VOLT_SENSORS   0
#define SP0_UPGRADE_MAX_TEMP_SENSORS    4
#define SP0_ORIGINAL_MAX_TEMP_SENSORS   3

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

/*
** Static Function Declarations
*/
static int32 PSP_SP0_PrintToBuffer(void);
static int32 PSP_SP0_ROMX_COMMAND(uint32_t uiCode);

/*
** Static Variables
*/
/** \name SP0 Information String Buffer */
/** \{ */
/** \brief SP0 String Buffer */
static char g_cSP0DataDump[SP0_TEXT_BUFFER_MAX_SIZE];
/** \brief Actual length of the string buffer */
static int32 g_iSP0DataDumpLength;
/** \} */

/**
 ** \brief SP0 Data Table
 */
static SP0_info_table_t g_sp0_info_table;

/**********************************************************
 * 
 * Function: PSP_SP0_GetInfo
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 PSP_SP0_GetInfo(void)
{
    RESET_SRC_REG_ENUM          resetSrc = 0;
    USER_SAFE_MODE_DATA_STRUCT  safeModeUserData = {};
    int32                       iStatus = 0;
    float                       fTemperature = 0.0f;
    float                       fVoltage = 0.0f;
    uint32                      uiIndex = 0;
    uint8                       ucMaxTempSensors = SP0_UPGRADE_MAX_TEMP_SENSORS;
    uint8                       ucMaxVoltageSensors = SP0_UPGRADE_MAX_VOLT_SENSORS;
    uint64                      ulBitExecuted = 0ULL;
    uint64                      ulBitResult   = 0ULL;
    int32                       iRetChar = 0;
    int32                       iRet_code = CFE_PSP_SUCCESS;
    
    OS_printf(SP0_PRINT_SCOPE "Collecting Data\n");

    /* Reset the structure to remove stale data */
    memset(&g_sp0_info_table, 0x00, sizeof(g_sp0_info_table));
    g_iSP0DataDumpLength = 0;

    /*
    This routine returns the model name of the CPU board. The returned string 
    depends on the board model and CPU version being used, for example, "PPC Board".
    */
    g_sp0_info_table.systemModel = sysModel();

    /*
    This routine returns a pointer to a BSP version and revision number, for
    example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
    */
    g_sp0_info_table.systemBspRev = sysBspRev();

    /*
    This function returns the address of the first missing byte of memory, which
    indicates the top of memory. Normally, the user specifies the amount of 
    physical memory with the macro LOCAL_MEM_SIZE in config.h.
    */
    g_sp0_info_table.systemPhysMemTop = (uint32) sysPhysMemTop();
    
    /*
    This routine returns the processor number for the CPU board, which is set 
    with sysProcNumSet( ).
    */
    g_sp0_info_table.systemProcNum = sysProcNumGet();

    /*
    Returns slot number of this card in the cPCI backplane
    */
    g_sp0_info_table.systemSlotId = sysGetSlotId();

    /*
    Returns the time in micro-seconds since system startup. Rolls over after 3 years
    */
    g_sp0_info_table.systemStartupUsecTime = (float) GetUsecTime();
    
    /*
    Returns true if the SP0 is the CPCI system controller. The hardware register
    read is done only once. This is so that after operation, in case the hardware 
    fails, we do not suddenly change our configuration.
    */
    g_sp0_info_table.systemCpciSysCtrl = isCpciSysController();
    
    /*
    Gets core clock speed in MHz
    Note: The Aitech defined setCoreClockSpeed() function actually does not
    work for SP0, so there is no way to change the core clock speed via software
    with the Aitech provided API.
    */
    g_sp0_info_table.systemCoreClockSpeed = getCoreClockSpeed();
    
    /*
    Gets last reset reason
    */
    iStatus = ReadResetSourceReg(&resetSrc,0);
    if (iStatus == OS_SUCCESS)
    {
        g_sp0_info_table.systemLastResetReason = (uint8) resetSrc;
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from ReadResetSourceReg()\n");
        iRet_code = CFE_PSP_ERROR;
    }
    
    /* Do I need this: ReadSafeModeUserData() */
    iStatus = ReadSafeModeUserData(&safeModeUserData,0);
    if (iStatus == OS_SUCCESS)
    {
        if (safeModeUserData.sbc == SM_LOCAL_SBC)
        {
            iRetChar = snprintf(g_sp0_info_table.safeModeUserData, 
                                SP0_SAFEMODEUSERDATA_BUFFER_SIZE, 
                                "{\"Safe mode\": %d, \"sbc\": \"%s\", \"reason\": %d, \"cause\": \"0x%08x\"}",
                                safeModeUserData.safeMode,
                                "LOCAL",
                                safeModeUserData.reason,
                                safeModeUserData.mckCause);
        }
        else
        {
            iRetChar = snprintf(g_sp0_info_table.safeModeUserData, 
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
        iRetChar = snprintf(g_sp0_info_table.safeModeUserData, 
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
    g_sp0_info_table.active_boot = (uint8) returnSelectedBootFlash();

    /*
    This routine returns the system clock rate. The number of ticks per second 
    of the system tick timer.
    */
    g_sp0_info_table.systemClkRateGet = sysClkRateGet();

    /*
    This routine returns the interrupt rate of the auxiliary clock.
    The number of ticks per second of the auxiliary clock.
    */
    g_sp0_info_table.systemAuxClkRateGet = sysAuxClkRateGet();

    /*
    uiMaxTempSensors is set by default to the SP0_UPGRADE target 
    If it is the original target, update number of onboard temperature sensors
    */
    if (sysGetBoardGeneration(FALSE) == SP0_ORIGINAL)
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
            g_sp0_info_table.temperatures[uiIndex] = fTemperature;
        }
        /* If temperature reading is unsuccessful, save lowest possible number to show error */
        else
        {
            OS_printf(SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n");
            g_sp0_info_table.temperatures[uiIndex] = (float)FLT_MIN;
            iRet_code = CFE_PSP_ERROR;
        }
    }
    
    /**** Read SP0 Voltages ****/
    if (sysGetBoardGeneration(FALSE) == SP0_UPGRADE)
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
                g_sp0_info_table.voltages[uiIndex] = fVoltage;
            }
            /* If voltage reading is unsuccessful, save lowest possible number to show error */
            else
            {
                OS_printf(SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n");
                g_sp0_info_table.voltages[uiIndex] = (float)FLT_MIN;
                iRet_code = CFE_PSP_ERROR;
            }
        }
    }

    /* This function returns the list of the test executed by AIMON in a 64 bit packed word. */
    iStatus = aimonGetBITExecuted(&ulBitExecuted, 0);
    if (iStatus == OS_SUCCESS)
    {
        memcpy(&g_sp0_info_table.bitExecuted, &ulBitExecuted, sizeof(g_sp0_info_table.bitExecuted));
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
        memcpy(&g_sp0_info_table.bitResult, &ulBitResult, sizeof(g_sp0_info_table.bitResult));
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from aimonGetBITResults()\n");
        iRet_code = CFE_PSP_ERROR;
    }

    /* Get real time clock from OS */
    iStatus = clock_gettime(CLOCK_REALTIME, &g_sp0_info_table.lastUpdatedUTC);
    if (iStatus != OK)
    {
        OS_printf(SP0_PRINT_SCOPE "Error getting local time\n");
        iRet_code = CFE_PSP_ERROR;
    }
    
    /* Print data to string */
    if(PSP_SP0_PrintToBuffer())
    {
        OS_printf(SP0_PRINT_SCOPE "Could not print to buffer. Data is left in the structure.");
        iRet_code = CFE_PSP_ERROR_LEVEL_0;
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
static int32 PSP_SP0_PrintToBuffer(void)
{
    uint32     uiTotalMemory_MiB = 0;
    char       *pActiveBootString = NULL;
    char       cActive_boot_primary[] = "PRIMARY";
    char       cActive_boot_secondary[] = "SECONDARY";
    int32      iRet_code = CFE_PSP_SUCCESS;
    
    /* Coverts bytes to Mibytes */
    uiTotalMemory_MiB = (g_sp0_info_table.systemPhysMemTop >> 20);

    if (g_sp0_info_table.active_boot == 1)
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
        g_sp0_info_table.lastUpdatedUTC.tv_sec,
        g_sp0_info_table.systemModel,
        g_sp0_info_table.systemBspRev,
        g_sp0_info_table.systemPhysMemTop,
        uiTotalMemory_MiB,
        g_sp0_info_table.systemProcNum,
        g_sp0_info_table.systemSlotId,
        g_sp0_info_table.systemStartupUsecTime,
        g_sp0_info_table.systemCpciSysCtrl,
        g_sp0_info_table.systemCoreClockSpeed,
        g_sp0_info_table.systemLastResetReason,
        g_sp0_info_table.active_boot,
        pActiveBootString,
        g_sp0_info_table.systemClkRateGet,
        g_sp0_info_table.systemAuxClkRateGet,
        g_sp0_info_table.bitExecuted,
        g_sp0_info_table.bitResult,
        g_sp0_info_table.temperatures[0],
        g_sp0_info_table.temperatures[1],
        g_sp0_info_table.temperatures[2],
        g_sp0_info_table.temperatures[3],
        g_sp0_info_table.voltages[0],
        g_sp0_info_table.voltages[1],
        g_sp0_info_table.voltages[2],
        g_sp0_info_table.voltages[3],
        g_sp0_info_table.voltages[4],
        g_sp0_info_table.voltages[5]
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
 * Function: PSP_SP0_GetInfoTable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
SP0_info_table_t PSP_SP0_GetInfoTable(bool print_to_console)
{
    /* Output to console */

    /*
    OS_printf function cannot print more than OS_BUFFER_SIZE and g_cSP0DataDump
    is usually much longer.
    */
    if (print_to_console)
    {
        if (g_iSP0DataDumpLength > 0)
        {
            // UndCC_NextLine(SSET134)
            printf("\n\n%s\n\n", &g_cSP0DataDump);
        }
    }

    return g_sp0_info_table;
}

/**********************************************************
 * 
 * Function: PSP_SP0_DumpData
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 PSP_SP0_DumpData(void)
{
    char    cFilename[] = SP0_DATA_DUMP_FILEPATH;
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
        OS_printf(SP0_PRINT_SCOPE "Data Dump has not been initialized or error occured\n");
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: PSP_SP0_GetDiskFreeSize
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int64_t PSP_SP0_GetDiskFreeSize(char *ram_disk_root_path)
{
    int64_t lFree_size_bytes = CFE_PSP_ERROR;
    long block_size = 0;
    int64_t lBlocks_available = 0;
    struct statfs ram_disk_stats = {'\0'};

    if ((ram_disk_root_path != NULL) && memchr(ram_disk_root_path, (int) NULL, CFE_PSP_MAXIMUM_TASK_LENGTH))
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
 * Function: PSP_SP0_ROM1_LOCK
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 PSP_SP0_ROM1_LOCK(void)
{
    return PSP_SP0_ROMX_COMMAND((uint32_t)SP0_ROM1_CODE_LOCK);
}

/**********************************************************
 * 
 * Function: PSP_SP0_ROM1_UNLOCK
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 PSP_SP0_ROM1_UNLOCK(void)
{
    return PSP_SP0_ROMX_COMMAND((uint32_t)SP0_ROM1_CODE_UNLOCK);
}

/**********************************************************
 * 
 * Function: PSP_SP0_ROM2_LOCK
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 PSP_SP0_ROM2_LOCK(void)
{
    return PSP_SP0_ROMX_COMMAND((uint32_t)SP0_ROM2_CODE_LOCK);
}

/**********************************************************
 * 
 * Function: PSP_SP0_ROM2_UNLOCK
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 PSP_SP0_ROM2_UNLOCK(void)
{
    return PSP_SP0_ROMX_COMMAND((uint32_t)SP0_ROM2_CODE_UNLOCK);
}

/*
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
** \param[in] uiCode - Code to indicate lock/unlock ROM1/2
**
** \return #CFE_PSP_SUCCESS - Successfully executed intended sequence
** \return #CFE_PSP_ERROR - Unsuccessfully executed intended sequence
*/
static int32 PSP_SP0_ROMX_COMMAND(uint32_t uiCode)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000000;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x000000AA;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000055;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x00000080;
    *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)0x000000A5;
    
    switch(uiCode)
    {
        case SP0_ROM1_CODE_LOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM1_CODE_LOCK;
            if (PSP_SP0_ROM1_Status() == true)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;
        
        case SP0_ROM1_CODE_UNLOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM1_CODE_UNLOCK;
            if (PSP_SP0_ROM1_Status() == false)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;

        case SP0_ROM2_CODE_LOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM2_CODE_LOCK;
            if (PSP_SP0_ROM2_Status() == true)
            {
                iReturnCode = CFE_PSP_SUCCESS;
            }
            break;
        
        case SP0_ROM2_CODE_UNLOCK:
            *(uint32_t *)SP0_BOOT_ROM_STATUS_ADDR = (uint32_t)SP0_ROM2_CODE_UNLOCK;
            if (PSP_SP0_ROM2_Status() == false)
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
 * Function: PSP_SP0_ROM1_Status
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
bool PSP_SP0_ROM1_Status(void)
{
    return (bool) (((*(uint32 *)SP0_BOOT_ROM_STATUS_ADDR) & SP0_ROM1_MASK) >> SP0_ROM1_STATUS_SHIFT);
}

/**********************************************************
 * 
 * Function: PSP_SP0_ROM2_Status
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
bool PSP_SP0_ROM2_Status(void)
{
    return (bool) (((*(uint32 *)SP0_BOOT_ROM_STATUS_ADDR) & SP0_ROM2_MASK) >> SP0_ROM2_STATUS_SHIFT);
}

/**********************************************************
 * 
 * MATT TEST AREA
 *      END
 *
 *********************************************************/
