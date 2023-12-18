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
*   \file vxworks_stubs.c
*
*   \brief VxWorks 6.9 stub functions
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

#include <vxWorks.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "utstubs.h"

/* ERROR CODES for IPCOM, used with (at least) CFE_PSP_StartNTPDaemon(void) */
#define IPCOM_SUCCESS               (0)     // Generic Success
/* Why is below error code causing undefined error? */
#define IPCOM_ERROR                 (-1)    // Generic Error
#define IPCOM_ERR_ALREADY_STARTED   (-2)    // Error, IPCOM Already Started
#define IPCOM_ERR_NOT_STARTED       (-3)    // Error, IPCOM Not Started

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
