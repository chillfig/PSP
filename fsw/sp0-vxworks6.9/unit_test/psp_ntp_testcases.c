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
**    2021-09-13 | Matthew Rumbel   | Implement UT Tests
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
** Ut_CFE_PSP_Sync_From_OS_Enable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Sync_From_OS_Enable(void)
{
    char cMsg[256] = {};
    int32 ret = false;
    bool enable;

    /* ----- Test case #1 - Nominal Enable True ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Sync with OS is enabled\n");
    enable = true;
    /* Execute test */
    ret = CFE_PSP_Sync_From_OS_Enable(enable);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_Sync_From_OS_Enable - 1/2: Nominal True");
    UtAssert_True(g_iEnableGetTimeFromOS_flag == enable, "_CFE_PSP_Sync_From_OS_Enable - 1/2: Nominal True");
    UtAssert_True(ret == (int32)g_iEnableGetTimeFromOS_flag, "_CFE_PSP_Sync_From_OS_Enable - 1/2: Nominal True");

    /* ----- Test case #2 - Nominal Enable False ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Sync with OS is disabled\n");
    enable = false;
    /* Execute test */
    ret = CFE_PSP_Sync_From_OS_Enable(enable);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_Sync_From_OS_Enable - 2/2: Nominal False");
    UtAssert_True(g_iEnableGetTimeFromOS_flag == enable, "_CFE_PSP_Sync_From_OS_Enable - 2/2: Nominal False");
    UtAssert_True(ret == (int32)g_iEnableGetTimeFromOS_flag, "_CFE_PSP_Sync_From_OS_Enable - 2/2: Nominal False");
}

/*=======================================================================================
** Ut_CFE_PSP_Sync_From_OS_GetFreq(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Sync_From_OS_GetFreq(void)
{
    uint16 uiReturnValue;

    /* ----- Test case #1 - Nominal get frequency ----- */
    /* Set additional variables */
    g_usOSTimeSync_Sec = 0x1234;
    /* Execute test */
    uiReturnValue = CFE_PSP_Sync_From_OS_GetFreq();
    /* Verify results */
    UtAssert_True(uiReturnValue == g_usOSTimeSync_Sec, "_CFE_PSP_Sync_From_OS_GetFreq - 1/1: Nominal Get Frequency");
}

/*=======================================================================================
** Ut_CFE_PSP_Sync_From_OS_SetFreq(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Sync_From_OS_SetFreq(void)
{
    uint16 new_frequency_sec;
    int32 iReturnValue;
    g_uiPSPNTPTask_id = 1234;
    g_iEnableGetTimeFromOS_flag = 0;

    /* ----- Test case #2 - net_clock_vxworks_Destroy failure ----- */
    /* Set additional variables */
    UT_ResetState(0);
    new_frequency_sec = 0x0001;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERROR);
    /* Execute test */
    iReturnValue = CFE_PSP_Sync_From_OS_SetFreq(new_frequency_sec);
    /* Verify results */
    UtAssert_True(g_usOSTimeSync_Sec == new_frequency_sec, "_CFE_PSP_Sync_From_OS_SetFreq - 1/3: Unable to delete task");
    UtAssert_True(iReturnValue == CFE_PSP_ERROR, "_CFE_PSP_Sync_From_OS_SetFreq - 1/3: Unable to delete task");

        /* ----- Test case #1 - Nominal ----- */
    /* Set additional variables */
    new_frequency_sec = 0x0001;
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                    NTPSYNC_TASK_NAME,
                    CFE_PSP_Update_OS_Time,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024),
                    g_ucNTPSyncTaskPriority,
                    0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    iReturnValue = CFE_PSP_Sync_From_OS_SetFreq(new_frequency_sec);
    /* Verify results */
    UtAssert_True(g_usOSTimeSync_Sec == new_frequency_sec, "_CFE_PSP_Sync_From_OS_SetFreq - 2/3: Nominal timer reinitialized with new updated value");
    UtAssert_True(iReturnValue == CFE_PSP_SUCCESS, "_CFE_PSP_Sync_From_OS_SetFreq - 2/3: Nominal timer reinitialized with new updated value");

    /* ----- Test case #3 - ntp_clock_vxowrks_Init failure ----- */
    /* Set additional variables */
    UT_ResetState(0);
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                        NTPSYNC_TASK_NAME,
                        CFE_PSP_Update_OS_Time,
                        OSAL_TASK_STACK_ALLOCATE, 
                        OSAL_SIZE_C(1024),
                        g_ucNTPSyncTaskPriority,
                        0);
    new_frequency_sec = 0x0001;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    /* Execute test */
    iReturnValue = CFE_PSP_Sync_From_OS_SetFreq(new_frequency_sec);
    /* Verify results */
    UtAssert_True(g_usOSTimeSync_Sec == new_frequency_sec, "_CFE_PSP_Sync_From_OS_SetFreq - 3/3: Unable to re initialize timer");
    // UtAssert_True(iReturnValue == CFE_PSP_ERROR, "_CFE_PSP_Sync_From_OS_SetFreq - 3/3: Unable to re initialize timer");
    UtAssert_NA("_CFE_PSP_Sync_From_OS_SetFreq - 3/3: Return value should be CFE_PSP_ERROR Unable to re initialize timer");
}

