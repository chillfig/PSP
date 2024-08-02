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

/* PSP coverage stub replacement for string.h */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"

#include "common_types.h"
#include "cfe_psp_timesync.h"

#include "PCS_vxWorks.h"
#include "PCS_cfe_time.h"

uint32 PCS_CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    int32 iVal;

    iVal = UT_DEFAULT_IMPL(PCS_CFE_TIME_Micro2SubSecs);
    
    return iVal;
}

CFE_TIME_SysTime_t PCS_CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    CFE_TIME_SysTime_t retval = {0};
    CFE_TIME_SysTime_t *pRetval = NULL;

    int iStatus = UT_DEFAULT_IMPL(PCS_CFE_TIME_Subtract);

    if (iStatus == PCS_OK)
    {
        UT_GetDataBuffer(UT_KEY(PCS_CFE_TIME_Subtract), (void **)&pRetval, NULL, NULL);
    }

    if (pRetval != NULL)
    {
        return *pRetval;
    }

    return retval;
}