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

/* PSP coverage stub replacement for taskLibCommon.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_taskLibCommon.h"
#include "PCS_errnoLib.h"

PCS_STATUS PCS_taskCpuAffinitySet(PCS_TASK_ID tid, PCS_cpuset_t newAffinity)
{
    return UT_DEFAULT_IMPL(PCS_taskCpuAffinitySet);
}

PCS_STATUS PCS_taskCpuAffinityGet(PCS_TASK_ID tid, PCS_cpuset_t *pAffinity)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_taskCpuAffinityGet);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(PCS_taskCpuAffinityGet), (PCS_cpuset_t *) pAffinity, sizeof(*pAffinity));
    }

    return iStatus;
}