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
*   \file cfe_psp_gr740info.c
*
*   \brief API for collecting GR740 hardware and software information
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

/*
**  Include Files
*/

#include <stdio.h>
#include <errno.h>
#include <vxWorks.h>
#include <sys/stat.h>

/* For supporting REALTIME clock */
#include <timers.h>

/* GR740 Hardware Registers Definitions */
#include <cfe_psp_gr740.h>

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_gr740info.h"

/*
** Static Function Declarations
*/
int32 CFE_PSP_GR740PrintToBuffer(void);

/*
** Static Variables
*/
/** \name GR740 Information String Buffer */
/** \{ */
/** \brief GR740 String Buffer */ //is the text buffer arbitrarily sized?
char g_cGR740DataDump[GR740_TEXT_BUFFER_MAX_SIZE];
/** \brief Actual length of the string buffer */
int32 g_iGR740DataDumpLength;
/** \} */

/**
 ** \brief GR740 Static Info Table
 */
CFE_PSP_GR740StaticInfoTable_t g_GR740StaticInfoTable;

/**
 ** \brief GR740 Dynamic Data Table
 */
CFE_PSP_GR740DynamicInfoTable_t g_GR740DynamicInfoTable;

int32 CFE_PSP_TempSensorInit(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* Configure Sensor DIV, DCORRECT, CLKEN */
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_DIV, TEMPERATURE_CONTROL_DIV_DEFAULT);
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_DCORRECT, 0x03);
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_CLKEN, 1);

    /* Raise RSTN and PDN */
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_SRSTN, 1);
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_PDN, 1);
    /* Wait 1ms + 25 sensor clock cycles*/

    return iReturnCode;
}

int32 CFE_PSP_GetTemperatureAwait(uint32 * uiTempPtr)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    uint32 uiLocalTemp = 0;

    if (uiTempPtr == NULL)
    {
        goto CFE_PSP_GetTemperatureAwait_Exit_Tag;
    }

    if (GET_TEMPERATURE_STATUS(TEMPERATURE_STATUS_UPD) == 1)
    {
        uiLocalTemp = (GET_TEMPERATURE_STATUS(TEMPERATURE_STATUS_DATA));
        memcpy(uiTempPtr, &uiLocalTemp, sizeof(uint32));
        iReturnCode = CFE_PSP_SUCCESS;
    }

