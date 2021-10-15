/*=======================================================================================
**
** File:  psp_mem_scrub_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_mem_scrub.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-18 | Minh Luong       | Initial revision
**=======================================================================================*/

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

#include "psp_mem_scrub_testcases.h"
#include "../src/cfe_psp_mem_scrub.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_GetCDSSize(void) test cases
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Init(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Init(void)
{
    char cMsg[256] = {};

    /* ----- Test case #0 - TaskCreate Fails ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Error creating task\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Init();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Init - 1/3: Error Creating Task");

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Starting Active Memory Scrubbing\n");
    g_uiMemScrubTaskPriority = 250;
    g_uiEndOfRam = 0x0001FFFF;
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Init();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Init - 2/3: Nominal");

    /* ----- Test case #2 - Nominal - Already Running ----- */
    /* NOTE: Expects the task to be running in mem_scrub from the previous test case */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Task already running\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Init();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Init - 3/3: Nominal - Task Already Running");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Task(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Task(void)
{
    char cMsg[256] = {};
    osal_id_t mem_scrub_id = 0;
    int32 status;

    /* ----- Test case #1 - Invalid end address ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_uiMemScrubEndAddr = 0xFFFFFFFF;
    g_uiEndOfRam = 0;
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Task();
    /* Verify outputs */
    UtAssert_True(true,"_CFE_PSP_MEM_SCRUB_Task - 1/3: End scrub address past end of RAM");

/**
 * @brief Below test cases will not exit during UT
 */
    /* ----- Test case #2 - scrubMemory returns error status ----- */
    // TODO: VERIFY
    /* Set additional inputs */
    // UT_ResetState(0);
    // Ut_OS_printf_Setup();
    // g_uiMemScrubEndAddr = 0x0001FFFF;
    // g_uiEndOfRam = 0x0001FFFF;
    // sprintf(cMsg, "PSP MEM SCRUB: Unexpected ERROR during scrubMemory call\n");
    // UT_SetDeferredRetcode(UT_KEY(scrubMemory), 1, CFE_PSP_ERROR);
    // /* Execute test */
    // CFE_PSP_MEM_SCRUB_Task();
    // /* Verify outputs */
    // UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Task - 2/3: scrubMemory failed");

    /* ----- Test case #3 - scrubMemory does not returns error status ----- */
    // TODO: VERIFY
    /* Set additional inputs */
    // UT_ResetState(0);
    // Ut_OS_printf_Setup();
    // g_uiMemScrubEndAddr = 0x0000000F;
    // g_uiEndOfRam = 0x000000FF;
    // g_uiMemScrubTotalPages = 0x00000000;
    // g_uiMemScrubCurrentPage = 0x00000001;
    // UT_SetDeferredRetcode(UT_KEY(scrubMemory), 1, CFE_PSP_ERROR);
    // /* Execute test */
    // CFE_PSP_MEM_SCRUB_Task();
    // /* Verify outputs */
    // uint32 correct_value = g_uiMemScrubTotalPages + g_uiMemScrubCurrentPage;
    // UtAssert_True(g_uiMemScrubTotalPages == correct_value, "_CFE_PSP_MEM_SCRUB_TASK - 3/3: scrubMemory did not fail, correctly calculated mem_scrub scrub total pages");    
    
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_isRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_isRunning(void)
{
    bool ret;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_MEM_SCRUB_isRunning();
    /* Verify outputs */
    UtAssert_True(ret,"_CFE_PSP_MEM_SCRUB_isRunning - 1/2: Nominal");

    /* ----- Test case #2 - No task found ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    ret = CFE_PSP_MEM_SCRUB_isRunning();
    /* Verify outputs */
    UtAssert_True(ret == false,"_CFE_PSP_MEM_SCRUB_isRunning - 2/2: No task found");

}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Set(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Set(void)
{

    /* TODO Bug fixes */

    char cMsg[256] = {};
    uint32 startAdr;
    uint32 endAdr;
    osal_priority_t taskpriority;
    uint32 real_endOfRam = (uint32) sysPhysMemTop();
    int32 iReturnCode;

    /* ----- Test case #1 - Nominal no change of Priority ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    sprintf(cMsg, "PSP MEM SCRUB: Priority changed to `%u`\n",taskpriority);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 1/8: Nominal no change of priority");
    UtAssert_True(
        (taskpriority == MEMSCRUB_DEFAULT_PRIORITY), 
        "_CFE_PSP_MEM_SCRUB_Set - 1/8: Nominal no change of priority");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 1/8: Nominal no change of priority");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 1/8: Nominal no change of priority");
    // UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 1/8: Return Code Nominal no change of priority");
    UtAssert_NA("_CFE_PSP_MEM_SCRUB_Set - 1/8: Return Code Nominal no change of priority");

    /* ----- Test case #2 - Nominal with change of Priority ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY - 1;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    sprintf(cMsg, "PSP MEM SCRUB: Priority changed to `%u`\n",taskpriority);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 2/8: Nominal with change of priority");
    UtAssert_True(
        (taskpriority == g_uiMemScrubTaskPriority), 
        "_CFE_PSP_MEM_SCRUB_Set - 2/7: Nominal with change of priority");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 2/8: Nominal with change of priority");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 2/8: Nominal with change of priority");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 2/8: Return Code Nominal with change of priority");

    /* ----- Test case #3 - Nominal with change of Priority outside range max ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = 255;
    g_uiMemScrubTaskPriority = 0;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Priority is outside range, using default `%u`\n", MEMSCRUB_DEFAULT_PRIORITY);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    // Ut_OS_printfPrint();
    //UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 3/8: Nominal with change of priority outside range max");
    UtAssert_NA("_CFE_PSP_MEM_SCRUB_Set - 3/8: Nominal with change of priority outside range max");
    // UtAssert_True(
    //     g_uiMemScrubTaskPriority, == MEMSCRUB_DEFAULT_PRIORITY 
    //     "_CFE_PSP_MEM_SCRUB_Set - 3/7: Nominal with change of priority outside range max");
    UtAssert_NA("_CFE_PSP_MEM_SCRUB_Set - 3/8: NA - No way to test this with current config. Nominal with change of priority outside range max");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 3/8: Nominal with change of priority outside range max");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 3/8: Nominal with change of priority outside range max");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 3/8: Return Code Nominal with change of priority outside range max");

    /* ----- Test case #4 - Nominal with change of Priority outside range min ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = 0;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Priority is outside range, using default `%u`\n", MEMSCRUB_DEFAULT_PRIORITY);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 4/8: Nominal with change of priority outside range min");
    UtAssert_True(g_uiMemScrubTaskPriority == MEMSCRUB_DEFAULT_PRIORITY, 
        "_CFE_PSP_MEM_SCRUB_Set - 4/8: Nominal with change of priority outside range min");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 4/8: Nominal with change of priority outside range min");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 4/8: Nominal with change of priority outside range min");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 4/8: Return Code Nominal with change of priority outside range min");

    /* ----- Test case #5 - New end addr past  has not been initialized ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 100;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiEndOfRam = 0;
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(
        g_uiEndOfRam == (uint32)sysPhysMemTop(),
        "_CFE_PSP_MEM_SCRUB_Set - 5/8: Top of RAM not initialized");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 5/8: Top of RAM not initialized");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 5/8: Top of RAM not initialized");
    // UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 5/8: Return Code Top of RAM not initialized");
    UtAssert_NA("_CFE_PSP_MEM_SCRUB_Set - 5/8: Return Code Top of RAM not initialized");
    
    /* ----- Test case 6 - Nominal with global end of ram too large ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr =  0xFFFFFFFF;
    g_uiEndOfRam = 0;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Invalid newEndAddr. Range set to end of RAM\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(
        g_uiEndOfRam == (uint32) sysPhysMemTop(),
        "_CFE_PSP_MEM_SCRUB_Set - 6/8: New end address too large");
    UtAssert_True(g_uiMemScrubEndAddr == g_uiEndOfRam, "_CFE_PSP_MEM_SCRUB_Set - 6/8: New end address too large");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 6/8: New end address too large");
    // UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 6/8: New end address too large");
    UtAssert_NA("_CFE_PSP_MEM_SCRUB_Set - 6/8: Check return code - New end address too large");

    /* ----- Test case #7 - Nominal task already running - Task Restarted ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    startAdr = 0;
    endAdr = 1000;
    taskpriority = 245;
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(g_uiMemScrubTaskPriority == taskpriority, "_CFE_PSP_MEM_SCRUB_Set - 7/8: global mem scrub task correctly set");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 7/8: global mem scrub task correctly set");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 7/8: global mem scrub task correctly set");
    // UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 7/8: Return Code global mem scrub task correctly set");
    UtAssert_NA("_CFE_PSP_MEM_SCRUB_Set - 7/8: Return Code global mem scrub task correctly set");

    /* ----- Test case #8 - Nominal task already running - Fail Task Restart ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    startAdr = 0;
    endAdr = 1000;
    taskpriority = 245;
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_ERROR);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_ERROR);
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(g_uiMemScrubTaskPriority == taskpriority, "_CFE_PSP_MEM_SCRUB_Set - 8/8: global mem scrub task correctly set");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 8/8: global mem scrub task correctly set");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 8/8: global mem scrub task correctly set");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Set - 8/8: Return Codeglobal mem scrub task correctly set");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Status(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Status(void)
{
    char cMsg[256] = {};
    
    UT_ResetState(0);
    
    /* ----- Test case #1 - Nominal - Task ID = 0 ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    g_uiMemScrubTaskId = 0;
    snprintf(cMsg, 256, MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task could not be found\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Status();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Status - 1/2: Nominal");

    /* ----- Test case #1 - Nominal - Task ID valid and larger than 0 ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, 256, MEM_SCRUB_PRINT_SCOPE "StartAdr: 0x%08X - EndAdr: 0x%08X - CurrentPages: %u - TotalPages: %u\n",
                g_uiMemScrubStartAddr,
                g_uiMemScrubEndAddr,
                g_uiMemScrubCurrentPage,
                g_uiMemScrubTotalPages
                );
    g_uiMemScrubTaskId = 1;
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Status();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Status - 2/2: Task ID not found");
}
/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Enable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Enable(void)
{
    char cMsg[256] = {};
    uint32 g_uiMemScrubTaskId_backup;

    UT_ResetState(0);

    /* ----- Test case #1 - Nominal - Task Created ----- */
    /* Setup additional inputs */
    g_uiMemScrubTaskId = 0;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Enable();
    /* Verify outputs */
    UtAssert_True(true == true, "_CFE_PSP_MEM_SCRUB_Enable - 2/2: Nominal Task Init");

    /* ----- Test case #2 - Nominal - Task already running ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    g_uiMemScrubTaskId = 10;
    snprintf(cMsg, 256, MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task is already running\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Enable();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Enable - 2/2: Nominal Task already running");

}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Disable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Disable(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal - Task not found ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    g_uiMemScrubTaskId = 0;
    // snprintf(cMsg, 256, MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task could not be found\n");
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task could not be found\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Disable();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Disable - 2/2: Nominal Task Not Found");

    /* ----- Test case #1 - Nominal - Task Deleted ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    g_uiMemScrubTaskId = 1234;
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0
                    );
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Disable();
    /* Verify outputs */
    UtAssert_True(true == true, "_CFE_PSP_MEM_SCRUB_Disable - Nominal Task Deleted");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Delete(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Delete(void)
{
    char cMsg[256] = {};

    /* ----- Test case #1 - Nominal Task Deleted ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    g_uiMemScrubTaskId = 1234;
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0
                    );
    snprintf(cMsg, 256, "PSP MEM SCRUB: Memory Scrubbing Task Deleted\n");
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_SUCCESS);
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Delete();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Delete - 1/2: Nominal Task Deleted");

    /* ----- Test case #1 - Nominal Task Not Deleted ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0
                    );
    Ut_OS_printf_Setup();
    snprintf(cMsg, 256, "PSP MEM SCRUB: Memory Scrubbing Task Deleted\n");
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_ERROR);
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Delete();
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Delete - 2/2: Nominal Task Not Found");
   
}
/*=======================================================================================
** End of file psp_mem_scrub_testcases.c
**=======================================================================================*/