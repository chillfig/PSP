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
 
/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/* PSP coverage stub replacement for unistd.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_unistd.h"

#define PCS_MAX_RDWR_SIZE 0x01000000 /* 16MB */

int PCS_close(int fd)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_close);

    return Status;
}

PCS_gid_t PCS_getegid(void)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_getegid);

    return Status;
}

PCS_uid_t PCS_geteuid(void)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_geteuid);

    return Status;
}

long int PCS_gethostid(void)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_gethostid);

    return Status;
}

int PCS_gethostname(char *name, size_t len)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_gethostname);

    if (Status == 0 && len > 0)
    {
        strncpy(name, "ut", len - 1);
        name[len - 1] = 0;
    }

    return Status;
}

PCS_pid_t PCS_getpid(void)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_getpid);

    return Status;
}

PCS_off_t PCS_lseek(int fd, PCS_off_t offset, int whence)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_lseek);

    return Status;
}

PCS_ssize_t PCS_read(int fd, void *buf, size_t n)
{
    int32  Status;
    size_t CopySize;

    Status = UT_DEFAULT_IMPL_RC(PCS_read, PCS_MAX_RDWR_SIZE);

    if (Status > 0)
    {
        if (Status > n)
        {
            CopySize = n;
        }
        else
        {
            CopySize = Status;
        }

        CopySize = UT_Stub_CopyToLocal(UT_KEY(PCS_read), buf, CopySize);

        if (CopySize != 0)
        {
            Status = CopySize;
        }
        else if (Status <= n)
        {
            memset(buf, 'r', Status);
        }
        else if (UT_GetStubCount(UT_KEY(PCS_read) < 4))
        {
            memset(buf, 'r', n);
            Status = n;
        }
        else
        {
            Status = 0;
        }
    }

    return Status;
}

int PCS_rmdir(const char *path)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_rmdir);

    return Status;
}

long int PCS_sysconf(int name)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_sysconf);

    return Status;
}

PCS_ssize_t PCS_write(int fd, const void *buf, size_t n)
{
    int32  Status;
    size_t CopySize;

    Status = UT_DEFAULT_IMPL_RC(PCS_write, PCS_MAX_RDWR_SIZE);

    if (Status > 0)
    {
        if (Status > n)
        {
            CopySize = n;
        }
        else
        {
            CopySize = Status;
        }

        CopySize = UT_Stub_CopyFromLocal(UT_KEY(PCS_write), buf, CopySize);

        if (CopySize != 0)
        {
            Status = CopySize;
        }
        else if (Status > n)
        {
            Status = n;
        }
    }

    return Status;
}
