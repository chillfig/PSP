#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"

#define MAX_RDWR_SIZE       0x01000000  /* 16MB */

int aimonGetBITExecuted(uint64 *pulRun, int iValue)
{
    UT_Stub_RegisterContext(UT_KEY(aimonGetBITExecuted), pulRun);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(aimonGetBITExecuted);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(aimonGetBITExecuted), (uint64 *)pulRun, sizeof(*pulRun));
    }

    return iStatus;
}

int aimonGetBITResults(uint64 *pulResult, int iValue)
{
    UT_Stub_RegisterContext(UT_KEY(aimonGetBITResults), pulResult);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(aimonGetBITResults);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(aimonGetBITResults), (uint64 *)pulResult, sizeof(*pulResult));
    }

    return iStatus;
}

int32 ReadResetSourceReg(uint32 *puiResetSrc, uint32 uiTalkAtive)
{
    UT_Stub_RegisterContext(UT_KEY(ReadResetSourceReg), puiResetSrc);
    UT_Stub_RegisterContextGenericArg(UT_KEY(ReadResetSourceReg), uiTalkAtive);

    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(ReadResetSourceReg);

    if (iStatus >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(ReadResetSourceReg), (uint32 *)puiResetSrc, sizeof(*puiResetSrc)) < sizeof(*puiResetSrc))
        {
            *puiResetSrc = 0x08;
        }
    }

    return iStatus;
}

int32 ReadSafeModeUserData(void *pSafeModeUserData, uint32 uiTalkAtive)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(ReadSafeModeUserData);

    return iStatus;
}

int32 taskPriorityGet(int32 iTid, int32 *piCurPrio)
{
    UT_Stub_RegisterContext(UT_KEY(taskPriorityGet), piCurPrio);
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(taskPriorityGet);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(taskPriorityGet), (uint64 *)piCurPrio, sizeof(*piCurPrio));
    }
    
    return iStatus;
}

int32 taskPrioritySet(int32 iTid, int32 iNewPrio)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(taskPrioritySet);

    return iStatus;
}

