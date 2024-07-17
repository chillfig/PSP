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

/* PSP coverage stub replacement for excLib.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_edrLib.h"

void PCS_excHookAdd(void (*Hook)(PCS_TASK_ID, int, void *))
{
    UT_DEFAULT_IMPL(PCS_excHookAdd);
}

bool PCS_currentedrPolicyHandlerHook1(int type, void *pInfo_param, bool debug)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_currentedrPolicyHandlerHook1);
    
    return iStatus;
}

void * PCS_edrPolicyHandlerHookGet(void)
{
    int32 iStatus;
    void *pRetVal = NULL;

    iStatus = UT_DEFAULT_IMPL(PCS_edrPolicyHandlerHookGet);
    
    if (iStatus == 0)
    {
        pRetVal = &PCS_currentedrPolicyHandlerHook1;
    }

    return pRetVal;
}

int PCS_edrPolicyHandlerHookAdd(void *pPtrFunc)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_edrPolicyHandlerHookAdd);

    return iStatus;
}

int PCS_edrErrorPolicyHookRemove(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_edrErrorPolicyHookRemove);

    return iStatus;
}