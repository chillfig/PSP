/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 *
 *    Copyright (c) 2020, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

/**
 * \file
 * \ingroup  vxworks
 * \author   
 *
 */

#include "coveragetest-psp-gr740-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include <gr740.h>

#include "PCS_vxWorks.h"
#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"
#include "PCS_stat.h"

#include "cfe_psp.h"
#include "cfe_psp_gr740info.h"

#define SAMPLE_TEMPERATURE  96

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern char g_cGR740DataDump[GR740_TEXT_BUFFER_MAX_SIZE];
extern int32 g_iGR740DataDumpLength;
extern CFE_PSP_GR740StaticInfoTable_t g_GR740StaticInfoTable;
extern CFE_PSP_GR740DynamicInfoTable_t g_GR740DynamicInfoTable;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_TempSensorInit(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_TempSensorInit(void)
{
    int32 iRetCode = CFE_PSP_ERROR;
    
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_TempSensorInit();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_TempSensorInit() - 1/1: Nominal");

}

/*=======================================================================================
** Ut_CFE_PSP_GetTemperatureAwait(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GetTemperatureAwait(void)
{
    int32 iRetCode = CFE_PSP_SUCCESS;
    uint32 uiTemp = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    TEMPERATURE_REG->status_reg |= (TEMPERATURE_STATUS_DATA_MASK & SAMPLE_TEMPERATURE);
    TEMPERATURE_REG->status_reg |= (TEMPERATURE_STATUS_UPD_MASK & (1 << TEMPERATURE_STATUS_UPD_POS));

    /* Execute test */
    iRetCode = CFE_PSP_GetTemperatureAwait(&uiTemp);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetTemperatureAwait() - 1/3: Nominal - Successful return code");
    UtAssert_True(uiTemp == SAMPLE_TEMPERATURE, "_CFE_PSP_GetTemperatureAwait() - 1/3: Nominal - Temperature %u", uiTemp);

    /* ----- Test case #2 - NULL uiTempPtr ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_GetTemperatureAwait(NULL);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetTemperatureAwait() - 1/3: Nominal - Successful return code");

    /* ----- Test case #2 - Temperature Status Disabled ----- */
    /* Setup additional inputs */
    uiTemp = 0;
    TEMPERATURE_REG->status_reg = 0;

    /* Execute test */
    iRetCode = CFE_PSP_GetTemperatureAwait(&uiTemp);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetTemperatureAwait() - 3/3: Temp Not Enabled - Successful return code");
    UtAssert_True(uiTemp == 0, "_CFE_PSP_GetTemperatureAwait() - 3/3: Temp Not Enabled - Temperature %u", uiTemp);
}

/*=======================================================================================
** Ut_CFE_PSP_GR740CollectStaticInfo(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GR740CollectStaticInfo(void)
{
    int32 iRetCode;
    char cMsgLocalTime[] = {GR740_PRINT_SCOPE "Error getting local time\n"};

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);

    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_GR740CollectStaticInfo - 1/4: Nominal GR740s");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - clock_gettime error ----- */
    /* Setup additional inputs */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), PCS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);

    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GR740CollectStaticInfo - 2/4: clock_gettime error");
    UtAssert_OS_print(cMsgLocalTime, "_CFE_PSP_GR740CollectStaticInfo - 2/4: clock_gettime error");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - snprintf error ----- */
    /* Setup additional inputs */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), -2);

    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR_LEVEL_0), "_CFE_PSP_GR740CollectStaticInfo - 3/4: Print to buffer error");
    UtAssert_True((g_iGR740DataDumpLength == -1), "_CFE_PSP_GR740CollectStaticInfo - 3/4: Print to buffer error");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - snprintf buffer overflow----- */
    /* Setup additional inputs */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), PCS_OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), GR740_TEXT_BUFFER_MAX_SIZE);

    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR_LEVEL_0), "_CFE_PSP_GR740CollectStaticInfo - 4/4: Print to buffer overflow error");
    UtAssert_True((g_iGR740DataDumpLength == GR740_TEXT_BUFFER_MAX_SIZE), "_CFE_PSP_GR740CollectStaticInfo - 4/4: Print to buffer overflow error");
}