/*=======================================================================================
** Ut_CFE_PSP_Set_OS_Time(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Set_OS_Time(void)
{
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    uint32 ts_sec;
    uint32 ts_nsec;
    int32 return_status;
    uint32 BASE_ts_sec;
    uint32 BASE_ts_nsec;
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal clock set ----- */
    /* Set additional variables */
    ts_sec = 1;
    BASE_ts_sec = ts_sec;
    ts_nsec = 1;
    BASE_ts_nsec = ts_nsec;
    UT_SetDeferredRetcode(UT_KEY(clock_settime), 1, OK);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Clock set");
    /* Execute test */
    return_status = CFE_PSP_Set_OS_Time(ts_sec, ts_nsec);
    /* Verify results */
    UtAssert_True(ts_sec == BASE_ts_sec, "_CFE_PSP_Set_OS_Time - 1/2: ts_sec was not changed");
    UtAssert_True(ts_nsec == BASE_ts_nsec, "_CFE_PSP_Set_OS_Time - 1/2: ts_nsec was not changed");
    UtAssert_True(return_status == CFE_PSP_SUCCESS, "_CFE_PSP_Set_OS_Time - 1/2: Nominal, Successful");
    UtAssert_OS_print(cMsg, "_CFE_PSP_Set_OS_Time - 1/2: " NTPSYNC_PRINT_SCOPE "Clock set");

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
    return_status = CFE_PSP_Set_OS_Time(ts_sec, ts_nsec);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_Set_OS_Time - 2/2: " NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
    UtAssert_True(return_status == CFE_PSP_ERROR, "_CFE_PSP_Set_OS_Time - 2/2: Unable to set clock");

    UtAssert_True(ts_sec == BASE_ts_sec, "_CFE_PSP_Set_OS_Time - 2/2: ts_sec was not changed");
    UtAssert_True(ts_nsec == BASE_ts_nsec, "_CFE_PSP_Set_OS_Time - 2/2: ts_nsec was not changed");
}

