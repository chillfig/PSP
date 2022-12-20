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
    CFE_PSP_ReservedMemoryMap.BootPtr = NULL;
    sprintf(cMsg, "PSP Restart called with %d\n", uiResetType);
    /* Execute test */
    CFE_PSP_Restart(uiResetType);
    /* Verify outputs */
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
** Ut_CFE_PSP_ValidatePath(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_ValidatePath(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    char cValidPath[] = "/ffx0/testme";
    char cInvalidPath[] = "/ff$^\nartup";

    /* ----- Test case #1 - Nominal - Valid Path ----- */
    /* Setup additional inputs */

    /* Execute test */
    uiRetCode = CFE_PSP_ValidatePath(cValidPath, strlen(cValidPath));
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ValidatePath() - 1/4: Valid Path - Nominal return code");

    /* ----- Test case #2 - Nominal - Invalid Path ----- */
    /* Setup additional inputs */

    /* Execute test */
    uiRetCode = CFE_PSP_ValidatePath(cInvalidPath, strlen(cInvalidPath));
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_ValidatePath() - 2/4: Invalid Path - Nominal return code");

    /* ----- Test case #3 - Error - Path is NULL ----- */
    /* Setup additional inputs */

    /* Execute test */
    uiRetCode = CFE_PSP_ValidatePath(NULL, strlen(cInvalidPath));
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_ValidatePath() - 3/4: Path Null - Nominal return code");

    /* ----- Test case #4 - Error - Path length too short ----- */
    /* Setup additional inputs */

    /* Execute test */
    uiRetCode = CFE_PSP_ValidatePath(cInvalidPath, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_ValidatePath() - 4/4: Path length is too short - Nominal return code");
}

/*=======================================================================================
** Ut_CFE_PSP_GetBootStartupString(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetBootStartupString(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    char boot_string[] = "motetsec(0,0)192.168.22.132:vxWorks_sp0_pmc_revb_3101 e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ffx0/startup o=192.168.22.253/28";
    char boot_string_invalid[] = "motetsec(0,0)192.168.22.132:vxWorks_sp0_pmc_revb_3101 e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ff$^\nartup o=192.168.22.253/28";
    char cMsg_error[200] = "";
    char str_buffer[250] = "";
    char str_buffer_check[] = "/ffx0/startup";
    char cStrInvalid[] = "/ff$^\nartup";

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal - non talkative ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_string, sizeof(boot_string), true);
    UT_SetDefaultReturnValue(UT_KEY(PCS_bootStringToStruct), OK);
    UT_SetDataBuffer(UT_KEY(PCS_bootStringToStruct), str_buffer_check, sizeof(str_buffer_check), true);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(str_buffer, sizeof(str_buffer), 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetBootStartupString() - 1/5: Nominal return code");
    UtAssert_StrnCmp(str_buffer, str_buffer_check, sizeof(str_buffer_check), "_CFE_PSP_GetBootStartupString() - 1/5: Nominal returned string");
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_GetBootStartupString() - 1/5: Nominal no printed messages");

    /* ----- Test case #2 - Input 1 error ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    PCS_snprintf(cMsg_error, sizeof(cMsg_error), "PSP: buffer size too small, it needs to be %d bytes\n", BOOT_FILE_LEN);
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(NULL, 100, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetBootStartupString() - 2/5: Error bad input 1 return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_GetBootStartupString() - 2/5: Error bad input 1 message");

    /* ----- Test case #3 - Input 2 error ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg_error, sizeof(cMsg_error), "PSP: buffer size too small, it needs to be %d bytes\n", BOOT_FILE_LEN);
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

    /* ----- Test case #5 - Nominal - talkative - invalid path ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(PCS_bootStringToStruct), OK);
    UT_SetDataBuffer(UT_KEY(PCS_bootStringToStruct), cStrInvalid, sizeof(cStrInvalid), true);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_string_invalid, sizeof(boot_string_invalid), true);
    str_buffer[0] = '\0';
    /* Execute test */
    uiRetCode = CFE_PSP_GetBootStartupString(str_buffer, sizeof(str_buffer), 1);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetBootStartupString() - 5/5: Nominal talkative - invalid path - return code");
    UtAssert_StrnCmp(str_buffer,cStrInvalid,sizeof(cStrInvalid), "_CFE_PSP_GetBootStartupString() - 5/5: Nominal talkative - invalid path - returned string");
    UtAssert_True(Ut_OS_printf_MsgCount() == 3, "_CFE_PSP_GetBootStartupString() - 5/5: Nominal talkative - invalid path - printed messages");
}

/*=======================================================================================
** Ut_CFE_PSP_SetBootStartupString(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetBootStartupString(void)
{
    int32 uiRetCode = CFE_PSP_ERROR;
    char boot_line[] = "motetsec(0,0)192.168.22.132:vxWorks_sp0_pmc_revb_3101 e=192.168.22.129:ffffffc0 h=192.168.22.131 f=0x480 s=/ffx0/startup o=192.168.22.253/28";
    char boot_string[] = "/ffx0/startup";
    char boot_string_too_long[] = "/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup11"
                                  "/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup11"
                                  "/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup/ffx0/startup11";
    char boot_string_invalid[] = "/ff$^\nartup";
    /* Error if CFE_PSP_SetBootStructure returns error */
    char cMsg_error[] = "PSP: Could not save new boot structure\n";
    /* Error if inputs are NULL or too short */
    char cMsg_error1[] = "PSP: Provided startup script path is NULL or cannot be longer than 160 bytes\n";
    /* Error if CFE_PSP_GetBootStructure returns error */
    char cMsg_error2[] = "PSP: ERROR, could not read the boot structure\n";
    /* Error if startup string is invalid */
    char cMsg_error3[] = "PSP: the provided startup script does not pass validation\n";

    UT_SetDataBuffer(UT_KEY(sysNvRamGet), boot_line, sizeof(boot_line), true);

    /* ----- Test case #1 - Nominal - non talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetBootStartupString() - 1/7: Nominal return code");
    UtAssert_True(Ut_OS_printf_MsgCount() == 0, "_CFE_PSP_SetBootStartupString() - 1/7: Nominal no printed messages");

    /* ----- Test case #2 - Nominal - talkative ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 1);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_SetBootStartupString() - 2/7: Nominal return code");
    UtAssert_True(Ut_OS_printf_MsgCount() == 4, "_CFE_PSP_SetBootStartupString() - 2/7: Nominal talkative printed messages");

    /* ----- Test case #3 - Error Input 1 ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(NULL, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 3/7: Error Input 1 return code");
    UtAssert_OS_print(cMsg_error1, "_CFE_PSP_SetBootStartupString() - 3/7: Error Input 1 printed messages");

    /* ----- Test case #4 - Error Input 2 ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string_too_long, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 4/7: Error Input 2 return code");
    UtAssert_OS_print(cMsg_error1, "_CFE_PSP_SetBootStartupString() - 4/7: Error Input 2 printed messages");

    /* ----- Test case #5 - Error GetBootStructure ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 5/7: Error GetBootStructure return code");
    UtAssert_OS_print(cMsg_error2, "_CFE_PSP_SetBootStartupString() - 5/7: Error GetBootStructure printed messages");

    /* ----- Test case #6 - Error SetBootStructure ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 6/7: Error SetBootStructure return code");
    UtAssert_OS_print(cMsg_error, "_CFE_PSP_SetBootStartupString() - 6/7: Error SetBootStructure printed messages");

    /* ----- Test case #7 - Invalid startup path ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamGet), OK);
    UT_SetDefaultReturnValue(UT_KEY(sysNvRamSet), OK);
    /* Execute test */
    uiRetCode = CFE_PSP_SetBootStartupString(boot_string_invalid, 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_SetBootStartupString() - 7/7: Invalid startup path - return code");
    UtAssert_OS_print(cMsg_error3, "_CFE_PSP_SetBootStartupString() - 7/7: Invalid startup path - printed messages");
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

    /* ----- Test case #3 - Error retrieving boot string ----- */
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
    /* Execute test */
    CFE_PSP_PrintBootParameters(&test_param);
    /* Verify outputs */
    UtAssert_True(Ut_OS_printf_MsgCount() == 2, "_CFE_PSP_PrintBootParameters() - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_KernelGetCRC(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_KernelGetCRC(void)
{
    /* This kernel name must match the predefined name in the stub */
    char cKernelName[] = "VXWORKS";
    uint32 iRetCRC = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(getCatalogPointer), OK);
    UT_SetDefaultReturnValue(UT_KEY(getCatalogEntryCount), 5);
    UT_SetDefaultReturnValue(UT_KEY(strncmp), 0);
    /* Execute test */
    iRetCRC = CFE_PSP_KernelGetCRC(cKernelName, true);
    /* Verify outputs */
    UtAssert_True((iRetCRC == 0), "_CFE_PSP_KernelGetCRC() - 1/5: Nominal found CRC");
    UtAssert_STUB_COUNT(strncmp, 1);
    UtAssert_STUB_COUNT(getCatalogEntryCount, 1);

    UT_ResetState(0);

    /* ----- Test case #2 - Could not find entry ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(getCatalogPointer), OK);
    UT_SetDefaultReturnValue(UT_KEY(getCatalogEntryCount), 5);
    UT_SetDefaultReturnValue(UT_KEY(strncmp), 1);
    /* Execute test */
    iRetCRC = CFE_PSP_KernelGetCRC(cKernelName, true);
    /* Verify outputs */
    UtAssert_True((iRetCRC == 0), "_CFE_PSP_KernelGetCRC() - 2/5: Nominal kernel name not found");
    UtAssert_STUB_COUNT(strncmp, 5);
    UtAssert_STUB_COUNT(getCatalogEntryCount, 1);

    UT_ResetState(0);

    /* ----- Test case #3 - Number of entries is wrong ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(getCatalogPointer), OK);
    UT_SetDefaultReturnValue(UT_KEY(getCatalogEntryCount), 500);
    UT_SetDefaultReturnValue(UT_KEY(strncmp), 1);
    /* Execute test */
    iRetCRC = CFE_PSP_KernelGetCRC(cKernelName, true);
    /* Verify outputs */
    UtAssert_True((iRetCRC == 0), "_CFE_PSP_KernelGetCRC() - 3/5: Error, wrong number of entries in catalog");
    UtAssert_STUB_COUNT(strncmp, 0);
    UtAssert_STUB_COUNT(getCatalogEntryCount, 1);

    UT_ResetState(0);

    /* ----- Test case #4 - Catalog Pointer wrong ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(getCatalogPointer), ERROR);
    UT_SetDefaultReturnValue(UT_KEY(getCatalogEntryCount), 5);
    UT_SetDefaultReturnValue(UT_KEY(strncmp), 1);
    /* Execute test */
    iRetCRC = CFE_PSP_KernelGetCRC(cKernelName, true);
    /* Verify outputs */
    UtAssert_True((iRetCRC == 0), "_CFE_PSP_KernelGetCRC() - 4/5: Error, bad catalog pointer");
    UtAssert_STUB_COUNT(strncmp, 0);
    UtAssert_STUB_COUNT(getCatalogEntryCount, 0);

    UT_ResetState(0);

    /* ----- Test case #3 - NULL pCatalogEntryName ----- */
    /* Execute test */
    iRetCRC = CFE_PSP_KernelGetCRC(NULL, true);
    /* Verify outputs */
    UtAssert_True((iRetCRC == 0), "_CFE_PSP_KernelGetCRC() - 5/5: Error, pCatalogEntryName null");
    UtAssert_STUB_COUNT(getCatalogEntryCount, 0);
}

/*=======================================================================================
** Ut_CFE_PSP_KernelLoadNew(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_KernelLoadNew(void)
{
    char cKernelPath[] = "/ram0/test.bin";
    char cKernelName[] = "VXWORKS";
    char cMsgError1[] = "PSP: Could not flash new kernel\n";
    char cMsgError2[] = "PSP: Could not pre-allocate buffer for new kernel\n";
    int32 iRetCode = CFE_PSP_ERROR;

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(CreateProgrammingBuffer), OK);
    UT_SetDefaultReturnValue(UT_KEY(flashProgFile), OK);
    /* Execute test */
    iRetCode = CFE_PSP_KernelLoadNew(cKernelPath, cKernelName);
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_SUCCESS), "_CFE_PSP_KernelLoadNew() - 1/1: Nominal");
    UtAssert_STUB_COUNT(flashProgFile, 1);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Could not flash program file ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(CreateProgrammingBuffer), OK);
    UT_SetDefaultReturnValue(UT_KEY(flashProgFile), ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_KernelLoadNew(cKernelPath, cKernelName);
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_KernelLoadNew() - 2/3: Could not flash - Return error");
    UtAssert_STUB_COUNT(flashProgFile, 1);
    UtAssert_OS_print(cMsgError1, "_CFE_PSP_KernelLoadNew() - 2/3: Could not flash - Message");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #3 - Could not allocate buffer ----- */
    /* Setup additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(CreateProgrammingBuffer), ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_KernelLoadNew(cKernelPath, cKernelName);
    /* Verify outputs */
    UtAssert_True((iRetCode == CFE_PSP_ERROR), "_CFE_PSP_KernelLoadNew() - 3/3: Could not allocate - Return Error");
    UtAssert_STUB_COUNT(flashProgFile, 0);
    UtAssert_OS_print(cMsgError2, "_CFE_PSP_KernelLoadNew() - 3/3: Could not allocate - Message");
}

/*=======================================================================================
** End of file psp_support_testcases.c
**=======================================================================================*/
