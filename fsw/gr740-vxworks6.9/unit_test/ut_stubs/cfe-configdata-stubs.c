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
*   \file cfe-configdata-stubs.c
*
*   \brief This file defines stub functions relating to cFE config data.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
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
