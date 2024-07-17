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

#include "cfe_psp.h"
#include "cfe_psp_start.h"
#include "cfe_psp_gr740info.h"
#include "cfe_psp_memory.h"

#include "PCS_taskLibCommon.h"
#include "PCS_taskLib.h"
#include "PCS_sysLib.h"
#include "PCS_gr740.h"
#include "PCS_stdlib.h"
#include "PCS_cfe_configdata.h"
#include "PCS_userReservedMem.h"
#include "PCS_unistd.h"
#include "PCS_fcntl.h"
#include "PCS_time.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern void UT_OS_Application_Startup(void);
extern void UT_OS_Application_Run(void);
extern int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern int volSensorRead(int8 sensor, uint8 dataType, float *voltage, bool talkative );
extern int tempSensorRead (int8 sensor, uint8 dataType, float *temperature, bool talkative );
extern int PCS_OS_BSPMain(void);

extern CFE_PSP_MemoryBlock_t GR740_ReservedMemBlock;
extern char UserReservedMemory[URM_SIZE];
extern char *pURM;
extern TASK_ID g_uiShellTaskID;
extern uint32 g_uiNumberOfProcessors;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_Main() test cases
**=======================================================================================*/
void Test_CFE_PSP_Main(void)
{
    /* ----- Test case #1: - Previous instance of CFS found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskNameToId), CFE_PSP_SUCCESS);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 0);
    UtAssert_NA("_CFE_PSP_Main - 1/3: Found previous instance of CFS");

    /* ----- Test case #2: - OS_BSPMain returns error ----- */
    UT_ResetState(0);
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskNameToId), (cpuaddr)PCS_TASK_ID_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_ERROR);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_NA("_CFE_PSP_Main - 2/3: OS_BSPMain returned error");

    /* ----- Test case #3: - Nominal ----- */
    UT_ResetState(0);
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskNameToId), (cpuaddr)PCS_TASK_ID_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_SUCCESS);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_NA("_CFE_PSP_Main - 3/3: Nominal, OS_BSPMain success");
}

