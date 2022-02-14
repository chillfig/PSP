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
#define SP0_UPGRADE_MAX_VOLTAGE_SENSORS     6
#define SP0_ORIGINAL_MAX_VOLTAGE_SENSORS    0
#define SP0_UPGRADE_MAX_TEMP_SENSORS        4
#define SP0_ORIGINAL_MAX_TEMP_SENSORS       3

/*
** Static Variables
*/
/** \name SP0 Information String Buffer */
/** \{ */
/** \brief SP0 String Buffer */
static char g_cSP0DataDump[SP0_TEXT_BUFFER_MAX_SIZE];
/** \brief Actual length of the string buffer */
static int g_iSP0DataDumpLength;
/** \} */

/**
 ** \brief SP0 Data Table
 */
static SP0_info_table_t g_sp0_info_table;

/**
** \func Collect SP0 Hardware and Firmware data
**
** \par Description:
** This function collects the SP0 hardware and firmware data and saves it
** in the g_sp0_info_table object, as well as a string in the g_cSP0DataDump object.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 PSP_SP0_GetInfo(void)
{
    RESET_SRC_REG_ENUM          resetSrc = 0;
    USER_SAFE_MODE_DATA_STRUCT  safeModeUserData = {};
    int32                       status = 0;
    float                       fTemperature = 0.0f;
    float                       fVoltage = 0.0f;
    int8                        i = 0;
    uint8                       uiMaxTempSensors = SP0_UPGRADE_MAX_TEMP_SENSORS;
    uint8                       uiMaxVoltageSensors = SP0_UPGRADE_MAX_VOLTAGE_SENSORS;
    uint64                      bitExecuted = 0ULL;
    uint64                      bitResult   = 0ULL;
    int                         iRetChar = 0;
    int32                       ret_code = CFE_PSP_SUCCESS;
    
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
    status = ReadResetSourceReg(&resetSrc,0);
    if (status == OS_SUCCESS)
    {
        g_sp0_info_table.systemLastResetReason = (uint8) resetSrc;
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from ReadResetSourceReg()\n");
        ret_code = CFE_PSP_ERROR;
    }
    
    /* Do I need this: ReadSafeModeUserData() */
    status = ReadSafeModeUserData(&safeModeUserData,0);
    if (status == OS_SUCCESS)
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
            OS_printf(SP0_PRINT_SCOPE "Could not save data in sp0 info table safeModeUserData");
            ret_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        iRetChar = snprintf(g_sp0_info_table.safeModeUserData, 
                            SP0_SAFEMODEUSERDATA_BUFFER_SIZE, 
                            "OS_Error Retrieving SafeModeUserData");
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from ReadSafeModeUserData()\n");
        ret_code = CFE_PSP_ERROR;
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
        uiMaxTempSensors = SP0_ORIGINAL_MAX_TEMP_SENSORS;
    }
    /* Read temperature sensors */
    for (i = 0; i < uiMaxTempSensors; i++)
    {
        /* This function takes about 3msec each time it is called */
        status = tempSensorRead(i, CURRENT_TEMP, &fTemperature, false);
        /* If temperature is read successfully, save on table */
        if (status == OS_SUCCESS)
        {
            g_sp0_info_table.temperatures[i] = fTemperature;
        }
        /* If temperature reading is unsuccessful, save lowest possible number to show error */
        else
        {
            OS_printf(SP0_PRINT_SCOPE "Error collecting data from tempSensorRead()\n");
            g_sp0_info_table.temperatures[i] = (float)FLT_MIN;
            ret_code = CFE_PSP_ERROR;
        }
    }
    
    /**** Read SP0 Voltages ****/
    if (sysGetBoardGeneration(FALSE) == SP0_UPGRADE)
    {
        /* Only SP0s DDR2 has Voltage Monitoring */
        for (i = 0; i < uiMaxVoltageSensors; i++)
        {
            /*
            Voltage sensor index is +1 since 0 is a special condition used only for 
            reading voltage statuses against the alarm. This is not implemented.
            */
           /* This function takes about 3msec each time it is called */
            status = volSensorRead(i + 1, CURRENT_VOL, &fVoltage, false);
            /* If voltage is read successfully, save on table */
            if (status == OS_SUCCESS)
            {
                g_sp0_info_table.voltages[i] = fVoltage;
            }
            /* If voltage reading is unsuccessful, save lowest possible number to show error */
            else
            {
                OS_printf(SP0_PRINT_SCOPE "Error collecting data from volSensorRead()\n");
                g_sp0_info_table.voltages[i] = (float)FLT_MIN;
                ret_code = CFE_PSP_ERROR;
            }
        }
    }

    /* This function returns the list of the test executed by AIMON in a 64 bit packed word. */
    status = aimonGetBITExecuted(&bitExecuted, 0);
    if (status == OS_SUCCESS)
    {
        memcpy(&g_sp0_info_table.bitExecuted, &bitExecuted, sizeof(g_sp0_info_table.bitExecuted));
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from aimonGetBITExecuted()\n");
        ret_code = CFE_PSP_ERROR;
    }

    /* This function returns the summary of the test results in a 64 bit packed word */
    status = aimonGetBITResults(&bitResult, 0);
    if (status == OS_SUCCESS)
    {
        memcpy(&g_sp0_info_table.bitResult, &bitResult, sizeof(g_sp0_info_table.bitResult));
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Error collecting data from aimonGetBITResults()\n");
        ret_code = CFE_PSP_ERROR;
    }

    /* Get real time clock from OS */
    status = clock_gettime(CLOCK_REALTIME, &g_sp0_info_table.lastUpdatedUTC);
    if (status != OK)
    {
        OS_printf(SP0_PRINT_SCOPE "Error getting local time\n");
        ret_code = CFE_PSP_ERROR;
    }
    
    /* Print data to string */
    if(PSP_SP0_PrintToBuffer())
    {
        OS_printf(SP0_PRINT_SCOPE "Could not print to buffer. Data is left in the structure.");
        ret_code = CFE_PSP_ERROR_LEVEL_0;
    }

    return ret_code;
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
    char       active_boot_primary[] = "PRIMARY";
    char       active_boot_secondary[] = "SECONDARY";
    int32      ret_code = CFE_PSP_SUCCESS;
    
    /* Coverts bytes to Mibytes */
    uiTotalMemory_MiB = (g_sp0_info_table.systemPhysMemTop >> 20);

    if (g_sp0_info_table.active_boot == 1)
    {
        pActiveBootString = active_boot_primary;
    }
    else
    {
        pActiveBootString = active_boot_secondary;
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
        ret_code = CFE_PSP_ERROR;
    }
    /* Check if it was truncated */
    if (g_iSP0DataDumpLength >= SP0_TEXT_BUFFER_MAX_SIZE)
    {
        /* Return error but don't do anything else. */
        ret_code = CFE_PSP_ERROR;
    }

    return ret_code;
}

