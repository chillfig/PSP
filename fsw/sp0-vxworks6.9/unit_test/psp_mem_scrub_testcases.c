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
    int32 iReturnCode;
    char cMsg[256] = {};
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Unable to create binary semaphore ----- */
    /* Set additional inputs */
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to create semaphore\n");
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SEM_FAILURE);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Init();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Init - 1/3: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Init - 1/3: Unable to create semaphore - message");

    /* ----- Test case #2 - Unable to Enable mem scrub task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_semUpdateMemAddr_id = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to enable mem scrub task\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Init();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Init - 2/3: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Init - 2/3: Unable to enable mem scrub task - message");

    /* ----- Tes case #3 - Successfully enable memory scrub task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_semUpdateMemAddr_id = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Init();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Init - 3/3: Correct return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Task(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Task(void)
{
    char cMsg_scrub_error[] = {MEM_SCRUB_PRINT_SCOPE "Unexpected ERROR during scrubMemory call\n"};
    osal_id_t mem_scrub_id = 0;
    int32 status;
    uint32_t testPages;
    char cMsg[256] = {};

    /* ----- Test case #1 - No address update needed, scrubMemory returns an error ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    g_bScrubAddrUpdates_flag = false;
    g_uiMemScrubEndAddr = 0x0001FFFF;
    g_uiEndOfRam = 0x0001FFFF;
    UT_SetDeferredRetcode(UT_KEY(scrubMemory), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(scrubMemory), 1, OS_ERROR);
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Task();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_scrub_error, "_CFE_PSP_MEM_SCRUB_Task - 1/4: No mem address update, scrubMemory failed - message");

    /* ----- Test case #2 - Mem address update needed, scrubMemory returns an error ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    g_bScrubAddrUpdates_flag = true;
    g_uiMemScrubEndAddr = 0x0001FFFF;
    g_uiEndOfRam = 0x0001FFFF;
    UT_SetDeferredRetcode(UT_KEY(scrubMemory), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(scrubMemory), 1, OS_ERROR);
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Task();
    /* Verify outputs */
    UtAssert_OS_print(cMsg_scrub_error, "_CFE_PSP_MEM_SCRUB_Task - 2/4: Mem addresses update, scrubMemory failed - message");

    /* ----- Test case #3 - Unable to take semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    g_bScrubAddrUpdates_flag = true;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to take semaphore\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Task();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Task - 3/4: Unable to take binary semaphore - message");

    /* ----- Test case #4 - Unable to give semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    g_bScrubAddrUpdates_flag = true;
    g_uiMemScrubEndAddr = 99;
    g_uiEndOfRam = 99;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to give semaphore\n");
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Task();
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Task - 4/4: Unable to give binary semaphore - message");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_isRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_isRunning(void)
{
    bool ret;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    /* Execute test */
    ret = CFE_PSP_MEM_SCRUB_isRunning();
    /* Verify outputs */
    UtAssert_True(ret,"_CFE_PSP_MEM_SCRUB_isRunning - 1/2: Nominal - return value");

    /* ----- Test case #2 - No task found ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    ret = CFE_PSP_MEM_SCRUB_isRunning();
    /* Verify outputs */
    UtAssert_True(ret == false,"_CFE_PSP_MEM_SCRUB_isRunning - 2/2: No task found - return value");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Set(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Set(void)
{
    char cMsg[256] = {};
    uint32 startAdr;
    uint32 endAdr;
    osal_priority_t taskpriority;
    uint32 real_endOfRam = (uint32) sysPhysMemTop();
    int32 iReturnCode;

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal no change of Priority ----- */
    /* Setup additional inputs */
    startAdr = 0;
    endAdr = 1000;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiEndOfRam = real_endOfRam;
    sprintf(cMsg, "PSP MEM SCRUB: Priority changed to `%u`\n",taskpriority);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 1/10: Nominal - change of priority - no message");
    UtAssert_True(
        (taskpriority == MEMSCRUB_DEFAULT_PRIORITY), 
        "_CFE_PSP_MEM_SCRUB_Set - 1/10: Nominal - confirm no change of priority");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 1/10: Nominal - priority same - confirm start addr");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 1/10: Nominal - priority same - confirm end addr");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 1/10: Nominal - priority same - return value");

    /* ----- Test case #2 - Nominal with change of Priority ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY - 1;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiEndOfRam = real_endOfRam;
    sprintf(cMsg, "PSP MEM SCRUB: Priority changed to `%u`\n",taskpriority);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 2/10: Nominal - priority changed - no message");
    UtAssert_True(
        (taskpriority == g_uiMemScrubTaskPriority), 
        "_CFE_PSP_MEM_SCRUB_Set - 2/10: Nominal - confirm change of priority");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 2/10: Nominal - priority changed - confirm start addr");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 2/10: Nominal - priority changed - confirm end addr");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 2/10: Nominal - priority changes - return value");

    /* ----- Test case #3 - Nominal with change of Priority outside range max ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = 255;
    g_uiMemScrubTaskPriority = 0;
    g_uiEndOfRam = real_endOfRam;
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    if (taskpriority > MEMSCRUB_PRIORITY_UP_RANGE)
    {
        UtAssert_True(g_uiMemScrubTaskPriority == MEMSCRUB_DEFAULT_PRIORITY,
            "_CFE_PSP_MEM_SCRUB_Set - 3/7: Nominal - priority changed set to default");
    }
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 3/10: Nominal - priority changed max - confirm start addr");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 3/10: Nominal - priority changed max - confirm end addr");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 3/10: Nominal - priority changes max - return value");

    /* ----- Test case #4 - Nominal with change of Priority outside range min ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = 0;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiEndOfRam = real_endOfRam;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Priority is outside range, using default `%u`\n", MEMSCRUB_DEFAULT_PRIORITY);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 4/10: Nominal - priority outside range min - message");
    UtAssert_True(g_uiMemScrubTaskPriority == MEMSCRUB_DEFAULT_PRIORITY,
        "_CFE_PSP_MEM_SCRUB_Set - 4/10: Nominal - priority changed set to default");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 4/10: Nominal - priority changed min - confirm start addr");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 4/10: Nominal - priority changed min - confirm end addr");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 4/10: Nominal - priority changes max - return value");

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
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(
        g_uiEndOfRam == (uint32)sysPhysMemTop(),
        "_CFE_PSP_MEM_SCRUB_Set - 5/10: Nominal - top of RAM init changed");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 5/10: Nominal - top of RAM init - confirm start addr");
    UtAssert_True(g_uiMemScrubEndAddr == endAdr, "_CFE_PSP_MEM_SCRUB_Set - 5/10: Nominal - top of RAM init - confirm end addr");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 5/10: Nominal - top of RAM init - return value");
    
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
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Invalid newEndAddr. Range set to end of RAM\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(
        g_uiEndOfRam == (uint32) sysPhysMemTop(),
        "_CFE_PSP_MEM_SCRUB_Set - 6/10: Nominal - end address too large");
    UtAssert_True(g_uiMemScrubStartAddr == startAdr, "_CFE_PSP_MEM_SCRUB_Set - 6/10: Nominal - start address too large - confirm start addr");
    UtAssert_True(g_uiMemScrubEndAddr == g_uiEndOfRam, "_CFE_PSP_MEM_SCRUB_Set - 6/10: Nominal - end address changed to default");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Set - 6/10: Nominal - end address too large - return value");


    /* ----- Test case #7 - newStartAddr is larger than newEndAddr ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "[ERR] Start address after end address\n");
    startAdr = 1000;
    endAdr = 10;
    taskpriority = 245;
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 7/10: Start address after end address");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Set - 7/10: Check return value error");

    /* Test case #8 - binary semaphore not created ----- */
    /* Set additional input */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_semUpdateMemAddr_id = OS_OBJECT_ID_UNDEFINED;
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Set(0, 0, 0);
    /* Verify results */
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Binary semaphore not created\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(0, 0, 0);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Set - 8/10: Binary semaphore not created, correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 8/10: Binary semaphore was never created - message");

    /* Test case #9 - Unable to take semaphore ----- */
    /* Set additional input */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to take binary semaphore\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(0, 0, 0);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCUB_Set - 9/10: Unable to take bsem, correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 9/10: Unable to take binary semaphore - message");

    /* Test case #10 - Unable to give semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    startAdr = 0;
    endAdr = 1000;
    taskpriority = MEMSCRUB_DEFAULT_PRIORITY;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to give semaphore\n");
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Set(startAdr, endAdr, taskpriority);
    /* Verify outputs */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Set - 10/10: Unable to give bsem, correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Set - 10/10: Unable to give binary semaphore - message");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Status(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Status(void)
{
    char cMsg[256] = {};
    MEM_SCRUB_STATUS_t mss_Status = {};
    g_uiMemScrubStartAddr = 99;
    g_uiMemScrubEndAddr = 99;
    g_uiMemScrubCurrentPage = 99;
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubTaskPriority = 99;

    /* ----- Test case #1 - Nominal, talk ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, 256, MEM_SCRUB_PRINT_SCOPE "StartAdr: 0x%08X - EndAdr: 0x%08X\n" \
                                              "CurrentPages: %u - TotalPages: %u\n" \
                                              "Scrub Task Priority: %u\n",
                g_uiMemScrubStartAddr,
                g_uiMemScrubEndAddr,
                g_uiMemScrubCurrentPage,
                g_uiMemScrubTotalPages,
                g_uiMemScrubTaskPriority
                );
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Status(&mss_Status, true);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Status - 1/1: Status print - message");
    UtAssert_True(mss_Status.uiMemScrubStartAddr == g_uiMemScrubStartAddr, "_CFE_PSP_MEM_SCRUB_Status - 1/1: Start address value test");
    UtAssert_True(mss_Status.uiMemScrubEndAddr == g_uiMemScrubEndAddr, "_CFE_PSP_MEM_SCRUB_Status - 1/1: End address value test");
    UtAssert_True(mss_Status.uiMemScrubCurrentPage == g_uiMemScrubCurrentPage, "_CFE_PSP_MEM_SCRUB_Status - 1/1: Current page value test");
    UtAssert_True(mss_Status.uiMemScrubTotalPages == g_uiMemScrubTotalPages, "_CFE_PSP_MEM_SCRUB_Status - 1/1: Total pages value test");
    UtAssert_True(mss_Status.opMemScrubTaskPriority == g_uiMemScrubTaskPriority, "_CFE_PSP_MEM_SCRUB_Status - 1/1: Task priority value test");

    /* ----- Test case #2 - Nominal, no talk ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    snprintf(cMsg, 256, MEM_SCRUB_PRINT_SCOPE "StartAdr: 0x%08X - EndAdr: 0x%08X\n" \
                                              "CurrentPages: %u - TotalPages: %u\n" \
                                              "Scrub Task Priority: %u\n",
                g_uiMemScrubStartAddr,
                g_uiMemScrubEndAddr,
                g_uiMemScrubCurrentPage,
                g_uiMemScrubTotalPages,
                g_uiMemScrubTaskPriority
                );
    /* Execute test */
    CFE_PSP_MEM_SCRUB_Status(&mss_Status, false);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Status - 1/1: Status print - message");
    UtAssert_True(mss_Status.uiMemScrubStartAddr == g_uiMemScrubStartAddr, "_CFE_PSP_MEM_SCRUB_Status - 2/2: Start address value test");
    UtAssert_True(mss_Status.uiMemScrubEndAddr == g_uiMemScrubEndAddr, "_CFE_PSP_MEM_SCRUB_Status - 2/2: End address value test");
    UtAssert_True(mss_Status.uiMemScrubCurrentPage == g_uiMemScrubCurrentPage, "_CFE_PSP_MEM_SCRUB_Status - 2/2: Current page value test");
    UtAssert_True(mss_Status.uiMemScrubTotalPages == g_uiMemScrubTotalPages, "_CFE_PSP_MEM_SCRUB_Status - 2/2: Total pages value test");
    UtAssert_True(mss_Status.opMemScrubTaskPriority == g_uiMemScrubTaskPriority, "_CFE_PSP_MEM_SCRUB_Status - 2/2: Task priority value test");
}
/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Enable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Enable(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Task already running ----- */
    /* Set additional inputs */
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task is already running\n");
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Enable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Enable - 1/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Enable - 1/4: Memory scrub task alrready running - message");

    /* ----- Test case #2 - Task not running, binary semaphore not created ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_semUpdateMemAddr_id = OS_OBJECT_ID_UNDEFINED;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Must create binary semaphore before starting mem scrub task\n");
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Enable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Enable - 2/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Enable - 2/4: Must create bin sem error - message");

    /* ----- Test case #3 - Unsuccessfully start mem scrub task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_semUpdateMemAddr_id = 99;
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Error creating memory scrub task\n");
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Enable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Enable - 3/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Enable - 3/4: Error creating memory scrub task - message");

    /* ----- Test case #4 - Succesfully create mem scrub task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    g_semUpdateMemAddr_id = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Enable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Enable - 4/4: Correct return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Disable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Disable(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};

    /* ----- Test case #1 - Task is not running ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Memory scrub task is not running\n");
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubTaskId = 99;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Disable - 1/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Disable - 1/5: Memory scrub task is not running - message");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Disable - 1/5: Memory scrub total pages reset");
    UtAssert_True(g_uiMemScrubTaskId == 0,"_CFE_PSP_MEM_SCRUB_Disable - 1/5: Memory scrub task ID reset");

    /* ----- Test case #2 - Fail to take semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to take semaphore\n");
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubTaskId = 99;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Disable - 2/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Disable - 2/5: Unable to take semaphore - message");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Disable - 2/5: Memory scrub total pages reset");
    UtAssert_True(g_uiMemScrubTaskId == 0,"_CFE_PSP_MEM_SCRUB_Disable - 2/5: Memory scrub task ID reset");

    /* ----- Test case #3 - Fail to delete task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERROR);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to delete memory scrub task\n");
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubTaskId = 99;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Disable - 3/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Disable - 3/5: Unable to delete memory scrub task - message");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Disable - 3/5: Memory scrub total pages reset");
    UtAssert_True(g_uiMemScrubTaskId == 0,"_CFE_PSP_MEM_SCRUB_Disable - 3/5: Memory scrub task ID reset");
    
    /* ----- Test case #4 - Fail release semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to release semaphore\n");
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubTaskId = 99;
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Disable - 4/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Disable - 4/5: Unable to release semaphore - message");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Disable - 4/5: Memory scrub total pages reset");
    UtAssert_True(g_uiMemScrubTaskId == 0,"_CFE_PSP_MEM_SCRUB_Disable - 4/5: Memory scrub task ID reset");
    
    /* ----- Test case #5 - Successful disable ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    g_uiMemScrubTotalPages = 99;
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Disable();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Disable - 5/5: Correct return code");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Disable - 5/5: Memory scrub total pages reset");
    UtAssert_True(g_uiMemScrubTaskId == 0,"_CFE_PSP_MEM_SCRUB_Disable - 5/5: Memory scrub task ID reset");
}

/*=======================================================================================
** Ut_CFE_PSP_MEM_SCRUB_Delete(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEM_SCRUB_Delete(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - SCRUB_Disable failure ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to disable mem scrub task\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Delete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Delete - 1/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Delete - 1/4: Unable to disable mem scrub task - message");

    /* ----- Test case #2 - Unable to take semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to take semaphore\n");
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0);
    g_uiEndOfRam = (uint32) sysPhysMemTop();
    g_uiMemScrubCurrentPage = 99;
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubStartAddr = 99;
    g_uiMemScrubEndAddr = g_uiEndOfRam - 1;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Delete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Correect return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Unable to take semaphore - message");
    UtAssert_True(g_uiMemScrubTaskId == 0, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Mem scrub task id reset");
    UtAssert_True(g_uiMemScrubCurrentPage == 0, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Mem scrub current page reset");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Mem scrub total pages reset");
    UtAssert_True(g_uiMemScrubStartAddr == 0, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Mem scrub start address reset");
    UtAssert_True(g_uiMemScrubEndAddr == g_uiEndOfRam, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Mem scrub end address reset");
    UtAssert_True(g_uiMemScrubTaskPriority == MEMSCRUB_DEFAULT_PRIORITY, "_CFE_PSP_MEM_SCRUB_Delete - 2/4: Mem scrub task priority reset");

    /* ----- Test case #3 - Unable to delete semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEM_SCRUB_PRINT_SCOPE "Unable to delete semaphore\n");
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0);
    g_uiEndOfRam = (uint32) sysPhysMemTop();
    g_uiMemScrubCurrentPage = 99;
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubStartAddr = 99;
    g_uiMemScrubEndAddr = g_uiEndOfRam - 1;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Delete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Unable to delete semaphore - message");
    UtAssert_True(g_uiMemScrubTaskId == 0, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Mem scrub task id reset");
    UtAssert_True(g_uiMemScrubCurrentPage == 0, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Mem scrub current page reset");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Mem scrub total pages reset");
    UtAssert_True(g_uiMemScrubStartAddr == 0, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Mem scrub start address reset");
    UtAssert_True(g_uiMemScrubEndAddr == g_uiEndOfRam, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Mem scrub end address reset");
    UtAssert_True(g_uiMemScrubTaskPriority == MEMSCRUB_DEFAULT_PRIORITY, "_CFE_PSP_MEM_SCRUB_Delete - 3/4: Mem scrub task priority reset");

    /* ----- Test case #4 - Successful scrub delete ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, PSP_MEM_SCRUB_BSEM_NAME, OS_SEM_FULL, 0);
    OS_TaskCreate(&g_uiMemScrubTaskId, 
                    MEMSCRUB_TASK_NAME, 
                    CFE_PSP_MEM_SCRUB_Task,
                    OSAL_TASK_STACK_ALLOCATE, 
                    OSAL_SIZE_C(1024), 
                    g_uiMemScrubTaskPriority, 
                    0);
    g_uiEndOfRam = (uint32) sysPhysMemTop();
    g_uiMemScrubCurrentPage = 99;
    g_uiMemScrubTotalPages = 99;
    g_uiMemScrubStartAddr = 99;
    g_uiMemScrubEndAddr = g_uiEndOfRam - 1;
    g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_MEM_SCRUB_Delete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Correct return code");
    UtAssert_True(g_uiMemScrubTaskId == 0, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Mem scrub task id reset");
    UtAssert_True(g_uiMemScrubCurrentPage == 0, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Mem scrub current page reset");
    UtAssert_True(g_uiMemScrubTotalPages == 0, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Mem scrub total pages reset");
    UtAssert_True(g_uiMemScrubStartAddr == 0, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Mem scrub start address reset");
    UtAssert_True(g_uiMemScrubEndAddr == g_uiEndOfRam, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Mem scrub end address reset");
    UtAssert_True(g_uiMemScrubTaskPriority == MEMSCRUB_DEFAULT_PRIORITY, "_CFE_PSP_MEM_SCRUB_Delete - 4/4: Mem scrub task priority reset");
}
/*=======================================================================================
** End of file psp_mem_scrub_testcases.c
**=======================================================================================*/