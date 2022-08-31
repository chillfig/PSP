/*=======================================================================================
**
** File:  psp_memscrub_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_memscrub.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
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

#include "psp_memscrub_testcases.h"
#include "../src/cfe_psp_memscrub.c"

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
** Ut_CFE_PSP_MemScrubInit(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubInit(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    Ut_OS_printf_Setup();

    g_MemScrub_Status.RunMode = MEMSCRUB_IDLE_MODE;
    g_bMemScrubStartOnStartup = true;

    /* ----- Test case #1 - Unable to create binary semaphore OS_SEM_FAILURE ----- */
    /* Set additional inputs */
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to create semaphore, OSAL ERROR %d\n", OS_SEM_FAILURE);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SEM_FAILURE);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubInit - 1/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubInit - 1/5: Unable to create semaphore OS_SEM_FAILURE - message");

    /* ----- Test case #2 - Unable to create binary semaphore already created ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Already Initialized\n");
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_ERR_NAME_TAKEN);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubInit - 2/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubInit - 2/5: Unable to create semaphore OS_ERR_NAME_TAKEN - message");

    /* ----- Test case #3 - Unable to Enable mem scrub task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_semUpdateMemAddr_id = OS_ObjectIdFromInteger(99);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to enable mem scrub task\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubInit - 3/5: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubInit - 3/5: Unable to enable mem scrub task - message");

    /* ----- Tes case #4 - Successfully enable memory scrub task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_semUpdateMemAddr_id = OS_ObjectIdFromInteger(99);
    g_MemScrub_Status.uiMemScrubEndAddr = 0;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MemScrubInit - 4/5: Correct return code");

    /* ----- Tes case #5 - Successfully initialized memory scrub run mode 3 ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_MemScrub_Status.RunMode = MEMSCRUB_MANUAL_MODE;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_semUpdateMemAddr_id = OS_ObjectIdFromInteger(99);
    g_MemScrub_Status.uiMemScrubEndAddr = 1;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MemScrubInit - 5/5: Correct return code");
    UtAssert_True((Ut_OS_printf_MsgCount() == 0), "_CFE_PSP_MemScrubInit - 5/5: No message output");
}


/*=======================================================================================
** Ut_CFE_PSP_MemScrubSet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubSet(void)
{
    char cMsg[256] = {};
    CFE_PSP_MemScrubStatus_t newMemScrubConfig = {0};
    uint32 real_endOfRam = (uint32) sysPhysMemTop();
    int32 iReturnCode;

    /* Set global defaults */
    g_MemScrub_Status.RunMode = MEMSCRUB_IDLE_MODE;
    g_MemScrub_Status.uiBlockSize_Pages = 10;
    g_MemScrub_Status.uiTaskDelay_msec = 500;
    g_MemScrub_Status.uiMemScrub_MaxPriority = 250;
    g_MemScrub_Status.uiMemScrub_MinPriority = 200;
    g_MemScrub_Status.opMemScrubTaskPriority = 245;
    g_MemScrub_Status.uiMemScrubStartAddr = 10;
    g_MemScrub_Status.uiMemScrubEndAddr = 100;
    g_uiEndOfRam = real_endOfRam;

    /* Copy current global default configuration to new configuration */
    memcpy(&newMemScrubConfig, &g_MemScrub_Status, sizeof(newMemScrubConfig));

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal no change of Priority ----- */
    /* Setup additional inputs */
    newMemScrubConfig.uiMemScrubStartAddr = 0;
    newMemScrubConfig.uiMemScrubEndAddr = 1000;
    sprintf(cMsg, "PSP MEM SCRUB: Priority changed to `%u`\n", newMemScrubConfig.opMemScrubTaskPriority);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MemScrubSet - 1/6: Nominal - change of priority - no message");
    UtAssert_True(
        (newMemScrubConfig.opMemScrubTaskPriority == 245), 
        "_CFE_PSP_MemScrubSet - 1/6: Nominal - confirm no change of priority");
    UtAssert_True((g_MemScrub_Status.uiMemScrubStartAddr == newMemScrubConfig.uiMemScrubStartAddr), "_CFE_PSP_MemScrubSet - 1/6: Nominal - priority same - confirm start addr");
    UtAssert_True((g_MemScrub_Status.uiMemScrubEndAddr == newMemScrubConfig.uiMemScrubEndAddr), "_CFE_PSP_MemScrubSet - 1/6: Nominal - priority same - confirm end addr");
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubSet - 1/6: Nominal - priority same - return value");

    /* ----- Test case #2 - Nominal with change of Priority ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    newMemScrubConfig.opMemScrubTaskPriority = 230;
    sprintf(cMsg, "PSP MEM SCRUB: Priority changed to `%u`\n", newMemScrubConfig.opMemScrubTaskPriority);
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MemScrubSet - 2/6: Nominal - priority changed - no message");
    UtAssert_True(
        (newMemScrubConfig.opMemScrubTaskPriority == g_MemScrub_Status.opMemScrubTaskPriority), 
        "_CFE_PSP_MemScrubSet - 2/6: Nominal - confirm change of priority");
    UtAssert_True((g_MemScrub_Status.uiMemScrubStartAddr == newMemScrubConfig.uiMemScrubStartAddr), "_CFE_PSP_MemScrubSet - 2/6: Nominal - priority changed - confirm start addr");
    UtAssert_True((g_MemScrub_Status.uiMemScrubEndAddr == newMemScrubConfig.uiMemScrubEndAddr), "_CFE_PSP_MemScrubSet - 2/6: Nominal - priority changed - confirm end addr");
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubSet - 2/6: Nominal - priority changes - return value");

    /* ----- Test case #3 - Does not pass validation ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    newMemScrubConfig.opMemScrubTaskPriority = 255;
    g_MemScrub_Status.opMemScrubTaskPriority = 0;
    UT_SetDeferredRetcode(UT_KEY(taskPriorityGet), 1, CFE_PSP_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(taskPrioritySet), 1, CFE_PSP_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "New configuration did not pass validation\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((g_MemScrub_Status.opMemScrubTaskPriority == 0), "_CFE_PSP_MemScrubSet - 3/6: Does not pass validation - priority not changed");
    UtAssert_True((g_MemScrub_Status.uiMemScrubStartAddr == newMemScrubConfig.uiMemScrubStartAddr), "_CFE_PSP_MemScrubSet - 3/6: Does not pass validation - start addr not changed");
    UtAssert_True((g_MemScrub_Status.uiMemScrubEndAddr == newMemScrubConfig.uiMemScrubEndAddr), "_CFE_PSP_MemScrubSet - 3/6: Does not pass validation - end addr not changed");
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubSet - 3/6: Does not pass validation - return value");

    /* ----- Test case #4 - binary semaphore not created ----- */
    /* Set additional input */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_semUpdateMemAddr_id = OS_OBJECT_ID_UNDEFINED;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Binary semaphore not created\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubSet - 4/6: Binary semaphore not created, correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubSet - 4/6: Binary semaphore was never created - message");

    /* ----- Test case #5 - Unable to take semaphore ----- */
    /* Set additional input */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to take binary semaphore\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubSet - 5/6: Unable to take bsem, correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubSet - 5/6: Unable to take binary semaphore - message");

    /* ----- Test case #6 - Unable to give semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    newMemScrubConfig.opMemScrubTaskPriority = 245;
    g_MemScrub_Status.opMemScrubTaskPriority = 245;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to give semaphore\n");
    OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_EMPTY, 0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubSet - 6/6: Unable to give bsem, correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubSet - 6/6: Unable to give binary semaphore - message");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubValidate(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubValidate(void)
{
    char cMsg[256] = {};
    CFE_PSP_MemScrubStatus_t newMemScrubConfig;
    int32 iReturnCode;

    newMemScrubConfig.uiMemScrubEndAddr = 100;
    newMemScrubConfig.uiMemScrubStartAddr = 1;
    newMemScrubConfig.opMemScrubTaskPriority = 200;
    newMemScrubConfig.uiMemScrub_MinPriority = 100;
    newMemScrubConfig.uiMemScrub_MaxPriority = 250;
    newMemScrubConfig.uiBlockSize_Pages = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubValidate - 1/6: Nominal - Return code");
    UtAssert_True((Ut_OS_printf_MsgCount() == 0), "_CFE_PSP_MemScrubValidate - 1/6: Nominal - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Bad address range ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 1;
    newMemScrubConfig.uiMemScrubStartAddr = 100;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Start address after end address\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 2/6: Bad address range - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubValidate - 2/6: Bad address range - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - end of RAM not set ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 100;
    newMemScrubConfig.uiMemScrubStartAddr = 1;
    g_uiEndOfRam = 0;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubValidate - 3/6: end of RAM not set - Return code");
    UtAssert_True((Ut_OS_printf_MsgCount() == 0), "_CFE_PSP_MemScrubValidate - 3/6: end of RAM not set - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - end address too big ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 0xFFFFFFFF;
    newMemScrubConfig.uiMemScrubStartAddr = 100;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Invalid new end address\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 4/6: end address too big - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubValidate - 4/6: end address too big - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #5 - priority outside range down ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 100;
    newMemScrubConfig.uiMemScrubStartAddr = 1;
    newMemScrubConfig.opMemScrubTaskPriority = 10;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Priority is outside range\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 5/6: priority outside range down - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubValidate - 5/6: priority outside range down - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #5 - priority outside range up ----- */
    newMemScrubConfig.opMemScrubTaskPriority = 255;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Priority is outside range\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 5/6: priority outside range up - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubValidate - 5/6: priority outside range up - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #6 - Block size too large ----- */
    newMemScrubConfig.uiBlockSize_Pages = 1234567;
    newMemScrubConfig.opMemScrubTaskPriority = 240;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Incorrect block size\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 6/6: Block size too large - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubValidate - 6/6: Block size too large - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #6 - Block size too small ----- */
    newMemScrubConfig.uiBlockSize_Pages = 0;
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Incorrect block size\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 6/6: Block size too small - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubValidate - 6/6: Block size too small - No messages");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubGet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubGet(void)
{
    char cMsg[256] = {};
    CFE_PSP_MemScrubStatus_t mss_Status = {};
    g_MemScrub_Status.uiMemScrubStartAddr = 99;
    g_MemScrub_Status.uiMemScrubEndAddr = 99;
    g_MemScrub_Status.uiMemScrubCurrentPage = 99;
    g_MemScrub_Status.uiMemScrubTotalPages = 99;
    g_MemScrub_Status.opMemScrubTaskPriority = 99;

    /* ----- Test case #1 - Nominal, talk ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, 256, MEMSCRUB_PRINT_SCOPE "Mode %u\n" \
                                            "Address Range [0x%08X-0x%08X]\n" \
                                            "Timed [0x%08X-0x%08X] - Delay %u msec - Blocks %u bytes\n" \
                                            "Cur Pages: %u - Total Pages: %u\n" \
                                            "Priority %u\n",
                                            g_MemScrub_Status.RunMode,
                                            g_MemScrub_Status.uiMemScrubStartAddr,
                                            g_MemScrub_Status.uiMemScrubEndAddr,
                                            g_MemScrub_Status.uiTimedStartAddr,
                                            g_MemScrub_Status.uiTimedEndAddr,
                                            g_MemScrub_Status.uiTaskDelay_msec,
                                            g_MemScrub_Status.uiBlockSize_Pages,
                                            g_MemScrub_Status.uiMemScrubCurrentPage,
                                            g_MemScrub_Status.uiMemScrubTotalPages,
                                            g_MemScrub_Status.opMemScrubTaskPriority
                );
    /* Execute test */
    CFE_PSP_MemScrubGet(&mss_Status, true);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubStatus - 1/1: Status print - message");
    UtAssert_True(mss_Status.uiMemScrubStartAddr == g_MemScrub_Status.uiMemScrubStartAddr, "_CFE_PSP_MemScrubStatus - 1/1: Start address value test");
    UtAssert_True(mss_Status.uiMemScrubEndAddr == g_MemScrub_Status.uiMemScrubEndAddr, "_CFE_PSP_MemScrubStatus - 1/1: End address value test");
    UtAssert_True(mss_Status.uiMemScrubCurrentPage == g_MemScrub_Status.uiMemScrubCurrentPage, "_CFE_PSP_MemScrubStatus - 1/1: Current page value test");
    UtAssert_True(mss_Status.uiMemScrubTotalPages == g_MemScrub_Status.uiMemScrubTotalPages, "_CFE_PSP_MemScrubStatus - 1/1: Total pages value test");
    UtAssert_True(mss_Status.opMemScrubTaskPriority == g_MemScrub_Status.opMemScrubTaskPriority, "_CFE_PSP_MemScrubStatus - 1/1: Task priority value test");

    /* ----- Test case #2 - Nominal, no talk ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Execute test */
    CFE_PSP_MemScrubGet(&mss_Status, false);
    /* Verify outputs */
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MemScrubStatus - 1/1: Status print - message");
    UtAssert_True(mss_Status.uiMemScrubStartAddr == g_MemScrub_Status.uiMemScrubStartAddr, "_CFE_PSP_MemScrubStatus - 2/2: Start address value test");
    UtAssert_True(mss_Status.uiMemScrubEndAddr == g_MemScrub_Status.uiMemScrubEndAddr, "_CFE_PSP_MemScrubStatus - 2/2: End address value test");
    UtAssert_True(mss_Status.uiMemScrubCurrentPage == g_MemScrub_Status.uiMemScrubCurrentPage, "_CFE_PSP_MemScrubStatus - 2/2: Current page value test");
    UtAssert_True(mss_Status.uiMemScrubTotalPages == g_MemScrub_Status.uiMemScrubTotalPages, "_CFE_PSP_MemScrubStatus - 2/2: Total pages value test");
    UtAssert_True(mss_Status.opMemScrubTaskPriority == g_MemScrub_Status.opMemScrubTaskPriority, "_CFE_PSP_MemScrubStatus - 2/2: Task priority value test");
}


