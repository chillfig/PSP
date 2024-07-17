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

/*
 *
 *    Copyright (c) 2020, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

/**
 * \file
 * \ingroup  vxworks
 * \author   
 *
 */

#include "coveragetest-psp-gr740-vxworks6.9.h"
#include "ut-adaptor-bootrec.h"

#include "cfe_psp.h"

#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"

void Test_CFE_PSP_Restart(void)
{
    /*
     * Test Case For:
     * void CFE_PSP_Restart(uint32 reset_type)
     */

    UT_Setup_ReservedMem_BootRec();
    CFE_PSP_Restart(CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_INT32_EQ(UT_Get_ReservedMem_BootType(), CFE_PSP_RST_TYPE_PROCESSOR);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_cacheTextUpdate)), 1);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_reboot)), 1);

    UT_Setup_ReservedMem_BootRec();
    CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);
    UtAssert_INT32_EQ(UT_Get_ReservedMem_BootType(), CFE_PSP_RST_TYPE_POWERON);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_cacheTextUpdate)), 2);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_reboot)), 2);
}

void Test_CFE_PSP_Panic(void)
{
    char cMsg[256] = {0};

    /* ----- Test #1 - Default case ----- */
    /* Setup */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* Execute Test */
    CFE_PSP_Panic(0);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_reboot)), 1);

    /* ----- Test #2 - Nominal ----- */
    /* Setup */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* Execute Test */
    CFE_PSP_Panic(CFE_PSP_PANIC_MEMORY_ALLOC);

    /* Verify results */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_reboot)), 1);
    snprintf(cMsg, sizeof(cMsg), "WARNING: PSP Panic called with %d\n", CFE_PSP_PANIC_MEMORY_ALLOC);
    UtAssert_OS_print(cMsg, "CFE_PSP_Panic - 2/2 Nominal ");
    snprintf(cMsg, sizeof(cMsg), "WARNING: PSP Restart called with %d\n", CFE_PSP_RST_TYPE_POWERON);
    UtAssert_OS_print(cMsg, "CFE_PSP_Panic - 2/2 Nominal ");

}

void Test_CFE_PSP_FlushCaches(void)
{
    /*
     * Test Case For:
     * void CFE_PSP_FlushCaches(uint32 type, void* address, uint32 size)
     */

    CFE_PSP_FlushCaches(0, NULL, 0);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_cacheTextUpdate)), 0);
    CFE_PSP_FlushCaches(1, NULL, 0);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(PCS_cacheTextUpdate)), 1);
}

void Test_CFE_PSP_GetProcessorId(void)
{
    /*
     * Test Case For:
     * uint32 CFE_PSP_GetProcessorId    (void)
     */

    /*
     * Note - the data structure used here is declared as "const" internally so
     * there is no way to modify the value at runtime, even in unit test.
     */
    UtAssert_INT32_EQ(CFE_PSP_GetProcessorId(), PCS_CONFIG_CPUNUMBER);

    /* Calling 2nd time to hit the stored processor ID case */
    UtAssert_INT32_EQ(CFE_PSP_GetProcessorId(), PCS_CONFIG_CPUNUMBER);
}

void Test_CFE_PSP_GetSpacecraftId(void)
{
    /*
     * Test Case For:
     * uint32 CFE_PSP_GetSpacecraftId   (void)
     */

    /*
     * Note - the data structure used here is declared as "const" internally so
     * there is no way to modify the value at runtime, even in unit test.
     */
    UtAssert_INT32_EQ(CFE_PSP_GetSpacecraftId(), PCS_CONFIG_SPACECRAFT);
}

void Test_CFE_PSP_GetProcessorName(void)
{
    /*
     * Test Case For:
     * char* CFE_PSP_GetProcessorName   (void)
     */

    /*
     * Note - the data structure used here is declared as "const" internally so
     * there is no way to modify the value at runtime, even in unit test.
     */

    UtAssert_StrCmp(CFE_PSP_GetProcessorName(), PCS_CONFIG_CPUNAME, "CFE_PSP_GetProcessorName - 1/1 Nominal");
}