/******************************************************************************
**
** File:  cfe_psp_support.c
**
**      Copyright (c) 2004-2011, United States Government as represented by
**      Administrator for The National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support
**   Package (BSP). The functions here allow CFS to provide a method to probe 
**   SP0 hardware for information from POST, Temperatures, Voltages, Active 
**   Boot EEPROM, etc. In addition, this module has a function to save a 
**   dump_core text file before aborting CFS execution. 
**   
**
******************************************************************************/

/*
**  Include Files
*/
#include "cfe_psp.h"
#include <fcntl.h>
#include <stdio.h>
#include "ioLib.h"
#include <vxWorks.h>

/* BSP Specific */
#include "aimonUtil.h"
#include "sys950Lib.h"
#include "sysApi.h"
#include "scratchRegMap.h"
#include "bflashCt.h"
/* #include "sysLib.c" */
#include "tempSensor.h"

#include "psp_sp0_info.h"

/*
** Macro Definitions
*/


/*
** Global Variables
*/


/*
**  External Declarations
*/
extern char * sysModel (void);
extern char * sysBspRev (void);
extern uint32 sysPhysMemTop (void);
extern int sysProcNumGet (void);
extern int sysClkRateGet (void);

/**
 * @brief Collects SP0 Hardware and Firmware information
 * 
 *        Data is saved in structure CFE_PSP_SP0_Info_t sp0_info_table and
 *        saved as string in sp0_data_dump.
 * 
 * @return int32 CFE_PSP_SUCCESS or CFE_PSP_ERROR
 * 
 * TODO: Fix the return code
 */