CFE_PSP_GetTemperatureAwait_Exit_Tag:
    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GR740GetStaticInfo
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_GR740CollectStaticInfo(void)
{
    int32   iStatus = 0;
    int32   iRetVal = CFE_PSP_SUCCESS;
    
    OS_printf(GR740_PRINT_SCOPE "Collecting Data\n");

    /* Reset the structure to remove stale data */
    memset(&g_GR740StaticInfoTable, 0x00, sizeof(g_GR740StaticInfoTable));
    g_iGR740DataDumpLength = 0;

    /* This routine returns the model name of the CPU board. The returned string 
       depends on the board model and CPU version being used, for example, "PPC Board". */
    g_GR740StaticInfoTable.systemModel = sysModel();

    /* This routine returns a pointer to a BSP version and revision number, for
    example, 1.1/0. BSP_REV is concatenated to BSP_VERSION and returned. */
    g_GR740StaticInfoTable.systemBspRev = sysBspRev();

    /* This function returns the address of the first missing byte of memory, which
       indicates the top of memory. Normally, the user specifies the amount of 
       physical memory with the macro LOCAL_MEM_SIZE in config.h. */
    g_GR740StaticInfoTable.systemPhysMemTop = (cpuaddr) sysPhysMemTop();

    /* This routine returns the processor number for the CPU board, which is set 
       with sysProcNumSet( ). */
    g_GR740StaticInfoTable.systemProcNum = sysProcNumGet();

    /* This routine returns the system clock rate. The number of ticks per second 
       of the system tick timer. */
    g_GR740StaticInfoTable.systemClkRateGet = sysClkRateGet();

    /* This routine returns the interrupt rate of the auxiliary clock.
       The number of ticks per second of the auxiliary clock. */
    g_GR740StaticInfoTable.systemAuxClkRateGet = sysAuxClkRateGet();

    /* Get real time clock from OS */
    iStatus = clock_gettime(CLOCK_REALTIME, &g_GR740StaticInfoTable.lastUpdatedUTC);
    if (iStatus != OK)
    {
        OS_printf(GR740_PRINT_SCOPE "Error getting local time\n");
        iRetVal = CFE_PSP_ERROR;
    }

    /* Print data to string */
    if (CFE_PSP_GR740PrintToBuffer())
    {
        OS_printf(GR740_PRINT_SCOPE "Could not print to buffer. Data is left in the structure.");
        iRetVal = CFE_PSP_ERROR_LEVEL_0;
    }

    return iRetVal;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GR740GetDynamicInfo
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_GR740CollectDynamicInfo(void)
{
    int32   iStatus = 0;
    uint32  uiTemperature = (uint32)UINT32_MAX;
    int32   iRetVal = CFE_PSP_SUCCESS;
    
    /* Read temperature sensor */
    iStatus = CFE_PSP_GetTemperatureAwait(&uiTemperature);
    /* If temperature is read successfully, save on table */
    if (iStatus == OS_SUCCESS)
    {
        g_GR740DynamicInfoTable.temperature = uiTemperature;
    }
    /* If temperature reading is unsuccessful, save maximum number to show error */
    else
    {
        OS_printf(GR740_PRINT_SCOPE "Error collecting data from tempSensorRead()\n");
        g_GR740DynamicInfoTable.temperature = (uint32)UINT32_MAX;
        iRetVal = CFE_PSP_ERROR;
    }
    
    /* Get real time clock from OS */
    iStatus = clock_gettime(CLOCK_REALTIME, &g_GR740DynamicInfoTable.lastUpdatedUTC);
    if (iStatus != OK)
    {
        OS_printf(GR740_PRINT_SCOPE "Error getting local time\n");
        iRetVal = CFE_PSP_ERROR;
    }

    return iRetVal;
}

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
int32 CFE_PSP_GR740PrintToBuffer(void)
{
    int32  iRetCode = CFE_PSP_SUCCESS;
    uint32 uiTotalMemory_MiB = 0;

    /* Coverts bytes to Mibytes */
    uiTotalMemory_MiB = (g_GR740StaticInfoTable.systemPhysMemTop >> 20);

    /* Output to local string buffer */
    g_iGR740DataDumpLength = snprintf(
        g_cGR740DataDump, 
        sizeof(g_cGR740DataDump),
        "UTC sec: %u\n"
        "SysModel: %s\n"
        "SysBspRev: %s\n"
        "SysPhysMemTop: 0x%08X (%u MiB)\n"
        "sysProcNum: %d\n"
        "sysClkRateGet: %d [Ticks/sec]\n"
        "sysAuxClkRateGet: %d [Ticks/sec]\n"
        "Temp:  %u [C]\n",
        g_GR740DynamicInfoTable.lastUpdatedUTC.tv_sec,
        g_GR740StaticInfoTable.systemModel,
        g_GR740StaticInfoTable.systemBspRev,
        g_GR740StaticInfoTable.systemPhysMemTop,
        uiTotalMemory_MiB,
        g_GR740StaticInfoTable.systemProcNum,
        g_GR740StaticInfoTable.systemClkRateGet,
        g_GR740StaticInfoTable.systemAuxClkRateGet,
        g_GR740DynamicInfoTable.temperature
    );

    /*
    If there was an error in writing using snprintf, then what we have in 
    g_cSP0DataDump is garbage 
    */
    if (g_iGR740DataDumpLength < 0)
    {
        /* This will make sure that we don't write garbage to file */
        g_iGR740DataDumpLength = -1;
        iRetCode = CFE_PSP_ERROR;
    }
    /* Check if it was truncated */
    if (g_iGR740DataDumpLength >= GR740_TEXT_BUFFER_MAX_SIZE)
    {
        /* Return error but don't do anything else. */
        iRetCode = CFE_PSP_ERROR;
    }

    return iRetCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GR740GetDynamicInfoTable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_GR740GetDynamicInfoTable(
                                    CFE_PSP_GR740DynamicInfoTable_t *pDynamic,
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
    if (sizeof(g_GR740DynamicInfoTable) > iDynamicSize)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    if (iRetCode == CFE_PSP_SUCCESS)
    {
        /* Copy internal structure */
        memcpy(pDynamic, &g_GR740DynamicInfoTable, sizeof(g_GR740DynamicInfoTable));

        /*
        OS_printf function cannot print more than OS_BUFFER_SIZE and g_cSP0DataDump
        is usually much longer.
        */
        if (g_iGR740DataDumpLength > 0)
        {
            if (print_to_console > 0)
            {
                // UndCC_NextLine(SSET134)
                printf("\n\n%s\n\n", &g_cGR740DataDump);
            }
        }
    }

    return iRetCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GR740GetStaticInfoTable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_GR740GetStaticInfoTable(CFE_PSP_GR740StaticInfoTable_t *pStatic, size_t iStaticSize, uint8_t print_to_console)
{
    int32   iRetCode = CFE_PSP_SUCCESS;

    /* Return error if the pointer is NULL */
    if (pStatic == NULL)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    /* Return error if the size of the memory pointed by pStatic is not large enough */
    if (sizeof(CFE_PSP_GR740StaticInfoTable_t) > iStaticSize)
    {
        iRetCode = CFE_PSP_ERROR;
    }

    if (iRetCode == CFE_PSP_SUCCESS)
    {
        /* Copy internal structure */
        memcpy(pStatic, &g_GR740StaticInfoTable, sizeof(g_GR740StaticInfoTable));
    
        /*
        OS_printf function cannot print more than OS_BUFFER_SIZE and g_cSP0DataDump
        is usually much longer.
        */
        if (g_iGR740DataDumpLength > 0)
        {
            if (print_to_console > 0)
            {
                // UndCC_NextLine(SSET134)
                printf("\n\n%s\n\n", &g_cGR740DataDump);
            }
        }
    }
    return iRetCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SP0GetDiskFreeSize
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int64_t CFE_PSP_GR740GetDiskFreeSize(char *disk_root_path)
{
    int64_t lFree_size_bytes = CFE_PSP_ERROR;
    long block_size = 0;
    int64_t lBlocks_available = 0;
    struct statfs disk_stats;

    memset(&disk_stats, 0, sizeof(disk_stats));

    if ((disk_root_path != NULL) && memchr(disk_root_path, (cpuaddr)NULL, CFE_PSP_MAXIMUM_TASK_LENGTH))
    {
        if (statfs(disk_root_path, &disk_stats) == OK)
        {
            block_size = disk_stats.f_bsize;
            lBlocks_available = disk_stats.f_bavail;
            lFree_size_bytes = block_size * lBlocks_available;
        }
    }
    return lFree_size_bytes;
}