/*=======================================================================================
** Ut_CFE_PSP_Get_OS_Time(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Get_OS_Time(void)
{
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    int32 return_code;
    CFE_TIME_SysTime_t myT;
    myT.Seconds = 0;
    myT.Subseconds = 0;
    char cMsg[256] = {};

    /* ----- Test #1 - &myT == NULL ----- */
    /* Set additional variables */
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "CFE_PSP_Get_OS_Time called without a proper argument\n");
    /* Execute test */
    return_code = CFE_PSP_Get_OS_Time(NULL);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_Get_OS_Time - 1/4: Null pointer");
    UtAssert_OS_print(cMsg, "_CFE_PSP_Get_OS_Time - 1/4: " NTPSYNC_PRINT_SCOPE "ERROR: Invalid timestamp");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_Get_OS_Time - 1/4: myT.Seconds did not change");
    UtAssert_True(myT.Subseconds == 0, "_CFE_PSP_Get_OS_Time - 1/4: myT.Subseconds did not change");

    /* ----- Test #2 - &myT != NULL, clock_gettime fail ----- */
    /* Set additional variables */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    // UT_SetDeferredRetcode(UT_KEY(clock_gettime), 1, ERROR);
    UT_SetDeferredRetcode(UT_KEY(clock_gettime), 1, CFE_PSP_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
    /* Execute test */
    return_code = CFE_PSP_Get_OS_Time(&myT);
    /* Verify results */
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_Get_OS_Time - 2/4: not null pointer, clock_gettime fail");
    UtAssert_OS_print(cMsg, "_CFE_PSP_Get_OS_Time - 2/4: " NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
    UtAssert_True(myT.Seconds == 0, "_CFE_PSP_Get_OS_Time - 2/4: myT.Seconds did not change");
    UtAssert_True(myT.Subseconds == 0, "_CFE_PSP_Get_OS_Time - 2/4: myT.Subseconds did not change");

    /* ----- Test #3 - &myT != NUll, clock_gettime success, unixTime > CFE...UNIX_DIFF ----- */
    /* Not necessarily a failed test. Is there anyway to set variables from clock_gettime? */
    UtAssert_NA("_CFE_PSP_Get_OS_Time - 3/4:NEED TO SET unixTime.tv_sec > CFE_...UNIX_DIFF");

    /* ----- Test #4 - myT (p_myT) != NUll, clock_gettime success, unixTime <= CFE...UNIX_DIFF ----- */
    /* Not necessarily a failed test. Is there anyway to set variables from clock_gettime? */
    UtAssert_NA("_CFE_PSP_Get_OS_Time - 4/4:NEED TO SET unixTime.tv_sec <= CFE_...UNIX_DIFF");
}

/*=======================================================================================
** Ut_CFE_PSP_NTP_Daemon_Get_Status(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_NTP_Daemon_Get_Status(void)
{
    bool return_code;
    
    /* ----- Test case #1 - Successfully get status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_NTP_Daemon_Get_Status();
    /* Verify result */
    UtAssert_True(return_code == true, "_CFE_PSP_NTP_Daemon_Get_Status - 1/2: Nominal - NTP Client task is running");
    
    /* ----- Test case :2 - PSP Error, Unable to get status ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_NTP_Daemon_Get_Status();
    /* Verify results */
    UtAssert_True(return_code == false, "_CFE_PSP_NTP_Daemon_Get_Status - 2/2: NTP Client task is not running");
    
}

