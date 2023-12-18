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
*   \file psp_ntp_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_ntp.c.
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

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_ntp_testcases.h"
#include "../src/cfe_psp_ntp.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_NTPDaemonIsRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_NTPDaemonIsRunning(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    
    /* ----- Test case #1 - Successfully get status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_NTPDaemonIsRunning();
    /* Verify result */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_NTPDaemonIsRunning - 1/2: Nominal - NTP Client task is running");
    
    /* ----- Test case #2 - PSP Error, Unable to get status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_NTPDaemonIsRunning();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_NTPDaemonIsRunning - 2/2: NTP Client task is not running");
    
}

/*=======================================================================================
** Ut_CFE_PSP_SetOSTime(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetOSTime(void)
{
    uint32 ts_sec;
    uint32 ts_nsec;
    int32 return_status;
    uint32 BASE_ts_sec;
    uint32 BASE_ts_nsec;
    char cMsg[256] = {};

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal clock set ----- */
    /* Set additional variables */
    ts_sec = 1;
    BASE_ts_sec = ts_sec;
    ts_nsec = 1;
    BASE_ts_nsec = ts_nsec;
    UT_SetDeferredRetcode(UT_KEY(clock_settime), 1, OK);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Clock set");
    /* Execute test */
    return_status = CFE_PSP_SetOSTime(ts_sec, ts_nsec);
    /* Verify results */
    UtAssert_True(ts_sec == BASE_ts_sec, "_CFE_PSP_SetOSTime - 1/2: ts_sec was not changed");
    UtAssert_True(ts_nsec == BASE_ts_nsec, "_CFE_PSP_SetOSTime - 1/2: ts_nsec was not changed");
    UtAssert_True(return_status == CFE_PSP_SUCCESS, "_CFE_PSP_SetOSTime - 1/2: Nominal, Successful");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetOSTime - 1/2: " NTPSYNC_PRINT_SCOPE "Clock set");

    /* ----- Test case #2 - Clock Not set ----- */
    /* Set additional variables */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    ts_sec = 1;
    BASE_ts_sec = ts_sec;
    ts_nsec = 1;
    BASE_ts_nsec = ts_nsec;
    UT_SetDeferredRetcode(UT_KEY(clock_settime), 1, CFE_PSP_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
    /* Execute test */
    return_status = CFE_PSP_SetOSTime(ts_sec, ts_nsec);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetOSTime - 2/2: " NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
    UtAssert_True(return_status == CFE_PSP_ERROR, "_CFE_PSP_SetOSTime - 2/2: Unable to set clock");

    UtAssert_True(ts_sec == BASE_ts_sec, "_CFE_PSP_SetOSTime - 2/2: ts_sec was not changed");
    UtAssert_True(ts_nsec == BASE_ts_nsec, "_CFE_PSP_SetOSTime - 2/2: ts_nsec was not changed");
}

