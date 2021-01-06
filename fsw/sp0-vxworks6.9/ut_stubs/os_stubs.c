#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "osapi.h"

#define OS_BUFFER_SIZE 172
#define MAX_OS_PRINTF_MESSAGES      50
#define MAX_LOGMSG_MESSAGES      50

/* Global array for OS_printf messages */
char OS_printfMsgList[MAX_OS_PRINTF_MESSAGES][OS_BUFFER_SIZE];
uint8   ucOS_printfMessageCounts = 0;

/* Global array for logMsg messages */
char logMsgList[MAX_LOGMSG_MESSAGES][OS_BUFFER_SIZE];
uint8   ucLogMsgCounts = 0;

int open(const char * file, int oflag, ...)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(open);

    return iStatus;
}

int creat(const char * file, int oflag, ...)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(creat);

    return iStatus;
}

int close(int fd)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(close);

    return iStatus;
}


int read(int fd, void * buf, size_t n)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(read);

    return iStatus;
}

int write(int fd, const void * buf, size_t n)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(write);

    return iStatus;
}

void reboot(int iBootType)
{
    UT_DEFAULT_IMPL(reboot);
}

void exit(int iCode)
{
    UT_DEFAULT_IMPL(exit);
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

void logMsg(const char *pcString, ...)
{
    UT_Stub_RegisterContext(UT_KEY(logMsg), pcString);

    int32   iStatus;
    int32   iLength = strlen(pcString);
    va_list va;

    va_start(va,pcString);

    iStatus = UT_DefaultStubImplWithArgs(__func__, UT_KEY(logMsg), 0, va);

    if (iStatus >= 0)
    {
        /*
         * Special treatment of a format string which is only "%s" ...
         * This is merely a way to avoid having to do full-blown printf processing
         * inside the UT stub (which would make it the full version, not a stub)
         */
        if (strcmp(pcString, "%s") == 0 ||
                strcmp(pcString, "%s\n") == 0)
        {
            pcString = va_arg(va,const char *);
        }
        iLength = strlen(pcString);
        if (iLength > 0)
        {
            /*
             * prune any supplied newlines -
             * one will be explicitly added
             *
             * (this is to ensure a consistent separator in the output buffer)
             */
            while (iLength > 0 && pcString[iLength-1] == '\n')
            {
                --iLength;
            }
            UT_Stub_CopyFromLocal(UT_KEY(logMsg), pcString, iLength);
            UT_Stub_CopyFromLocal(UT_KEY(logMsg), "\n", 1);
        }
    }

    va_end(va);
}

int32 Ut_logMsg_hook(void *pUserObj, int32 iStubRetcode,
        uint32 uiCallCount, const UT_StubContext_t *pContext, va_list va)
{
    char cTestText[OS_BUFFER_SIZE];
    const char *cString;
    cTestText[0] = 0;

    /*
     * The logMsg stub passes the string as the
     * first context argument.
     */
    if (pContext->ArgCount > 0)
    {
        cString = UT_Hook_GetArgValueByName(pContext, "pcString", const char *);
        if (cString != NULL)
        {
            vsnprintf(cTestText, sizeof(cTestText), cString, va);
        }

        strcpy(logMsgList[ucLogMsgCounts], cTestText);
        ucLogMsgCounts++;
    }

    return iStubRetcode;
}

/*
 * Helper function to set up for event checking
 * This attaches the hook function to logMsg
 */
void Ut_logMsg_Setup(void)
{
    /* Set function hook, reset internal storage */
    UT_SetVaHookFunction(UT_KEY(logMsg), Ut_logMsg_hook, NULL);
    memset(logMsgList, 0, sizeof(logMsgList));
    ucLogMsgCounts = 0;
}

/* 
 * Helper function for asserting agains logMsg. Used to compare string
 */
bool Ut_logMsgHistoryWithText(const char *pcText)
{
    bool bResult = false;
    uint16 uiIdx;

    for (uiIdx = 0; uiIdx < ucLogMsgCounts; ++uiIdx)
    {
        if (strcmp(logMsgList[uiIdx], pcText) == 0)
        {
            bResult = true;
            break;
        }
    }

    return bResult;
}

int sysClkRateGet(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysClkRateGet);

    return iStatus;
}

void vxTimeBaseGet(uint32 *uiU, uint32 *uiL)
{
    UT_DEFAULT_IMPL(vxTimeBaseGet);
    *uiU = 1;
    *uiL = 0;
}

uint32 sysTimestampFreq(void)
{
    uint32 uiStatus;

    uiStatus = UT_DEFAULT_IMPL(sysTimestampFreq);

    return uiStatus;
}

uint32 vxDecGet (void)
{
    uint32 uiStatus;

    uiStatus = UT_DEFAULT_IMPL(vxDecGet);

    return uiStatus;
}
