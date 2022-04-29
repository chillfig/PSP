/*=======================================================================================
**
** File:  psp_support_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_support.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2021-01-05 | Minh Luong       | Initial revision
**=======================================================================================*/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
#include <target_config.h>

#include "uttest.h"
#include "utstubs.h"
#include "osapi.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_support_testcases.h"
#include "../src/cfe_psp_support.c"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_Restart(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Restart(void)
{
    uint32  uiResetType = CFE_PSP_RST_TYPE_POWERON;
    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = NULL;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 0;
    
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = NULL;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 0;

    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = NULL;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = 0;

    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = NULL;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = 0;
    char cMsg[256] = "";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal POWERON reboot ----- */
    /* Setup additional inputs */
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(userNvRamSet), 0);
    sprintf(cMsg, "PSP Restart called with %d\n", uiResetType);
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal reboot to shell ----- */
    /* Setup additional inputs */
    uiResetType = CFE_PSP_RST_TYPE_SHELL;
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    sprintf(cMsg, "PSP Restart called with %d\n", uiResetType);
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_PROCESSOR,
                  "_CFE_PSP_Restart() - 2/3: Nominal PROCESSOR reboot");
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Nominal reboot to toggle ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    strcpy(g_StartupInfo.active_cfs_partition, "/ffx0");
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    uiResetType = CFE_PSP_RST_TYPE_CFS_TOGGLE;
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    sprintf(cMsg, "PSP Restart called with %d\n", uiResetType);
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_PROCESSOR,
                  "_CFE_PSP_Restart() - 3/3: Nominal PROCESSOR reboot");
    UtAssert_STUB_COUNT(reboot,1);
}

/*=======================================================================================
** Ut_CFE_PSP_Panic(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_Panic(void)
{
    int32 iErrorCode = -1;
    char cMsg[256] = "";
    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal CFE_PSP_PANIC_MEMORY_ALLOC ----- */
    /* Setup additional inputs */
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = 0;
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_MEMORY_ALLOC;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal CFE_PSP_PANIC_STARTUP ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_STARTUP;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Nominal CFE_PSP_PANIC_VOLATILE_DISK ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_VOLATILE_DISK;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #4 - Nominal CFE_PSP_PANIC_NONVOL_DISK ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelay), OS_SUCCESS);
    iErrorCode = CFE_PSP_PANIC_NONVOL_DISK;
    snprintf(cMsg, sizeof(cMsg), "PSP Panic called with %d\n", iErrorCode);
    /* Execute test */
    CFE_PSP_Panic(iErrorCode);
    /* Verify outputs */
    UtAssert_STUB_COUNT(reboot,1);
}

/*=======================================================================================
** Ut_CFE_PSP_FlushCaches(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FlushCaches(void)
{
    uint32 ftype = 0;
    void *faddress = NULL;
    uint32 fsize = 1;
    /* Nothing to test because the empty function */
    CFE_PSP_FlushCaches(ftype, faddress, fsize);
    UtAssert_NA("_CFE_PSP_FlushCaches() - 1/1: N/A");
}