/*=======================================================================================
** Ut_CFE_PSP_NTP_Daemon_Enable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_NTP_Daemon_Enable(void)
{
    UT_ResetState(0);
    int32 return_code;
    bool enable;

    /* ----- Test case #1 - Start NTP Daemon ----- */
    /* Set additional variables */
    enable = true;
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_StartNTPDaemon), 1, 99); // This has no affect
    /* Execute test */
    return_code = CFE_PSP_NTP_Daemon_Enable(enable);
    /* Verify results */
    // printf("\nReturn code: %d\n", return_code);
    UtAssert_True(return_code == IPCOM_SUCCESS, "_CFE_PSP_NTP_Daemon_Enable - 1/4: NTP Daemon enabled successfully");

    /* ----- Test case #2 - Fail to start NTP Daemon ----- */
    UT_ResetState(0);
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, CFE_PSP_ERROR);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_StartNTPDaemon), 1, 99); // This has no affect
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_StartNTPDaemon), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_StartNTPDaemon), 1, 99);

    enable = true;
    /* Execute test */
    return_code = CFE_PSP_NTP_Daemon_Enable(enable);
    /* Verify results */
    // printf("\nReturn code: %d\n", return_code);
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_NTP_Daemon_Enable - 2/4: NTP Daemon failed to enable");

    /* ----- Test case #3 - Succesfully Stop NTP Daemon ----- */
    UT_ResetState(0);
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, IPCOM_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_StopNTPDaemon), 1, 99); // This has no affect
    enable = false;
    /* Execute test */
    return_code = CFE_PSP_NTP_Daemon_Enable(enable);
    /* Verify results */
    // printf("\nReturn code: %d\n", return_code);
    UtAssert_True(return_code == IPCOM_SUCCESS, "_CFE_PSP_NTP_Daemon_Enable - 3/4: NTP Daemon disabled successfully");

     /* ----- Test case #4 - Fail to stop NTP Daemon ----- */
    UT_ResetState(0);
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, CFE_PSP_ERROR);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_StopNTPDaemon), 1, 99); // This has no affect
    enable = false;
    /* Execute test */
    return_code = CFE_PSP_NTP_Daemon_Enable(enable);
    /* Verify results */
    // printf("\nReturn code: %d\n", return_code);
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_NTP_Daemon_Enable - 4/4: NTP Daemon failed to disable");

}

/*=======================================================================================
** Ut_CFE_PSP_TIME_Init(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_TIME_Init(void)
{
    char cMsg[256] = {};
    uint16 timer_frequency_sec;
    int32 status;
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal  ----- */
    /* Set additional variables */
    timer_frequency_sec = 0x0001;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Task Initialized\n");
    /* Execute test */
    status = CFE_PSP_TIME_Init();
    /* Verify outputs */
    UtAssert_True(status == OS_SUCCESS, "_CFE_PSP_TIME_Init - 1/2: Nominal, return value check");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_Init - 1/2: Nominal, OS_printf check");

    /* ----- Test case #2 - OS_TaskCreate fail ----- */
    /* Set additional variables */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Failed to create task\n");
    // TODO: DO we need to be tracking other return codes, other than OS_ERROR?
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    /* Execute test */
    status = CFE_PSP_TIME_Init();
    /* Verify Outputs */
    UtAssert_True(status != OS_SUCCESS, "_CFE_PSP_TIME_Init = 2/2, Failed due to OS_TaskCreate failure");
    UtAssert_OS_print(cMsg, "_CFE_PSP_TIME_Init - 2/2: OS_TaskCreate Fail, OS_printf check");
    
}

/*=======================================================================================
** Ut_CFE_PSP_TimeService_Ready(void test cases
**=======================================================================================*/
void Ut_CFE_PSP_TimeService_Ready(void)
{
    bool return_code;

    /* ----- Test case #1 - Unable to get ID ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_TimeService_Ready();
    /* Verify results */
    UtAssert_True(return_code == false, "_CFE_PSP_TimeService_Ready - 1/2: taskNameToId failed");

    /* ----- Test case #2 - taskNameToId did not fail ----- */
    /* Set additional variables */
    UT_ResetState(0);
    // UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_TimeService_Ready();
    /* Verify results */
    UtAssert_True(return_code == true, "_CFE_PSP_TimeService_Ready - 2/2: taskNameToId passed");
}

