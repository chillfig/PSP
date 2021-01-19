/*
 *      Copyright (c) 2019, United States government as represented by the
 *      administrator of the National Aeronautics Space Administration.
 *      All rights reserved. This software was created at NASA Goddard
 *      Space Flight Center pursuant to government contracts.
 *
 *      This is governed by the NASA Open Source Agreement and may be used,
 *      distributed and modified only according to the terms of that agreement.
 */

/* PSP coverage stub replacement for string.h */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"

#include <common_types.h>
#include <target_config.h>

/**
 * Stub for the main system entry function implemented in CFE ES
 */
void SystemMain(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath);

/**
 * Stub for 1Hz ISR function implemented in CFE TIME
 */
void System1HzISR(void);

/**
 * Stub for notification function implemented in CFE ES
 */
void SystemNotify(void);

Target_CfeConfigData GLOBAL_CFE_CONFIGDATA =
{

        /**
         * 1Hz ISR entry point.  Called from PSP once per second on HW clock.
         */
        .System1HzISR = System1HzISR,

        /**
         * Main CFE entry point.  Called from PSP startup code.
         */
        .SystemMain = SystemMain,

        /**
         * Notification function. Called from PSP after async event handling.
         */
        .SystemNotify = SystemNotify
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
        .PspConfig = &GLOBAL_PSP_CONFIGDATA
};



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
 * Stub for 1Hz ISR function implemented in CFE TIME
 */
void System1HzISR(void)
{
    UT_DEFAULT_IMPL(System1HzISR);
}


/**
 * Stub for notification function implemented in CFE ES
 */
void SystemNotify(void)
{
    UT_DEFAULT_IMPL(SystemNotify);
}