/*=======================================================================================
** Ut_OS_Application_Startup() test cases
** Since there is a multiple definition issue for "OS_Application_Run()". This function
** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
** is located in unit_test/CMakeLists.txt
**=======================================================================================*/
void Test_OS_Application_Startup(void)
{
    char cMsg[256] = {""};
    char cMsg_notice[] = "PSP: Application Startup Complete\n";
    char cMsg_mem_init_failed[] = "PSP: CFS is rebooting due to not having enough User Reserved Memory\n";

    Ut_OS_printf_Setup();

    /* For CFE_PSP_InitProcessorReservedMemory */
    pURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(PCS_userReservedGet), URM_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(PCS_open), 99);
    UT_SetDefaultReturnValue(UT_KEY(PCS_write), 10);
    UT_SetDefaultReturnValue(UT_KEY(PCS_close), PCS_OK);
    
    /* For CFE_PSP_SetupReservedMemoryMap */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);

    /* For CFE_PSP_MemScrubInit */
    /* For CFE_PSP_MemSyncInit */
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemCreate), OS_ERROR);

    /* For CFE_PSP_SetSysTasksPrio */
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskNameToId), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskPriorityGet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskPrioritySet), OS_SUCCESS);

    /* For CFE_PSP_Reset */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    TEMPERATURE_REG->status_reg |= (TEMPERATURE_STATUS_UPD_MASK & (1 << TEMPERATURE_STATUS_UPD_POS));
    UT_SetDefaultReturnValue(UT_KEY(PCS_userReservedGet), URM_SIZE);
    GR740_ReservedMemBlock.BlockSize = URM_SIZE-10;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 1/4: Successful startup notice message");
    UtAssert_STUB_COUNT(PCS_userReservedGet, 2);

    /* ----- Test case #2 - Failed to initialize URM ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    pURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_userReservedGet), 100);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 2/4: Missing startup notice message");
    UtAssert_OS_print(cMsg_mem_init_failed, "_OS_Application_Startup - 2/4: Initialization of User Reserved Memory failed message");


    /* ----- Test case #3 - Initialize the OS API data structures failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    pURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_userReservedGet), URM_SIZE);
    GR740_ReservedMemBlock.BlockSize = URM_SIZE-10;
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    PCS_snprintf(cMsg, sizeof(cMsg), "PSP: OS_Application_Startup() - OS_API_Init() failed %d\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/4: OS_API_Init error message");
    UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 3/4: Successful startup notice message missing");

    /* ----- Test case #4 - Fail to add FS mapping folder and fail to set priorities ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    pURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_userReservedGet), URM_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_taskNameToId), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_ERROR);
    GR740_ReservedMemBlock.BlockSize = URM_SIZE-10;
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    snprintf(cMsg, sizeof(cMsg), "PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/4: CFE_PSP_SetFileSysAddFixedMap error Message");
    snprintf(cMsg, sizeof(cMsg), "PSP: Some or all Virtual FS Mapping has failed\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/4: OS_Application_Startup error Message");
    snprintf(cMsg, sizeof(cMsg), "PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/4: SetSysTasksPrio error Message");
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 4/4: Successful startup notice message");

    /* ----- Test case #5 - CFE_PSP_GR740CollectStaticInfo fail ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    TEMPERATURE_REG->status_reg |= (TEMPERATURE_STATUS_UPD_MASK & (1 << TEMPERATURE_STATUS_UPD_POS));
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_clock_gettime), CFE_PSP_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_userReservedGet), URM_SIZE);
    GR740_ReservedMemBlock.BlockSize = URM_SIZE-10;
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 1/4: Successful startup notice message");
    UtAssert_STUB_COUNT(PCS_userReservedGet, 2);
}

/*=======================================================================================
** Ut_CFE_PSP_SetTaskPrio() test cases
**=======================================================================================*/
void Test_CFE_PSP_SetTaskPrio(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    int32 iNewPrio = 0;
    int32 iCurPrio = 10;
    const char *tName = "tLogTask";
    char *tName_null = NULL;
    const char *tName_too_long = "123456789012345678901234567890";

    /* ----- Test case #1 - Nominal Priority set to 0 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), "PSP: Setting %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(PCS_taskPriorityGet), &iCurPrio, sizeof(iCurPrio), false);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPriorityGet), 1, PCS_OK);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPrioritySet), 1, PCS_OK);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from -1 to 0 */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 1/7: Nominal - Priority set to 0");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 1/7: Nominal - Priority set to 0 ");
    
    /* ----- Test case #2 - Nominal Priority set to 255 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 255;
    snprintf(cMsg, sizeof(cMsg), "PSP: Setting %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(PCS_taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPrioritySet), 1, PCS_OK);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from 260 to 255 */
    /* We cast 260 to uint8 type so ccppc doesn't give build error. Result is the same? */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 2/7: Nominal - Priority set to 255");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 2/7: Nominal - Priority set to 255");

    /* ----- Test case #3 - Failed to set priority to 25 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iCurPrio = 0;
    iNewPrio = 25;
    PCS_snprintf(cMsg, sizeof(cMsg), "PSP: taskPrioritySet() - Failed for %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(PCS_taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask" */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 3/7: Failed - Failed to set priority to 25");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 3/7: Failed - Failed to set priority to 25");

    /* ----- Test case #4 - Failed to set priority to 25 for task name "tLogTask",
     * PCS_taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iCurPrio = 0;
    iNewPrio = 25;
    PCS_snprintf(cMsg, sizeof(cMsg), "PSP: Could not find task %s \n", tName);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 4/7: Failed - Task ID lookup failure");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 4/7: Task ID lookup failure");

    /* ----- Test case #5 - task name is null,
     * PCS_taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_null, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 5/7: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 5/7: Failed - task name is null does not print any message");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 5/7: Null task name returns error");

    /* ----- Test case #6 - task name is null,
     * PCS_taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_too_long, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 6/7: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 6/7: Failed - task name array does not contain NULL terminating string");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 6/7: task name string without NULL returns error");

    /* ----- Test case #7 - taskPriorityGet Fail" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 255;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPriorityGet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from 260 to 255 */
    /* We cast 260 to uint8 type so ccppc doesn't give build error. Result is the same? */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 7/7: taskPriorityGet Fail");
}