/*=======================================================================================
** Ut_CFE_PSP_GR740CollectDynamicInfo(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GR740CollectDynamicInfo(void)
{
    int32 iRetCode = CFE_PSP_ERROR;
    char cMsgLocalTime[] = {GR740_PRINT_SCOPE "Error getting local time\n"};

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    TEMPERATURE_REG->status_reg |= (TEMPERATURE_STATUS_UPD_MASK & (1 << TEMPERATURE_STATUS_UPD_POS));
    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_GR740CollectDynamicInfo - 1/3: Nominal - Return Success");
    UtAssert_True(g_GR740DynamicInfoTable.temperature >= 0, 
        "_CFE_PSP_GR740CollectDynamicInfo - 1/3: Nominal - Return Success");
    UtAssert_True(g_GR740DynamicInfoTable.temperature <= 127, 
        "_CFE_PSP_GR740CollectDynamicInfo - 1/3: Nominal - Return Success");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - clock_gettime error ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), PCS_ERROR);
    TEMPERATURE_REG->status_reg |= (TEMPERATURE_STATUS_UPD_MASK & (1 << TEMPERATURE_STATUS_UPD_POS));
    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GR740CollectDynamicInfo - 2/3: clock_gettime error");
    UtAssert_OS_print(cMsgLocalTime, "_CFE_PSP_GR740CollectDynamicInfo - 2/3: clock_gettime error");
    UtAssert_True(g_GR740DynamicInfoTable.temperature >= 0, 
        "_CFE_PSP_GR740CollectDynamicInfo - 2/3: clock_gettime error");
    UtAssert_True(g_GR740DynamicInfoTable.temperature <= 127, 
        "_CFE_PSP_GR740CollectDynamicInfo - 2/3: clock_gettime error");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - CFE_PSP_TemperatureAwait failure ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    TEMPERATURE_REG->status_reg = 0;
    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GR740CollectDynamicInfo - 3/3: TemperatureAwait failure");
    UtAssert_True(g_GR740DynamicInfoTable.temperature == UINT32_MAX, 
        "_CFE_PSP_GR740CollectDynamicInfo - 3/3: Nominal - TemperatureAwait failure");
}

/*=======================================================================================
** Ut_CFE_PSP_GR740PrintToBuffer(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GR740PrintToBuffer(void)
{
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    iRetCode = CFE_PSP_GR740PrintToBuffer();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GR740PrintToBuffer - 1/3: Nominal");

    /* ----- Test case #2 - snprintf error ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), -2);
    /* Execute test */
    iRetCode = CFE_PSP_GR740PrintToBuffer();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_GR740PrintToBuffer - 2/3: snprintf error");
    UtAssert_True(g_iGR740DataDumpLength == -1, "_CFE_PSP_GR740PrintToBuffer - 2/3: snprintf error");

    /* ----- Test case #3 - snprintf buffer overflow----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), GR740_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    iRetCode = CFE_PSP_GR740PrintToBuffer();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_GR740PrintToBuffer - 3/3: snprintf buffer overflow");
    UtAssert_True(g_iGR740DataDumpLength == GR740_TEXT_BUFFER_MAX_SIZE, "_CFE_PSP_GR740PrintToBuffer - 2/3: snprintf buffer overflow");

}

/*=======================================================================================
** Ut_CFE_PSP_GR740GetStaticInfoTable(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GR740GetStaticInfoTable(void)
{
    CFE_PSP_GR740StaticInfoTable_t gr740Table;
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal with no console print ----- */
    /* Setup additional inputs */
    g_GR740StaticInfoTable.lastUpdatedUTC.tv_sec = 1000;
    g_iGR740DataDumpLength = snprintf(g_cGR740DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetStaticInfoTable(&gr740Table, sizeof(gr740Table), 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS,
                    "_CFE_PSP_GR740GetStaticInfoTable - 1/5: Nominal return success");
    UtAssert_True(gr740Table.lastUpdatedUTC.tv_sec == g_GR740StaticInfoTable.lastUpdatedUTC.tv_sec,
        "_CFE_PSP_GR740GetStaticInfoTable - 1/5: Nominal without console print");

    /* ----- Test case #2 - Nominal with console print ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    g_iGR740DataDumpLength = snprintf(g_cGR740DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetStaticInfoTable(&gr740Table, sizeof(gr740Table), 1);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS,
                    "_CFE_PSP_GR740GetStaticInfoTable - 2/5: Nominal return success");
    UtAssert_NA("_CFE_PSP_GR740GetStaticInfoTable - 2/5: Nominal with console print");

    /* ----- Test case #3 - data dump string array length is zero ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    g_iGR740DataDumpLength = 0;
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetStaticInfoTable(&gr740Table, sizeof(gr740Table), 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS,
                    "_CFE_PSP_GR740GetStaticInfoTable - 3/5: Data dump string length zero - return success");

    /* ----- Test case #4 - Data structure size insufficient ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetStaticInfoTable(&gr740Table, 5, 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR,
                    "_CFE_PSP_GR740GetStaticInfoTable - 4/5: Data structure size insufficient - return error");

    /* ----- Test case #5 - Null structure ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetStaticInfoTable(NULL, 1000, 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR,
                    "_CFE_PSP_GR740GetStaticInfoTable - 5/5: Nominal return success");
}

/*=======================================================================================
** Ut_CFE_PSP_GR740GetDynamicInfoTable(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GR740GetDynamicInfoTable(void)
{
    CFE_PSP_GR740DynamicInfoTable_t gr740Table;
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal with no console print ----- */
    /* Setup additional inputs */
    g_GR740DynamicInfoTable.lastUpdatedUTC.tv_sec = 1000;
    g_iGR740DataDumpLength = snprintf(g_cGR740DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetDynamicInfoTable(&gr740Table, sizeof(gr740Table), 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS,
                "_CFE_PSP_GR740GetDynamicInfoTable - 1/5: Nominal return success");
    UtAssert_True(gr740Table.lastUpdatedUTC.tv_sec == g_GR740DynamicInfoTable.lastUpdatedUTC.tv_sec,
                    "_CFE_PSP_GR740GetDynamicInfoTable - 1/5: Nominal without console print");

    /* ----- Test case #2 - Nominal with console print ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    g_iGR740DataDumpLength = snprintf(g_cGR740DataDump, 20, "Test Print");
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetDynamicInfoTable(&gr740Table, sizeof(gr740Table), 1);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS,
                    "_CFE_PSP_GR740GetDynamicInfoTable - 2/5: Nominal return success");
    UtAssert_NA("_CFE_PSP_GR740GetDynamicInfoTable - 2/5: Nominal with console print");

    /* ----- Test case #3 - data dump string array length is zero ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump, 0, GR740_TEXT_BUFFER_MAX_SIZE);
    g_iGR740DataDumpLength = 0;
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetDynamicInfoTable(&gr740Table, sizeof(gr740Table), 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS,
                    "_CFE_PSP_GR740GetDynamicInfoTable - 3/5: Data dump string length zero - return success");

    /* ----- Test case #4 - Data structure size insufficient ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetDynamicInfoTable(&gr740Table, 5, 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR,
                    "_CFE_PSP_GR740GetDynamicInfoTable - 4/5: Data structure size insufficient - return error");

    /* ----- Test case #5 - Null structure ----- */
    /* Setup additional inputs */
    /* Set the content of the output data to a fixed value for testing */
    /* Execute test */
    iRetCode = CFE_PSP_GR740GetDynamicInfoTable(NULL, 1000, 0);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR,
                    "_CFE_PSP_GR740GetDynamicInfoTable - 5/5: Nominal return success");
}

