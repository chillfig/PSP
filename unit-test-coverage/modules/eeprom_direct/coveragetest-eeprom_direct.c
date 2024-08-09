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
 * \ingroup  modules
 *
 * Coverage test for VxWorks timebase module implementation
 */

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

#define UT_RAMBLOCK_SIZE 32

static union
{
    uint8  u8[UT_RAMBLOCK_SIZE];
    uint16 u16[UT_RAMBLOCK_SIZE / sizeof(uint16)];
    uint32 u32[UT_RAMBLOCK_SIZE / sizeof(uint32)];
} UT_RAM_BLOCK;

extern void eeprom_direct_Init(uint32 PspModuleId);

/*
 * --------------------------------------------
 * TEST FUNCTIONS
 * --------------------------------------------
 */

/* ********************************
 * eeprom_direct_Init
 * ********************************/
void Test_eeprom_direct_Init(void)
{
    /* Test For:
     * void eeprom_direct_Init(uint32 PspModuleId)
     */
    UtAssert_VOIDCALL(eeprom_direct_Init(1));
}

/* ********************************
 * CFE_PSP_EepromWrite32
 * ********************************/
void Test_CFE_PSP_EepromWrite32_Nominal(void)
{
    /* Arrange */
    cpuaddr UtAddress = (cpuaddr)&UT_RAM_BLOCK.u32[0];
    uint32 writevalue = 0x12345678;

    /* Act */
    UtAssert_INT32_EQ(CFE_PSP_EepromWrite32(UtAddress, writevalue), CFE_PSP_SUCCESS);

    /* Assert */
    UtAssert_INT32_EQ(*((uint32 *)UtAddress), writevalue);
}

void Test_CFE_PSP_EepromWrite32_AddressMisaligned(void)
{
    /* Arrange */
    cpuaddr UtAddress = (cpuaddr)&UT_RAM_BLOCK.u32[1];
    uint32 writevalue = 0x12345678;

    /* Act */
    UtAssert_INT32_EQ(CFE_PSP_EepromWrite32(UtAddress, writevalue), CFE_PSP_ERROR_ADDRESS_MISALIGNED);
}

/* ********************************
 * CFE_PSP_EepromWrite16
 * ********************************/
void Test_CFE_PSP_EepromWrite16_Nominal(void)
{
    /* Arrange */
    cpuaddr address = 0x1000;  // Properly aligned address
    uint32 writevalue = 1;

    /* Act */
    UtAssert_INT32_EQ(CFE_PSP_EepromWrite16(address, writevalue), CFE_PSP_SUCCESS);

    /* Assert */
    UtAssert_INT32_EQ(*((uint32 *)address), 1);
}

void Test_CFE_PSP_EepromWrite16_AddressMisaligned(void)
{
    /* Arrange */
    cpuaddr address = 0x1001;  // Misaligned address
    uint32 writevalue = 1;

    /* Act */
    UtAssert_INT32_EQ(CFE_PSP_EepromWrite16(address, writevalue), CFE_PSP_ERROR_ADDRESS_MISALIGNED);
}
void Test_CFE_PSP_EepromWrite8(void)
{
    /*
    int32 CFE_PSP_EepromWrite8(cpuaddr MemoryAddress, uint8 ByteValue)
    */
    UtAssert_INT32_EQ(CFE_PSP_EepromWrite8(0, 1), CFE_PSP_SUCCESS);
}

void Test_CFE_PSP_EepromWriteEnable(void)
{
    /*
    int32 CFE_PSP_EepromWriteEnable(uint32 Bank)
    */
    UtAssert_INT32_EQ(CFE_PSP_EepromWriteEnable(1), CFE_PSP_SUCCESS);
}

void Test_CFE_PSP_EepromWriteDisable(void)
{
    /*
    int32 CFE_PSP_EepromWriteDisable(uint32 Bank)
    */
    UtAssert_INT32_EQ(CFE_PSP_EepromWriteDisable(1), CFE_PSP_SUCCESS);
}

void Test_CFE_PSP_EepromPowerUp(void)
{
    /*
    int32 CFE_PSP_EepromPowerUp(uint32 Bank)
    */
    UtAssert_INT32_EQ(CFE_PSP_EepromPowerUp(1), CFE_PSP_SUCCESS);
}

void Test_CFE_PSP_EepromPowerDown(void)
{
    /*
    int32 CFE_PSP_EepromPowerDown(uint32 Bank)
    */
    UtAssert_INT32_EQ(CFE_PSP_EepromPowerDown(1), CFE_PSP_SUCCESS);
}

/*
 * Macro to add a test case to the list of tests to execute
 */
#define ADD_TEST(test) UtTest_Add(test, NULL, NULL, #test)

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(Test_eeprom_direct_Init);
    ADD_TEST(Test_CFE_PSP_EepromWrite32_Nominal);
    ADD_TEST(Test_CFE_PSP_EepromWrite32_AddressMisaligned);
    ADD_TEST(Test_CFE_PSP_EepromWrite16_Nominal);
    ADD_TEST(Test_CFE_PSP_EepromWrite8);
    ADD_TEST(Test_CFE_PSP_EepromWriteEnable);
    ADD_TEST(Test_CFE_PSP_EepromWriteDisable);
    ADD_TEST(Test_CFE_PSP_EepromPowerUp);
    ADD_TEST(Test_CFE_PSP_EepromPowerDown);
}
