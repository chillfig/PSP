/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file psp_memory_testcases.c
*
*   \brief This file contains test cases for the cfe_psp_memory.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This software was customized at NASA's Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Includes
**=======================================================================================*/
#include <unistd.h>
#include <string.h>
#include <target_config.h>
#include "uttest.h"
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp.h"

#include "psp_memory_testcases.h"
#include "../src/cfe_psp_memory.c"

/*=======================================================================================
** Preprocessor Directives
**=======================================================================================*/
#define UT_MEMORY_PRINT_SCOPE       "_CFE_PSP_"
#define UT_MEMORY_SYNC_PRINT_SCOPE  "_CFE_PSP_MemSync"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

extern char UserReservedMemory[URM_SIZE];
extern char *pEndOfURM;

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_GetCDSSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCDSSize(void)
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
** Ut_CFE_PSP_WriteToCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteToCDS(void)
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
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToCDS() - 1/4: Nominal - new data same as original one");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToCDS() - 2/4: Nominal - Need to update memory");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_WriteToCDS() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteToCDS() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteToCDS() - 4/4: Failed data offset is larger than CDSMemory block size");

}

/*=======================================================================================
** Ut_CFE_PSP_ReadFromCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ReadFromCDS(void)
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
                    "_CFE_PSP_ReadFromCDS() - 1/3: Nominal - Read data is matched with CDSMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadFromCDS() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadFromCDS() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadFromCDS() - 3/3: Failed data offset is larger than CDSMemory block size");
}

/*=======================================================================================
** Ut_CFE_PSP_GetResetArea() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetResetArea(void)
{
    uint32  uiRetCode = 0;
    uint32  uiBlockAddress = 0;
    uint32  uiBlockSize = 0;
    uint32  *puiBlockAddress = NULL;
    uint32  *puiBlockSize = NULL;

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
    UtAssert_IntegerCmpAbs(uiBlockAddress, (uint32)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetResetArea - 1/3: Nominal ResetMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiBlockSize, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, 0, 
                           "_CFE_PSP_GetResetArea - 1/3: Nominal ResetMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetResetArea - 1/3: Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = NULL;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetResetArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetResetArea - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_GetUserReservedArea() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetUserReservedArea(void)
{
    uint32  uiRetCode = 0;
    uint32  uiUserArea = 0;
    uint32  uiUserAreaSize = 0;
    uint32  *puiUserArea = NULL;
    uint32  *puiUserAreaSize = NULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiUserArea = &uiUserArea;
    puiUserAreaSize = &uiUserAreaSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiUserArea, (uint32)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetUserReservedArea - 1/3: Nominal UserReservedMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiUserAreaSize, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize, 0, 
                           "_CFE_PSP_GetUserReservedArea - 1/3: Nominal UserReservedMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetUserReservedArea - 1/3: Nominal");

    /* ----- Test case #2 - UserArea address pointer is NULL ----- */
    /* Setup additional inputs */
    puiUserArea = NULL;
    puiUserAreaSize = &uiUserAreaSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetUserReservedArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - UserArea size pointer is NULL ----- */
    /* Setup additional inputs */
    puiUserArea = &uiUserArea;
    puiUserAreaSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetUserReservedArea - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_GetVolatileDiskMem() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetVolatileDiskMem(void)
{
    uint32  uiRetCode = 0;
    uint32  uiVolDisk = 0;
    uint32  uiVolDiskSize = 0;
    uint32  *puiVolDisk = NULL;
    uint32  *puiVolDiskSize = NULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiVolDisk = &uiVolDisk;
    puiVolDiskSize = &uiVolDiskSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiVolDisk, (uint32)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetVolatileDiskMem - 1/3: Nominal VolatileDiskMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiVolDiskSize, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize, 0, 
                           "_CFE_PSP_GetVolatileDiskMem - 1/3: Nominal VolatileDiskMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetVolatileDiskMem - 1/3: Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiVolDisk = NULL;
    puiVolDiskSize = &uiVolDiskSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetVolatileDiskMem - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiVolDisk = &uiVolDisk;
    puiVolDiskSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetVolatileDiskMem - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_InitProcessorReservedMemory() test cases
**=======================================================================================*/
void Ut_CFE_PSP_InitProcessorReservedMemory(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    char cMsg[256] = {};

    CFE_PSP_SetupReservedMemoryMap();

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal POWERON ----- */
    /* Set additional inputs */
    sprintf(cMsg, MEMORY_PRINT_SCOPE "Clearing Processor Reserved Memory.\n");
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
    sprintf(cMsg, MEMORY_PRINT_SCOPE "VxWorks Reserved Memory Block Size not large enough, "
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
** Ut_CFE_PSP_SetupReservedMemoryMap() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetupReservedMemoryMap(void)
{
    char cMsg[256] = {};

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

    /* ----- Test case #3 - Nominal - MemRangeSet error ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(userReservedGet), URM_SIZE);
    /* Reset URM Allocations */
    sprintf(cMsg, MEMORY_PRINT_SCOPE "SetupReservedMemory: MemRangeSet failed\n");
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_ERROR);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetupReservedMemoryMap 2/2: Nominal - MemRangeSet failure");

}

/*=======================================================================================
** Ut_CFE_PSP_DeleteProcessorReservedMemory() test cases
**=======================================================================================*/
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void)
{
    /* Nothing to test */
    CFE_PSP_DeleteProcessorReservedMemory();

    UtAssert_NA("_CFE_PSP_DeleteProcessorReservedMemory 1/1: N/A");
}

/*=======================================================================================
** Ut_CFE_PSP_GetKernelTextSegmentInfo() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetKernelTextSegmentInfo(void)
{
    uint32  uiRetCode = 0;
    uint32  uiKernelSegment = 0;
    uint32  uiKernelSegmentSize = 0;
    uint32  *puiKernelSegment = NULL;
    uint32  *puiKernelSegmentSize = NULL;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiKernelSegment = &uiKernelSegment;
    puiKernelSegmentSize = &uiKernelSegmentSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetKernelTextSegmentInfo(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetKernelTextSegmentInfo - 1/2: Nominal");

    UT_ResetState(0);

    /* ----- Test case #2 - Kernel segment size pointer is NULL ----- */
    /* Setup additional inputs */
    puiKernelSegment = &uiKernelSegment;
    puiKernelSegmentSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetKernelTextSegmentInfo(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetKernelTextSegmentInfo - 2/2: Failed Kernel segment size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_GetCFETextSegmentInfo() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCFETextSegmentInfo(void)
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

    moduleInfo.segInfo.textAddr = task_name;
    moduleInfo.segInfo.textSize = strlen(task_name);

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Module found by Name and Get ----- */
    /* Setup additional inputs */
    CFESegment = 0;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(moduleFindByName), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(moduleInfoGet), OK);
    UT_SetDataBuffer(UT_KEY(moduleInfoGet), &moduleInfo, sizeof(&moduleInfo), false);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 1/6: Nominal moduleInfoGet returned success");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 1);
    UtAssert_STUB_COUNT(moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #2 - Module found by Name but could not Get ----- */
    /* Setup additional inputs */
    CFESegment = 0;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(moduleFindByName), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(moduleInfoGet), OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 2/6: CFESegment was not changed");
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 2/6: Nominal moduleInfoGet returned error");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 1);
    UtAssert_STUB_COUNT(moduleInfoGet, 1);


    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #3 - puiSizeOfCFESegment pointer is NULL ----- */
    /* Setup additional inputs */
    puiSizeOfCFESegment = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 3/6: puiSizeOfCFESegment pointer is NULL");
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 3/6: CFESegment was not changed");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 0);
    UtAssert_STUB_COUNT(moduleFindByName, 0);
    UtAssert_STUB_COUNT(moduleInfoGet, 0);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #4 - pCFESegment pointer is NULL ----- */
    /* Setup additional inputs */
    pCFESegment = NULL;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 4/6: pCFESegment pointer is NULL");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 0);
    UtAssert_STUB_COUNT(moduleFindByName, 0);
    UtAssert_STUB_COUNT(moduleInfoGet, 0);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #5 - moduleFindByName return NULL pointer ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(moduleFindByName), OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 5/6: Failed because moduleFindByName returned NULL");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 1);
    UtAssert_STUB_COUNT(moduleInfoGet, 0);

    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* ----- Test case #6 - Module found by OS_SymbolLookup and Get ----- */
    /* Setup additional inputs */
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &GetModuleIdAddr, sizeof(GetModuleIdAddr), false);
    UT_SetDefaultReturnValue(UT_KEY(PCS_OS_BSPMain), 10);
    UT_SetDefaultReturnValue(UT_KEY(moduleInfoGet), OK);
    UT_SetDataBuffer(UT_KEY(moduleInfoGet), &moduleInfo, sizeof(&moduleInfo), false);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 6/6: Nominal moduleInfoGet returned success");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 0);
    UtAssert_STUB_COUNT(PCS_OS_BSPMain, 1);
    UtAssert_STUB_COUNT(moduleInfoGet, 1);
}


/*=======================================================================================
** End of file psp_memory_testcases.c
**=======================================================================================*/
