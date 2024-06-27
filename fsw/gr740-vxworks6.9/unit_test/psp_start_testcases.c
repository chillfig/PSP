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
*   \file psp_start_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_start.c.
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
#include <ioLib.h>

#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "target_config.h"
#include "cfe_psp.h"
#include "cfe_psp_start.h"

#include "psp_start_testcases.h"
#include "../src/cfe_psp_start.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern int volSensorRead(int8 sensor, uint8 dataType, float *voltage, bool talkative );
extern int tempSensorRead (int8 sensor, uint8 dataType, float *temperature, bool talkative );
extern char UserReservedMemory[URM_SIZE];
extern char *pEndOfURM;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_Main() test cases
**=======================================================================================*/
void Ut_CFE_PSP_Main(void)
{
    /* ----- Test case #1: - Previous instance of CFS found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), 10);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 0);
    UtAssert_NA("_CFE_PSP_Main - 1/3: Found previous instance of CFS");

    /* ----- Test case #2: - OS_BSPMain returns error ----- */
    UT_ResetState(0);
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), TASK_ID_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), OS_ERROR);
    /* Execute test */
    CFE_PSP_Main();
    /* Verify results */
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_NA("_CFE_PSP_Main - 2/3: OS_BSPMain returned error");

    /* ----- Test case #3: - Nominal ----- */
    UT_ResetState(0);
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), TASK_ID_ERROR);
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
void Ut_OS_Application_Startup(void)
{
    char cMsg[256] = {""};
    char cMsg_notice[] = "PSP: Application Startup Complete\n";
    char cMsg_mem_init_failed[] = "PSP: CFS is rebooting due to not having enough User Reserved Memory\n";
    uint32_t uiResetSrc = 0;
    uint64_t bitResult   = 1ULL;
    uint64_t bitExecuted = 3ULL;

    Ut_OS_printf_Setup();

    /* For CFE_PSP_InitProcessorReservedMemory */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(open), 99);
    UT_SetDefaultReturnValue(UT_KEY(write), 10);
    UT_SetDefaultReturnValue(UT_KEY(close), OK);
    
    /* For CFE_PSP_SetupReservedMemoryMap */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);

    /* For CFE_PSP_MemScrubInit */
    /* For CFE_PSP_MemSyncInit */
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemCreate), OS_ERROR);

    /* For CFE_PSP_SetSysTasksPrio */
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(taskPriorityGet), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(taskPrioritySet), OS_SUCCESS);

    /* For CFE_PSP_Reset */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 1/4: Successful startup notice message");
    UtAssert_STUB_COUNT(userReservedGet, 2);

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Failed to initialize URM ----- */
    /* Setup additional inputs */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), 100);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_SUCCESS);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 2/4: Missing startup notice message");
    UtAssert_OS_print(cMsg_mem_init_failed, "_OS_Application_Startup - 2/4: Initialization of User Reserved Memory failed message");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Initialize the OS API data structures failed ----- */
    /* Setup additional inputs */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    sprintf(cMsg, "PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 3/4: OS_API_Init error message");
    UtAssert_NoOS_print(cMsg_notice, "_OS_Application_Startup - 3/4: Successful startup notice message missing");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Fail to add FS mapping folder and fail to set priorities ----- */
    /* Setup additional inputs */
    pEndOfURM = UserReservedMemory;
    UT_SetDefaultReturnValue(UT_KEY(OS_API_Init), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_FileSysAddFixedMap), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(taskNameToId), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_ERROR);
    /* Execute test */
    UT_OS_Application_Startup();
    /* Verify outputs */
    sprintf(cMsg, "PSP: OS_FileSysAddFixedMap() failure: %d\n", OS_ERROR);
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/4: CFE_PSP_SetFileSysAddFixedMap error Message");
    sprintf(cMsg, "PSP: Some or all Virtual FS Mapping has failed\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/4: OS_Application_Startup error Message");
    sprintf(cMsg, "PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    UtAssert_OS_print(cMsg, "_OS_Application_Startup - 4/4: SetSysTasksPrio error Message");
    UtAssert_OS_print(cMsg_notice, "_OS_Application_Startup - 4/4: Successful startup notice message");
}

