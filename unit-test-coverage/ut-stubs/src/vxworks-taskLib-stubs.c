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

/* PSP coverage stub replacement for taskLib.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_taskLib.h"
#include "PCS_errnoLib.h"

static PCS_WIND_TCB PCS_LOCAL_TASK = {0};

const char *PCS_taskName(PCS_TASK_ID task_id)
{
    int32 Status;
    char *BufPtr;

    BufPtr = NULL;
    Status = UT_DEFAULT_IMPL(PCS_taskName);
    if (Status == 0)
    {
        UT_GetDataBuffer(UT_KEY(PCS_taskName), (void **)&BufPtr, NULL, NULL);
    }

    return BufPtr;
}

void PCS_taskExit(int code)
{
    UT_DEFAULT_IMPL(PCS_taskExit);
}

PCS_TASK_ID PCS_taskIdSelf(void)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_taskIdSelf);
    if (Status != 0)
    {
        return (PCS_TASK_ID)PCS_ERROR;
    }

    return &PCS_LOCAL_TASK;
}

PCS_TASK_ID PCS_taskNameToId(const char *name)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_taskNameToId);
    if (Status != 0)
    {
        return (PCS_TASK_ID)PCS_ERROR;
    }

    return &PCS_LOCAL_TASK;
}

PCS_STATUS PCS_taskDelay(int ticks)
{
    return UT_DEFAULT_IMPL(PCS_taskDelay);
}

PCS_STATUS PCS_taskDelete(PCS_TASK_ID tid)
{
    return UT_DEFAULT_IMPL(PCS_taskDelete);
}

PCS_STATUS PCS_taskDeleteForce(PCS_TASK_ID tid)
{
    return UT_DEFAULT_IMPL(PCS_taskDeleteForce);
}

PCS_STATUS PCS_taskSuspend(PCS_TASK_ID tid)
{
    return UT_DEFAULT_IMPL(PCS_taskSuspend);
}

PCS_STATUS PCS_taskResume(PCS_TASK_ID tid)
{
    return UT_DEFAULT_IMPL(PCS_taskResume);
}

PCS_STATUS PCS_taskPrioritySet(PCS_TASK_ID tid, int newPriority)
{
    return UT_DEFAULT_IMPL(PCS_taskPrioritySet);
}

PCS_STATUS PCS_taskPriorityGet(PCS_TASK_ID tid, int *pPriority)
{
    int32 iStatus;
    UT_Stub_RegisterContext(UT_KEY(PCS_taskPriorityGet), pPriority);

    iStatus = UT_DEFAULT_IMPL(PCS_taskPriorityGet);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(PCS_taskPriorityGet), (int32 *) pPriority, sizeof(*pPriority));
    }
    
    return iStatus;
}

PCS_STATUS PCS_taskInit(PCS_WIND_TCB *pTcb, char *name, int priority, int options, char *pStackBase, int stackSize,
                        PCS_FUNCPTR entryPt, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7,
                        int arg8, int arg9, int arg10)
{
    return UT_DEFAULT_IMPL(PCS_taskInit);
}

PCS_TASK_ID PCS_taskSpawn(char *name, int priority, int options, int stackSize, PCS_FUNCPTR entryPt, int arg1, int arg2,
                          int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)

{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_taskSpawn);
    if (Status != 0)
    {
        return (PCS_TASK_ID)PCS_ERROR;
    }

    return &PCS_LOCAL_TASK;
}

PCS_STATUS PCS_taskActivate(PCS_TASK_ID tid)
{
    int32 Status;

    Status = UT_DEFAULT_IMPL(PCS_taskActivate);

    return Status;
}

PCS_WIND_TCB *PCS_taskTcb(PCS_TASK_ID tid)
{
    int32         Status;
    PCS_WIND_TCB *LocalTcb;

    Status = UT_DEFAULT_IMPL(PCS_taskTcb);
    if (Status != 0)
    {
        return NULL;
    }

    if (UT_Stub_CopyToLocal(UT_KEY(PCS_taskTcb), &LocalTcb, sizeof(LocalTcb)) < sizeof(LocalTcb))
    {
        /*
         * On VxWorks the TASK_ID is defined as a direct type cast
         * of the TCB address.  This is actually documented
         * in the API and application code that works with TCBs
         * certainly will depend on this being the case.
         */
        LocalTcb = (PCS_WIND_TCB *)tid;
    }

    return LocalTcb;
}

PCS_STATUS PCS_taskLock(void)
{
    return UT_DEFAULT_IMPL(PCS_taskLock);
}

PCS_STATUS PCS_taskUnlock(void)
{
    return UT_DEFAULT_IMPL(PCS_taskUnlock);
}

PCS_STATUS PCS_taskCpuLock(void)
{
    return UT_DEFAULT_IMPL(PCS_taskCpuLock);
}

PCS_STATUS PCS_taskCpuUnlock(void)
{
    return UT_DEFAULT_IMPL(PCS_taskCpuUnlock);
}