int32 getSP0Info(void)
{
    RESET_SRC_REG_ENUM resetSrc = 0;
    USER_SAFE_MODE_DATA_STRUCT safeModeUserData;
    int active_boot = 0;
    int32 status = 0;
    float temperature;
    float voltage;
    int i;
    uint64 bitExecuted = 0ULL;
    uint64 bitResult   = 0ULL;
    int ret_code = CFE_PSP_SUCCESS;

    /*
    This routine returns the model name of the CPU board. The returned string 
    depends on the board model and CPU version being used, for example, "PPC Board".
    */
    sp0_info_table.systemModel = sysModel();

    /*
    This routine returns a pointer to a BSP version and revision number, for
    example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned.
    */
    sp0_info_table.systemBspRev = sysBspRev();

    /*
    This function returns the address of the first missing byte of memory, which
    indicates the top of memory. Normally, the user specifies the amount of 
    physical memory with the macro LOCAL_MEM_SIZE in config.h.
    */
    sp0_info_table.systemPhysMemTop = sysPhysMemTop();

    /*
    This routine returns the processor number for the CPU board, which is set 
    with sysProcNumSet( ).
    */
    sp0_info_table.systemProcNum = sysProcNumGet();

    /*
    Returns slot number of this card in the cPCI backplane
    */
    sp0_info_table.systemSlotId = sysGetSlotId();

    /*
    Returns the time in micro-seconds since system startup. Rolls over after 3 years
    */
    sp0_info_table.systemStartupUsecTime = GetUsecTime();

    /*
    Returns true if the SP0 is the CPCI system controller. The hardware register
    read is done only once. This is so that after operation, in case the hardware 
    fails, we do not suddenly change our configuration.
    */
    sp0_info_table.systemCpciSysCtrl = isCpciSysController();

    /*
    Gets core clock speed in MHz
    */
    sp0_info_table.systemCoreClockSpeed = getCoreClockSpeed();

    /*
    Gets last reset reason
    */
    status = ReadResetSourceReg(&resetSrc,0);
    if (status == OS_SUCCESS)
    {
        sp0_info_table.systemLastResetReason = (uint8) resetSrc;
    }

    /* Do I need this: ReadSafeModeUserData() */
    status = ReadSafeModeUserData(&safeModeUserData,0);
    if (status == OS_SUCCESS)
    {
        snprintf(sp0_info_table.safeModeUserData, 256, "{\"Safe mode\": %d, \"sbc\": \"%s\", \"reason\": %d, \"cause\": \"0x%08x\"}",
                safeModeUserData.safeMode,
                (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
                safeModeUserData.reason,
                safeModeUserData.mckCause);
    }

    /*
    Returns the active boot EEPROM at the time of startup
    Boot device = 2 is returned when the board fails to boot at the first attempt
    and the (external) watchdog timer reboots the board successfully using the 
    alternative boot device.
    */
    sp0_info_table.active_boot = (uint8) returnSelectedBootFlash();

    /*
    This routine returns the system clock rate. The number of ticks per second 
    of the system tick timer.
    */
    sp0_info_table.systemClkRateGet = sysClkRateGet();

    /*
    This routine returns the interrupt rate of the auxiliary clock.
    The number of ticks per second of the auxiliary clock.
    */
    sp0_info_table.systemAuxClkRateGet = sysAuxClkRateGet();

    /**** Read 4 Temperature sensors of SP0s, 3 if SP0 ****/
    for (i = 0; i < 4; i++)
    {
        /* This function takes about 3msec each time */
        status = tempSensorRead(i,0,&temperature,0);
        if (status == OS_SUCCESS)
        {
            sp0_info_table.temperatures[i] = temperature;
        }
    }

    /**** Voltages ****/
    for (i = 1; i < 7; i++)
    {
        /* This function takes about 3msec each time */
        status = volSensorRead(i,0,&voltage,0);
        if (status == OS_SUCCESS)
        {
            sp0_info_table.voltages[i-1] = voltage;
        }
    }

    /* This function returns the list of the test executed by AIMON in a 64 bit packed word. */
    status = aimonGetBITExecuted(&bitExecuted, 0);
    if (status == OS_SUCCESS)
    {
        sp0_info_table.bitExecuted = bitExecuted;
    }

    /* This function returns the summary of the test results in a 64 bit packed word */
    status = aimonGetBITResults(&bitResult, 0);
    if (status == OS_SUCCESS)
    {
        sp0_info_table.bitResult = bitResult;
    }

    /* Output to local string buffer */
    sp0_data_dump_length = snprintf(
        sp0_data_dump, 
        1000,
        "SysModel: %s\nSysBspRev: %s\n"
        "SysPhysMemTop: 0x%08X (%.0f MiB)\n"
        "sysProcNum: %d\n"
        "sysSlotId: %d\n"
        "sysStartupUsecTime: %0.3f\n"
        "sysCpciSysCtrl: %d\n"
        "sysCoreClockSpeed: %u [MHz]\n"
        "sysLastResetReason: %u\n"
        "active_boot: %u '%s'\n"
        "sysClkRateGet: %d [Ticks/sec]\n"
        "sysAuxClkRateGet: %d [Ticks/sec]\n"
        "bitExecuted: 0x%08X\n"
        "bitResult: 0x%08X\n"
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
        sp0_info_table.systemModel,
        sp0_info_table.systemBspRev,
        sp0_info_table.systemPhysMemTop,
        (float)(sp0_info_table.systemPhysMemTop/1048576),   /* 1048576 is 1 mebibyte */
        sp0_info_table.systemProcNum,
        sp0_info_table.systemSlotId,
        sp0_info_table.systemStartupUsecTime,
        sp0_info_table.systemCpciSysCtrl,
        sp0_info_table.systemCoreClockSpeed,
        sp0_info_table.systemLastResetReason,
        sp0_info_table.active_boot,
        (sp0_info_table.active_boot ? "PRIMARY": "SECONDARY"),
        sp0_info_table.systemClkRateGet,
        sp0_info_table.systemAuxClkRateGet,
        sp0_info_table.bitExecuted,
        sp0_info_table.bitResult,
        sp0_info_table.temperatures[0],
        sp0_info_table.temperatures[1],
        sp0_info_table.temperatures[2],
        sp0_info_table.temperatures[3],
        sp0_info_table.voltages[0],
        sp0_info_table.voltages[1],
        sp0_info_table.voltages[2],
        sp0_info_table.voltages[3],
        sp0_info_table.voltages[4],
        sp0_info_table.voltages[5]
    );

    return ret_code;
}

/**
 * @brief Print the data collected to console and return a string containing
 *        the printout
 * 
 */
void printSP0_info_table(void)
{
    /* Output to console */
    printf("\n%s\n",&sp0_data_dump);
}

/**
 * @brief Function dumps the collected data to file
 * 
 *        The safest place to put this file is the Flash memory
 */
void psp_dump_data(void)
{
    char filename[] = "/ffx0/PSP_SP0_DUMP";
    int fd;

	/*  */
    remove(filename);

	fd = creat(filename, O_RDWR);
    write(fd, sp0_data_dump, (size_t) sp0_data_dump_length);
    close(fd);
}