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

/* PSP coverage stub replacement for userReservedMem.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_userReservedMem.h"

#define URM_SIZE 10000

char UserReservedMemory[URM_SIZE] = {'\0'};
size_t uURM = URM_SIZE;
char *pURM = UserReservedMemory;
char *pEndOfURM = UserReservedMemory + sizeof(UserReservedMemory);

void PCS_userReservedGet( char **  pUserReservedAddr, size_t * pUserReservedSize )
{
    uint32 iSize;

    iSize = UT_DEFAULT_IMPL(PCS_userReservedGet);

    *pUserReservedAddr = pURM;
    uURM = (size_t)iSize;
    *pUserReservedSize = uURM;
}

int PCS_userMemAlloc(uint32_t *addr, uint32_t size, bool talk)
{
    int iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_userMemAlloc);
    
    if (iStatus >= 0)
    {
        /* Check that there is enough memory available in the UserReservedMemory array */
        if ( ((cpuaddr)pEndOfURM + size) <= ((cpuaddr)UserReservedMemory + URM_SIZE) )
        {
            *addr = (cpuaddr)pEndOfURM;
            pEndOfURM = pEndOfURM + size;
        }
        else
        {
            pEndOfURM = UserReservedMemory;
        }
    }

    return iStatus;
}