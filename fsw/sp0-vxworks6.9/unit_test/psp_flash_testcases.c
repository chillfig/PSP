/*=======================================================================================
**
** File:  psp_flash_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_flash.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2022-02-01 | Matthew Rumbel   | Initial revision
**=======================================================================================*/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "psp_flash_testcases.h"
#include "../src/cfe_psp_flash.c"

/*=======================================================================================
** Preprocessor Directives
**=======================================================================================*/
#define UT_FLASH_PRINT_SCOPE "_CFE_PSP_FLASH_"
#define FAKEFILE "testFile.matt"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_FLASH_ReadFromFLASH(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FLASH_ReadFromFLASH(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuf[10];
    size_t sLocalBufSize = 10;
    memset((void *)uiLocalBuf, 1, sLocalBufSize);
    char cMsg[256] = {};

    /* ----- Test case #1 - p_dest or fname NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, FLASH_PRINT_SCOPE "ReadFromFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_ReadFromFLASH(NULL, 0, NULL);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 1/5: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 1/5: Error message - message");

    /* ----- Test case #2 - Fail to open file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    sprintf(cMsg, FLASH_PRINT_SCOPE "ReadFromFLASH: Failed to open file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_ReadFromFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 2/5: Failed to open file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 2/5: Failed to open file - message");

    /* ----- Test case #3 - Fail to read correct amount of data ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, 0);
    sprintf(cMsg, FLASH_PRINT_SCOPE "ReadFromFLASH: Read incorrect amount of data\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_ReadFromFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 3/5: Failed to read correct amount of data - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 3/5: Failed to read correct amount of data - message");

    /* ----- Test case #4 - Fail to close file ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    sprintf(cMsg, FLASH_PRINT_SCOPE "ReadFromFLASH: Unable to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_ReadFromFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 4/5: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 4/5: Failed to close file - message");

    /* ----- Test case #5 - Successful read ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_ReadFromFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 5/5: Successful read - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_FLASH_WriteToFLASH(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FLASH_WriteToFLASH(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuf[10];
    size_t sLocalBufSize = 10;
    memset((void *)uiLocalBuf, 1, sLocalBufSize);
    char cMsg[256] = {};

    /* ----- Test case #1 - p_dest or fname NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, FLASH_PRINT_SCOPE "WriteToFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_WriteToFLASH(NULL, 0, NULL);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 1/5: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 1/5: Error message - message");

    /* ----- Test case #2 - Fail to open file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    sprintf(cMsg, FLASH_PRINT_SCOPE "WriteToFLASH: Failed to open file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_WriteToFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 2/5: Failed to open file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 2/5: Failed to open file - message");

    /* ----- Test case #3 - Fail to read correct amount of data ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(write), 1, 0);
    sprintf(cMsg, FLASH_PRINT_SCOPE "WriteToFLASH: Wrote incorrect amount of data\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_WriteToFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 3/5: Failed to write correct amount of data - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 3/5: Failed to write correct amount of data - message");

    /* ----- Test case #4 - Fail to close file ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(write), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    sprintf(cMsg, FLASH_PRINT_SCOPE "WriteToFLASH: Unable to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_WriteToFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 4/5: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 4/5: Failed to close file - message");

    /* ----- Test case #5 - Successful read ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(write), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_WriteToFLASH(uiLocalBuf, sLocalBufSize, FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 5/5: Successful write - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_FLASH_DeleteFile(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FLASH_DeleteFile(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL Filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, FLASH_PRINT_SCOPE "DeleteFile: NULL filename\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_FLASH_DeleteFile(NULL);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, UT_FLASH_PRINT_SCOPE "DeleteFile - 1/4: NULL Pointer - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "DeleteFile - 1/4: NULL Pointer - message");

    /* ----- Test case #2 - File is not present on system ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_DeleteFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "DeleteFile - 2/4: File not present on sysem - return code");

    /* ----- Test case #3 - Remove fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(remove), 1, OS_ERROR);
    sprintf(cMsg, FLASH_PRINT_SCOPE "DeleteFile: Failed to remove file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_DeleteFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_FLASH_PRINT_SCOPE "DeleteFile - 3/4: Failed to remove file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "DeleteFile - 3/4: Failed to remove file - messsage");

    /* ----- Test case #4 - Successful remove ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(remove), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_DeleteFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "DeleteFile - 4/4: Failed to remove file - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_FLASH_CreateFile(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FLASH_CreateFile(void)
{
    int32 iReturnCode;
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL Filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, FLASH_PRINT_SCOPE "CreateFile: NULL filename\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_CreateFile(NULL);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, UT_FLASH_PRINT_SCOPE "CreateFile - 1/5: NULL filename - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 1/5: NULL filename - messsage");

    /* ----- Test case #2 - File does exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_CreateFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "CreateFile - 2/5 File exists - return code");

    /* ----- Test case #3 - Create fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    sprintf(cMsg, FLASH_PRINT_SCOPE "CreateFile: Failed to create file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_CreateFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_FLASH_PRINT_SCOPE "CreateFile - 3/5: Failed to create file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 3/5: Failed to create file - messsage");

    /* ----- Test case #4 - Fail to close file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    sprintf(cMsg, FLASH_PRINT_SCOPE "CreateFile: Failed to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_CreateFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "CreateFile - 4/5: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 4/5: Failed to close file - messsage");

    /* ----- Test case #5 - Successfuly created file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_FLASH_CreateFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_FLASH_PRINT_SCOPE "CreateFile - 5/5: Failed to close file - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_FLASH_CheckFile(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_FLASH_CheckFile(void)
{
    bool iReturnValue;
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, FLASH_PRINT_SCOPE "CheckFile: NULL filename\n");
    /* Execute test */
    iReturnValue = CFE_PSP_FLASH_CheckFile(NULL);
    /* Verify results */
    UtAssert_True(iReturnValue == false, UT_FLASH_PRINT_SCOPE "CheckFile - 1/3: NULL filename - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CheckFile - 1/3: NULL filename - messsage");

    /* ----- Test case #2 - File does exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    /* Execute test */
    iReturnValue = CFE_PSP_FLASH_CheckFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnValue == true, UT_FLASH_PRINT_SCOPE "CheckFile - 2/3: File exists - return code");

    /* ----- Test case #3 - File does not exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    /* Execute test */
    iReturnValue = CFE_PSP_FLASH_CheckFile(FAKEFILE);
    /* Verify results */
    UtAssert_True(iReturnValue == false, UT_FLASH_PRINT_SCOPE "CheckFile - 3/3: File does not exists - return code");
}