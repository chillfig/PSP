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
#include "cfe_psp_memory.h"

#include <userReservedMem.h>
#include <moduleLib.h>

#include "PCS_stdlib.h"
#include "PCS_rebootLib.h"
#include "PCS_cacheLib.h"
#include "PCS_cfe_configdata.h"
#include "PCS_string.h"

/*=======================================================================================
** Preprocessor Directives
**=======================================================================================*/
#define UT_MEMORY_PRINT_SCOPE       "PSP MEMORY: "
#define UT_MEMORY_SYNC_PRINT_SCOPE  "MEMORY SYNC: "


/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/
extern CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap;
extern char UserReservedMemory[URM_SIZE];
extern char *pEndOfURM;
extern CFE_PSP_MemoryBlock_t GR740_ReservedMemBlock;
extern int PCS_OS_BSPMain(void);
extern int32 CFE_PSP_GetMemSize(uint32 *p_size, MEMORY_SECTION_t op);
extern int32 CFE_PSP_GetMemArea(cpuaddr *p_area, uint32 *p_size, MEMORY_SECTION_t op);
extern int32 CFE_PSP_ReadFromRAM(const void *p_data, uint32 offset, uint32 size, MEMORY_SECTION_t op);
extern int32 CFE_PSP_WriteToRAM(const void *p_data, uint32 offset, uint32 size, MEMORY_SECTION_t op);

/*=======================================================================================
** Test_CFE_PSP_GetCDSSize(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GetCDSSize(void)
{
    int32   iRetCode = CFE_PSP_ERROR;
    uint32  *puiSizeOfCDS = NULL;
    uint32  uiSize = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiSizeOfCDS = &uiSize;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 100;
    /* Execute test */
    iRetCode = CFE_PSP_GetCDSSize(puiSizeOfCDS);
    /* Verify outputs */
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 
                  "_CFE_PSP_GetCDSSize() - 1/2: Get CDS file correctly");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCDSSize() - 1/2: Nominal");

    /* ----- Test case #2 - Failed Argument is NULL ----- */
    /* Setup additional inputs */
    puiSizeOfCDS = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_GetCDSSize(puiSizeOfCDS);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCDSSize() - 2/2: Argument is NULL");
}