/*=======================================================================================
** Ut_CFE_PSP_GetProcessorId(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetProcessorId(void)
{
    uint32   uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetCode = CFE_PSP_GetProcessorId();
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_CPU_ID, "_CFE_PSP_GetProcessorId() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetSpacecraftId(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetSpacecraftId(void)
{
    uint32   uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    uiRetCode = CFE_PSP_GetSpacecraftId();
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SPACECRAFT_ID, "_CFE_PSP_GetSpacecraftId() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetProcessorName(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetProcessorName(void)
{
    const char *pRetString;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    /* Execute test */
    pRetString = CFE_PSP_GetProcessorName();
    /* Verify outputs */
    UtAssert_StrCmp(pRetString, CFE_PSP_CPU_NAME, "_CFE_PSP_GetProcessorName() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_ToggleCFSBootPartition(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_ToggleCFSBootPartition(void)
{
    char cMsg[256] = {'\0'};

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    strcpy(g_StartupInfo.active_cfs_partition, "/ffx0");
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    CFE_PSP_ToggleCFSBootPartition();
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_ToggleCFSBootPartition() - 1/3: Nominal no printed messages");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Could not find active_cfs_partition ----- */
    /* Setup additional inputs */
    strcpy(g_StartupInfo.active_cfs_partition, "/ff");
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    CFE_PSP_ToggleCFSBootPartition();
    /* Verify outputs */
    UtAssert_STUB_COUNT(PCS_snprintf, 0);
    UtAssert_STUB_COUNT(PCS_bootStringToStruct, 1);
    UtAssert_STUB_COUNT(sysNvRamGet, 1);
    UtAssert_STUB_COUNT(sysNvRamSet, 1);
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_ToggleCFSBootPartition() - 2/3: Nominal no printed messages");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - snprintf error ----- */
    /* Setup additional inputs */
    strcpy(g_StartupInfo.active_cfs_partition, "/ffx1");
    UT_SetDeferredRetcode(UT_KEY(PCS_snprintf), 1, OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    CFE_PSP_ToggleCFSBootPartition();
    /* Verify outputs */
    UtAssert_STUB_COUNT(PCS_snprintf, 1);
    UtAssert_STUB_COUNT(PCS_bootStringToStruct, 1);
    UtAssert_STUB_COUNT(sysNvRamGet, 1);
    UtAssert_STUB_COUNT(sysNvRamSet, 1);
    snprintf(cMsg, 256, "PSP: Could not construct new boot startup string.\n`%s`/`%s`\n", g_cAvailable_cfs_partitions[0], CFE_PSP_STARTUP_FILENAME);
    UtAssert_OS_print(cMsg, "_CFE_PSP_ToggleCFSBootPartition() - 3/3: snprintf error message");
    UtAssert_True(Ut_OS_printf_MsgCount() == 1, "_CFE_PSP_ToggleCFSBootPartition() - 3/3: Nominal no printed messages");
}

/*=======================================================================================
** Ut_CFE_PSP_GetBootStartupString(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetBootStartupString(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    char boot_string[] = "motetsec(0,0)192.168.22.132:vxWorks_sp0_pmc_revb_3101 e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ffx0/startup o=192.168.22.253/28";
    char cMsg_error[200] = "";
    char str_buffer[250] = "";
    char str_buffer_check[] = "/ffx0/startup";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal - non talkative ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_string, sizeof(boot_string), true);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(str_buffer, sizeof(str_buffer), 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetBootStartupString() - 1/5: Nominal return code");
    UtAssert_StrnCmp(str_buffer, str_buffer_check, sizeof(str_buffer_check), "_CFE_PSP_GetBootStartupString() - 1/5: Nominal returned string");
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_GetBootStartupString() - 1/5: Nominal no printed messages");

    /* ----- Test case #2 - Input 1 error ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    PCS_snprintf(cMsg_error, sizeof(cMsg_error), "PSP: bufferSize too small, it needs to be %d bytes\n", BOOT_FILE_LEN);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(NULL, 100, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetBootStartupString() - 2/5: Error bad input 1 return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_GetBootStartupString() - 2/5: Error bad input 1 message");

    /* ----- Test case #3 - Input 2 error ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg_error, sizeof(cMsg_error), "PSP: bufferSize too small, it needs to be %d bytes\n", BOOT_FILE_LEN);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(str_buffer, 10, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetBootStartupString() - 3/5: Error bad input 2 return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_GetBootStartupString() - 3/5: Error bad input 2 message");

    /* ----- Test case #4 - Error get structure ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), ERROR);
    snprintf(cMsg_error, sizeof(cMsg_error), "PSP: ERROR, could not read the boot structure\n");
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(str_buffer, sizeof(str_buffer), 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetBootStartupString() - 4/5: Error get structure return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_GetBootStartupString() - 4/5: Error bad input 2 message");

    /* ----- Test case #5 - Nominal - talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_string, sizeof(boot_string), true);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(str_buffer, sizeof(str_buffer), 1);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetBootStartupString() - 5/5: Nominal talkative return code");
    UtAssert_StrnCmp(str_buffer,str_buffer_check,sizeof(str_buffer_check), "_CFE_PSP_GetBootStartupString() - 5/5: Nominal talkative returned string");
    UtAssert_True(Ut_OS_printf_MsgCount() == 5, "_CFE_PSP_GetBootStartupString() - 5/5: Nominal talkative printed messages");
}

/*=======================================================================================
** Ut_CFE_PSP_SetBootStartupString(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetBootStartupString(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    char boot_string[] = "/ffx0/startup";
    char boot_string_too_long[] = "/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup11"
                                  "/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup11"
                                  "/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup11";
    /* Error if CFE_PSP_SetBootStructure returnes error */
    char cMsg_error[] = "PSP: Could not save new boot structure\n";
    /* Error if inputs are NULL or too short */
    char cMsg_error1[] = "PSP: Provided startup script path is NULL or cannot be longer than 160 bytes\n";
    /* Error if CFE_PSP_GetBootStructure returns error */
    char cMsg_error2[] = "PSP: ERROR, could not read the boot structure\n";


    /* ----- Test case #1 - Nominal - non talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetBootStartupString() - 1/6: Nominal return code");
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetBootStartupString() - 1/6: Nominal no printed messages");

    /* ----- Test case #2 - Nominal - talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 1);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetBootStartupString() - 2/6: Nominal return code");
    UtAssert_True(Ut_OS_printf_MsgCount() == 6, "_CFE_PSP_SetBootStartupString() - 2/6: Nominal talkative printed messages");

    /* ----- Test case #3 - Error Input 1 ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(NULL, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 3/6: Error Input 1 return code");
    UtAssert_OS_print(cMsg_error1, "_CFE_PSP_SetBootStartupString() - 3/6: Error Input 1 printed messages");

    /* ----- Test case #4 - Error Input 2 ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string_too_long, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 4/6: Error Input 2 return code");
    UtAssert_OS_print(cMsg_error1, "_CFE_PSP_SetBootStartupString() - 4/6: Error Input 2 printed messages");

    /* ----- Test case #5 - Error GetBootStructure ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 5/6: Error GetBootStructure return code");
    UtAssert_OS_print(cMsg_error2, "_CFE_PSP_SetBootStartupString() - 5/6: Error GetBootStructure printed messages");

    /* ----- Test case #6 - Error SetBootStructure ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 6/6: Error SetBootStructure return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_SetBootStartupString() - 6/6: Error SetBootStructure printed messages");
}

/*=======================================================================================
** Ut_CFE_PSP_GetBootStructure(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetBootStructure(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    BOOT_PARAMS test_param = {{0}};
    char boot_string[] = "motetsec(0,0)192.168.22.132:vxWorks_sp0_pmc_revb_3101 e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ffx0/startup o=192.168.22.253/28";
    char cMsg_talkative[] = {"PSP: Boot String:\n"};
    char cMsg_error[] = {"PSP: Could not get boot string\n"};

    /* ----- Test case #1 - Nominal - non talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_string, sizeof(boot_string), true);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStructure(&test_param, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetBootStructure() - 1/3: Nominal return code");
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_GetBootStructure() - 1/3: Nominal no messages");

    /* ----- Test case #2 - Nominal - talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_string, sizeof(boot_string), true);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStructure(&test_param, 1);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetBootStructure() - 2/3: Nominal return code - talkative");
    UtAssert_OS_print(cMsg_talkative, "_CFE_PSP_GetBootStructure() - 2/3: Nominal message - talkative");

    /* ----- Test case #3 - Error retriving boot string ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStructure(&test_param, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetBootStructure() - 3/3: Error return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_GetBootStructure() - 3/3: Error message");
}

/*=======================================================================================
** Ut_CFE_PSP_SetBootStructure(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetBootStructure(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    BOOT_PARAMS test_param = {{0}};
    char boot_string[] = "motetsec(0,0)192.168.22.132:vxWorks e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ffx0/startup o=192.168.22.253/28";
    char cMsg_talkative[] = {"PSP: New Boot String:\n"};
    char cMsg_error_saving[] = {"PSP: Could not set new boot string\n"};
    char cMsg_error_structure[] = {"PSP: Could not convert structure to boot string\n"};

    UT_SetDefaultReturnValue(UT_KEY(bootStructToString), OK);

    /* ----- Test case #1 - Nominal - non talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Set new parameters */
    bootStringToStruct(boot_string, &test_param);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStructure(test_param, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetBootStructure() - 1/4: Nominal return code");
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetBootStructure() - 1/4: Nominal no messages");

    /* ----- Test case #2 - Nominal - talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Set new parameters */
    bootStringToStruct(boot_string, &test_param);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStructure(test_param, 1);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetBootStructure() - 2/4: Nominal return code - talkative");
    UtAssert_OS_print(cMsg_talkative, "_CFE_PSP_SetBootStructure() - 2/4: Nominal message - talkative");

    /* ----- Test case #3 - Error saving boot string ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), ERROR);
    /* Set new parameters */
    bootStringToStruct(boot_string, &test_param);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStructure(test_param, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStructure() - 3/4: Error saving - return code");
    UtAssert_OS_print(cMsg_error_saving, "_CFE_PSP_SetBootStructure() - 3/4: Error saving - message");

    /* ----- Test case #4 - Error converting structure ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    UT_SetDefaultReturnValue(UT_KEY(bootStructToString), ERROR);
    /* Set new parameters */
    memset(&test_param,'1',sizeof(test_param));
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStructure(test_param, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStructure() - 4/4: Error converting - return code");
    UtAssert_OS_print(cMsg_error_structure, "_CFE_PSP_SetBootStructure() - 4/4: Error converting - message");
}

/*=======================================================================================
** Ut_CFE_PSP_PrintBootParameters(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_PrintBootParameters(void)
{
    BOOT_PARAMS test_param = {{0}};
    char boot_string[] = "motetsec(0,0)192.168.22.132:vxWorks e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ffx0/startup o=192.168.22.253/28";

    Ut_OS_printf_Setup();
    bootStringToStruct(boot_string, &test_param);

    CFE_PSP_PrintBootParameters(&test_param);

    UtAssert_True(Ut_OS_printf_MsgCount() == 2, "_CFE_PSP_PrintBootParameters() - 1/1: Nominal");
}

/*=======================================================================================
** End of file psp_support_testcases.c
**=======================================================================================*/