/**
** \func Get the structure containing the SP0 Hardware and Firmware data
**
** \par Description:
** This function returns and print the structure containing the SP0 Hardware and Firmware
** data.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param print_to_console Print string buffer to console if True
**
** \return SP0_info_table_t structure containing all the collect info from SP0
*/
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
int32 PSP_SP0_DumpData(void)
{
    char    filename[] = SP0_DATA_DUMP_FILEPATH;
    int     fd = -1;
    ssize_t iBytes = 0;
    int     iStatus = OK;
    int32   ret_code = CFE_PSP_SUCCESS;

    /* Save the SP0 info dump only if the data available */
    if (g_iSP0DataDumpLength > 0)
    {
        /* Delete previous dump file if exists */
        iStatus = remove(filename);

        /* Create a new file for writing */
        fd = open(filename, O_CREAT | O_RDWR, 0644);
        /* If there are no errors */
        if (fd != ERROR) //UndCC_Line(SSET055) - returned by function
        {
            /* Write data to file */
            iBytes = write(fd, g_cSP0DataDump, (size_t) g_iSP0DataDumpLength);
            /* If there was an error during writing, let the user know and continue */
            if (iBytes == ERROR) //UndCC_Line(SSET055) - returned by function
            {
                OS_printf(SP0_PRINT_SCOPE "Error while writing SP0 info data dump to file\n");
                ret_code = CFE_PSP_ERROR;
            }
            /* Close file */
            iStatus = close(fd);
            /* If there was an error during closing, let the user know and continue */
            if (iStatus == ERROR) //UndCC_Line(SSET055) - returned by function
            {
                OS_printf(SP0_PRINT_SCOPE "Could not close file SP0 info data dump\n");
                ret_code = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf(SP0_PRINT_SCOPE "Could not create the SP0 info file dump\n");
            ret_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(SP0_PRINT_SCOPE "Data Dump has not been initialized or error occured\n");
        ret_code = CFE_PSP_ERROR;
    }

PSP_SP0_DumpData_Exit_TAG:
    return ret_code;
}

/**
 ** \func Get disk free disk space in Mibytes
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
int64_t PSP_SP0_GetDiskFreeSize(char *ram_disk_root_path)
{
    int64_t free_size_bytes = CFE_PSP_ERROR;
    long block_size = 0;
    int64_t blocks_available = 0;
    struct statfs ram_disk_stats = {};

    if ((ram_disk_root_path != NULL) && memchr(ram_disk_root_path, (int) NULL, CFE_PSP_MAXIMUM_TASK_LENGTH))
    {
        if (statfs(ram_disk_root_path, &ram_disk_stats) == OK)
        {
            block_size = ram_disk_stats.f_bsize;
            blocks_available = ram_disk_stats.f_bavail;
            free_size_bytes = block_size * blocks_available;
        }
    }
    return free_size_bytes;
}