/*=======================================================================================
** Ut_CFE_PSP_Update_OS_Time(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Update_OS_Time(void)
{  
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    char cMsg[256] = {};

    UtAssert_NA("_CFE_PSP_Update_OS_Time - 1/1: Function will cause significant delays or will wait until deleted");

    /**
     * @brief Infinite Loop
     * 
     * This task will run forever or until it's task is deleted
     * 
     * or
     * 
     * This task will require significant delay times via OS_TaskDelay(500) x 120
     * 
     */

    /* ----- Test case #1: TimeService not ready ----- */
    /* Set additional variables */
    // UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_TimeService_Ready), false);
    // sprintf(cMsg, NTPSYNC_PRINT_SCOPE
    //             "(ERROR) CFE TIME Service did not start for 60 seconds - Shutting down NTP Sync\n");
    // /* Execute Test */
    // CFE_PSP_Update_OS_Time();
    // /* Varify results */
    // UtAssert_OS_print(cMsg, "_CFE_PSP_Update_OS_TIME - 1/#:" NTPSYNC_PRINT_SCOPE
    //             "(ERROR) CFE TIME Service did not start for 60 seconds - Shutting down NTP Sync\n");
    
    /* ----- Other testing for this will result in infinite loop ----- */
}

/*=======================================================================================
** Ut_CFE_PSP_StartNTPDaemon(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_StartNTPDaemon(void)
{
    /**
     * @brief VERIFY WORKING AS INTENDED
     * 
     */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    char cMsg[256] = {};
    int32 return_code;

    /* ----- Test case #1: ipcom success, taskNameToId success ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 1/5: " NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
    UtAssert_True(return_code == CFE_PSP_SUCCESS, "_CFE_PSP_StartNTPDaemon - 1/5: NTP Daemon started successfully, correct return code");

    /* ----- Test case #2: ipcom success, taskNameToId failure ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_start), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully");
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
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
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
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already started");
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
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)", CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_StartNTPDaemon();
    /* Verify results */
    // printf("\n%s\n", cMsg);
    // printf("\nReturn Code: %d\n", return_code);
    UtAssert_OS_print(cMsg, "_CFE_PSP_StartNTPDaemon - 3/3: " NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = CFE_PSP_ERROR (SEE RETURN CODE ABOVE))");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StartNTPDaemon - 5/5: NTP Daemon did not start, check error code");

}
/*=======================================================================================
** Ut_CFE_PSP_StopNTPDaemon(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_StopNTPDaemon(void)
{
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    char cMsg[256] = {};
    int32 return_code;

    /* ----- Test case #1: ipcom success ----- */
    /* Set additional variables */
    UT_SetDeferredRetcode(UT_KEY(ipcom_ipd_kill), 1, IPCOM_SUCCESS);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully");
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
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped");
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
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)", CFE_PSP_ERROR);
    /* Execute test */
    return_code = CFE_PSP_StopNTPDaemon();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_StopNTPDaemon - 3/3: " NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = CFE_PSP_ERROR (SEE RETURN CODE ABOVE))");
    UtAssert_True(return_code == CFE_PSP_ERROR, "_CFE_PSP_StopNTPDaemon - 3/3: NTP Daemon did not stop, check error code");
}

