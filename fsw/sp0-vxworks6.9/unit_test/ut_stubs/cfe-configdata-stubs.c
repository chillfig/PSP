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
*   \file cfe-configdata-stubs.c
*
*   \brief This file defines stub functions relating to cFE config data.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

/* PSP coverage stub replacement for string.h */
#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include "utstubs.h"

#include <common_types.h>
#include <target_config.h>

/**
 * Stub for the main system entry function implemented in CFE ES
 */
void SystemMain(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(SystemMain), StartType);
    UT_Stub_RegisterContextGenericArg(UT_KEY(SystemMain), StartSubtype);
    UT_Stub_RegisterContextGenericArg(UT_KEY(SystemMain), ModeId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(SystemMain), StartFilePath);
    UT_DEFAULT_IMPL(SystemMain); 
}

/**
 * Stub for notification function implemented in CFE ES
 */
void SystemNotify(void)
{
    UT_DEFAULT_IMPL(SystemNotify);
}

CFE_StaticModuleLoadEntry_t CFE_PSP_BASE_MODULE_LIST[] = {{NULL}};

Target_CfeConfigData GLOBAL_CFE_CONFIGDATA =
{
        /**
         * Main CFE entry point.  Called from PSP startup code.
         */
        .SystemMain = SystemMain,

        /**
         * Notification function. Called from PSP after async event handling.
         */
        .SystemNotify = SystemNotify,

        .ResetAreaSize = 100,
        .RamDiskSectorSize = 512,
        .RamDiskTotalSectors = 2,
        .CdsSize = 100,
        .UserReservedSize = 100
};

Target_ConfigData GLOBAL_CONFIGDATA =
{
        .MissionVersion = "MissionUnitTest",
        .CfeVersion = "CfeUnitTest",
        .OsalVersion = "OsalUnitTest",
        .Config = "MissionConfig",
        .Date = "MissionBuildDate",
        .User = "MissionBuildUser",
        .Default_CpuName = "UnitTestCpu",
        .Default_CpuId = 1,
        .Default_SpacecraftId = 0x42,
        .CfeConfig = &GLOBAL_CFE_CONFIGDATA,
        .PspModuleList = NULL
};
