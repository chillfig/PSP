/*
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
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
 * Stub for notification function implemented in CFE ES
 */
void SystemNotify(void);

Target_CfeConfigData GLOBAL_CFE_CONFIGDATA =
{
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
        .CfeConfig = &GLOBAL_CFE_CONFIGDATA
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
 * Stub for notification function implemented in CFE ES
 */
void SystemNotify(void)
{
    UT_DEFAULT_IMPL(SystemNotify);
}