/*=======================================================================================
** Ut_CFE_PSP_GR740GetDiskFreeSize(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GR740GetDiskFreeSize(void)
{
    char        ram_disk_path_ram[] = "/ram0";
    char        ram_disk_path_bad[] = "/f";
    char        ram_disk_path_too_long[200] = {0};
    int64_t     return_value = 0;
    struct PCS_statfs test_statfs;

    /* Setup */
    test_statfs.f_bsize = 10;
    test_statfs.f_bavail = 20;
    UT_SetDataBuffer(UT_KEY(PCS_statfs), &test_statfs, sizeof(test_statfs), false);
    /* Execute test */
    return_value = CFE_PSP_GR740GetDiskFreeSize(ram_disk_path_ram);
    /* Verify outputs */
    UtAssert_True(return_value > 0, "_CFE_PSP_GR740GetDiskFreeSize - 1/4: Nominal");

    UT_ResetState(0);

    /* Execute test */
    test_statfs.f_bsize = 10;
    test_statfs.f_bavail = 20;
    UT_SetDataBuffer(UT_KEY(PCS_statfs), &test_statfs, sizeof(test_statfs), false);
    UT_SetDeferredRetcode(UT_KEY(PCS_statfs), 1, PCS_ERROR);
    return_value = CFE_PSP_GR740GetDiskFreeSize(ram_disk_path_bad);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_CFE_PSP_GR740GetDiskFreeSize - 2/4: Bad path");

    UT_ResetState(0);

    /* Execute test */
    return_value = CFE_PSP_GR740GetDiskFreeSize(NULL);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_CFE_PSP_GR740GetDiskFreeSize - 3/4: NULL path");

    UT_ResetState(0);

    memset(ram_disk_path_too_long,0x11,sizeof(ram_disk_path_too_long));
    /* Execute test */
    return_value = CFE_PSP_GR740GetDiskFreeSize(ram_disk_path_too_long);
    /* Verify outputs */
    UtAssert_True(return_value == CFE_PSP_ERROR, "_CFE_PSP_GR740GetDiskFreeSize - 4/4: Path too long");
}


/*=======================================================================================
** End of file psp_gr740info_testcases.c
**=======================================================================================*/