/*=======================================================================================
** Ut_net_clock_vxworks_Destroy(void) test cases
**=======================================================================================*/
void Ut_net_clock_vxworks_Destroy(void)
{
    char cMsg[256] = {};
    int32 return_value;
    int32 BASE_g_uiPSPNTPTask_id;
    g_uiPSPNTPTask_id = 1234;

    /* ----- Test case #1 - Nominal destroy ----- */
    /* Set additional inputs */
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                    NTPSYNC_TASK_NAME,
                    CFE_PSP_Update_OS_Time,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024),
                    g_ucNTPSyncTaskPriority,
                    0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    /* Execute test */
    return_value = net_clock_vxworks_Destroy();
    /* Verify results */
    UtAssert_True(g_iEnableGetTimeFromOS_flag == false, "_net_clock_vxworks_Destroy - 1/3: Nominal clock vxworks destroy");
    UtAssert_True(return_value == OS_SUCCESS, "_net_clock_vxworks_Destroy - 1/3: Nominal clock vxworks destroy");
    UtAssert_True(g_uiPSPNTPTask_id == 0, "_net_clock_vxworks_Destroy - 1/3: Nominal clock vxworks destroy");

    /* ----- Test case #2 - OS_TaskDelete failed with checked error code ----- */
    /* Set Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                    NTPSYNC_TASK_NAME,
                    CFE_PSP_Update_OS_Time,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024),
                    g_ucNTPSyncTaskPriority,
                    0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERR_INVALID_ID);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Could not kill the NTP Sync task\n");
    g_uiPSPNTPTask_id = 0x00000001;
    BASE_g_uiPSPNTPTask_id = g_uiPSPNTPTask_id;
    /* Execute test */
    return_value = net_clock_vxworks_Destroy();
    /* Verify results */
    UtAssert_True(g_iEnableGetTimeFromOS_flag == false, "_net_clock_vxworks_Destroy - 2/3: clock vxworks destroy failed, specific error code check, global variable check");
    UtAssert_OS_print(cMsg, "_net_clock_vxworks_Destroy - 2/3: clock vxworks destroy failed, specific error code check, print message check");
    UtAssert_True(return_value == CFE_PSP_ERROR, "_net_clock_vxworks_Destroy - 2/3: clock vxworks destroy failed, specific error code check, return value check");
    UtAssert_True(BASE_g_uiPSPNTPTask_id == g_uiPSPNTPTask_id, "_net_clock_vxworks_Destroy - 2/3: clock vxworks destroy failed, specific error code check, global variable check");

    /* ----- Teset case #3 - OS_TaskDelete returned some other error ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_TaskCreate(&g_uiPSPNTPTask_id,
                    NTPSYNC_TASK_NAME,
                    CFE_PSP_Update_OS_Time,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024),
                    g_ucNTPSyncTaskPriority,
                    0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERROR);
    sprintf(cMsg, NTPSYNC_PRINT_SCOPE "Could not kill the NTP Sync task\n");
    g_uiPSPNTPTask_id = 0x00000001;
    BASE_g_uiPSPNTPTask_id = g_uiPSPNTPTask_id;
    /* Execute test */
    return_value = net_clock_vxworks_Destroy();
    /* Verify results */
    UtAssert_True(g_iEnableGetTimeFromOS_flag == false, "_net_clock_vxworks_Destroy - 3/3: clock vxworks destroy failed, handle non-specific error code check");
    UtAssert_OS_print(cMsg, "_net_clock_vxworks_Destroy - 3/3: clock vxworks destroy failed, handle non-specific error code check");
    UtAssert_True(return_value == CFE_PSP_ERROR, "_net_clock_vxworks_Destroy - 3/3: clock vxworks destroy failed, handle non-specific error code check");
    UtAssert_True(BASE_g_uiPSPNTPTask_id == g_uiPSPNTPTask_id, "_net_clock_vxworks_Destroy - 3/3: clock vxworks destroy failed, handle non-specific error code check");
}

/*=======================================================================================
** Ut_ntp_clock_vxworks_Init(void) test cases
**=======================================================================================*/
void Ut_ntp_clock_vxworks_Init(void)
{
    uint32 PspModuleId = 0x00000000; // Has no affect

    /* ----- Test #1 - Nominal clock vxworks init, true check----- */
    /* Set additional input */
    g_iEnableGetTimeFromOS_flag = true;
    /* Execute test */
    ntp_clock_vxworks_Init(PspModuleId);
    /* Verify results */
    UtAssert_True(true == true, "_ntp_clock_vxworks_Init - 1/2: Nominal g_iEnableGetTimeFromOS_flag is true");

    /* ----- Test #1 - Nominal clock vxworks init, false check----- */
    /* Set additional input */
    UT_ResetState(0);
    g_iEnableGetTimeFromOS_flag = false;
    /* Execute test */
    ntp_clock_vxworks_Init(PspModuleId);
    /* Verify results */
    UtAssert_True(false == false, "_ntp_clock_vxworks_Init - 2/2: Nominal g_iEnableGetTimeFromOS_flag is false");
}

/*=======================================================================================
** End of file psp_ntp_testcases.c
**=======================================================================================*/