/*=======================================================================================
** Ut_CFE_PSP_SetSysTasksPrio() test cases
**=======================================================================================*/
void Test_CFE_PSP_SetSysTasksPrio(void)
{
    uint32 uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    uiRetCode = CFE_PSP_SetSysTasksPrio();
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_SetSysTasksPrio - 1/2: Nominal");

    /* ----- Test case #2 - Failed to set priority for a task ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_SetSysTasksPrio();
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_SetSysTasksPrio - 2/2: Set priority for a task failed");
}

/*=======================================================================================
** Ut_OS_Application_Run() test cases
** Since there is a multiple definition issue for "OS_Application_Run()". This function
** will be renamed to "UT_OS_Application_Run()" in compiling time. The rename section
** was located in unit_test/CMakeLists.txt
**=======================================================================================*/
void Test_OS_Application_Run(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    UT_OS_Application_Run();
    /* Verify outputs */
    UtAssert_NA("_OS_Application_Run - 1/1: NA - Function is empty");
}

/*=======================================================================================
** Test_CFE_PSP_SuspendConsoleShellTask() test cases
**=======================================================================================*/
void Test_CFE_PSP_ResetSysTasksPrio(void)
{
    char cMsg[256] = "\nResetting system tasks' priority to default\n";

    /* ----- Test #1 - Nominal ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();

    /* Execute test */
    CFE_PSP_ResetSysTasksPrio();
    UtAssert_OS_print(cMsg, "_CFE_PSP_ResetSysTasksPrio - 1/1: Task priorities successfully reset");

}

/*=======================================================================================
** Test_CFE_PSP_SuspendConsoleShellTask() test cases
**=======================================================================================*/
void Test_CFE_PSP_SuspendConsoleShellTask(void)
{
    bool suspend;
    int32 status;
    char cMsg[256] = {};

    /* ----- Test #1 - Nominal true, no task id, task suspend successful ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = true;
    g_uiShellTaskID = (TASK_ID)0;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskSuspend), 1, PCS_OK);
    PCS_snprintf(cMsg, sizeof(cMsg), "Shell Task Suspended [%p]\n",PCS_taskIdSelf());
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_SUCCESS, "_CFE_PSP_SuspendConsoleShellTask - 1/4: Task successfully suspended");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 1/4: Task successfully suspended");

    /* ----- Test #2 - true, no task id, task suspend failure ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = true;
    g_uiShellTaskID = (TASK_ID)0;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskSuspend), 1, CFE_PSP_ERROR);
    PCS_snprintf(cMsg, sizeof(cMsg), "Shell Task could not be suspended [%p]\n",PCS_taskIdSelf());
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 2/4: Task did not successfully suspend");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 2/4: Task did not successfully suspend");

    /* ----- Test #3 - Nominal false, no task id, task resume successful ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = (TASK_ID)0;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskResume), 1, PCS_OK);
    PCS_snprintf(cMsg, sizeof(cMsg), "Shell Task Resumed [%p]\n", PCS_taskIdSelf());
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_SUCCESS, "_CFE_PSP_SuspendConsoleShellTask - 3/4: Task successfully resumed");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 3/4: Task successfully resumed");

    /* ----- Test #4 - true, no task id, task resume failure ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = (TASK_ID)0;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskResume), 1, CFE_PSP_ERROR);
    PCS_snprintf(cMsg, sizeof(cMsg), "Shell Task could not be resumed [%p]\n", PCS_taskIdSelf());
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");

    /* ----- Test #5 - Shell task id already acquired ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = (TASK_ID)1;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(PCS_taskResume), 1, CFE_PSP_ERROR);
    PCS_snprintf(cMsg, sizeof(cMsg), "Shell Task could not be resumed [%p]\n", g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
}

/*=======================================================================================
** Ut_CFE_PSP_SetFileSysAddFixedMap() test cases
**=======================================================================================*/
void Test_CFE_PSP_SetFileSysAddFixedMap(void)
{
    osal_id_t   fid;
    int32       ret = CFE_PSP_SUCCESS;
    char        cMsg[256] = {};

    PCS_snprintf(cMsg, sizeof(cMsg), "PSP: Set 1 Virtual Path(s)\n");

    /* ----- Test #1 - Nominal ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), CFE_PSP_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_SetFileSysAddFixedMap(&fid);
    /* Verify results */
    UtAssert_True(ret == CFE_PSP_SUCCESS, "_CFE_PSP_SetFileSysAddFixedMap - 1/2: Virtual Mapping Returned Success");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetFileSysAddFixedMap - 1/2: Printed Nominal String");

    /* ----- Test #2 - Error mapping one or more filesystems ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), CFE_PSP_ERROR);
    /* Execute test */
    ret = CFE_PSP_SetFileSysAddFixedMap(&fid);
    /* Verify results */
    PCS_snprintf(cMsg, 256, "PSP: OS_FileSysAddFixedMap() failure: %d\n", (int)CFE_PSP_ERROR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Printed Error String");
    UtAssert_True(ret == CFE_PSP_ERROR, "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Virtual Mapping Returned Error");
}

/*=======================================================================================
** Ut_CFE_PSP_FindProcessor() test cases
**=======================================================================================*/
void Test_CFE_PSP_FindProcessor(void)
{
    int32 iRetNumber = -99;
    char cTaskName[] = "temp";

    /* ----- Test #1 - Nominal - Not found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(strncmp), -1);
    /* Execute test */
    iRetNumber = CFE_PSP_FindProcessor(cTaskName);
    /* Verify results */
    UtAssert_True(iRetNumber == -1, 
        "_CFE_PSP_SetFileSysAddFixedMap - 1/2: Nominal - Not Found");

    /* ----- Test #2 - Nominal - Found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(strncmp), 0);
    /* Execute test */
    iRetNumber = CFE_PSP_FindProcessor(cTaskName);
    /* Verify results */
    UtAssert_True(iRetNumber != -1,
        "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Nominal - Entry Found");
}

/*=======================================================================================
** Ut_CFE_PSP_OS_EventHandler() test cases
**=======================================================================================*/
void Test_CFE_PSP_OS_EventHandler(void)
{
    int32 iRetCode = OS_ERROR;
    OS_Event_t test_event = OS_EVENT_RESOURCE_ALLOCATED;
    osal_id_t test_object_id = OS_OBJECT_ID_UNDEFINED;
    char data[10] = "";
    char cValidTaskName[] = "VS";
    char cInvalidTaskName[] = "XYZ";

    /* ----- Test #1 - Nominal - OS_EVENT_RESOURCE_ALLOCATED ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_RESOURCE_ALLOCATED;
    g_uiNumberOfProcessors = 4;
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 1/4: Nominal - OS_EVENT_RESOURCE_ALLOCATED");

    /* ----- Test #2 - Nominal - OS_EVENT_RESOURCE_CREATED ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_RESOURCE_CREATED;
    g_uiNumberOfProcessors = 4;
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 2/4: Nominal - OS_EVENT_RESOURCE_CREATED");

    /* ----- Test #3 - Nominal - OS_EVENT_RESOURCE_DELETED ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_RESOURCE_DELETED;
    g_uiNumberOfProcessors = 4;
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - OS_EVENT_RESOURCE_DELETED");

    /* ----- Test #4 - Nominal - OS_EVENT_TASK_STARTUP Nominal ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_TASK_STARTUP;
    UT_SetDataBuffer(UT_KEY(OS_GetResourceName), &cValidTaskName, 
                     OS_strnlen(cValidTaskName, sizeof(cValidTaskName)), true);
    g_uiNumberOfProcessors = 4;
    
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - OS_EVENT_TASK_STARTUP");

    /* ----- Test #5 - Nominal - OS_EVENT_TASK_STARTUP OS_GetResourceName fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_TASK_STARTUP;
    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_ERR_NAME_NOT_FOUND);
    g_uiNumberOfProcessors = 4;
    
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - OS_EVENT_TASK_STARTUP OS_GetResourceName fail");

    /* ----- Test #5 - Nominal - OS_EVENT_TASK_STARTUP Invalid Processor ID ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_TASK_STARTUP;
    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_GetResourceName), &cInvalidTaskName, 
                     OS_strnlen(cInvalidTaskName, sizeof(cInvalidTaskName)), true);
    g_uiNumberOfProcessors = 4;
    
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - OS_EVENT_TASK_STARTUP Invalid Processor ID");

    /* ----- Test #6 - Nominal - OS_EVENT_TASK_STARTUP Processor ID > num of processors available ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_TASK_STARTUP;
    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_GetResourceName), &cValidTaskName, 
                     OS_strnlen(cValidTaskName, sizeof(cValidTaskName)), true);
    g_uiNumberOfProcessors = 1;
    
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - OS_EVENT_TASK_STARTUP Processor ID > num of processors available");

    /* ----- Test #7 - Nominal - OS_EVENT_TASK_STARTUP taskCpuAffinitySet fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_TASK_STARTUP;
    UT_SetDataBuffer(UT_KEY(OS_GetResourceName), &cValidTaskName, 
                     OS_strnlen(cValidTaskName, sizeof(cValidTaskName)), true);
    g_uiNumberOfProcessors = 4;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskCpuAffinitySet), 1, PCS_ERROR);
    
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - OS_EVENT_TASK_STARTUP taskCpuAffinitySet fail");

    /* ----- Test #8 - Unknown event ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    test_event = OS_EVENT_RESERVED;
    UT_SetDataBuffer(UT_KEY(OS_GetResourceName), &cValidTaskName, 
                     OS_strnlen(cValidTaskName, sizeof(cValidTaskName)), true);
    g_uiNumberOfProcessors = 4;
    UT_SetDeferredRetcode(UT_KEY(PCS_taskCpuAffinitySet), 1, PCS_ERROR);
    
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/4: Nominal - Unknown event");

}

/*=======================================================================================
** End of file psp_start_testcases.c
**=======================================================================================*/