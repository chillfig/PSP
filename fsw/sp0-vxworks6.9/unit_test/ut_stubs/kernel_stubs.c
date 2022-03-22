#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include <bootLib.h>
#include "utstubs.h"

#define MAX_RDWR_SIZE       0x01000000  /* 16MB */

typedef enum {SP0_ORIGINAL = 1, SP0_UPGRADE = 2} SP0_BOARD_GENERATION_TYPE;

char UserReservedMemory[4000] = {'\0'};
char *pURM = UserReservedMemory;
size_t uURM = 4000;
char bString[BOOT_FILE_LEN] = "/ffx0/startup";

void userReservedGet( char **  pUserReservedAddr, size_t * pUserReservedSize )
{
    int iSize;

    iSize = UT_DEFAULT_IMPL(userReservedGet);

    pUserReservedAddr = &pURM;
    uURM = iSize;
    pUserReservedSize = &uURM;
}

int userMemAlloc(uint32 *addr, uint32 size, bool talk)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(userMemAlloc);
    if (iStatus >= 0)
    {
        *addr = (uint32)pURM;
    }

    return iStatus;
}

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

/**
 ** \brief UT Read the Safe Mode User Data
 ** \par Description:
 ** iStatus >= 0 --> pSafeModeUserData
 ** iStatus = -1 --> Error
 ** \param[out] pSafeModeUserData 
 ** \param[in] uiTalkAtive 
 ** \return int32 
 */
int32 ReadSafeModeUserData(void *pSafeModeUserData, uint32 uiTalkAtive)
{
    int32 iStatus;

    UT_Stub_RegisterContext(UT_KEY(ReadSafeModeUserData), pSafeModeUserData);

    iStatus = UT_DEFAULT_IMPL(ReadSafeModeUserData);

    if (iStatus >= 0)
    {
        /* 24 is the number of bytes of the USER_SAFE_MODE_DATA_STRUCT structure */
        UT_Stub_CopyToLocal(UT_KEY(ReadSafeModeUserData), (void *)pSafeModeUserData, 24);
        return OK;
    }

    return ERROR;
}

int32 taskPriorityGet(int32 iTid, int32 *piCurPrio)
{
    int32 iStatus;
    UT_Stub_RegisterContext(UT_KEY(taskPriorityGet), piCurPrio);

    iStatus = UT_DEFAULT_IMPL(taskPriorityGet);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(taskPriorityGet), (int32 *) piCurPrio, sizeof(*piCurPrio));
    }
    
    return iStatus;
}

int32 taskPrioritySet(int32 iTid, int32 iNewPrio)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(taskPrioritySet);

    return iStatus;
}

char * taskName(TASK_ID tid)
{
    char * iStatus;

    iStatus = (char *) UT_DEFAULT_IMPL(taskName);

    return iStatus;
}

/* STATUS statfs64 (
    char          *name,
    struct statfs64 *pStat
    )
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(statfs64);

    return iStatus;
} */

char *sysModel(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysModel);
    if (iStatus == 1)
    {
        return "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE"
               "VERY LONG MESSAGE TO FILL THE BUFFER ABOVE THE MAXIMUM LENGTH ALLOWED"
               "THE CURRENT MAXIMUM LENGTH IS 1000 CHARACTERS. SO THIS MESSAGE"
               "MUST BE VERY VERY LONG TO MAKE A DIFFERENCE";
    }
    return "Typical Message";
}

uint32 getCoreClockSpeed(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(getCoreClockSpeed);
    if (iStatus == 0)
    {
        return 399;
    }
    return 333;
}

int returnSelectedBootFlash(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(returnSelectedBootFlash);

    return iStatus;
}

/*  Watchdog Stubs */
int sysEnableFpgaWdt ( BOOL globalReset )
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysEnableFpgaWdt);

    return iStatus;
}
int sysDisableFpgaWdt ( BOOL globalReset )
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysDisableFpgaWdt);

    return iStatus;
}
int sysSetFpgaWdt(float secs /* Number of seconds for timeout */)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(sysSetFpgaWdt);

    return iStatus;
}

/**
 * SP0_ORIGINAL = 1
 * SP0_UPGRADE =2
 */
SP0_BOARD_GENERATION_TYPE sysGetBoardGeneration(bool talkative)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(sysGetBoardGeneration);

    if (Status == 1)
    {
        return SP0_ORIGINAL;
    }
    else if (Status == 2)
    {
        return SP0_UPGRADE;
    }
    else
    {
        return 0;
    }
}

char *PCS_bootStringToStruct (char * bootString, BOOT_PARAMS * pBootParams)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(bootStringToStruct);

    if (Status == OK)
    {
        memcpy(pBootParams->startupScript, bString, BOOT_FILE_LEN);
    }

    return bString;
}

char *PCS_strncpy(char *dst, const char *src, size_t size)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_strncpy);

    if (Status == 0)
    {
        /* "nominal" response */
        return strncpy(dst, src, size);
    }

    return (char *)0;
}

int PCS_snprintf(char *s, size_t maxlen, const char *format, ...)
{
    int32   Status;
    int     actual = 0;
    va_list ap;

    Status = UT_DEFAULT_IMPL(PCS_snprintf);

    /* need to actually _do_ the snprintf */
    if (Status >= 0)
    {
        va_start(ap, format);
        actual = vsnprintf(s, maxlen, format, ap);
        va_end(ap);
    }

    if (Status != 0)
    {
        actual = Status;
    }
    return actual;
}

int PCS_OS_BSPMain(void)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_OS_BSPMain);

    return iStatus;
}