/*=======================================================================================
** Test_CFE_PSP_GetCDSArea(void) test cases
**=======================================================================================*/
void Test_CFE_PSP_GetCDSArea(void)
{
    uint32  uiRetCode = 0;
    cpuaddr uiBlockAddress = 0;
    uint32  uiBlockSize = 0;
    cpuaddr *puiBlockAddress = NULL;
    uint32  *puiBlockSize = NULL;

    uint8   localCDSMemoryBuffer[2000];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSMemoryBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 2000;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCDSArea((cpuaddr*)puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiBlockAddress, (cpuaddr)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetCDSArea - 1/3: Nominal CDSMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiBlockSize, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0, 
                           "_CFE_PSP_GetCDSArea - 1/3: Nominal CDSMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCDSArea - 1/3: Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = NULL;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCDSArea((cpuaddr*)puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetCDSArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCDSArea((cpuaddr*)puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetCDSArea - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Test_CFE_PSP_WriteToCDS() test cases
**=======================================================================================*/
void Test_CFE_PSP_WriteToCDS(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localCDSBuffer[200];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 200;
    memset(localCDSBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal - new data same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToCDS() - 1/5: Nominal - new data same as original one");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToCDS() - 2/5: Nominal - Need to update memory");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_WriteToCDS() - 2/5: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteToCDS() - 3/5: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteToCDS() - 4/5: Failed data offset is larger than CDSMemory block size");

    /* ----- Test case #5 - Bytes to write larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteToCDS() - 5/5: Writing more than CDSMemory block size");

}

/*=======================================================================================
** Test_CFE_PSP_ReadFromCDS() test cases
**=======================================================================================*/
void Test_CFE_PSP_ReadFromCDS(void)
{
    int32 iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;
    uint8   ucIdx = 0;

    uint8   localCDSBuffer[100];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 100;

    /* Modify first 100 bytes from CDSMemory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr)[ucIdx] = ucIdx;
    }

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 1/4: Nominal - Read data is matched with CDSMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromCDS() - 1/4: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadFromCDS() - 2/4: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadFromCDS() - 3/4: Failed data offset is larger than CDSMemory block size");

    /* ----- Test case #4 - Reading more than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadFromCDS() - 4/4: Reading more than CDSMemory block size");
}

/*=======================================================================================
** CFE_PSP_GetVOLATILEDISKSize() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetVOLATILEDISKSize(void)
{
    int32 iReturnCode;
    uint32 uiSize = 99;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = 70;

    /* ----- Test case #1 - NULL input ------ */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_GetVOLATILEDISKSize(NULL);
    /* Verify Results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetVOLATILEDISKSize 1/2: NULL argument\n");

    /* ----- Test case #2 - Get Size ------ */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_GetVOLATILEDISKSize(&uiSize);
    /* Verify Results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetVOLATILEDISKSize 2/2: Get size - return code\n");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize, "_CFE_PSP_Get.Size 2/2: Verify block size\n");
}

/*=======================================================================================
** Test_CFE_PSP_GetVolatileDiskMem() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetVolatileDiskMem(void)
{
    uint32  uiRetCode = 0;
    cpuaddr  uiVolDisk = 0;
    uint32  uiVolDiskSize = 0;
    cpuaddr *puiVolDisk = NULL;
    uint32  *puiVolDiskSize = NULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiVolDisk = &uiVolDisk;
    puiVolDiskSize = &uiVolDiskSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiVolDisk, (cpuaddr)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetVolatileDiskMem - 1/3: Nominal VolatileDiskMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiVolDiskSize, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize, 0, 
                           "_CFE_PSP_GetVolatileDiskMem - 1/3: Nominal VolatileDiskMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetVolatileDiskMem - 1/3: Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiVolDiskSize = &uiVolDiskSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(NULL, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetVolatileDiskMem - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiVolDisk = &uiVolDisk;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, NULL);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetVolatileDiskMem - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Test_CFE_PSP_ReadFromVOLATILEDISK() test cases
**=======================================================================================*/
void Test_CFE_PSP_ReadFromVOLATILEDISK(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiVOLATILEDISKOffset = 0;
    uint32  uiNumBytes = 100;
    uint8   ucIdx = 0;

    uint8   localVOLATILEDISKBuffer[100];
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = (void*)localVOLATILEDISKBuffer;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = 100;

    /* Modify first 100 bytes from VolatileDiskMemory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr)[ucIdx] = ucIdx;
    }

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromVOLATILEDISK() - 1/3: Nominal - Read data is matched with VolatileDiskMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromVOLATILEDISK() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_ReadFromVOLATILEDISK() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than VolatileDiskMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr;
    uiVOLATILEDISKOffset = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_ReadFromVOLATILEDISK() - 3/3: Failed data offset is larger than VolatileDiskMemory block size");
}

/*=======================================================================================
** Test_CFE_PSP_WriteToVOLATILEDISK() test cases
**=======================================================================================*/
void Test_CFE_PSP_WriteToVOLATILEDISK(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiVOLATILEDISKOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localVOLATILEDISKBuffer[200];
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = (void*)localVOLATILEDISKBuffer;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = 200;
    memset(localVOLATILEDISKBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal - new data same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToVOLATILEDISK() - 1/4: Nominal - new data same as original one");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToVOLATILEDISK() - 2/4: Nominal");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_WriteToVOLATILEDISK() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_WriteToVOLATILEDISK() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than VolatileDiskMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiVOLATILEDISKOffset = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_WriteToVOLATILEDISK() - 4/4: Failed data offset is larger than VolatileDiskMemory block size");
}

/*=======================================================================================
** Test_CFE_PSP_GetUSERRESERVEDSize() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetUSERRESERVEDSize(void)
{
    int32 iReturnCode;
    uint32 uiSize = 99;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = 70;

    /* ----- Test case #1 - NULL input ------ */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_GetUSERRESERVEDSize(NULL);
    /* Verify Results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetUSERRESERVEDSize 1/2: NULL argument\n");

    /* ----- Test case #2 - Get Size ------ */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_GetUSERRESERVEDSize(&uiSize);
    /* Verify Results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetUSERRESERVEDSize 2/2: Get size - return code\n");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize, "_CFE_PSP_Get.Size 2/2: Verify block size\n");
}

/*=======================================================================================
** Test_CFE_PSP_ReadFromUSERRESERVED() test cases
**=======================================================================================*/
void Test_CFE_PSP_ReadFromUSERRESERVED(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiUSERRESERVEDOffset = 0;
    uint32  uiNumBytes = 100;
    uint8   ucIdx = 0;

    uint8   localUSERRESERVEDBuffer[100];
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = (void*)localUSERRESERVEDBuffer;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = 100;

    /* Modify first 100 bytes from UserReservedMemory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr)[ucIdx] = ucIdx;
    }

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromUSERRESERVED() - 1/3: Nominal - Read data is matched with UserReservedMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromUSERRESERVED() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_ReadFromUSERRESERVED() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than UserReservedMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr;
    uiUSERRESERVEDOffset = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_ReadFromUSERRESERVED() - 3/3: Failed data offset is larger than UserReservedMemory block size");
}

/*=======================================================================================
** Test_CFE_PSP_WriteToUSERRESERVED() test cases
**=======================================================================================*/
void Test_CFE_PSP_WriteToUSERRESERVED(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiUSERRESERVEDOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localUSERRESERVEDBuffer[200];
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = (void*)localUSERRESERVEDBuffer;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = 200;
    memset(localUSERRESERVEDBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal - new data same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToUSERRESERVED() - 1/4: Nominal - new data same as original one");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToUSERRESERVED() - 2/4: Nominal");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_WriteToUSERRESERVED() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_WriteToUSERRESERVED() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than UserReservedMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiUSERRESERVEDOffset = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_WriteToUSERRESERVED() - 4/4: Failed data offset is larger than UserReservedMemory block size");
}

/*=======================================================================================
** Test_CFE_PSP_GetMemSize() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetMemSize(void)
{
    char cMsg[256] = {};
    int32 iReturnCode;
    uint32 uiSize = 0;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 99;

    /* ----- Test case #1 - NULL Input ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "GetMemSize: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_GetMemSize(NULL, 100);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetMemSize - 1/3: Invalid pointer - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetMemSize - 1/3: Invalid pointer - message");

    /* ----- Test case #2 - Invalid memory selection ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "GetMemSize: Invalid Memory Option\n");
    /* Execute test */
    iReturnCode = CFE_PSP_GetMemSize(&uiSize, 100);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_GetMemSize - 2/3: Invalid memory option - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetMemSize - 2/3: Invalid memory option - message");

    /* ----- Test case #3 - Get Mem Size ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_GetMemSize(&uiSize, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetMemSize - 3/3: Got memory size - return code");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_GetMemSize - 3/3: Got memory size - value check");
}

/*=======================================================================================
** Test_CFE_PSP_GetMemArea() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetMemArea(void)
{
    char cMsg[256] = {};
    int32 iReturnCode;
    uint32 uiSize = 0;
    cpuaddr uiLocalBlockPtr = 0x00112233;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 99;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void *)0x12345678;

    /* ----- Test case #1 - NULL Input ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "GetMemArea: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_GetMemArea(NULL, NULL, 99);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetMemArea - 1/3: Invalid pointer - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetMemArea - 1/3: Invalid pointer - message");

    /* ----- Test case #2 - Invalid memory selection ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "GetMemArea: Invalid Memory Option\n");
    /* Execute test */
    iReturnCode = CFE_PSP_GetMemArea((cpuaddr *)&uiLocalBlockPtr, &uiSize, 99);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, "_CFE_PSP_GetMemArea - 2/3: Invalid memory option - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_GetMemArea - 2/3: Invalid memory option - message");

    /* ----- Test case #3 - Get Mem Size ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_GetMemArea((cpuaddr *)&uiLocalBlockPtr, &uiSize, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetMemArea - 3/3: Got memory info - return code");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_GetMemArea - 3/3: Got memory area size - value check");  
    UtAssert_True(uiLocalBlockPtr == (cpuaddr)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, "_CFE_PSP_GetMemArea - 3/3: Got memory area ptr - value check"); 
}

/*=======================================================================================
** Test_CFE_PSP_ReadFromRAM() test cases
**=======================================================================================*/
void Test_CFE_PSP_ReadFromRAM(void)
{
    int32 iReturnCode = 0;
    char cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;
    uint8   ucIdx = 0;

    uint8   localCDSBuffer[100];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 100;

    /* Modify first 100 bytes from CDSMemory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr)[ucIdx] = ucIdx;
    }

    memset(uiBuffer, 1, uiNumBytes);
    memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 1, uiNumBytes);

    /* ----- Test case #1 - NULL ptr ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "ReadFromRAM: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromRAM(NULL, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_ReadFromRAM - 1/6: Memory not allocated - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadFromRAM - 1/6: Memory not allocated - message");

    /* ----- Test case #2 - Default switch/case ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "ReadFromRAM: Invalid Memory Option\n");
    pucData = uiBuffer;
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, 999);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, "_CFE_PSP_ReadFromRAM - 2/6: Invalid memory section option - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadFromRAM - 2/6: Invalid memory section option - message");    

    /* ----- Test case #3 - Memory block ptr NULL ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = NULL;
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_ReadFromRAM - 3/6: Memory block ptr is NULL - return code");

    /* ----- Test case #4 - Illegal offset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromRAM(pucData, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 999, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_ReadFromRAM - 4/6: Invalid offset value - return code");;

    /* ----- Test case #5 - Data is match/clean ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromRAM - 5/6: Data match, no writing required - return code");

    /* ----- Test case #6 - Data mismatch ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    memset(uiBuffer, '0', uiNumBytes);
    memset(localCDSBuffer, '2', 100);
    /* Execute test */
    iReturnCode = CFE_PSP_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromRAM - 6/6: Data mismatch, data write - return code");
    UtAssert_MemCmp(localCDSBuffer, uiBuffer, uiNumBytes, "_CFE_PSP_ReadFromRAM - 6/6: Data mismatch, data write - memory compare");
}

/*=======================================================================================
** Test_CFE_PSP_WriteToRAM() test cases
**=======================================================================================*/
void Test_CFE_PSP_WriteToRAM(void)
{
    int32   iReturnCode = 0;
    char    cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localCDSBuffer[200];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 200;
    memset(localCDSBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - NULL ptr ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "WriteToRAM: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToRAM(NULL, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_WriteToRAM - 1/6: Memory not allocated - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteToRAM - 1/6: Memory not allocated - message");

    /* ----- Test case #2 - Default switch/case ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    pucData = uiBuffer;
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "WriteToRAM: Invalid Memory Selection\n");
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, 9999);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, "_CFE_PSP_WriteToRAM - 2/6: Invalid memory selection - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteToRAM - 2/6: Invalid memory selection - message");

    /* ----- Test case #3 - Memory block ptr NULL ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = NULL;
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_WriteToRAM - 3/6: Memory block ptr was null - return code");

    /* ----- Test case #4 - Illegal offset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "WriteToRAM: Invalid Mem Range\n");
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToRAM(pucData, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 99, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_WriteToRAM - 4/6: Invalid offset - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteToRAM - 4/6: message");

    /* ----- Test case #5 - Data is match/clean ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToRAM - 5/6: Data match, no writing required - return code");

    /* ----- Test case #6 - Data mismatch ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    memset(localCDSBuffer, '2', 200);
    memset(localCDSBuffer, '0', 100);
    memset(uiBuffer, '0', uiNumBytes);
    memset(localCDSBuffer, '2', 100);
    /* Execute test */
    iReturnCode = CFE_PSP_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToRAM - 6/6: Data mismatch, data write - return code");
    UtAssert_MemCmp(localCDSBuffer, uiBuffer, uiNumBytes, "_CFE_PSP_WriteToRAM - 6/6: Data mismatch, data write - memory compare");
}

/*=======================================================================================
** Test_CFE_PSP_GetRESETSize() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetRESETSize(void)
{
    int32 iReturnCode;
    uint32 uiSize = 99;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 70;

    /* ----- Test case #1 - NULL input ------ */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_GetRESETSize(NULL);
    /* Verify Results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetRESETSize 1/2: NULL argument\n");

    /* ----- Test case #2 - Get Size ------ */
    /* Set additional inputs */
    /* Execute test */
    iReturnCode = CFE_PSP_GetRESETSize(&uiSize);
    /* Verify Results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetRESETSize 2/2: Get size - return code\n");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, "_CFE_PSP_Get.Size 2/2: Verify block size\n");
}

/*=======================================================================================
** Test_CFE_PSP_GetResetArea() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetResetArea(void)
{
    uint32  uiRetCode = 0;
    cpuaddr  uiBlockAddress = 0;
    uint32  uiBlockSize = 0;
    cpuaddr *puiBlockAddress = NULL;
    uint32 *puiBlockSize = NULL;

    uint8   localResetMemoryBuffer[2000];
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)localResetMemoryBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 2000;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiBlockAddress, (cpuaddr)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetResetArea - 1/3: Nominal ResetMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiBlockSize, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, 0, 
                           "_CFE_PSP_GetResetArea - 1/3: Nominal ResetMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetResetArea - 1/3: Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(NULL, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetResetArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, NULL);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetResetArea - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Test_CFE_PSP_ReadFromRESET() test cases
**=======================================================================================*/
void Test_CFE_PSP_ReadFromRESET(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiRESETOffset = 0;
    uint32  uiNumBytes = 100;
    uint8   ucIdx = 0;

    uint8   localRESETBuffer[100];
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)localRESETBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 100;

    /* Modify first 100 bytes from ResetMemory for testing */
    for (ucIdx = 0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr)[ucIdx] = ucIdx;
    }

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromRESET() - 1/3: Nominal - Read data is matched with ResetMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromRESET() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_ReadFromRESET() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than ResetMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
    uiRESETOffset = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_ReadFromRESET() - 3/3: Failed data offset is larger than ResetMemory block size");
}

/*=======================================================================================
** Test_CFE_PSP_WriteToRESET() test cases
**=======================================================================================*/
void Test_CFE_PSP_WriteToRESET(void)
{
    int32   iRetCode = 0;
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiRESETOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localRESETBuffer[200];
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)localRESETBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 200;
    memset(localRESETBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal - new data same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToRESET() - 1/4: Nominal - new data same as original one");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToRESET() - 2/4: Nominal");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_WriteToRESET() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_WriteToRESET() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than ResetMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiRESETOffset = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_WriteToRESET() - 4/4: Failed data offset is larger than ResetMemory block size");
}

/*=======================================================================================
** Test_CFE_PSP_GetUserReservedArea() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetUserReservedArea(void)
{
    uint32  uiRetCode = 0;
    cpuaddr  uiUserArea = 0;
    uint32  uiUserAreaSize = 0;
    cpuaddr *puiUserArea = NULL;
    uint32  *puiUserAreaSize = NULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiUserArea = &uiUserArea;
    puiUserAreaSize = &uiUserAreaSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiUserArea, (cpuaddr)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetUserReservedArea - 1/3: Nominal UserReservedMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiUserAreaSize, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize, 0, 
                           "_CFE_PSP_GetUserReservedArea - 1/3: Nominal UserReservedMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetUserReservedArea - 1/3: Nominal");

    /* ----- Test case #2 - UserArea address pointer is NULL ----- */
    /* Setup additional inputs */
    puiUserAreaSize = &uiUserAreaSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(NULL, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetUserReservedArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - UserArea size pointer is NULL ----- */
    /* Setup additional inputs */
    puiUserArea = &uiUserArea;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, NULL);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetUserReservedArea - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Test_CFE_PSP_InitProcessorReservedMemory() test cases
**=======================================================================================*/
void Test_CFE_PSP_InitProcessorReservedMemory(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    char cMsg[256] = {};

    CFE_PSP_SetupReservedMemoryMap();

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal POWERON ----- */
    /* Set additional inputs */
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "Clearing Processor Reserved Memory.\n");
    GR740_ReservedMemBlock.BlockSize = URM_SIZE-10;
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_POWERON);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory 1/3: Nominal POWERON - clearing message");
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory 1/3: Nominal POWERON - success return code");

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #2 - Nominal PROCESSOR ----- */
    /* Set additional inputs */
    GR740_ReservedMemBlock.BlockSize = URM_SIZE-10;
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_PROCESSOR);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory 2/3: Nominal PROCESSOR - success return code");

    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* ----- Test case #3 - Error not enough User Reserved Memory ----- */
    /* Set additional inputs */
    GR740_ReservedMemBlock.BlockSize = URM_SIZE+1;
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    snprintf(cMsg, sizeof(cMsg), UT_MEMORY_PRINT_SCOPE "VxWorks Reserved Memory Block Size not large enough, "
                  "Total Size = 0x%lx, "
                  "VxWorks Reserved Size=0x%lx\n",
                  (unsigned long)GR740_ReservedMemBlock.BlockSize,
                  (unsigned long)URM_SIZE);
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_PROCESSOR);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory 3/3: Not enough memory - error message");
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_InitProcessorReservedMemory 3/3: Not enough memory - error return code");
}

