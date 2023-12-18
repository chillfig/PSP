/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file osal_stubs.c
*
*   \brief This file contains OSAL stub functions.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include "utstubs.h"
#include "uttools.h"

#include "osapi.h"

#define OS_BUFFER_SIZE 172
#define MAX_OS_PRINTF_MESSAGES      50

/* Global array for OS_printf messages */
char OS_printfMsgList[MAX_OS_PRINTF_MESSAGES][OS_BUFFER_SIZE];
uint8   ucOS_printfMessageCounts = 0;

int32 OS_SymbolLookup(cpuaddr *symbol_address, const char *symbol_name)
{
    int32 iStatus;
    UT_Stub_RegisterContext(UT_KEY(OS_SymbolLookup), symbol_address);
    UT_Stub_RegisterContext(UT_KEY(OS_SymbolLookup), symbol_name);

    iStatus = UT_DEFAULT_IMPL(OS_SymbolLookup);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(OS_SymbolLookup), (cpuaddr *)symbol_address, sizeof(*symbol_address));
    }
    else
    {
        symbol_address = NULL;
    }
    return iStatus;
}

int32 Ut_OS_printf_hook(void *pUserObj, int32 iStubRetcode,
        uint32 uiCallCount, const UT_StubContext_t *pContext, va_list va)
{
    char cTestText[OS_BUFFER_SIZE];
    const char *cString;
    cTestText[0] = 0;

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

        strcpy(OS_printfMsgList[ucOS_printfMessageCounts], cTestText);
        ucOS_printfMessageCounts++;
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