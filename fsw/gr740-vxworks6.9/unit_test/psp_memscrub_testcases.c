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

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    g_MemScrub_Status.RunMode = MEMSCRUB_IDLE_MODE;
    g_bMemScrubStartOnStartup = true;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MemScrubInit - 1/3: Nominal - return code");

    UT_ResetState(0);

    /* ----- Test case #2 - Nominal ----- */
    /* Set additional inputs */
    g_MemScrub_Status.RunMode = MEMSCRUB_AUTOMATIC_MODE;
    g_bMemScrubStartOnStartup = false;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MemScrubInit - 2/3: Nominal - return code");

    UT_ResetState(0);

    /* ----- Test case #3 - Validation error ----- */
    /* Set additional inputs */
    g_MemScrub_Status.RunMode = MEMSCRUB_AUTOMATIC_MODE;
    g_MemScrub_Status.uiMemScrubStartAddr = 100;
    g_MemScrub_Status.uiMemScrubEndAddr = 10;
    g_bMemScrubStartOnStartup = false;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubInit();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MemScrubInit - 3/3: Validation error - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubIsRunning(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubIsRunning(void)
{
    CFE_PSP_MemScrubIsRunning();
    UtAssert_NA("_CFE_PSP_MemScrubIsRunning: Nothing to test");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubDisable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubDisable(void)
{
    CFE_PSP_MemScrubDisable();
    UtAssert_NA("_CFE_PSP_MemScrubDisable: Nothing to test");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubEnable(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubEnable(void)
{
    CFE_PSP_MemScrubEnable();
    UtAssert_NA("_CFE_PSP_MemScrubEnable: Nothing to test");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubValidate(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubValidate(void)
{
    char cMsg_startAddress[] = MEMSCRUB_PRINT_SCOPE "Start address after end address\n";
    char cMsg_endAddress[] = MEMSCRUB_PRINT_SCOPE "Invalid new end address\n";
    CFE_PSP_MemScrubStatus_t newMemScrubConfig;
    int32 iReturnCode;

    newMemScrubConfig.uiMemScrubEndAddr = 100;
    newMemScrubConfig.uiMemScrubStartAddr = 1;

    /* ----- Test case #1 - Nominal ----- */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubValidate - 1/4: Nominal - Return code");
    UtAssert_True((Ut_OS_printf_MsgCount() == 0), "_CFE_PSP_MemScrubValidate - 1/4: Nominal - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Bad address range ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 1;
    newMemScrubConfig.uiMemScrubStartAddr = 100;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 2/4: Bad address range - Return code");
    UtAssert_OS_print(cMsg_startAddress, "_CFE_PSP_MemScrubValidate - 2/4: Bad address range - No messages");

    Ut_OS_printf_Setup();

    /* ----- Test case #3 - end of RAM not set ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 100;
    newMemScrubConfig.uiMemScrubStartAddr = 1;
    g_uiEndOfRam = 0;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubValidate - 3/4: end of RAM not set - Return code");
    UtAssert_True((Ut_OS_printf_MsgCount() == 0), "_CFE_PSP_MemScrubValidate - 3/4: end of RAM not set - No messages");
    UtAssert_True((g_uiEndOfRam != 0),  "_CFE_PSP_MemScrubValidate - 3/4: end of RAM not set - Value changed");

    Ut_OS_printf_Setup();

    /* ----- Test case #4 - end address too big ----- */
    /* Set additional inputs */
    newMemScrubConfig.uiMemScrubEndAddr = 0xFFFFFFFF;
    newMemScrubConfig.uiMemScrubStartAddr = 100;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubValidate(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubValidate - 4/4: end address too big - Return code");
    UtAssert_OS_print(cMsg_endAddress, "_CFE_PSP_MemScrubValidate - 4/4: end address too big - No messages");
}


/*=======================================================================================
** Ut_CFE_PSP_MemScrubSet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubSet(void)
{
    CFE_PSP_MemScrubStatus_t newMemScrubConfig = {0};
    uint32 real_endOfRam = (uint32) sysPhysMemTop();
    int32 iReturnCode;

    /* Set global defaults */
    g_MemScrub_Status.uiMemScrubStartAddr = 10;
    g_MemScrub_Status.uiMemScrubEndAddr = 100;
    g_uiEndOfRam = real_endOfRam;
    /* Turn off memory scrubber */
    SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SCRUBBER_ENABLE, 0);

    /* Copy current global default configuration to new configuration */
    memcpy(&newMemScrubConfig, &g_MemScrub_Status, sizeof(newMemScrubConfig));

    /* ----- Test case #1 - Nominal successful validation no active memscrub ----- */
    /* Setup additional inputs */
    newMemScrubConfig.uiMemScrubStartAddr = 0;
    newMemScrubConfig.uiMemScrubEndAddr = 1000;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((g_MemScrub_Status.uiMemScrubStartAddr == newMemScrubConfig.uiMemScrubStartAddr), "_CFE_PSP_MemScrubSet - 1/3: Nominal successful validation no active memscrub - confirm start addr");
    UtAssert_True((g_MemScrub_Status.uiMemScrubEndAddr == newMemScrubConfig.uiMemScrubEndAddr), "_CFE_PSP_MemScrubSet - 1/3: Nominal successful validation no active memscrub - confirm end addr");
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubSet - 1/3: Nominal successful validation no active memscrub - return value");

    /* ----- Test case #2 - Nominal successful validation with active memscrub ----- */
    /* Setup additional inputs */
    UT_ResetState(0);

    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((g_MemScrub_Status.uiMemScrubStartAddr == newMemScrubConfig.uiMemScrubStartAddr), "_CFE_PSP_MemScrubSet - 1/3: Nominal successful validation with active memscrub - confirm start addr");
    UtAssert_True((g_MemScrub_Status.uiMemScrubEndAddr == newMemScrubConfig.uiMemScrubEndAddr), "_CFE_PSP_MemScrubSet - 1/3: Nominal successful validation with active memscrub - confirm end addr");
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubSet - 1/3: Nominal successful validation with active memscrub - return value");

    /* ----- Test case #3 - Does not pass validation ----- */
    /* Setup additional inputs */
    UT_ResetState(0);

    /* Setup additional inputs */
    newMemScrubConfig.uiMemScrubStartAddr = 100;
    newMemScrubConfig.uiMemScrubEndAddr = 10;
    /* Execute test */
    iReturnCode = CFE_PSP_MemScrubSet(&newMemScrubConfig);
    /* Verify outputs */
    UtAssert_True((g_MemScrub_Status.uiMemScrubStartAddr != newMemScrubConfig.uiMemScrubStartAddr), "_CFE_PSP_MemScrubSet - 3/3: Does not pass validation - start addr not changed");
    UtAssert_True((g_MemScrub_Status.uiMemScrubEndAddr != newMemScrubConfig.uiMemScrubEndAddr), "_CFE_PSP_MemScrubSet - 3/3: Does not pass validation - end addr not changed");
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubSet - 3/3: Does not pass validation - return value");
}


/*=======================================================================================
** Ut_CFE_PSP_MemScrubGet(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubGet(void)
{
    int32 iRetCode = CFE_PSP_ERROR;
    char cMsg[256] = {};
    CFE_PSP_MemScrubStatus_t mss_Status = {};
    g_MemScrub_Status.uiMemScrubStartAddr = 99;
    g_MemScrub_Status.uiMemScrubEndAddr = 99;

    /* reset local configuration structure */
    memset(&mss_Status, 0x00, sizeof(mss_Status));

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal, talk ----- */
    /* Setup additional inputs */
    snprintf(cMsg, 256, MEMSCRUB_PRINT_SCOPE "Mode %u\nAddress Range [0x%08X-0x%08X]\n",
                        g_MemScrub_Status.RunMode,
                        g_MemScrub_Status.uiMemScrubStartAddr,
                        g_MemScrub_Status.uiMemScrubEndAddr
            );
    /* Execute test */
    iRetCode = CFE_PSP_MemScrubGet(&mss_Status, sizeof(mss_Status), true);
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubGet - 1/3: Nominal no talk - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubGet - 1/1: Status print - message");
    UtAssert_True(mss_Status.uiMemScrubStartAddr == g_MemScrub_Status.uiMemScrubStartAddr, "_CFE_PSP_MemScrubGet - 1/1: Start address value test");
    UtAssert_True(mss_Status.uiMemScrubEndAddr == g_MemScrub_Status.uiMemScrubEndAddr, "_CFE_PSP_MemScrubGet - 1/1: End address value test");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal, no talk ----- */
    /* Setup additional inputs */
    /* reset local configuration structure */
    memset(&mss_Status, 0x00, sizeof(mss_Status));
    /* Execute test */
    iRetCode = CFE_PSP_MemScrubGet(&mss_Status, sizeof(mss_Status), false);
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubGet - 2/3: Nominal no talk - Return code");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_MemScrubGet - 2/3: Status print - message");
    UtAssert_True(mss_Status.uiMemScrubStartAddr == g_MemScrub_Status.uiMemScrubStartAddr, "_CFE_PSP_MemScrubGet - 2/3: Start address value test");
    UtAssert_True(mss_Status.uiMemScrubEndAddr == g_MemScrub_Status.uiMemScrubEndAddr, "_CFE_PSP_MemScrubGet - 2/3: End address value test");

    /* ----- Test case #3 - Failed size ----- */
    /* Setup additional inputs */

    /* Execute test */
    iRetCode = CFE_PSP_MemScrubGet(&mss_Status, sizeof(iRetCode), false);
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubGet - 3/3: Failed size check - Return Code");
}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubDelete(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubErrStats(void)
{
    char cMsg[256] = {};
    int32 iRetCode = CFE_PSP_ERROR;

    /*
    ** We are going to make the assumption that we won't
    ** reach 99 or greater total errors during the course
    ** of this unit test
    */
    CFE_PSP_MemScrubErrStats_t errStats = {0, 0, 0, 0, 0};

    Ut_OS_printf_Setup();
   
    /* ----- Test case #1 - Nominal no print ----- */
    /* Set additional inputs */
    errStats.uiRUNCOUNT = 99;
    /* Execute test */
    iRetCode = CFE_PSP_MemScrubErrStats(&errStats, sizeof(errStats), false);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubErrStats - 1/3: Single value check - Return code");
    UtAssert_True(errStats.uiRUNCOUNT < 99, "_CFE_PSP_MemScrubErrStats - 1/3: Single value check");

    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal print ----- */
    /* Set additional inputs */
    errStats.uiRUNCOUNT = 999999;
    snprintf(cMsg, 256, MEMSCRUB_PRINT_SCOPE "Mem Scrub Statistics\n"
            "CurrentPosition = %u\n"
            "RUNCOUNT = %u\n"
            "BLKCOUNT = %u\n"
            "CECNT = %u\n"
            "UECNT = %u\n",
            g_MemScrub_Stats.uiCurrentPosition,
            g_MemScrub_Stats.uiRUNCOUNT,
            g_MemScrub_Stats.uiBLKCOUNT,
            g_MemScrub_Stats.uiCECNT,
            g_MemScrub_Stats.uiUECNT);
    /* Execute test */
    iRetCode = CFE_PSP_MemScrubErrStats(&errStats, sizeof(errStats), true);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_MemScrubErrStats - 2/3: Single value check - Return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MemScrubErrStats - 2/3: Single value check - Message");
    UtAssert_True(errStats.uiRUNCOUNT != 999999, "_CFE_PSP_MemScrubErrStats - 2/3: Single value check");

    /* ----- Test case #3 - Failed size ----- */
    /* Set additional inputs */
    
    /* Execute test */
    iRetCode = CFE_PSP_MemScrubErrStats(&errStats, sizeof(iRetCode), false);
    /* Verify results */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_MemScrubErrStats - 3/3: Structure Size check");

}

/*=======================================================================================
** Ut_CFE_PSP_MemScrubDelete(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_MemScrubDelete(void)
{

    /* ----- Test case #1 - SCRUB disabled ----- */
    /* Set additional inputs */
    SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_LOOP_MODE, 1);
    /* Execute test */
    CFE_PSP_MemScrubDelete();
    /* Verify results */
    UtAssert_True((GET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SCRUBBER_ENABLE) == 0), "_CFE_PSP_MemScrubDelete - 1/2: SCRUB disabled");

    UT_ResetState(0);

    /* ----- Test case #2 - SCRUB already disabled ----- */
    /* Set additional inputs */
    SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_LOOP_MODE, 0);
    /* Execute test */
    CFE_PSP_MemScrubDelete();
    /* Verify results */
    UtAssert_True((GET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SCRUBBER_ENABLE) == 0), "_CFE_PSP_MemScrubDelete - 2/2: SCRUB already disabled");
}

/*=======================================================================================
** End of file psp_memscrub_testcases.c
**=======================================================================================*/