/*=======================================================================================
** Ut_CFE_PSP_MemScrubDelete(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubDelete(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};
    Ut_OS_printf_Setup();

    /* ----- Test case #1 - SCRUB_Disable failure ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to disable mem scrub task\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubDelete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubDelete - 1/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubDelete - 1/4: Unable to disable mem scrub task - message");

    /* ----- Test case #2 - Unable to take semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to take semaphore\n");

    g_uiEndOfRam = (uint32) sysPhysMemTop();
    g_MemScrub_Status.uiMemScrubCurrentPage = 99;
    g_MemScrub_Status.uiMemScrubTotalPages = 99;
    g_MemScrub_Status.uiMemScrubStartAddr = 99;
    g_MemScrub_Status.uiMemScrubEndAddr = g_uiEndOfRam - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubDelete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubDelete - 2/4: Correect return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubDelete - 2/4: Unable to take semaphore - message");
    UtAssert_True(g_MemScrub_Status.uiMemScrubCurrentPage == 0, "_CFE_PSP_MemScrubDelete - 2/4: Mem scrub current page reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubTotalPages == 0, "_CFE_PSP_MemScrubDelete - 2/4: Mem scrub total pages reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubStartAddr == MEMSCRUB_DEFAULT_START_ADDR, "_CFE_PSP_MemScrubDelete - 2/4: Mem scrub start address reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubEndAddr == MEMSCRUB_DEFAULT_END_ADDR, "_CFE_PSP_MemScrubDelete - 2/4: Mem scrub end address reset");

    /* ----- Test case #3 - Unable to delete semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMSCRUB_PRINT_SCOPE "Unable to delete semaphore\n");
    g_uiEndOfRam = (uint32) sysPhysMemTop();
    g_MemScrub_Status.uiMemScrubCurrentPage = 99;
    g_MemScrub_Status.uiMemScrubTotalPages = 99;
    g_MemScrub_Status.uiMemScrubStartAddr = 99;
    g_MemScrub_Status.uiMemScrubEndAddr = g_uiEndOfRam - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubDelete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubDelete - 3/4: Correct return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubDelete - 3/4: Unable to delete semaphore - message");
    UtAssert_True(g_MemScrub_Status.uiMemScrubCurrentPage == 0, "_CFE_PSP_MemScrubDelete - 3/4: Mem scrub current page reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubTotalPages == 0, "_CFE_PSP_MemScrubDelete - 3/4: Mem scrub total pages reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubStartAddr == MEMSCRUB_DEFAULT_START_ADDR, "_CFE_PSP_MemScrubDelete - 3/4: Mem scrub start address reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubEndAddr == MEMSCRUB_DEFAULT_END_ADDR, "_CFE_PSP_MemScrubDelete - 3/4: Mem scrub end address reset");

    /* ----- Test case #4 - Successful scrub delete ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_SUCCESS);
    OS_BinSemCreate(&g_semUpdateMemAddr_id, MEMSCRUB_BSEM_NAME, OS_SEM_FULL, 0);
    g_uiEndOfRam = (uint32) sysPhysMemTop();
    g_MemScrub_Status.uiMemScrubCurrentPage = 99;
    g_MemScrub_Status.uiMemScrubTotalPages = 99;
    g_MemScrub_Status.uiMemScrubStartAddr = 99;
    g_MemScrub_Status.uiMemScrubEndAddr = g_uiEndOfRam - 1;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubDelete();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MemScrubDelete - 4/4: Correct return code");
    UtAssert_True(g_MemScrub_Status.uiMemScrubCurrentPage == 0, "_CFE_PSP_MemScrubDelete - 4/4: Mem scrub current page reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubTotalPages == 0, "_CFE_PSP_MemScrubDelete - 4/4: Mem scrub total pages reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubStartAddr == MEMSCRUB_DEFAULT_START_ADDR, "_CFE_PSP_MemScrubDelete - 4/4: Mem scrub start address reset");
    UtAssert_True(g_MemScrub_Status.uiMemScrubEndAddr == MEMSCRUB_DEFAULT_END_ADDR, "_CFE_PSP_MemScrubDelete - 4/4: Mem scrub end address reset");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubDelete(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubErrStats(void)
{
    /*
    ** We are going to make the assumption that we won't
    ** reach 99 or greater total errors during the course
    ** of this unit test
    */
    CFE_PSP_MemScrubErrStats_t errStats = {0, 0, 0, 0, 0, 0, 0, 0};
    bool talkative;
   
    /* ----- Test case #1 - Nominal no print ----- */
    /* Set additional inputs */
    talkative = false;
    errStats.uil2errTotal = 99;
    /* Execute test */
    CFE_PSP_MemScrubErrStats(&errStats, talkative);
    /* Verify results */
    UtAssert_True(errStats.uil2errTotal < 99, "_CFE_PSP_MemScrubErrStats - 1/2: Single value check");

    /* ----- Test case #2 - Nominal print ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    talkative = true;
    errStats.uil2errTotal = 99;
    /* Execute test */
    CFE_PSP_MemScrubErrStats(&errStats, talkative);
    /* Verify results */
    UtAssert_True(errStats.uil2errTotal < 99, "_CFE_PSP_MemScrubErrStats - 1/2: Single value check");
}

/*=======================================================================================
** End of file psp_memscrub_testcases.c
**=======================================================================================*/