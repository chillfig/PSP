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
*   \file psp_gr740_info_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_gr740info.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
#include <target_config.h>

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_gr740info_testcases.h"
#include "../src/cfe_psp_gr740info.c"

/**********************************************************
 * PREPROCESSOR DIRECTIVES
 *********************************************************/
#define UT_GR740_PRINT_SCOPE "_PSP_GR740_"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int       PCS_snprintf(char *s, size_t maxlen, const char *format, ...);

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_TempSensorInit(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TempSensorInit(void)
{
    int32   iRetCode = CFE_PSP_ERROR;
    
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
void Ut_CFE_PSP_GetTemperatureAwait(void)
{
    int32   iRetCode = CFE_PSP_SUCCESS;
    uint32  uiTemp = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_GetTemperatureAwait(&uiTemp);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetTemperatureAwait() - 1/2: Nominal - Successful return code");
    UtAssert_True(&uiTemp > 0, "_CFE_PSP_GetTemperatureAwait() - 1/2: Nominal - Temperature %u", uiTemp);
}

/*=======================================================================================
** Ut_CFE_PSP_GR740CollectStaticInfo(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GR740CollectStaticInfo(void)
{
    int32 iRetCode;
    char cMsgLocalTime[] = {GR740_PRINT_SCOPE "Error getting local time\n"};

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    memset(g_cGR740DataDump,(int)NULL, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDefaultReturnValue(UT_KEY(PCS_snprintf), OS_SUCCESS);

    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectStaticInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_GR740CollectStaticInfo - 1/4: Nominal GR740s");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - clock_gettime error ----- */
    /* Setup additional inputs */
    memset(g_cGR740DataDump,(int)NULL, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), ERROR);
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
    memset(g_cGR740DataDump,(int)NULL, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
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
    memset(g_cGR740DataDump,(int)NULL, GR740_TEXT_BUFFER_MAX_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
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
void Ut_CFE_PSP_GR740CollectDynamicInfo(void)
{
    int32 iRetCode = CFE_PSP_ERROR;
    char cMsgLocalTime[] = {GR740_PRINT_SCOPE "Error getting local time\n"};

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OS_SUCCESS);
    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_GR740CollectDynamicInfo - 1/2: Nominal - Return Success");
    UtAssert_True(g_GR740DynamicInfoTable.temperature >= 0, 
        "_CFE_PSP_GR740CollectDynamicInfo - 1/2: Nominal - Return Success");
    UtAssert_True(g_GR740DynamicInfoTable.temperature <= 127, 
        "_CFE_PSP_GR740CollectDynamicInfo - 1/2: Nominal - Return Success");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - clock_gettime error ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_GR740CollectDynamicInfo();
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_GR740CollectDynamicInfo - 2/2: clock_gettime error");
    UtAssert_OS_print(cMsgLocalTime, "_CFE_PSP_GR740CollectDynamicInfo - 2/2: clock_gettime error");
    UtAssert_True(g_GR740DynamicInfoTable.temperature >= 0, 
        "_CFE_PSP_GR740CollectDynamicInfo - 2/2: clock_gettime error");
    UtAssert_True(g_GR740DynamicInfoTable.temperature <= 127, 
        "_CFE_PSP_GR740CollectDynamicInfo - 2/2: clock_gettime error");

}

/*=======================================================================================
** Ut_CFE_PSP_GR740PrintToBuffer(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GR740PrintToBuffer(void)
{
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump,(int)NULL,GR740_TEXT_BUFFER_MAX_SIZE);
    /* Execute test */
    iRetCode = CFE_PSP_GR740PrintToBuffer();
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GR740PrintToBuffer - 1/3: Nominal");

    /* ----- Test case #2 - snprintf error ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set the content of the output data to a fixed value for testing */
    memset(g_cGR740DataDump,(int)NULL,GR740_TEXT_BUFFER_MAX_SIZE);
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
    memset(g_cGR740DataDump,(int)NULL,GR740_TEXT_BUFFER_MAX_SIZE);
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
void Ut_CFE_PSP_GR740GetStaticInfoTable(void)
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
    memset(g_cGR740DataDump,(int)NULL, GR740_TEXT_BUFFER_MAX_SIZE);
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
void Ut_CFE_PSP_GR740GetDynamicInfoTable(void)
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
    memset(g_cGR740DataDump,(int)NULL, GR740_TEXT_BUFFER_MAX_SIZE);
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
** End of file psp_gr740info_testcases.c
**=======================================================================================*/