/*=======================================================================================
** Ut_CFE_PSP_SetTaskPrio() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetTaskPrio(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    int32 iNewPrio = 0;
    int32 iCurPrio = 10;
    char *tName = "tLogTask";
    char *tName_null = NULL;
    char *tName_too_long = "123456789012345678901234567890";

    /* ----- Test case #1 - Nominal Priority set to 0 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, "PSP: Setting %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), false);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OK);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from -1 to 0 */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 1/6: Nominal - Priority set to 0");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 1/6: Nominal - Priority set to 0 ");
    
    /* ----- Test case #2 - Nominal Priority set to 255 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 255;
    sprintf(cMsg, "PSP: Setting %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OK);
    /* Execute test */
    /* Test for task name "tLogTask". New priority will change from 260 to 255 */
    /* We cast 260 to uint8 type so ccppc doesn't give build error. Result is the same? */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 2/6: Nominal - Priority set to 255");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetTaskPrio - 2/6: Nominal - Priority set to 255");

    /* ----- Test case #3 - Failed to set priority to 25 for task name "tLogTask" ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iCurPrio = 0;
    iNewPrio = 25;
    sprintf(cMsg, "PSP: taskPrioritySet() - Failed for %s priority from %u to %u\n", tName, iCurPrio, iNewPrio);
    UT_SetDataBuffer(UT_KEY(taskPriorityGet), &iCurPrio, sizeof(iCurPrio), true);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
    /* Execute test */
    /* Test for task name "tLogTask" */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 3/6: Failed - Failed to set priority to 25");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 3/6: Failed - Failed to set priority to 25");

    /* ----- Test case #4 - Failed to set priority to 25 for task name "tLogTask",
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iCurPrio = 0;
    iNewPrio = 25;
    sprintf(cMsg, "PSP: Could not find task %s \n", tName);
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName, iNewPrio);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetTaskPrio - 4/6: Failed - Task ID lookup failure");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 4/6: Task ID lookup failure");

    /* ----- Test case #5 - task name is null,
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_null, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 5/6: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 5/6: Failed - task name is null does not print any message");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 5/6: Null task name returns error");

    /* ----- Test case #5 - task name is null,
     * taskNameToId failure -----
     */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    iNewPrio = 25;
    /* Execute test */
    iReturnCode = CFE_PSP_SetTaskPrio(tName_too_long, iNewPrio);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetTaskPrio - 6/6: No printed messages");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_SetTaskPrio - 6/6: Failed - task name array does not contain NULL terminating string");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_SetTaskPrio - 6/6: task name string without NULL returns error");
}

/*=======================================================================================
** Ut_CFE_PSP_SetSysTasksPrio() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetSysTasksPrio(void)
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
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, OS_ERROR);
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
void Ut_OS_Application_Run(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */

    /* Execute test */
    UT_OS_Application_Run();
    /* Verify outputs */
    UtAssert_NA("_OS_Application_Run - 1/1: NA - Function is empty");
}

/*=======================================================================================
** Ut_CFE_PSP_SuspendConsoleShellTask() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SuspendConsoleShellTask(void)
{
    bool suspend;
    int32 status;
    char cMsg[256] = {};

    /* ----- Test #1 - Nominal true, no task id, task suspend successful ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = true;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskSuspend), 1, OK);
    sprintf(cMsg, "Shell Task Suspended [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_SUCCESS, "_CFE_PSP_SuspendConsoleShellTask - 1/4: Task successfully suspended");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 1/4: Task successfully suspended");

    /* ----- Test #2 - true, no task id, task suspend failure ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = true;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskSuspend), 1, CFE_PSP_ERROR);
    sprintf(cMsg, "Shell Task could not be suspended [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 2/4: Task did not successfully suspend");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 2/4: Task did not successfully suspend");

    /* ----- Test #3 - Nominal false, no task id, task resume successful ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskResume), 1, OK);
    sprintf(cMsg, "Shell Task Resumed [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_SUCCESS, "_CFE_PSP_SuspendConsoleShellTask - 3/4: Task successfully resumed");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 3/4: Task successfully resumed");

    /* ----- Test #4 - true, no task id, task resume failure ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = 0;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskResume), 1, CFE_PSP_ERROR);
    sprintf(cMsg, "Shell Task could not be resumed [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");

    /* ----- Test #5 - Shell task id already acquired ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    suspend = false;
    g_uiShellTaskID = 1;
    UT_SetDeferredRetcode(UT_KEY(taskNameToId), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskResume), 1, CFE_PSP_ERROR);
    sprintf(cMsg, "Shell Task could not be resumed [0x%08X]\n",g_uiShellTaskID);
    /* Execute test */
    status = CFE_PSP_SuspendConsoleShellTask(suspend);
    /* Verify results */
    UtAssert_True(status == CFE_PSP_ERROR, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
    UtAssert_OS_print(cMsg, "_CFE_PSP_SuspendConsoleShellTask - 4/4: Task did not successfully resume");
}