/*=======================================================================================
** Ut_CFE_PSP_GetOSTime(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetOSTime(void)
{
    int32 return_code;
    char cMsg[256] = {};

    /* Used for clock_gettime */
    struct timespec getTimeReturnStruct;
    getTimeReturnStruct.tv_nsec = 0;

    CFE_TIME_SysTime_t myT;
    myT.Seconds = 0;
    myT.Subseconds = 0;

    Ut_OS_printf_Setup();

    /* ----- Test #1 - &myT == NULL ----- */
    /* Set additional variables */
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "CFE_PSP_GetOSTime called without a proper argument\n");
    /* Execute test */
    return_code = CFE_PSP_GetOSTime(NULL);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_GetOSTime - 1/4: Null pointer");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetOSTime - 1/4: " NTPSYNC_PRINT_SCOPE "ERROR: Invalid timestamp");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_GetOSTime - 1/4: myT.Seconds did not change");
    UtAssert_True(myT.Subseconds == 0, "_CFE_PSP_GetOSTime - 1/4: myT.Subseconds did not change");

    /* ----- Test #2 - &myT != NULL, clock_gettime fail ----- */
    /* Set additional variables */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
    /* Execute test */
    return_code = CFE_PSP_GetOSTime(&myT);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_GetOSTime - 2/4: not null pointer, clock_gettime fail");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetOSTime - 2/4: " NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_GetOSTime - 2/4: myT.Seconds did not change");
    UtAssert_True(myT.Subseconds == 0, "_CFE_PSP_GetOSTime - 2/4: myT.Subseconds did not change");

    /* ----- Test #3 - &myT != NUll, clock_gettime success, unixTime > CFE...UNIX_DIFF ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    /* Execute test */
    return_code = CFE_PSP_GetOSTime(&myT);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_GetOSTime - 3/4: clock_gettime success");
    UtAssert_True(myT.Seconds == 1, "_CFE_PSP_GetOSTime - 3/4: myT.Seconds changed");


    /* ----- Test #4 - &myT != NUll, clock_gettime success, unixTime < CFE...UNIX_DIFF ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    myT.Seconds = 0;
    myT.Subseconds = 0;
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF - 1;
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    /* Execute test */
    return_code = CFE_PSP_GetOSTime(&myT);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_GetOSTime - 4/4: clock_gettime success");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_GetOSTime - 4/4: myT.Seconds did not change");
}

/*=======================================================================================
** Ut_CFE_PSP_StartNTPDaemon(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_StartNTPDaemon(void)
{
    char cMsg[256] = {};
    int32 return_code;

    Ut_OS_printf_Setup();

    /* ----- Test case #1: ipcom success, taskNameToId success ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully\n");
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 1/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_StartNTPDaemon - 1/5: NTP Daemon started successfully, correct return code");

    /* ----- Test case #2: ipcom success, taskNameToId failure ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully\n");
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 2/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StartNTPDaemon - 2/5: NTP Daemon started succesfully, taskNameToId failure, correct return code");

    /* ----- Test case #3: ipcom already started, taskNameToId success ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_ERR_ALREADY_STARTED);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already started\n");
    /* Not sure what would/should be returned in real execution */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 3/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_StartNTPDaemon - 3/5: NTP Daemon already started, correct return code");

    /* ----- Test case #4: ipcom already started, taskNameToId failure ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_ERR_ALREADY_STARTED);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already started\n");
    /* Not sure what would/should be returned in real execution */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 4/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StartNTPDaemon - 4/5: NTP Daemon started succesffully, taskNameToId failure, correct return code");

    /* ----- Test case #5: some other ipcom error ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, CFE_PSP_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)\n", CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 5/5: " NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = CFE_PSP_ERROR (SEE RETURN CODE ABOVE))");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StartNTPDaemon - 5/5: NTP Daemon did not start, check error code");
}

/*=======================================================================================
** Ut_CFE_PSP_StopNTPDaemon(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_StopNTPDaemon(void)
{
    char cMsg[256] = {};
    int32 return_code;

    Ut_OS_printf_Setup();

    /* ----- Test case #1: ipcom success ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully\n");
    /* Execute test */
    return_code = CFE_PSP_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StopNTPDaemon - 1/3: " NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_StopNTPDaemon - 1/3: NTP Daemon stopped successfully, correct return code");

    /* ----- Test case #2: ipcom already started ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, IPCOM_ERR_NOT_STARTED);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped\n");
    /* Execute test */
    return_code = CFE_PSP_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StopNTPDaemon - 2/3: " NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StopNTPDaemon - 2/3: NTP Daemon already stopped, correct return code");

    /* ----- Test case #3: some other ipcom error ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, CFE_PSP_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)\n", CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StopNTPDaemon - 3/3: " NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = CFE_PSP_ERROR (SEE RETURN CODE ABOVE))");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StopNTPDaemon - 3/3: NTP Daemon did not stop, check error code");
}

/*=======================================================================================
** End of file psp_ntp_testcases.c
**=======================================================================================*/
