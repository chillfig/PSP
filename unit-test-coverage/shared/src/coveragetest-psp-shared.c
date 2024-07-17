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
 * \author   joseph.p.hickey@nasa.gov
 *
 */

#include <stdarg.h>

#include "utassert.h"
#include "utstubs.h"
#include "ut-adaptor-exceptions.h"

#include "cfe_psp.h"
#include "PCS_stdio.h"

#define OS_BUFFER_SIZE 172
#define MAX_OS_PRINTF_MESSAGES      50

/* Global array for OS_printf messages */
char OS_printfMsgList[MAX_OS_PRINTF_MESSAGES][OS_BUFFER_SIZE];
uint8 ucOS_printfMessageCounts = 0;

int32 Ut_OS_printf_hook(void *pUserObj, int32 iStubRetcode,
        uint32 uiCallCount, const UT_StubContext_t *pContext, va_list va)
{
    char cTestText[OS_BUFFER_SIZE] = {0};
    const char *cString = NULL;
    /*
     * The OS_printf stub passes the string as the
     * first context argument.
     */
    if (pContext->ArgCount > 0)
    {
        cString = UT_Hook_GetArgValueByName(pContext, "string", const char *);
        if (cString != NULL)
        {
            vsnprintf(cTestText, sizeof(cTestText), cString, va);
        }

        if (ucOS_printfMessageCounts < MAX_OS_PRINTF_MESSAGES)
        {
            snprintf(OS_printfMsgList[ucOS_printfMessageCounts], OS_BUFFER_SIZE, cTestText);
            ucOS_printfMessageCounts++;
        }
    }

    return iStubRetcode;
}

/*
 * Helper function to set up for event checking
 * This attaches the hook function to OS_printf
 */
void Ut_OS_printf_Setup(void)
{
    /* Set function hook, reset internal storage */
    UT_SetVaHookFunction(UT_KEY(OS_printf), Ut_OS_printf_hook, NULL);
    memset(OS_printfMsgList, 0, sizeof(OS_printfMsgList));
    ucOS_printfMessageCounts = 0;
}

/* 
 * Helper function for asserting agains OS_printf. Used to compare string
 */
bool Ut_OS_printfHistoryWithText(const char *pcText)
{
    bool bResult = false;
    uint16 uiIdx;

    for (uiIdx = 0; uiIdx < ucOS_printfMessageCounts; ++uiIdx)
    {
        if (strcmp(OS_printfMsgList[uiIdx], pcText) == 0)
        {
            bResult = true;
            break;
        }
    }

    return bResult;
}

/**
 ** \brief Prints the whole array of strings printed using OS_printf
 ** \par Description:
 ** DEBUG Helper function Print all OS_printf messages
 */
void Ut_OS_printfPrint(void)
{
    uint16 uiIdx;

    printf("OS_printf Queue [%d]\n",ucOS_printfMessageCounts);
    for (uiIdx = 0; uiIdx < ucOS_printfMessageCounts; ++uiIdx)
    {
        printf("%s ",OS_printfMsgList[uiIdx]);
    }
    printf("\n");
}

/**
 ** \brief Gives the number of messages printed
 ** \return uint8
 */
uint8   Ut_OS_printf_MsgCount(void)
{
    return ucOS_printfMessageCounts;
}