/*=======================================================================================
** Ut_CFE_PSP_SetFileSysAddFixedMap() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetFileSysAddFixedMap(void)
{
    osal_id_t   fid;
    int32       ret = CFE_PSP_SUCCESS;
    char        cMsg[256] = {};

    sprintf(cMsg, "PSP: Set 1 Virtual Path(s)\n");

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
    snprintf(cMsg, 256, "PSP: OS_FileSysAddFixedMap() failure: %d\n", (int)CFE_PSP_ERROR);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Printed Error String");
    UtAssert_True(ret == CFE_PSP_ERROR, "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Virtual Mapping Returned Error");
}

/*=======================================================================================
** Ut_CFE_PSP_FindProcessor() test cases
**=======================================================================================*/
void Ut_CFE_PSP_FindProcessor(void)
{
    int32 iRetNumber = -99;
    char cTaskName[] = "temp";

    /* ----- Test #1 - Nominal - Not found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(strncmp), -1);
    /* Execute test */
    iRetNumber = CFE_PSP_FindProcessor(&cTaskName);
    /* Verify results */
    UtAssert_True(iRetNumber == -1, 
        "_CFE_PSP_SetFileSysAddFixedMap - 1/2: Nominal - Not Found");

    /* ----- Test #2 - Nominal - Found ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(strncmp), 0);
    /* Execute test */
    iRetNumber = CFE_PSP_FindProcessor(&cTaskName);
    /* Verify results */
    UtAssert_True(iRetNumber != -1,
        "_CFE_PSP_SetFileSysAddFixedMap - 2/2: Nominal - Entry Found");
}

/*=======================================================================================
** Ut_CFE_PSP_OS_EventHandler() test cases
**=======================================================================================*/
void Ut_CFE_PSP_OS_EventHandler(void)
{
    int32 iRetCode = OS_ERROR;
    OS_Event_t test_event = OS_EVENT_RESOURCE_ALLOCATED;
    osal_id_t test_object_id = OS_OBJECT_ID_UNDEFINED;
    char data[10] = "";

    /* ----- Test #1 - Nominal - OS_EVENT_RESOURCE_ALLOCATED ----- */
    /* Set additional inputs */
    test_event = OS_EVENT_RESOURCE_ALLOCATED;
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 1/1: Nominal - OS_EVENT_RESOURCE_ALLOCATED");

    /* ----- Test #2 - Nominal - OS_EVENT_RESOURCE_CREATED ----- */
    /* Set additional inputs */
    test_event = OS_EVENT_RESOURCE_CREATED;
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 2/1: Nominal - OS_EVENT_RESOURCE_CREATED");

    /* ----- Test #3 - Nominal - OS_EVENT_RESOURCE_DELETED ----- */
    /* Set additional inputs */
    test_event = OS_EVENT_RESOURCE_DELETED;
    /* Execute test */
    iRetCode = CFE_PSP_OS_EventHandler(test_event, test_object_id, &data);
    /* Verify results */
    UtAssert_True(iRetCode == OS_SUCCESS, 
        "_CFE_PSP_OS_EventHandler - 3/1: Nominal - OS_EVENT_RESOURCE_DELETED");

}

/*=======================================================================================
** End of file psp_start_testcases.c
**=======================================================================================*/
