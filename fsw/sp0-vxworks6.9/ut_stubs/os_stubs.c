#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "osapi.h"

#define OS_BUFFER_SIZE 172
#define MAX_OS_PRINTF_MESSAGES      50

/* Global array for OS_printf messages */
char OS_printfMsgList[MAX_OS_PRINTF_MESSAGES][OS_BUFFER_SIZE];
uint8   ucOS_printfMessageCounts = 0;

/*****************************************************************************
 *
 * Stub function for OS_printf()
 *
 *****************************************************************************/
// void OS_printf(const char *String, ...)
// {
//     int32   status;
//     int32   length = strlen(String);
//     va_list va;

//     UT_Stub_RegisterContext(UT_KEY(OS_printf), String);
//     va_start(va, String);
//     status = UT_DEFAULT_IMPL_VARARGS(OS_printf, va);

//     if (status >= 0)
//     {
//         /*
//          * Special treatment of a format string which is only "%s" ...
//          * This is merely a way to avoid having to do full-blown printf processing
//          * inside the UT stub (which would make it the full version, not a stub)
//          */
//         if (strcmp(String, "%s") == 0 ||
//                 strcmp(String, "%s\n") == 0)
//         {
//             String = va_arg(va,const char *);
//         }
//         length = strlen(String);
//         if (length > 0)
//         {
//             /*
//              * prune any supplied newlines -
//              * one will be explicitly added
//              *
//              * (this is to ensure a consistent separator in the output buffer)
//              */
//             while (length > 0 && String[length-1] == '\n')
//             {
//                 --length;
//             }
//             UT_Stub_CopyFromLocal(UT_KEY(OS_printf), String, length);
//             UT_Stub_CopyFromLocal(UT_KEY(OS_printf), "\n", 1);
//         }
//     }

//     va_end(va);
// }

int open (const char * file, int oflag, ...)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(open);

    return Status;
}

int creat (const char * file, int oflag, ...)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(creat);

    return Status;
}

int close (int fd)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(close);

    return Status;
}


int read (int fd, void * buf, size_t n)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(read);

    return Status;
}

int write(int fd, const void * buf, size_t n)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(write);

    return Status;
}

int32 Ut_OS_printf_hook(void *UserObj, int32 StubRetcode,
        uint32 CallCount, const UT_StubContext_t *Context, va_list va)
{
    char cTestText[OS_BUFFER_SIZE];
    const char *cString;
    cTestText[0] = 0;

    /*
     * The OS_printf stub passes the String as the
     * first context argument.
     */
    if (Context->ArgCount > 0)
    {
        cString = UT_Hook_GetArgValueByName(Context, "string", const char *);
        if (cString != NULL)
        {
            vsnprintf(cTestText, sizeof(cTestText), cString, va);
        }

        strcpy(OS_printfMsgList[ucOS_printfMessageCounts], cTestText);
        ucOS_printfMessageCounts++;
    }

    return StubRetcode;
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
bool Ut_OS_printfHistoryWithText(const char *cText)
{
    bool bResult = false;
    uint16 uiIdx;

    for (uiIdx = 0; uiIdx < ucOS_printfMessageCounts; ++uiIdx)
    {
        if (strcmp(OS_printfMsgList[uiIdx], cText) == 0)
        {
            bResult = true;
            break;
        }
    }

    return bResult;
}

