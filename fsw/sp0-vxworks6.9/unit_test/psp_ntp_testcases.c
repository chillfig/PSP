/*=======================================================================================
**
** File:  psp_ntp_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_ntp.c
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2021-09-08 | Claudio Olmi     | Initial revision
**    2021-09-13 | Matthew Rumbel   | Implement UT Tests/Improvements
**=======================================================================================*/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
/* #include <target_config.h> */
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
/* #include "cfe_psp.h" */

#include "psp_ntp_testcases.h"
#include "../../modules/ntp_clock_wxworks/cfe_psp_ntp.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_GetFreq(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_GetFreq(void)
{
    uint16 uiReturnValue;

    /* ----- Test case #1 - Nominal get frequency ----- */
    /* Set additional variables */
    g_usOSTimeSync_Sec = 0x1234;
    /* Execute test */
    uiReturnValue = CFE_PSP_TIME_NTPSync_GetFreq();
    /* Verify results */
    UtAssert_True(uiReturnValue == g_usOSTimeSync_Sec, "_CFE_PSP_TIME_NTPSync_GetFreq - 1/1: Nominal Get Frequency");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_SetFreq(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_SetFreq(void)
{
    uint16 uiNewFreqSec;

    /* ----- Test case #1 - Nominal set frequency ----- */
    /* Set additional variables */
    g_usOSTimeSync_Sec = 30;
    uiNewFreqSec = 10;
    /* Execute test */
    CFE_PSP_TIME_NTPSync_SetFreq(uiNewFreqSec);
    /* Verify results */
    UtAssert_True(g_usOSTimeSync_Sec == uiNewFreqSec, "_CFE_PSP_TIME_NTPSync_SetFreq - 1/1: Nominal set new frequency");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_Set_OS_Time(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_Set_OS_Time(void)
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
    return_status = CFE_PSP_TIME_Set_OS_Time(ts_sec, ts_nsec);
    /* Verify results */
    UtAssert_True(ts_sec == BASE_ts_sec, "_CFE_PSP_TIME_Set_OS_Time - 1/2: ts_sec was not changed");
    UtAssert_True(ts_nsec == BASE_ts_nsec, "_CFE_PSP_TIME_Set_OS_Time - 1/2: ts_nsec was not changed");
    UtAssert_True(return_status == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_Set_OS_Time - 1/2: Nominal, Successful");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_Set_OS_Time - 1/2: " NTPSYNC_PRINT_SCOPE "Clock set");

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
    return_status = CFE_PSP_TIME_Set_OS_Time(ts_sec, ts_nsec);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_Set_OS_Time - 2/2: " NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
    UtAssert_True(return_status == CFE_PSP_ERROR, "_CFE_PSP_TIME_Set_OS_Time - 2/2: Unable to set clock");

    UtAssert_True(ts_sec == BASE_ts_sec, "_CFE_PSP_TIME_Set_OS_Time - 2/2: ts_sec was not changed");
    UtAssert_True(ts_nsec == BASE_ts_nsec, "_CFE_PSP_TIME_Set_OS_Time - 2/2: ts_nsec was not changed");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_Get_OS_Time(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_Get_OS_Time(void)
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
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "CFE_PSP_TIME_Get_OS_Time called without a proper argument\n");
    /* Execute test */
    return_code = CFE_PSP_TIME_Get_OS_Time(NULL);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_TIME_Get_OS_Time - 1/4: Null pointer");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_Get_OS_Time - 1/4: " NTPSYNC_PRINT_SCOPE "ERROR: Invalid timestamp");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_TIME_Get_OS_Time - 1/4: myT.Seconds did not change");
    UtAssert_True(myT.Subseconds == 0, "_CFE_PSP_TIME_Get_OS_Time - 1/4: myT.Subseconds did not change");

    /* ----- Test #2 - &myT != NULL, clock_gettime fail ----- */
    /* Set additional variables */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
    /* Execute test */
    return_code = CFE_PSP_TIME_Get_OS_Time(&myT);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_TIME_Get_OS_Time - 2/4: not null pointer, clock_gettime fail");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_Get_OS_Time - 2/4: " NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_TIME_Get_OS_Time - 2/4: myT.Seconds did not change");
    UtAssert_True(myT.Subseconds == 0, "_CFE_PSP_TIME_Get_OS_Time - 2/4: myT.Subseconds did not change");

    /* ----- Test #3 - &myT != NUll, clock_gettime success, unixTime > CFE...UNIX_DIFF ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), OK);
    /* Execute test */
    return_code = CFE_PSP_TIME_Get_OS_Time(&myT);
    /* Not necessarily a failed test. Is there anyway to set variables from clock_gettime? */
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_Get_OS_Time - 3/4: clock_gettime success");
    UtAssert_True(myT.Seconds == 1, "_CFE_PSP_TIME_Get_OS_Time - 3/4: myT.Seconds changed");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTP_Daemon_isRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTP_Daemon_isRunning(void)
{
    bool return_code;
    
    /* ----- Test case #1 - Successfully get status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_TIME_NTP_Daemon_isRunning();
    /* Verify result */
    UtAssert_True(return_code == true, "_CFE_PSP_TIME_NTP_Daemon_isRunning - 1/2: Nominal - NTP Client task is running");
    
    /* ----- Test case :2 - PSP Error, Unable to get status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TIME_NTP_Daemon_isRunning();
    /* Verify results */
    UtAssert_True(return_code == false, "_CFE_PSP_TIME_NTP_Daemon_isRunning - 2/2: NTP Client task is not running");
    
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_Task_Enable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_Task_Enable(void)
{
    char cMsg[256] = {};
    uint16 timer_frequency_sec;
    int32 iReturnCode;

    /* ----- Test case #1 - Nominal, task not running  ----- */
    /* Set additional variables */
    timer_frequency_sec = 0x0001;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Enable();
    /* Verify outputs */
    UtAssert_True(iReturnCode == OS_SUCCESS, "_CFE_PSP_TIME_NTPSync_Task_Enable - 1/3: Nominal, return value check");

    /* ----- Test case #2 - OS_TaskCreate fail, task not running ----- */
    /* Set additional variables */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Error creating NTP Sync task\n");
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Enable();
    /* Verify Outputs */
    UtAssert_True(iReturnCode != OS_SUCCESS, "_CFE_PSP_TIME_NTPSync_Task_Enable = 2/3, Failed due to OS_TaskCreate failure");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_NTPSync_Task_Enable - 2/3: OS_TaskCreate Fail, OS_printf check");

    /* ----- Test case #3 - Task is already running ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Sync task is already running\n");
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Enable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_NTPSync_Task_Enable - 3/3: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_NTPSync_Task_Enable - 3/3: NTP Sync task already running - message");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_Task_Disable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_Task_Disable(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};

    /* ----- Test case #1 - Task is not running ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    g_uiPSPNTPTask_id = 99;
    g_bEnableGetTimeFromOS_flag = true;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Sync task is not running\n");
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: NTP Sync task not running - message");
    UtAssert_True(g_uiPSPNTPTask_id == 0, "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: NTP Sync task id reset");
    UtAssert_True(g_bEnableGetTimeFromOS_flag == false, "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: Get time from OS set to false");
    // UtAssert_True(
    //                 (g_uiPSPNTPTask_id == 0)
    //                 && (g_bEnableGetTimeFromOS_flag == false),
    //                 "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: Values were reset");
    
    /* ----- Test case #2 - Unable to delete NTP Sync task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_uiPSPNTPTask_id = 99;
    g_bEnableGetTimeFromOS_flag = true;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Unable to delete NTP Sync task\n");
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_TIME_NTPSync_Task_Disable - 2/3: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_NTPSync_Task_Disable - 2/3: Unable to delete NTP Sync task - message");
    UtAssert_True(g_uiPSPNTPTask_id == 0, "_CFE_PSP_TIME_NTPSync_Task_Disable - 2/3: NTP Sync task id reset");
    UtAssert_True(g_bEnableGetTimeFromOS_flag == false, "_CFE_PSP_TIME_NTPSync_Task_Disable - 2/3: Get time from OS set to false");
    // UtAssert_True(
    //                 (g_uiPSPNTPTask_id == 0)
    //                 && (g_bEnableGetTimeFromOS_flag == false),
    //                 "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: Values were reset");

    /* ----- Test case #3 - Successfully delete NTP Sync task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    g_uiPSPNTPTask_id = 99;
    g_bEnableGetTimeFromOS_flag = true;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                    NTPSYNC_TASK_NAME,
                    CFE_PSP_TIME_NTPSync_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024),
                    g_ucNTPSyncTaskPriority,
                    0
                    );
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_NTPSync_Task_Disable - 3/3: Correct return code");
    UtAssert_True(g_uiPSPNTPTask_id == 0, "_CFE_PSP_TIME_NTPSync_Task_Disable - 3/3: NTP Sync task id reset");
    UtAssert_True(g_bEnableGetTimeFromOS_flag == false, "_CFE_PSP_TIME_NTPSync_Task_Disable - 3/3: Get time from OS set to false");
    // UtAssert_True(
    //                 (g_uiPSPNTPTask_id == 0)
    //                 && (g_bEnableGetTimeFromOS_flag == false),
    //                 "_CFE_PSP_TIME_NTPSync_Task_Disable - 1/3: Values were reset");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_CFETimeService_isRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_CFETimeService_isRunning(void)
{
    bool return_code;

    /* ----- Test case #1 - Unable to get ID ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TIME_CFETimeService_isRunning();
    /* Verify results */
    UtAssert_True(return_code == false, "_CFE_PSP_TIME_CFETimeService_isRunning - 1/2: taskNameToId failed");

    UT_ResetState(0);

    /* ----- Test case #2 - taskNameToId did not fail ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_TIME_CFETimeService_isRunning();
    /* Verify results */
    UtAssert_True(return_code == true, "_CFE_PSP_TIME_CFETimeService_isRunning - 2/2: taskNameToId passed");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_Task(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_Task(void)
{  
    /* Used for clock_gettime */
    struct timespec getTimeReturnStruct;
    getTimeReturnStruct.tv_nsec = 0;

    char cMsg_ready[] = {NTPSYNC_PRINT_SCOPE "CFE TIME Service is ready - Starting NTP Sync\n"};
    char cMsg_get_os_time_failed[] = {NTPSYNC_PRINT_SCOPE "OS has not sync with NTP server yet, trying again later.\n"};
    char cMsg_task_delay_error[] = {NTPSYNC_PRINT_SCOPE "OS_TaskDelay error\n"};
    char cMsg_not_ready[] = {NTPSYNC_PRINT_SCOPE "(ERROR) CFE TIME Service did not start for 60 seconds - Shutting down NTP Sync\n"};
    char cMsg_exit_loop[] = {NTPSYNC_PRINT_SCOPE "NTP Sync encountered an error that caused the main task to exit\n"};

    Ut_OS_printf_Setup();

    /* ----- Test case #1: Nominal - with premature exit ----- */
    /* ----- Test case #1: CFE_PSP_TIME_Get_OS_Time error ----- */
    /* ----- Test case #1: TimeService not ready ----- */

    /* Set additional variables */
    /* Make CFE_PSP_TimeService_Ready return false right away */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelay), 4, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelay), 4, OS_ERROR);
    /* Go inside while loop */
    g_bEnableGetTimeFromOS_flag = true;
    /* Setup CFE_PSP_TIME_Get_OS_Time */
    UT_SetDeferredRetcode(UT_KEY(clock_gettime), 1, OK);
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDeferredRetcode(UT_KEY(clock_gettime), 4, ERROR);
    /* Setup CFE_PSP_Set_OS_Time */
    UT_SetDefaultReturnValue(UT_KEY(clock_settime), OK);
    g_usOSTimeSync_Sec = 1;
    /* Execute Test */
    CFE_PSP_TIME_NTPSync_Task();
    /* Varify results */
    UtAssert_OS_print(cMsg_ready, "_CFE_PSP_TIME_NTPSync_Task - 1/4: CFE Time Service has started - message\n");
    UtAssert_OS_print(cMsg_get_os_time_failed, "_CFE_PSP_TIME_NTPSync_Task - 1/4: Get OS Time failed - message\n");
    UtAssert_OS_print(cMsg_task_delay_error, "_CFE_PSP_TIME_NTPSync_Task - 1/4: Task Delay failed - message\n");
    UtAssert_OS_print(cMsg_exit_loop, "_CFE_PSP_TIME_NTPSync_Task - 1/4: Exit Loop - message\n");

    UT_ResetState(0);
    Ut_OS_printf_Setup();    
    /* ----- Test case #2: g_bEnableGetTimeFromOS_flag not set ----- */
    /* Set additional variables */
    /* Make CFE_PSP_TimeService_Ready return false right away */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelay), 4, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelay), 4, OS_ERROR);
    /* Don't go inside while loop */
    g_bEnableGetTimeFromOS_flag = false;
    /* Execute Test */
    CFE_PSP_TIME_NTPSync_Task();
    /* Varify results */
    UtAssert_OS_print(cMsg_ready, "_CFE_PSP_TIME_NTPSync_Task - 2/4: CFE Time Service has started - message\n");
    UtAssert_OS_print(cMsg_task_delay_error, "_CFE_PSP_TIME_NTPSync_Task - 2/4: Task Delay failed - message\n");
    UtAssert_OS_print(cMsg_exit_loop, "_CFE_PSP_TIME_NTPSync_Task - 2/4: Exit Loop - message\n");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3: CFE Time Service did not start on time ----- */
    /* Set additional variables */
    /* Make CFE_PSP_TimeService_Ready return false right away */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), CFE_PSP_ERROR);
    /* UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS); */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    /* Execute Test */
    CFE_PSP_TIME_NTPSync_Task();
    /* Varify results */
    UtAssert_OS_print(cMsg_not_ready, "_CFE_PSP_TIME_NTPSync_Task - 3/4: CFE Time Service never started - message\n");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4: task delay returns error ----- */
    /* Set additional variables */
    /* Make CFE_PSP_TimeService_Ready return false right away */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), CFE_PSP_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_ERROR);
    /* Execute Test */
    CFE_PSP_TIME_NTPSync_Task();
    /* Varify results */
    UtAssert_OS_print(cMsg_task_delay_error, "_CFE_PSP_TIME_NTPSync_Task - 4/4: Task Delay error - message\n");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_Task_isRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_Task_isRunning(void)
{
    bool bReturnValue = false;

    /* ----- Test case #1 - Task is not running ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    bReturnValue = CFE_PSP_TIME_NTPSync_Task_isRunning();
    /* Verify results */
    UtAssert_True(bReturnValue == false, "_CFE_PSP_TIME_NTPSync_Task_isRunning - 1/2: Task not running, currect return value");

    /* ----- Test case #2 - Task is not running ----- */
    /* Set additional inputs */
    /*
    ** May need ot add task creation code here
    */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    /* Execute test */
    bReturnValue = CFE_PSP_TIME_NTPSync_Task_isRunning();
    /* Verify results */
    UtAssert_True(bReturnValue == true, "_CFE_PSP_TIME_NTPSync_Task_isRunning - 2/2: Task running, currect return value");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_NTPSync_Task_Priority_Set(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_NTPSync_Task_Priority_Set(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    osal_priority_t opBasePriority = 99;
    char cMsg[256] = {};

    /* ----- Test case #1 - No change in priority ----- */
    /* Set additional inputs */
    g_ucNTPSyncTaskPriority = opBasePriority;
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Priority_Set(opBasePriority);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 1/4: Correct return code");

    /* ----- Test case #2 - Priority outside upper bound ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    opBasePriority = 255;
    g_ucNTPSyncTaskPriority = opBasePriority;
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Task Priority outside allowed range\n");
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Priority_Set(opBasePriority);
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 2/4: Correct return code");
    if (opBasePriority > NTPSYNC_PRIORITY_UP_RANGE)
    {
        UtAssert_True(g_ucNTPSyncTaskPriority == NTPSYNC_DEFAULT_PRIORITY, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 2/4: Correct default priority set");
        UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 3/4: Task priority outside allowed range - message");
    }

    /* ----- Test case #3 - Priority outside lower bound ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    opBasePriority = 1;
    g_ucNTPSyncTaskPriority = 99;
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Task Priority outside allowed range\n");
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Priority_Set(opBasePriority);
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 3/4: Correct return code");
    UtAssert_True(g_ucNTPSyncTaskPriority == NTPSYNC_DEFAULT_PRIORITY, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 3/4: Correct default priority set");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 3/4: Task priority outside allowed range - message");

    /* ----- Test case #4 - Set new priority ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    opBasePriority = NTPSYNC_PRIORITY_UP_RANGE - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_TIME_NTPSync_Task_Priority_Set(opBasePriority);
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 4/4: Correct return code");
    UtAssert_True(g_ucNTPSyncTaskPriority == opBasePriority, "_CFE_PSP_TIME_NTPSync_Task_Priority_Set - 4/4: Correct priority set");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_StartNTPDaemon(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_StartNTPDaemon(void)
{
    char cMsg[256] = {};
    int32 return_code;

    Ut_OS_printf_Setup();

    /* ----- Test case #1: ipcom success, taskNameToId success ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_TIME_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StartNTPDaemon - 1/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_StartNTPDaemon - 1/5: NTP Daemon started successfully, correct return code");

    /* ----- Test case #2: ipcom success, taskNameToId failure ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TIME_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StartNTPDaemon - 2/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_TIME_StartNTPDaemon - 2/5: NTP Daemon started succesfully, taskNameToId failure, correct return code");

    /* ----- Test case #3: ipcom already started, taskNameToId success ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_ERR_ALREADY_STARTED);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
    /* Not sure what would/should be returned in real execution */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_TIME_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StartNTPDaemon - 3/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_StartNTPDaemon - 3/5: NTP Daemon already started, correct return code");

    /* ----- Test case #4: ipcom already started, taskNameToId failure ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_ERR_ALREADY_STARTED);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
    /* Not sure what would/should be returned in real execution */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TIME_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StartNTPDaemon - 4/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_TIME_StartNTPDaemon - 4/5: NTP Daemon started succesffully, taskNameToId failure, correct return code");

    /* ----- Test case #5: some other ipcom error ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, CFE_PSP_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)", CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TIME_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StartNTPDaemon - 3/3: " NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = CFE_PSP_ERROR (SEE RETURN CODE ABOVE))");
    UtAssert_True(return_code == IPCOM_ERR_NOT_STARTED, "_CFE_PSP_TIME_StartNTPDaemon - 5/5: NTP Daemon did not start, check error code");
}

/*=======================================================================================
** Ut_CFE_PSP_TIME_StopNTPDaemon(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_StopNTPDaemon(void)
{
    char cMsg[256] = {};
    int32 return_code;

    Ut_OS_printf_Setup();

    /* ----- Test case #1: ipcom success ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully");
    /* Execute test */
    return_code = CFE_PSP_TIME_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StopNTPDaemon - 1/3: " NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_TIME_StopNTPDaemon - 1/3: NTP Daemon stopped successfully, correct return code");

    /* ----- Test case #2: ipcom already started ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, IPCOM_ERR_NOT_STARTED);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped");
    /* Execute test */
    return_code = CFE_PSP_TIME_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StopNTPDaemon - 2/3: " NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_TIME_StopNTPDaemon - 2/3: NTP Daemon already stopped, correct return code");

    /* ----- Test case #3: some other ipcom error ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, CFE_PSP_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)", CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TIME_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_StopNTPDaemon - 3/3: " NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = CFE_PSP_ERROR (SEE RETURN CODE ABOVE))");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_TIME_StopNTPDaemon - 3/3: NTP Daemon did not stop, check error code");
}

/*=======================================================================================
** Ut_ntp_clock_vxworks_Destroy(void) test cases
**=======================================================================================*/
void Ut_ntp_clock_vxworks_Destroy(void)
{
    char cMsg[256] = {};
    int32 iReturnCode;

    /* ----- Test case #1 - Task disable failed ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Unable to destroy NTP Sync task\n");
    g_uiPSPNTPTask_id = 1234;
    g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY - 1;
    g_usOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC - 1;
    g_bEnableGetTimeFromOS_flag = !(CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP);
    /* Execute test */
    iReturnCode = ntp_clock_vxworks_Destroy();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_ntp_clock_vxworks_Destroy - 1/2: Unable to destroy NTP Sync task - message");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_ntp_clock_vxworks_Destroy - 1/2: Correct return code");
    UtAssert_True(g_uiPSPNTPTask_id == 0, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP Task id reset");
    UtAssert_True(g_ucNTPSyncTaskPriority == NTPSYNC_DEFAULT_PRIORITY, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP Task priority reset");
    UtAssert_True(g_usOSTimeSync_Sec == CFE_MISSION_TIME_SYNC_OS_SEC, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP OS time sync sec reset");
    UtAssert_True(g_bEnableGetTimeFromOS_flag == CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP time sync on startup reset");
    // UtAssert_True(
    //                 (g_uiPSPNTPTask_id == 0) 
    //                 && (g_ucNTPSyncTaskPriority == NTPSYNC_DEFAULT_PRIORITY)
    //                 && (g_usOSTimeSync_Sec == CFE_MISSION_TIME_SYNC_OS_SEC)
    //                 && (g_bEnableGetTimeFromOS_flag == CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP), 
    //                 "_ntp_clock_vxworks_Destroy - 1/2: Values reset properly"
    //                 );
    
    /* ----- Test case #2 - Task disable successful ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY - 1;
    g_usOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC - 1;
    g_bEnableGetTimeFromOS_flag = !(CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP);
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                    NTPSYNC_TASK_NAME,
                    CFE_PSP_TIME_NTPSync_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024),
                    g_ucNTPSyncTaskPriority,
                    0
                    );
    /* Execute test */
    iReturnCode = ntp_clock_vxworks_Destroy();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_ntp_clock_vxworks_Destroy - 2/2: Correct return code");
    UtAssert_True(g_ucNTPSyncTaskPriority == NTPSYNC_DEFAULT_PRIORITY, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP Task priority reset");
    UtAssert_True(g_usOSTimeSync_Sec == CFE_MISSION_TIME_SYNC_OS_SEC, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP OS time sync sec reset");
    UtAssert_True(g_bEnableGetTimeFromOS_flag == CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP, "_ntp_clock_vxworks_Destroy - 1/2: PSP NTP time sync on startup reset");
    // UtAssert_True(
    //                 (g_uiPSPNTPTask_id == 0) 
    //                 && (g_ucNTPSyncTaskPriority == NTPSYNC_DEFAULT_PRIORITY)
    //                 && (g_usOSTimeSync_Sec == CFE_MISSION_TIME_SYNC_OS_SEC)
    //                 && (g_bEnableGetTimeFromOS_flag == CFE_MISSION_TIME_SYNC_TIME_ON_STARTUP), 
    //                 "_ntp_clock_vxworks_Destroy - 2/2: Values reset properly"
    //                 );
}

/*=======================================================================================
** Ut_ntp_clock_vxworks_Init(void) test cases
**=======================================================================================*/
void Ut_ntp_clock_vxworks_Init(void)
{
    uint32 PspModuleId = 0x00000000; // Has no affect
    char cMsg[256] = {};

    /* ----- Test case #1 - Unsuccessful task enable ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Unable to initialize NTP Sync\n");
    /* Execute Test */
    ntp_clock_vxworks_Init(PspModuleId);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_ntp_clock_vxworks_Init - 1/2: Unable to initialize NTP Sync - message");

    /* ----- Test case #2 - Success ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Unable to initialize NTP Sync\n");
    /* Execute Test */
    ntp_clock_vxworks_Init(PspModuleId);
    /* Verify results */
    UtAssert_NoOS_print(cMsg, "_ntp_clock_vxworks_Init - 2/2: Did not print error message - no message");
}

/*=======================================================================================
** End of file psp_ntp_testcases.c
**=======================================================================================*/