/*=======================================================================================
** Test_CFE_PSP_SetupReservedMemoryMap() test cases
**=======================================================================================*/
void Test_CFE_PSP_SetupReservedMemoryMap(void)
{
    /* ----- Test case #1 - Nominal - No errors ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify results */
    UtAssert_True(GR740_ReservedMemBlock.BlockSize > 0, "_CFE_PSP_SetupReservedMemoryMap 1/2: Nominal - No errors");

    UT_ResetState(0);
    Ut_OS_printf_Setup();
}

/*=======================================================================================
** Test_CFE_PSP_DeleteProcessorReservedMemory() test cases
**=======================================================================================*/
void Test_CFE_PSP_DeleteProcessorReservedMemory(void)
{
    /* Nothing to test */
    CFE_PSP_DeleteProcessorReservedMemory();

    UtAssert_NA("_CFE_PSP_DeleteProcessorReservedMemory 1/1: N/A");
}

/*=======================================================================================
** Test_CFE_PSP_GetKernelTextSegmentInfo() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetKernelTextSegmentInfo(void)
{
    uint32  uiRetCode = 0;
    uint32  uiKernelSegment = 0;
    uint32  uiKernelSegmentSize = 0;
    cpuaddr puiKernelSegment = (cpuaddr)NULL;
    uint32  *puiKernelSegmentSize = NULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiKernelSegment = (cpuaddr)&uiKernelSegment;
    puiKernelSegmentSize = &uiKernelSegmentSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetKernelTextSegmentInfo(&puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetKernelTextSegmentInfo - 1/3: Nominal");

    UT_ResetState(0);

    /* ----- Test case #2 - Kernel segment size pointer is NULL ----- */
    /* Setup additional inputs */
    puiKernelSegment = (cpuaddr)&uiKernelSegment;
    puiKernelSegmentSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetKernelTextSegmentInfo(&puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetKernelTextSegmentInfo - 2/3: Failed Kernel segment size pointer is NULL");

    /* ----- Test case #3 - Kernel segment pointer is NULL ----- */
    /* Setup additional inputs */
    puiKernelSegmentSize = &uiKernelSegmentSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetKernelTextSegmentInfo(NULL, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetKernelTextSegmentInfo - 3/3: Failed Kernel segment pointer is NULL");
}

/*=======================================================================================
** Test_CFE_PSP_GetCFETextSegmentInfo() test cases
**=======================================================================================*/
void Test_CFE_PSP_GetCFETextSegmentInfo(void)
{
    uint32  uiRetCode = 0;
    cpuaddr CFESegment = 0;
    cpuaddr *pCFESegment = &CFESegment;
    uint32  uiSizeOfCFESegment = 0;
    uint32  *puiSizeOfCFESegment = NULL;
    char task_name[] = "test";
    MODULE_INFO moduleInfo;
    /* Let OS_SymbolLookup return a pointer to a function that we can control */
    cpuaddr GetModuleIdAddr = (cpuaddr)&PCS_OS_BSPMain;

    moduleInfo.segInfo.textAddr = (cpuaddr)task_name;
    moduleInfo.segInfo.textSize = strlen(task_name);

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Module found by Name and Get ----- */
    /* Setup additional inputs */
    CFESegment = 0;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleFindByName), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleInfoGet), OK);
    UT_SetDataBuffer(UT_KEY(PCS_moduleInfoGet), &moduleInfo, sizeof(&moduleInfo), false);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 1/8: Nominal moduleInfoGet returned success");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 1);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #2 - Module found by Name but could not Get ----- */
    /* Setup additional inputs */
    CFESegment = 0;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleFindByName), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleInfoGet), OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 2/8: CFESegment was not changed");
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 2/8: Nominal moduleInfoGet returned error");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 1);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #3 - puiSizeOfCFESegment pointer is NULL ----- */
    /* Setup additional inputs */
    puiSizeOfCFESegment = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 3/8: puiSizeOfCFESegment pointer is NULL");
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 3/8: CFESegment was not changed");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 0);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 0);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 0);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #4 - pCFESegment pointer is NULL ----- */
    /* Setup additional inputs */
    pCFESegment = NULL;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 4/8: pCFESegment pointer is NULL");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 0);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 0);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 0);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #5 - moduleFindByName return NULL pointer ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleFindByName), OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 5/8: Failed because moduleFindByName returned NULL");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 1);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 0);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #6 - Module found by OS_SymbolLookup and Get ----- */
    /* Setup additional inputs */
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &GetModuleIdAddr, sizeof(GetModuleIdAddr), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), 10);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleInfoGet), OK);
    UT_SetDataBuffer(UT_KEY(PCS_moduleInfoGet), &moduleInfo, sizeof(&moduleInfo), false);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 6/8: Nominal moduleInfoGet returned success");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 0);
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #7 - OS_SymbolLookup SUCCESS, but NULL address ----- */
    /* Setup additional inputs */
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    GetModuleIdAddr = (cpuaddr)NULL;
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &GetModuleIdAddr, sizeof(GetModuleIdAddr), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleFindByName), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleInfoGet), OK);
    UT_SetDataBuffer(UT_KEY(PCS_moduleInfoGet), &moduleInfo, sizeof(&moduleInfo), false);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 7/8: Nominal moduleInfoGet returned success");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 1);
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 0);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #8 - Module found by OS_SymbolLookup and Get Failed ----- */
    /* Setup additional inputs */
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    GetModuleIdAddr = (cpuaddr)&PCS_OS_BSPMain;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &GetModuleIdAddr, sizeof(GetModuleIdAddr), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), 10);
    UT_SetDefaultReturnValue(UT_KEY(PCS_moduleInfoGet), ERROR);
    UT_SetDataBuffer(UT_KEY(PCS_moduleInfoGet), &moduleInfo, sizeof(&moduleInfo), false);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 8/8: Nominal moduleInfoGet returned error");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(PCS_moduleFindByName, 0);
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_STUB_COUNT(PCS_moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);
}