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

#include "coveragetest-psp-sp0-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include "cfe_psp.h"
#include "cfe_psp_flash.h"

#include "PCS_stdio.h"
#include "PCS_sys_stat.h"
#include "PCS_fcntl.h"
#include "PCS_unistd.h"
#include "PCS_errno.h"

#define UT_FLASH_PRINT_SCOPE "PSP FLASH: "

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Test_CFE_PSP_ReadFromFlash(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_ReadFromFlash(void)
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
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "ReadFromFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(NULL, 0, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 1/7: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 1/7: Error message - message");

    /* ----- Test case #2 - fname NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "ReadFromFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, 0, (char *)NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 2/7: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 2/7: Error message - message");

    /* ----- Test case #3 - File doesn't exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "ReadFromFLASH: %s doesn't exist\n", cFakeFileName);
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 3/7: file doesn't exist - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 3/7: file doesn't exist - message");

    /* ----- Test case #4 - Fail to open file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "ReadFromFLASH: Failed to open file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 4/7: Failed to open file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 4/7: Failed to open file - message");

    /* ----- Test case #5 - Fail to read correct amount of data ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_read), 1, 0);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "ReadFromFLASH: Read incorrect amount of data\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 5/7: Failed to read correct amount of data - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 5/7: Failed to read correct amount of data - message");

    /* ----- Test case #6 - Fail to close file ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_read), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(PCS_close), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "ReadFromFLASH: Unable to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 6/7: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 6/7: Failed to close file - message");

    /* ----- Test case #7 - Successful read ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_read), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(PCS_close), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "ReadFromFLASH - 7/7: Successful read - return code");
}

/*=======================================================================================
** Test_CFE_PSP_WriteToFlash(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_WriteToFlash(void)
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
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "WriteToFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(NULL, 0, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 1/6: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 1/6: Error message - message");

     /* ----- Test case #2 - fname NULL ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "WriteToFLASH: NULL pointer(s)\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, 0, NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 2/6: NULL Pointer(s) check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 2/6: Error message - message");

    /* ----- Test case #3 - Fail to open file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "WriteToFLASH: Failed to open file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 3/6: Failed to open file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 3/6: Failed to open file - message");

    /* ----- Test case #4 - Fail to read correct amount of data ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_write), 1, 0);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "WriteToFLASH: Wrote incorrect amount of data\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 4/6: Failed to write correct amount of data - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 4/6: Failed to write correct amount of data - message");

    /* ----- Test case #5 - Fail to close file ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_write), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(PCS_close), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "WriteToFLASH: Unable to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 5/6: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "WriteToFLASH - 5/6: Failed to close file - message");

    /* ----- Test case #6 - Successful read ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_write), 1, sLocalBufSize);
    UT_SetDeferredRetcode(UT_KEY(PCS_close), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToFlash(uiLocalBuf, sLocalBufSize, cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "WriteToFLASH - 6/6: Successful write - return code");
}

/*=======================================================================================
** Test_CFE_PSP_DeleteFile(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_DeleteFile(void)
{
    int32 iReturnCode;
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL Filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "DeleteFile: NULL filename\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_DeleteFile(NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "DeleteFile - 1/4: NULL Pointer - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "DeleteFile - 1/4: NULL Pointer - message");

    /* ----- Test case #2 - File is not present on system ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 1);
    /* Execute test */
    iReturnCode = CFE_PSP_DeleteFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "DeleteFile - 2/4: File not present on system - return code");

    /* ----- Test case #3 - Remove fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_remove), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "DeleteFile: Failed to remove file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_DeleteFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "DeleteFile - 3/4: Failed to remove file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "DeleteFile - 3/4: Failed to remove file - message");

    /* ----- Test case #4 - Successful remove ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(PCS_remove), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_DeleteFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "DeleteFile - 4/4: Failed to remove file - return code");
}

/*=======================================================================================
** Test_CFE_PSP_CreateFile(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_CreateFile(void)
{
    int32 iReturnCode;
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL Filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CreateFile: NULL filename\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "CreateFile - 1/5: NULL filename - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 1/5: NULL filename - message");

    /* ----- Test case #2 - File does exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateFile - 2/5 File exists - return code");

    /* ----- Test case #3 - Create fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(PCS_creat), 1, -1);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CreateFile: Failed to create file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "CreateFile - 3/5: Failed to create file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 3/5: Failed to create file - message");

    /* ----- Test case #4 - Fail to close file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(PCS_creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_close), 1, OS_ERROR);
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CreateFile: Failed to close file\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateFile - 4/5: Failed to close file - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateFile - 4/5: Failed to close file - message");

    /* ----- Test case #5 - Successfully created file ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(PCS_creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(PCS_close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateFile - 5/5: Failed to close file - return code");
}

/*=======================================================================================
** Test_CFE_PSP_CheckFile(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_CheckFile(void)
{
    bool iReturnValue;
    char cFakeFileName[] = "testFile.matt";
    char cMsg[256] = {};

    /* ----- Test case #1 - NULL filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CheckFile: NULL filename\n");
    /* Execute test */
    iReturnValue = CFE_PSP_CheckFile(NULL);
    /* Verify results */
    UtAssert_True((iReturnValue == false), UT_FLASH_PRINT_SCOPE "CheckFile - 1/3: NULL filename - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CheckFile - 1/3: NULL filename - message");

    /* ----- Test case #2 - File does exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 0);
    /* Execute test */
    iReturnValue = CFE_PSP_CheckFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnValue == true), UT_FLASH_PRINT_SCOPE "CheckFile - 2/3: File exists - return code");

    /* ----- Test case #3 - File does not exist ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_stat), 1, 1);
    /* Execute test */
    iReturnValue = CFE_PSP_CheckFile(cFakeFileName);
    /* Verify results */
    UtAssert_True((iReturnValue == false), UT_FLASH_PRINT_SCOPE "CheckFile - 3/3: File does not exists - return code");
}

/*=======================================================================================
** Test_CFE_PSP_FLASH_CreateDirectory(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_CreateDirectory(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};

    char caDirectory[20] = "/testdir";

    /* ----- Test case #1 - NULL check ----- */
    /* Additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CreateDirectory: Invalid Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory((char *)NULL);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_INVALID_POINTER), UT_FLASH_PRINT_SCOPE "CreateDirectory - 1/4: NULL check - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateDirectory - 1/4: NULL check - message");

    /* ----- Test case #2 - Status < 0, errno != 17 ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_mkdir), 1, -5);
    PCS_errno = 2;
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CreateDirectory: STATUS: %d\n", -5);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory(caDirectory);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_ERROR), UT_FLASH_PRINT_SCOPE "CreateDirectory - 2/4: mkdir error - return code");
    UtAssert_OS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateDirectory - 2/4: mkdir error - message");

    /* ----- Test case #2 - Status < 0, errno == 17 (Folder already exists) ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(PCS_mkdir), 1, -5);
    PCS_errno = 17;
    snprintf(cMsg, sizeof(cMsg), UT_FLASH_PRINT_SCOPE "CreateDirectory: STATUS: %d\n", -5);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory(caDirectory);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateDirectory - 3/4: mkdir error - return code");
    UtAssert_NoOS_print(cMsg, UT_FLASH_PRINT_SCOPE "CreateDirectory - 3/4: mkdir error - message");

    /* ----- Test case #3 - Successful run ----- */
    /* Additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(PCS_mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_CreateDirectory(caDirectory);
    /* Verify results */
    UtAssert_True((iReturnCode == CFE_PSP_SUCCESS), UT_FLASH_PRINT_SCOPE "CreateDirectory - 4/4: Successful execution - return code");
}

