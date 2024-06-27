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
*   \file psp_flash_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_flash.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_ReadFromFlash(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_ReadFromFlash(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuf[10];
    size_t sLocalBufSize = 10;
    memset((void *)uiLocalBuf, 1, sLocalBufSize);
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - p_dest NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "ReadFromFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(NULL, 0, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 1/6: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 1/6: Error message - message");

    /* ----- Test case #2 - fname NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "ReadFromFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, 0, (char *)NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 2/6: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 2/6: Error message - message");

    /* ----- Test case #3 - Fail to open file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "ReadFromFLASH: Failed to open file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 3/6: Failed to open file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 3/6: Failed to open file - message");

    /* ----- Test case #4 - Fail to read correct amount of data ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, 0);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "ReadFromFLASH: Read incorrect amount of data\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 4/6: Failed to read correct amount of data - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 4/6: Failed to read correct amount of data - message");

    /* ----- Test case #5 - Fail to close file ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "ReadFromFLASH: Unable to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 5/6: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 5/6: Failed to close file - message");

    /* ----- Test case #6 - Successful read ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 6/6: Successful read - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_WriteToFlash(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteToFlash(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuf[10];
    size_t sLocalBufSize = 10;
    memset((void *)uiLocalBuf, 1, sLocalBufSize);
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - p_dest NULL----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "WriteToFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(NULL, 0, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 1/6: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 1/6: Error message - message");

     /* ----- Test case #2 - fname NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "WriteToFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, 0, NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 2/6: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 2/6: Error message - message");

    /* ----- Test case #3 - Fail to open file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "WriteToFLASH: Failed to open file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 3/6: Failed to open file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 3/6: Failed to open file - message");

    /* ----- Test case #4 - Fail to read correct amount of data ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(write), 1, 0);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "WriteToFLASH: Wrote incorrect amount of data\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 4/6: Failed to write correct amount of data - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 4/6: Failed to write correct amount of data - message");

    /* ----- Test case #5 - Fail to close file ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(write), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "WriteToFLASH: Unable to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 5/6: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 5/6: Failed to close file - message");

    /* ----- Test case #6 - Successful read ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(write), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 6/6: Successful write - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_DeleteFile(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_DeleteFile(void)
{
    int32 iReturnCode;
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL Filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "DeleteFile: NULL filename\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_DeleteFile(NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "DeleteFile - 1/4: NULL Pointer - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "DeleteFile - 1/4: NULL Pointer - message");

    /* ----- Test case #2 - File is not present on system ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    /* Execute test */
    iReturnCode = CFE_PSP_DeleteFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "DeleteFile - 2/4: File not present on system - return code");

    /* ----- Test case #3 - Remove fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(remove), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "DeleteFile: Failed to remove file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_DeleteFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "DeleteFile - 3/4: Failed to remove file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "DeleteFile - 3/4: Failed to remove file - message");

    /* ----- Test case #4 - Successful remove ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(remove), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_DeleteFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "DeleteFile - 4/4: Failed to remove file - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_CreateFile(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_CreateFile(void)
{
    int32 iReturnCode;
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL Filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CreateFile: NULL filename\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "CreateFile - 1/5: NULL filename - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 1/5: NULL filename - message");

    /* ----- Test case #2 - File does exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateFile - 2/5 File exists - return code");

    /* ----- Test case #3 - Create fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CreateFile: Failed to create file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "CreateFile - 3/5: Failed to create file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 3/5: Failed to create file - message");

    /* ----- Test case #4 - Fail to close file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CreateFile: Failed to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateFile - 4/5: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 4/5: Failed to close file - message");

    /* ----- Test case #5 - Successfully created file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateFile - 5/5: Failed to close file - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_CheckFile(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_CheckFile(void)
{
    bool iReturnValue;
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CheckFile: NULL filename\n");
    /* Execute test */
    iReturnValue = CFE_PSP_CheckFile(NULL);
    /* Verify results */
    UtAssert_True((iReturnValue == false), UT_FLASH_PRINT_SCOPE "CheckFile - 1/3: NULL filename - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CheckFile - 1/3: NULL filename - message");

    /* ----- Test case #2 - File does exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    /* Execute test */
    iReturnValue = CFE_PSP_CheckFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnValue == true), UT_FLASH_PRINT_SCOPE "CheckFile - 2/3: File exists - return code");

    /* ----- Test case #3 - File does not exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    /* Execute test */
    iReturnValue = CFE_PSP_CheckFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnValue == false), UT_FLASH_PRINT_SCOPE "CheckFile - 3/3: File does not exists - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_FLASH_CreateDirectory(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_CreateDirectory(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};

    char caDirectory[20] = "/testdir";

    /* ----- Test case #1 - NULL check ----- */
    /* Additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CreateDirectory: Invalid Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory((char *)NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "CreateDirectory - 1/4: NULL check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateDirectory - 1/4: NULL check - message");

    /* ----- Test case #2 - Status < 0, errno != 17 ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, -5);
    errno = 2;
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CreateDirectory: STATUS: %d\n", -5);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory(caDirectory);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "CreateDirectory - 2/4: mkdir error - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateDirectory - 2/4: mkdir error - message");

    /* ----- Test case #2 - Status < 0, errno == 17 (Folder already exists) ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, -5);
    errno = 17;
    snprintf(cMsg, sizeof(cMsg), FLASH_PRINT_SCOPE "CreateDirectory: STATUS: %d\n", -5);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory(caDirectory);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateDirectory - 3/4: mkdir error - return code");
    UtAssert_NoOS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateDirectory - 3/4: mkdir error - message");

    /* ----- Test case #3 - Successful run ----- */
    /* Additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory(caDirectory);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateDirectory - 4/4: Successful execution - return code");
}
