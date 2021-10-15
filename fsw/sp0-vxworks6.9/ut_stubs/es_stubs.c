#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"
/* #include "cfe_es.h" */

#define MAX_ES_WRITETOSYSLOG_MESSAGES      100
#define ES_BUFFER_SIZE 172

/* Global array for CFE_ES_WriteToSysLog messages */
char    gcES_WriteToSysLogMsgList[MAX_ES_WRITETOSYSLOG_MESSAGES][ES_BUFFER_SIZE];
uint8   gucES_WriteToSysLogMsgCounts = 0;

/*****************************************************************************/
/**
** \brief CFE_ES_WriteToSysLog stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_WriteToSysLog.  It always returns CFE_SUCCESS when called.
**        The log message is compared to known CFE function responses and if
**        a match is found the value WriteSysLogRtn.value is set to a
**        specified constant value.  WriteSysLogRtn.count is incremented to
**        keep track of the number of messages logged.  The unit test code
**        compares the WriteSysLogRtn value and count variables against
**        expected totals to ensure the proper response of other functions
**        being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_WriteToSysLog), SpecStringPtr);

    int32   status;
    va_list va;

    va_start(va,SpecStringPtr);
    status = UT_DEFAULT_IMPL_VARARGS(CFE_ES_WriteToSysLog, va);
    va_end(va);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_WriteToSysLog), (const uint8*)SpecStringPtr, strlen(SpecStringPtr));
    }

    return status;
}

int32 Ut_ES_WriteToSysLog_Hook(void *UserObj, int32 StubRetcode,
        uint32 CallCount, const UT_StubContext_t *Context, va_list va)
{
    char cTestText[ES_BUFFER_SIZE];
    const char *cString;
    cTestText[0] = 0;

    /*
     * The CFE_ES_WriteToSysLog stub passes the SpecStringPtr as the
     * first context argument.
     */
    if (Context->ArgCount > 0)
    {
        cString = UT_Hook_GetArgValueByName(Context, "SpecStringPtr", const char *);
        if (cString != NULL)
        {
            vsnprintf(cTestText, sizeof(cTestText), cString, va);
        }
        strcpy(gcES_WriteToSysLogMsgList[gucES_WriteToSysLogMsgCounts], cTestText);
        gucES_WriteToSysLogMsgCounts++;
    }

    return StubRetcode;
}

/*
 * Helper function to set up for event checking
 * This attaches the hook function to CFE_ES_WriteToSysLog
 */
void Ut_ES_WriteToSysLog_Setup(void)
{
    /* Set function hook, reset internal storage */
    UT_SetVaHookFunction(UT_KEY(CFE_ES_WriteToSysLog), Ut_ES_WriteToSysLog_Hook, NULL);
    memset(gcES_WriteToSysLogMsgList, 0, sizeof(gcES_WriteToSysLogMsgList));
    gucES_WriteToSysLogMsgCounts = 0;
}

/* 
 * Helper function for asserting agains CFE_ES_WriteToSysLog. Used to compare string
 */
bool Ut_ES_WriteToSysLogWithText(const char *cText)
{
    bool bResult = false;
    uint16 uiIdx;

    for (uiIdx = 0; uiIdx < gucES_WriteToSysLogMsgCounts; ++uiIdx)
    {
        if (strcmp(gcES_WriteToSysLogMsgList[uiIdx], cText) == 0)
        {
            bResult = true;
            break;
        }
    }

    return bResult;
}

