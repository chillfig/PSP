#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "utstubs.h"

#include "osapi.h"

#define OS_BUFFER_SIZE 172
#define MAX_OS_PRINTF_MESSAGES      50
#define MAX_LOGMSG_MESSAGES      50

/* ERROR CODES for IPCOM, used with (at least) CFE_PSP_StartNTPDaemon(void) */
#define IPCOM_SUCCESS               (0)     // Generic Success
/* Why is below error code causing undefined error? */
#define IPCOM_ERROR                 (-1)    // Generic Error
#define IPCOM_ERR_ALREADY_STARTED   (-2)    // Error, IPCOM Already Started
#define IPCOM_ERR_NOT_STARTED       (-3)    // Error, IPCOM Not Started

/* Global array for OS_printf messages */
char OS_printfMsgList[MAX_OS_PRINTF_MESSAGES][OS_BUFFER_SIZE];
uint8   ucOS_printfMessageCounts = 0;

/* Global array for logMsg messages */
char logMsgList[MAX_LOGMSG_MESSAGES][OS_BUFFER_SIZE];
uint8   ucLogMsgCounts = 0;

STATUS remove(const char * name)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(remove);

    return iStatus;
}

int open(const char * file, int oflag, ...)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(open);
    
    if (iStatus < 0)
    {
        errno = 9;
    }
    else
    {
        errno = 0;
    }

    return iStatus;
}

int creat(const char * file, int oflag, ...)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(creat);

    return iStatus;
}

int mkdir(const char * dir)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(mkdir);

    return iStatus;
}

int close(int fd)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(close);

    if (iStatus < 0)
    {
        errno = 9;
    }
    else
    {
        errno = 0;
    }

    return iStatus;
}

int read(int fd, void * buf, size_t n)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(read);
    if (iStatus > 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(read), (void *) buf, n);
    }

    return iStatus;
}

int write(int fd, const void * buf, size_t n)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(write);

    if (iStatus < 0)
    {
        errno = 9;
    }
    else
    {
        errno = 0;
    }

    return iStatus;
}

off_t lseek(int fd, off_t offset, int whence)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(lseek);

    return iStatus;
}

struct stat {
    int value;
};

int stat(char *fname, struct stat *buf)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(stat);

    return iStatus;
}

void reboot(int iBootType)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(reboot);
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
 ** \param[in] quite - When True, it will only return the number of messages
 ** in the OS_printf array
 */
void Ut_OS_printfPrint(void)
{
    uint16 uiIdx;
    bool quite = false;

    if (quite == false)
    {
        printf("OS_printf Queue [%d]\n",ucOS_printfMessageCounts);
        for (uiIdx = 0; uiIdx < ucOS_printfMessageCounts; ++uiIdx)
        {
            printf("%s ",OS_printfMsgList[uiIdx]);
        }
        printf("\n");
    }
}


/**
 ** \brief Gives the number of messages printed
 ** \return uint8
 */
uint8   Ut_OS_printf_MsgCount(void)
{
    return ucOS_printfMessageCounts;
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


int32 taskNameToId(const char *name) 
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(taskNameToId);

    return iStatus;
}

int32 taskSuspend(TASK_ID task_id) 
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(taskSuspend);

    return iStatus;
}

int32 taskResume(TASK_ID task_id) 
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(taskResume);

    return iStatus;
}

struct timespec {
    uint32 tv_sec;
    uint32 tv_nsec;
};

int clock_settime(int clk_id, struct timespec *t)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(clock_settime);

    return iStatus;
}

int clock_gettime(int clk_id, struct timespec *t)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(clock_gettime);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(clock_gettime), (struct timespec *) t, sizeof(*t));
    }

    return iStatus;
}

int ipcom_ipd_start(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(ipcom_ipd_start);

    return iStatus;
}

/* ipcom_ipd_kill( ) - stops a daemon  */
int ipcom_ipd_kill(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(ipcom_ipd_kill);

    return iStatus;
}

int tempSensorRead (int8 sensor, uint8 dataType, float *temperature, bool talkative )
{
    int32 iStatus;
    iStatus = UT_DEFAULT_IMPL(tempSensorRead);
    *temperature = 25.5f;

    return iStatus;
}

int volSensorRead(int8 sensor, uint8 dataType, float *voltage, bool talkative )
{
    int32 iStatus;
    iStatus = UT_DEFAULT_IMPL(volSensorRead);
    *voltage = 5.05f;

    return iStatus;
}

/* GetUsecTime( ) – gets the time in micro-seconds since startup. */
double GetUsecTime(void)
{
    /* Time in microseconds */
    return (double)100.00;
}

STATUS scrubMemory(uint32_t startAddr, uint32_t endAddr, uint32_t *pNumPagesScrubbed)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(scrubMemory);

    return iStatus;
}

STATUS sysNvRamGet(
char *dat_ptr, /* pointer to data location */
int nbytes, /* size of the buffer */
int offset /* byte offset in EEPROM */
)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(sysNvRamGet);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(sysNvRamGet), (char *) dat_ptr, nbytes);
    }

    return iStatus;
}

STATUS sysNvRamSet(
char *dat_ptr, /* pointer to data location */
int nbytes, /* size of the buffer */
int offset /* byte offset in EEPROM */
)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(sysNvRamSet);

    return iStatus;
}
