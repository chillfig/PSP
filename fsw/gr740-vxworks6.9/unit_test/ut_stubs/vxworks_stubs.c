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
*   \file vxworks_stubs.c
*
*   \brief This file defines stub functions related to VxWorks.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
**********************************************************************************************************/

#include <vxWorks.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utstubs.h"
#include "ut_psp_utils.h"

/* ERROR CODES for IPCOM, used with (at least) CFE_PSP_StartNTPDaemon(void) */
#define IPCOM_SUCCESS               (0)     // Generic Success
/* Why is below error code causing undefined error? */
#define IPCOM_ERROR                 (-1)    // Generic Error
#define IPCOM_ERR_ALREADY_STARTED   (-2)    // Error, IPCOM Already Started
#define IPCOM_ERR_NOT_STARTED       (-3)    // Error, IPCOM Not Started

char UserReservedMemory[URM_SIZE] = {'\0'};
size_t uURM = URM_SIZE;
char *pURM = UserReservedMemory;
char *pEndOfURM = UserReservedMemory;

void userReservedGet( char **  pUserReservedAddr, size_t * pUserReservedSize )
{
    uint32 iSize;

    iSize = UT_DEFAULT_IMPL(userReservedGet);

    *pUserReservedAddr = pURM;
    uURM = (size_t)iSize;
    *pUserReservedSize = uURM;
}

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

int strncmp(const char *__s1, const char *__s2, size_t __n)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(strncmp);

    return Status;
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
