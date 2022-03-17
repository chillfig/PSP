/*=======================================================================================
**
** File:  psp_memory_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP cfe_psp_memory.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-18 | Minh Luong       | Initial revision
**=======================================================================================*/

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
#define UT_MEMORY_PRINT_SCOPE       "_CFE_PSP_MEMORY_"
#define UT_MEMORY_SYNC_PRINT_SCOPE  "_CFE_PSP_MEMORY_SYNC_"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function definitions
**=======================================================================================*/

/*=======================================================================================
** Ut_CFE_PSP_GetCDSSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCDSSize(void)
{
    int32   iRetCode = 0;
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
                  "_ProcessResetType() - 1/2: Get CDS file correctly");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCDSSize() - 1/2: Nominal");

    /* ----- Test case #2 - Failed Argument is NULL ----- */
    /* Setup additional inputs */
    puiSizeOfCDS = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_GetCDSSize(puiSizeOfCDS);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "CFE_PSP_GetCDSSize() - 2/2: Argument is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_CalculateCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_CalculateCRC(void)
{
    uint32 uiRetCode = 0;
    uint8  pData[100] = {};
    uint8  ucIdx = 0; 
    uint32  uiCrcCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    for (ucIdx = 0; ucIdx < 100; ucIdx++)
    {
        pData[ucIdx] = ucIdx;
    }
    /* Set pre-calculated CRC code (0xFFFFDFEA) for above array pData */
    uiCrcCode = 0xFFFFDFEA;
    /* Execute test */
    uiRetCode = CFE_PSP_CalculateCRC(pData, sizeof(pData), 0);
    /* Verify outputs */
    UtAssert_True(uiRetCode = uiCrcCode, "_CFE_PSP_CalculateCRC - 1/1: Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_WriteToCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteToCDS(void)
{
    int32   iRetCode = 0;
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

    /* ----- Test case #1 - Nominal unchange while new data as the same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToCDS() - 1/4: Nominal No changed since the data buffer unchanged");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteToCDS() - 2/4: Nominal");
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
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetUserReservedArea - 1/3: Nominal");

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
    int32 iReturnCode;
    char cMsg[256] = {};

    uint32 uiRestartType = CFE_PSP_RST_TYPE_POWERON;
    uint32 uiMemBlockSize = 100;
    
    uint8 uiToCompareMemBlock_ZERO[uiMemBlockSize];
    memset(uiToCompareMemBlock_ZERO, 0, uiMemBlockSize);

    g_uiTotalReservedAllocSize = uiMemBlockSize;

    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    uint32 uiZEROBuf[10];

    uint32 uiLocalCDSBuf[10];
    uint32 uiLocalCDSBufSize = 10;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = uiLocalCDSBuf;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = uiLocalCDSBufSize;
    
    uint32 uiLocalRESETBuf[10];
    uint32 uiLocalRESETBufSize = 10;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = uiLocalRESETBuf;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = uiLocalRESETBufSize;

    uint32 uiLocalVOLATILEDISKBuf[10];
    uint32 uiLocalVOLATILEDISKBufSize = 10;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = uiLocalVOLATILEDISKBuf;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = uiLocalVOLATILEDISKBufSize;

    uint32 uiLocalUSERRESERVEDBuf[10];
    uint32 uiLocalUSERRESERVEDBufSize = 10;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = uiLocalUSERRESERVEDBuf;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = uiLocalUSERRESERVEDBufSize;

    memset(uiZEROBuf, 0, 10);
    memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    memset(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);


    cpuaddr start_addr = 0;
    uint32 reserve_memory_size;
    userReservedGet((char **)&start_addr, &reserve_memory_size);


    /* ----- Test case #1 - Reserved Mem Block too large ----- */
    /* Set additional inputs */
    g_uiTotalReservedAllocSize = (size_t)0xFFFFFFFF;
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_PROCESSOR);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_InitProcessorReservedMemory 1/4: Invalid Block Size");

    /* ----- Test case #2 - Restart type CFE_PSP_RST_TYPE_POWERON ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_uiTotalReservedAllocSize = (size_t)0x00000000;
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_MAX;
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_POWERON);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory 2/4: Not processor reset - return code");
    UtAssert_True(CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type == CFE_PSP_RST_TYPE_PROCESSOR, "_CFE_PSP_InitProcessorReservedMemory 2/4: Not processor reset - boot reset type check");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 2/4: CDS Block Check ");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 2/4: Reset Block Check");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 2/4: Volatile Disk Block Check");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 2/4: User Reserved Block Check");

    /* ----- Test case #3 - Restore Data DEFAULT SUCCESS ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    memset(uiZEROBuf, 0, 10);
    memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_MAX;
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_PROCESSOR);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory 3/4: Not processor reset - return code");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 3/4: Reset Block Check");

    /* ----- Test case #3 - Restore Data DEFAULT ERROR ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    memset(uiZEROBuf, 0, 10);
    memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    memset(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, 1, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_MAX;
    /* Execute test */
    iReturnCode = CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_PROCESSOR);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory 4/4: Not processor reset - return code");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 4/4: CDS Block Check ");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 4/4: Reset Block Check");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 4/4: Volatile Disk Block Check");
    UtAssert_MemCmp(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiZEROBuf, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize, "_CFE_PSP_InitProcessorReservedMemory 4/4: User Reserved Block Check");
}

/*=======================================================================================
** Ut_CFE_PSP_SetupReservedMemoryMap() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetupReservedMemoryMap(void)
{
    char cMsg[256] = {};

    uint32 totalAllocSize = 0;

    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize= GLOBAL_CONFIGDATA.CfeConfig->ResetAreaSize;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->CdsSize;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize =  GLOBAL_CONFIGDATA.CfeConfig->RamDiskSectorSize * GLOBAL_CONFIGDATA.CfeConfig->RamDiskTotalSectors;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->UserReservedSize;

    totalAllocSize += sizeof(CFE_PSP_ReservedMemoryBootRecord_t);
    totalAllocSize += sizeof(CFE_PSP_ExceptionStorage_t);
    totalAllocSize += CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
    totalAllocSize += CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
    totalAllocSize += CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
    totalAllocSize += CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;

    /* ----- Test case #1 - Nominal Setup ----- */
    /* Set additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(userMemAlloc), OK);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify results */
    UtAssert_True(totalAllocSize == g_uiTotalReservedAllocSize, "_CFE_PSP_SetupReservedMemoryMap 1/4: Successfully allocate memory -  Memory alloc size check");

    /* ----- Test case #2 - Fail to allocate space ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    g_uiTotalReservedAllocSize = 0;
    UT_SetDefaultReturnValue(UT_KEY(userMemAlloc), ERROR);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify results */
    UtAssert_True(g_uiTotalReservedAllocSize == 0, "_CFE_PSP_SetupReservedMemoryMap 2/4: Fail to allocate memory - Memory alloc size check");

    /* ----- Test case #3 - Mem Range Set failure ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "SetupReservedMemory: MemRangeSet failed\n");
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_ERROR);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetupReservedMemoryMap 3/4: MemRangeSet failure");

    /* ---- Test case #4 - Fail create binary semaphores ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(userMemAlloc), OK);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemCreate), OS_SEM_FAILURE);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify results */
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE \
                    "Init: Failed to create RESET binary semaphore, status: %d\n", OS_SEM_FAILURE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetupReservedMemoryMap 4/4: Create RESET BinSem Failure - message");
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE \
                    "Init: Failed to create VOLATILEDISK binary semaphore, status: %d\n", OS_SEM_FAILURE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetupReservedMemoryMap 4/4: Create VOLATILEDISK BinSem Failure - message");
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE \
                    "Init: Failed to create CDS binary semaphore, status: %d\n", OS_SEM_FAILURE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetupReservedMemoryMap 4/4: Create CDS BinSem Failure - message");
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE \
                    "Init: Failed to create USERRESERVED binary semaphore, status: %d\n", OS_SEM_FAILURE);
    UtAssert_OS_print(cMsg, "_CFE_PSP_SetupReservedMemoryMap 4/4: Create USERRESERVED BinSem Failure - message");
}

/*=======================================================================================
** Ut_CFE_PSP_DeleteProcessorReservedMemory() test cases
**=======================================================================================*/
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void)
{
    char cMsg[256] = {"CFE_PSP: Aitech does not support Non-Volatile User Reserved Memory\n"};
    Ut_OS_printf_Setup();

    /* Nothing to test */
    CFE_PSP_DeleteProcessorReservedMemory();

    UtAssert_OS_print(cMsg, "_CFE_PSP_DeleteProcessorReservedMemory 1/1: Nominal");
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
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetKernelTextSegmentInfo - 1/2:Nominal");

    UT_ResetState(0);

    /* ----- Test case #2 - Kernel segement size pointer is NULL ----- */
    /* Setup additional inputs */
    puiKernelSegment = &uiKernelSegment;
    puiKernelSegmentSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetKernelTextSegmentInfo(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetKernelTextSegmentInfo - 2/2: Failed Kernel segement size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_GetCFETextSegmentInfo() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCFETextSegmentInfo(void)
{
    uint32  uiRetCode = 0;
    cpuaddr CFESegment = 0;
    cpuaddr *pCFESegment = NULL;
    uint32  uiSizeOfCFESegment = 0;
    uint32  *puiSizeOfCFESegment = NULL;
    char task_name[] = "test";
    MODULE_ID  moduleID;
    MODULE_INFO moduleInfo;

    extern cpuaddr GetModuleId(void);

    moduleInfo.segInfo.textAddr = task_name;
    moduleInfo.segInfo.textSize = strlen(task_name);

    Ut_OS_printf_Setup();

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &GetModuleId, sizeof(&GetModuleId), false);
    UT_SetDefaultReturnValue(UT_KEY(moduleInfoGet), OS_SUCCESS);
    UT_SetDataBuffer(UT_KEY(moduleInfoGet), &moduleInfo, sizeof(moduleInfo), true);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(pCFESegment != 0, "_CFE_PSP_GetCFETextSegmentInfo - 1/5: CFESegment was changed");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 1/5: Nominal return code");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 0);
    UtAssert_STUB_COUNT(moduleInfoGet, 1);
    UtAssert_STUB_COUNT(GetModuleId, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #2 - Nominal moduleInfoGet return error ----- */
    /* Setup additional inputs */
    CFESegment = 0;
    pCFESegment = &CFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    /* UT_SetDataBuffer(UT_KEY(OS_SymbolLookup), &moduleID, sizeof(moduleID), false); */
    UT_SetDefaultReturnValue(UT_KEY(moduleInfoGet), ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 2/5: CFESegment was not changed");
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 2/5: Nominal moduleInfoGet returned error");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 0);
    UtAssert_STUB_COUNT(moduleInfoGet, 1);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #3 - puiSizeOfCFESegment pointer is NULL ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    puiSizeOfCFESegment = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 3/5: puiSizeOfCFESegment pointer is NULL");
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 3/5: CFESegment was not changed");
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
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 4/5: pCFESegment pointer is NULL");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 0);
    UtAssert_STUB_COUNT(moduleFindByName, 0);
    UtAssert_STUB_COUNT(moduleInfoGet, 0);

    Ut_OS_printf_Setup();
    UT_ResetState(0);

    /* ----- Test case #5 - moduleFindByName return NULL pointer ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(moduleFindByName), 0);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 5/5: Failed because moduleFindByName returned NULL");
    UtAssert_STUB_COUNT(OS_SymbolLookup, 1);
    UtAssert_STUB_COUNT(moduleFindByName, 1);
    UtAssert_STUB_COUNT(moduleInfoGet, 0);
}

/*=======================================================================================
** Ut_CFE_PSP_GetRESETSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetRESETSize(void)
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
** Ut_CFE_PSP_GetVOLATILEDISKSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetVOLATILEDISKSize(void)
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
** Ut_CFE_PSP_GetUSERRESERVEDSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetUSERRESERVEDSize(void)
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
** Ut_CFE_PSP_GetMemSize(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetMemSize(void)
{
    char cMsg[256] = {};
    int32 iReturnCode;
    uint32 uiSize = 0;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 99;

    /* ----- Test case #1 - NULL Input ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "GetMemSize: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_GetMemSize(NULL, 100);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_GetMemSize - 1/3: Invalid pointer - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_GetMemSize - 1/3: Invalid pointer - message");

    /* ----- Test case #2 - Invalid memory selection ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "GetMemSize: Invalid Memory Option\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_GetMemSize(&uiSize, 100);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEMORY_GetMemSize - 2/3: Invalid memory option - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_GetMemSize - 2/3: Invalid memory option - message");

    /* ----- Test case #3 - Get Mem Size ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_GetMemSize(&uiSize, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_GetMemSize - 3/3: Got memory size - return code");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_MEMORY_GetMemSize - 3/3: Got memory size - value check");   
}

/*=======================================================================================
** Ut_CFE_PSP_GetMemArea(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetMemArea(void)
{
    char cMsg[256] = {};
    int32 iReturnCode;
    uint32 uiSize = 0;
    uint32 uiLocalBlockPtr = 0x00112233;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 99;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void *)0x12345678;

    /* ----- Test case #1 - NULL Input ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "GetMemArea: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_GetMemArea(NULL, NULL, 99);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_GetMemArea - 1/3: Invalid pointer - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_GetMemArea - 1/3: Invalid pointer - message");

    /* ----- Test case #2 - Invalid memory selection ----- */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "GetMemArea: Invalid Memory Option\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_GetMemArea((cpuaddr *)&uiLocalBlockPtr, &uiSize, 99);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, "_CFE_PSP_MEMORY_GetMemArea - 2/3: Invalid memory option - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_GetMemArea - 2/3: Invalid memory option - message");

    /* ----- Test case #3 - Get Mem Size ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_GetMemArea((cpuaddr *)&uiLocalBlockPtr, &uiSize, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_GetMemArea - 3/3: Got memory info - return code");
    UtAssert_True(uiSize == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_MEMORY_GetMemArea - 3/3: Got memory area size - value check");  
    UtAssert_True((void *)uiLocalBlockPtr == CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, "_CFE_PSP_MEMORY_GetMemArea - 3/3: Got memory area ptr - value check"); 
}

/*=======================================================================================
** Ut_CFE_PSP_GetCDSArea(void) test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCDSArea(void)
{
    uint32  uiRetCode = 0;
    uint32  uiBlockAddress = 0;
    uint32  uiBlockSize = 0;
    uint32  *puiBlockAddress = NULL;
    uint32  *puiBlockSize = NULL;

    uint8   localCDSMemoryBuffer[2000];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSMemoryBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 2000;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCDSArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiBlockAddress, (uint32)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetCDSArea - 1/3: Nominal CDSMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiBlockSize, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0, 
                           "_CFE_PSP_GetCDSArea - 1/3: Nominal CDSMemory block size is matched");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCDSArea - 1/3: Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = NULL;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCDSArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetCDSArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCDSArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_GetCDSArea - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_WriteToRESET() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_WriteToRESET(void)
{
    int32   iRetCode = 0;
    char    cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiRESETOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localRESETBuffer[200];
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)localRESETBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 200;
    memset(localRESETBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal unchange while new data as the same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToRESET() - 1/4: Nominal No changed since the data buffer unchanged");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToRESET() - 2/4: Nominal");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_MEMORY_WriteToRESET() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_WriteToRESET() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than ResetMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiRESETOffset = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_WriteToRESET() - 4/4: Failed data offset is larger than ResetMemory block size");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_ReadFromRESET() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_ReadFromRESET(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiRESETOffset = 0;
    uint32  uiNumBytes = 100;
    uint8   ucIdx = 0;

    uint8   localRESETBuffer[100];
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)localRESETBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = 100;

    /* Modify first 100 bytes from ResetMemory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr)[ucIdx] = ucIdx;
    }

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_MEMORY_ReadFromRESET() - 1/3: Nominal - Read data is matched with ResetMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_ReadFromRESET() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_ReadFromRESET() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than ResetMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
    uiRESETOffset = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromRESET(pucData, uiRESETOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_ReadFromRESET() - 3/3: Failed data offset is larger than ResetMemory block size");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_WriteToVOLATILEDISK() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_WriteToVOLATILEDISK(void)
{
    int32   iRetCode = 0;
    char    cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiVOLATILEDISKOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localVOLATILEDISKBuffer[200];
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = (void*)localVOLATILEDISKBuffer;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = 200;
    memset(localVOLATILEDISKBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal unchange while new data as the same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToVOLATILEDISK() - 1/4: Nominal No changed since the data buffer unchanged");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToVOLATILEDISK() - 2/4: Nominal");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_MEMORY_WriteToVOLATILEDISK() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_WriteToVOLATILEDISK() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than VolatileDiskMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiVOLATILEDISKOffset = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_WriteToVOLATILEDISK() - 4/4: Failed data offset is larger than VolatileDiskMemory block size");

}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_ReadFromVOLATILEDISK() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_ReadFromVOLATILEDISK(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
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
    iRetCode = CFE_PSP_MEMORY_ReadFromVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_MEMORY_ReadFromVOLATILEDISK() - 1/3: Nominal - Read data is matched with VolatileDiskMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_ReadFromVOLATILEDISK() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_ReadFromVOLATILEDISK() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than VolatileDiskMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr;
    uiVOLATILEDISKOffset = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromVOLATILEDISK(pucData, uiVOLATILEDISKOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_ReadFromVOLATILEDISK() - 3/3: Failed data offset is larger than VolatileDiskMemory block size");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_WriteToUSERRESERVED() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_WriteToUSERRESERVED(void)
{
    int32   iRetCode = 0;
    char    cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiUSERRESERVEDOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localUSERRESERVEDBuffer[200];
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = (void*)localUSERRESERVEDBuffer;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = 200;
    memset(localUSERRESERVEDBuffer, '1', 200);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal unchange while new data as the same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToUSERRESERVED() - 1/4: Nominal No changed since the data buffer unchanged");

    /* ----- Test case #2 - Need to update memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = 99;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToUSERRESERVED() - 2/4: Nominal");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_MEMORY_WriteToUSERRESERVED() - 2/4: Nominal - data match");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_WriteToUSERRESERVED() - 3/4: Input argument is NULL");

    /* ----- Test case #4 - Data offset is larger than UserReservedMemory block size ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    uiUSERRESERVEDOffset = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_WriteToUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_WriteToUSERRESERVED() - 4/4: Failed data offset is larger than UserReservedMemory block size");

}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_ReadFromUSERRESERVED() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_ReadFromUSERRESERVED(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
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
    iRetCode = CFE_PSP_MEMORY_ReadFromUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_MEMORY_ReadFromUSERRESERVED() - 1/3: Nominal - Read data is matched with UserReservedMemory data");
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_ReadFromUSERRESERVED() - 1/3: Nominal - return value");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_ReadFromUSERRESERVED() - 2/3: Input argument is NULL");

    /* ----- Test case #3 - Data offset is larger than UserReservedMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr;
    uiUSERRESERVEDOffset = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_MEMORY_ReadFromUSERRESERVED(pucData, uiUSERRESERVEDOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_ReadFromUSERRESERVED() - 3/3: Failed data offset is larger than UserReservedMemory block size");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_RestoreRESET() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_RestoreRESET(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuffer[10];
    uint32 uiLocalBufferSize = 10;
    char cMsg[256] = {};
    
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = uiLocalBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = uiLocalBufferSize;

    /* ----- Test case #1 - File not on system, create fail ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreRESET();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreRESET - 1/6: Failed to create file - return code");

    /* ----- Test case #2 - File not on system, create success, write fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreRESET();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreRESET - 2/6: Successfully create file, write error - return code");

    /* ----- Test case #3 - File not on system, create success, write success ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreRESET();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreRESET - 3/6: Successfully create file, write success - return code");

    /* ----- Test case #4 - Fail to read from flash, error write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreRESET();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreRESET - 4/6: Fail to read data, error write - return code");

        /* ----- Test case #6 - Fail to read from flash, success write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    iReturnCode = CFE_PSP_MEMORY_RestoreRESET();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreRESET - 5/6: Fail to read data, success write - return code");

    /* ----- Test case #6 - Successfully read from flash ----- */
     /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreRESET();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreRESET - 6/6: Successfully read from FLASH - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_RestoreCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_RestoreCDS(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuffer[10];
    uint32 uiLocalBufferSize = 10;
    char cMsg[256] = {};
    
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = uiLocalBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = uiLocalBufferSize;

    /* ----- Test case #1 - File not on system, create fail ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreCDS();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreCDS - 1/6: Failed to create file - return code");

    /* ----- Test case #2 - File not on system, create success, write fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreCDS();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreCDS - 2/6: Successfully create file, write error - return code");

    /* ----- Test case #3 - File not on system, create success, write success ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreCDS();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreCDS - 3/6: Successfully create file, write success - return code");

    /* ----- Test case #4 - Fail to read from flash, error write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreCDS();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreCDS - 4/6: Fail to read data, error write - return code");

    /* ----- Test case #5 - Fail to read from flash, success write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    iReturnCode = CFE_PSP_MEMORY_RestoreCDS();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreCDS - 5/6: Fail to read data, success write - return code");

    /* ----- Test case #6 - Successfully read from flash ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreCDS();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreCDS - 6/6: Successfully read from FLASH - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_RestoreVOLATILEDISK() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_RestoreVOLATILEDISK(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuffer[10];
    uint32 uiLocalBufferSize = 10;
    char cMsg[256] = {};
    
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = uiLocalBuffer;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize = uiLocalBufferSize;

    /* ----- Test case #1 - File not on system, create fail ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreVOLATILEDISK();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreVOLATILEDISK - 1/6: Failed to create file - return code");

    /* ----- Test case #2 - File not on system, create success, write fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreVOLATILEDISK();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreVOLATILEDISK - 2/6: Successfully create file, write error - return code");

    /* ----- Test case #3 - File not on system, create success, write success ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreVOLATILEDISK();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreVOLATILEDISK - 3/6: Successfully create file, write success - return code");

    /* ----- Test case #4 - Fail to read from flash, error write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreVOLATILEDISK();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreVOLATILEDISK - 4/6: Fail to read data, error write - return code");

        /* ----- Test case # 5- Fail to read from flash, success write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    iReturnCode = CFE_PSP_MEMORY_RestoreVOLATILEDISK();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreVOLATILEDISK - 5/6: Fail to read data, success write - return code");

    /* ----- Test case #6 - Successfully read from flash ----- */
     /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreVOLATILEDISK();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreVOLATILEDISK - 6/6: Successfully read from FLASH - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_RestoreUSERRESERVED() test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_RestoreUSERRESERVED(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuffer[10];
    uint32 uiLocalBufferSize = 10;
    char cMsg[256] = {};
    
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = uiLocalBuffer;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = uiLocalBufferSize;

    /* ----- Test case #1 - File not on system, create fail ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreUSERRESERVED();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreUSERRESERVED - 1/6: Failed to create file - return code");

    /* ----- Test case #2 - File not on system, create success, write fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreUSERRESERVED();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreUSERRESERVED - 2/6: Successfully create file, write error - return code");

    /* ----- Test case #3 - File not on system, create success, write success ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreUSERRESERVED();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreUSERRESERVED - 3/6: Successfully create file, write success - return code");

    /* ----- Test case #4 - Fail to read from flash, error write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreUSERRESERVED();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreUSERRESERVED - 4/6: Fail to read data, error write - return code");

        /* ----- Test case #5 - Fail to read from flash, success write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    iReturnCode = CFE_PSP_MEMORY_RestoreUSERRESERVED();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreUSERRESERVED - 5/6: Fail to read data, success write - return code");

    /* ----- Test case #6 - Successfully read from flash ----- */
     /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreUSERRESERVED();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreUSERRESERVED - 6/6: Successfully read from FLASH - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_WriteToRAM test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_WriteToRAM(void)
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
    uint32 uiCrcCheck = 0;

    /* ----- Test case #1 - NULL ptr ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "WriteToRAM: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(NULL, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_WriteToRAM - 1/8: Memory not allocated - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_WriteToRAM - 1/8: Memory not allocated - message");

    /* ----- Test case #2 - Default switch/case ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    pucData = uiBuffer;
    sprintf(cMsg, MEMORY_PRINT_SCOPE "WriteToRAM: Invalid Memory Selection\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, 9999);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, "_CFE_PSP_MEMORY_WriteToRAM - 2/8: Invalid memory selection - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_WriteToRAM - 2/8: Invalid memory selection - message");

    /* ----- Test case #3 - Memory block ptr NULL ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = NULL;
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_WriteToRAM - 3/8: Memory block ptr was null - return code");

    /* ----- Test case #4 - Illegal offset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "WriteToRAM: Invalid Mem Range\n");
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 99, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_WriteToRAM - 4/8: Invalid offset - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_WriteToRAM - 4/8: message");

    /* ----- Test case #5 - Data is match/clean ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToRAM - 5/8: Data match, no writing required - return code");

    /* ----- Test case #6 - Data mismatch ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    memset(localCDSBuffer, '2', 200);
    memset(localCDSBuffer, '0', 100);
    memset(uiBuffer, '0', uiNumBytes);
    uiCrcCheck = CFE_PSP_CalculateCRC(localCDSBuffer, 200, 0);
    memset(localCDSBuffer, '2', 100);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToRAM - 6/8: Data mismatch, data write - return code");
    UtAssert_MemCmp(localCDSBuffer, uiBuffer, uiNumBytes, "_CFE_PSP_MEMORY_WriteToRAM - 6/8: Data mismatch, data write - memory compare");

    /* ----- Test case #7 - Fail to take semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    memset(localCDSBuffer, '2', 200);
    memset(localCDSBuffer, '0', 100);
    memset(uiBuffer, '0', uiNumBytes);
    uiCrcCheck = CFE_PSP_CalculateCRC(localCDSBuffer, 200, 0);
    memset(localCDSBuffer, '2', 100);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_PRINT_SCOPE "WriteToRAM: Failed to take bin sem, status: %d\n", OS_SEM_FAILURE);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, "_CFE_PSP_MEMORY_WriteToRAM - 7/8: Fail to take binary semaphore - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_WriteToRAM - 7/8: Fail to take binary semaphore - return code");


    /* ----- Test case #8 - Fail to give semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    memset(localCDSBuffer, '2', 200);
    memset(localCDSBuffer, '0', 100);
    memset(uiBuffer, '0', uiNumBytes);
    uiCrcCheck = CFE_PSP_CalculateCRC(localCDSBuffer, 200, 0);
    memset(localCDSBuffer, '2', 100);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_PRINT_SCOPE "WriteToRAM: Fail to give bin sem, status %d\n", OS_SEM_FAILURE);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToRAM - 8/8: Fail to give binary semaphore - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_WriteToRAM - 8/8: Fail to give binary semaphore - return code");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_ReadFromRAM test cases
**=======================================================================================*/
void Ut_CFE_PSP_MEMORY_ReadFromRAM(void)
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
    sprintf(cMsg, MEMORY_PRINT_SCOPE "ReadFromRAM: NULL Pointer\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(NULL, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_ReadFromRAM - 1/6: Memory not allocated - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_ReadFromRAM - 1/6: Memory not allocated - message");

    /* ----- Test case #2 - Default switch/case ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "ReadFromRAM: Invalid Memory Option\n");
    pucData = uiBuffer;
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, 999);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, "_CFE_PSP_MEMORY_ReadFromRAM - 2/6: Invalid memory section option - return code");
    UtAssert_OS_print(cMsg, "_CFE_PSP_MEMORY_ReadFromRAM - 2/6: Invalid memory section option - message");    

    /* ----- Test case #3 - Memory block ptr NULL ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = NULL;
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_POINTER, "_CFE_PSP_MEMORY_ReadFromRAM - 3/6: Memory block ptr is NULL - return code");

    /* ----- Test case #4 - Illegal offset ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(pucData, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 999, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_RANGE, "_CFE_PSP_MEMORY_ReadFromRAM - 4/6: Invalid offset value - return code");;

    /* ----- Test case #5 - Data is match/clean ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_WriteToRAM - 5/6: Data match, no writing required - return code");

    /* ----- Test case #6 - Data mismatch ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    memset(uiBuffer, '0', uiNumBytes);
    memset(localCDSBuffer, '2', 100);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(pucData, uiCDSOffset, uiNumBytes, OP_CDS);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, "_CFE_PSP_MEMORY_ReadFromRAM - 6/6: Data mismatch, data write - return code");
    UtAssert_MemCmp(localCDSBuffer, uiBuffer, uiNumBytes, "_CFE_PSP_MEMORY_ReadFromRAM - 6/6: Data mismatch, data write - memory compare");
}

/*=======================================================================================
** Ut_CFE_PSP_MEMORY_RestoreDATA() test cases
**=======================================================================================*/

void Ut_CFE_PSP_MEMORY_RestoreDATA(void)
{
    int32 iReturnCode;
    uint32 uiLocalBuffer[10];
    uint32 uiLocalBufferSize = 10;
    char cMsg[256] = {};
    
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = uiLocalBuffer;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = uiLocalBufferSize;

    /* ----- Test case #1 - Invalid Memory Option or Invalid filename ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_PRINT_SCOPE "RestoreDATA: Invalid Memory Option\n");
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(99999);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_INVALID_MEM_TYPE, UT_MEMORY_PRINT_SCOPE "RestoreData - 1/7: Inavlid memory option - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_PRINT_SCOPE "RestoreData - 1/7: Invalid memory option - message");

    /* ----- Test case #2 - File not on system, create fail ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreReset - 2/7: Failed to create file - return code");

    /* ----- Test case #3 - File not on system, create success, write fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreReset - 3/7: Successfully create file, write error - return code");

    /* ----- Test case #4 - File not on system, create success, write success ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreReset - 4/7: Successfully create file, write success - return code");

    /* ----- Test case #5 - Fail to read from flash, error write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_PRINT_SCOPE "RestoreReset - 5/7: Fail to read data, error write - return code");

    /* ----- Test case #6 - Fail to read from flash, success write ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreReset - 6/7: Fail to read data, success write - return code");

    /* ----- Test case #7 - Successfully read from flash ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(stat), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(read), 1, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_PRINT_SCOPE "RestoreDATA - 7/7: Successfully read from FLASH - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_Init(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_Init(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};

    /* ----- Test case #1: Bin Sem Create Fail ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_ERROR);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE \
                    "Init: Failed to create MEMORY SYNC TASK binary semaphore, status: %d\n", OS_ERROR);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Init();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Init - 1/4: Failed to create binary semaphore - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Init - 1/4: Failed to create binary semaphore - message");

    /* ----- Test case #2: Start on startup false ----- */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Init();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Init - 2/4: Successfully create binary semaphore - return code");

    /* ----- Test case #3: Start on startup true, fail to start ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Init: Failed to start task\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Init();
    /* Verify results */
    UtAssert_True(iReturnCode ==CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Init - 3/4: Failed to start task - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Init - 3/4: Failed to start tasl - message");

    /* ----- Test case #4: Start on startup true, start task----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Init();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Init - 4/4: Failed to start task - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_Destroy(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_Destroy(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};

    /* ----- Test case #1: Task fail to stop ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Destroy: Unable to kill MEMORY SYNC task\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Destroy();
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Destroy - 1/4: Fail to stop task - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 1/4: Fail to stop task - message");

    /* ----- Test case #2: Fail to take binsem ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Destroy: Semaphore Error, status: %d\n", OS_SEM_FAILURE);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Destroy();
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Destroy - 2/4: Fail to take bsem - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 2/4: Fail take bsem - message");

    /* ----- Test case #3: Fail to delete bsem ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Destroy: Semaphore Error, status: %d\n", OS_SEM_FAILURE);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Destroy();
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Destroy - 3/4: Fail to delete bsem - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 3/4: Fail delete bsem - message");

    /* ----- Test case #4: Succesfful destroy ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_SUCCESS);
    OS_BinSemCreate(&g_MemorySyncTaskBinSem, MEMORY_SYNC_BSEM_NAME, OS_SEM_FULL, 0);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Destroy();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Destroy - 4/4: Success - return code");
    UtAssert_True(g_MemorySyncTaskBinSem == OS_OBJECT_ID_UNDEFINED, UT_MEMORY_SYNC_PRINT_SCOPE "Destroy - 4/4: Success - Semaphore value check");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_Start(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_Start(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};

    /* ----- Test case #1: Task is already running ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Task already running\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Start();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 1/4: Task already running - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 1/4: Task already running - message");

    /* ----- Test case #2: Binary semaphore not created ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_MemorySyncTaskBinSem = 0;
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Start: Data not initialized\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Start();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 2/4: Data not initialized - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 2/4: Data not initialized - message");

    /* ----- Test case #3: Task creation fails ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_ERROR);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Error starting MEMORY SYNC task\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Start();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 3/4: Failed to start task - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 3/4: Failed to start task - message");
    
    /* ----- Test case #4: Task creation successful ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    g_MemorySyncTaskBinSem = 99;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskCreate), 1, OS_SUCCESS);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Start();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Start - 4/4: Start Task - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_Stop(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_Stop(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};
    
    /* ----- Test case #1: Task is not running ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Stop: Task not running\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Stop();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 1/5: Task not running - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 1/5: Task not running - message");

    /* ----- Test case #2: Fail to take binsem ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Stop: Failed to take bsem, status: %d\n", OS_SEM_FAILURE);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Stop();
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 2/5: Fail to take binary semaphore - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 2/5: Fail to take binary semaphore - message");

    /* ----- Test case #3: Fail to delete task ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_ERROR);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Stop: Unable to delete task, status: %d\n", OS_ERROR);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Stop();
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 3/5: Fail to delete task - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 3/5: Fail to delete task - message");

    /* ----- Test case #4: fail to give binsem----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Stop: Failed to give bsem, status: %d\n", OS_SEM_FAILURE);
    OS_TaskCreate(
                    &g_uiMemorySyncTaskId,
                    MEMORY_SYNC_TASK_NAME,
                    CFE_PSP_MEMORY_SYNC_Task,
                    OSAL_TASK_STACK_ALLOCATE,
                    OSAL_SIZE_C(4096),
                    g_uiMemorySyncTaskPriority,
                    0
                );
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Stop();
    /* Verify results */
    UtAssert_True(iReturnCode = CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 4/5: Fail to give binary semaphore - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 4/5: Fail to give binary semaphore - message");

    /* ----- Test case #5: Successful stop----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelete), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    OS_TaskCreate(
                    &g_uiMemorySyncTaskId,
                    MEMORY_SYNC_TASK_NAME,
                    CFE_PSP_MEMORY_SYNC_Task,
                    OSAL_TASK_STACK_ALLOCATE,
                    OSAL_SIZE_C(4096),
                    g_uiMemorySyncTaskPriority,
                    0
                );
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_Stop();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "Stop - 5/5: Successfully stop task - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_isRunning(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_isRunning(void)
{
    bool iReturnValue = true;
    char cMsg[256] = {};
    
    /* ----- Test case #1: Task is not running ----- */
    /* Set additional inputs */
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    /* Execute tests */
    iReturnValue = CFE_PSP_MEMORY_SYNC_isRunning();
    /* Verify results */
    UtAssert_True(iReturnValue == false, UT_MEMORY_SYNC_PRINT_SCOPE "isRunning - 1/2: Task not running - return code");

    /* ----- Test case #1: Task is running ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    /* Execute tests */
    iReturnValue = CFE_PSP_MEMORY_SYNC_isRunning();
    /* Verify results */
    UtAssert_True(iReturnValue == true, UT_MEMORY_SYNC_PRINT_SCOPE "isRunning - 2/2: Task is running - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_setPriority(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_setPriority(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};
    osal_priority_t newPriority = 0;
    
    /* ----- Test case #1: New priority too high ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "setPriority: New priority too high\n");
    newPriority = 250;
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_setPriority(newPriority);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 1/5: New priority too high - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 1/5: New priority too high - message");

    /* ----- Test case #2: New priority too low ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "setPriority: New priority too low\n");
    newPriority = 1;
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_setPriority(newPriority);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 2/5: New priority too low- return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 2/5: New priority too low - message");

    /* ----- Test case #3: Task not running ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    newPriority = MEMORY_SYNC_PRIORITY_DEFAULT;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_setPriority(newPriority);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 3/5: New priority too low- return code");
    UtAssert_True(g_uiMemorySyncTaskPriority == newPriority, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 3/5: New priority set, task not running - priority value check");

    /* ----- Test case #4: Set priority fail ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    newPriority = MEMORY_SYNC_PRIORITY_DEFAULT;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskSetPriority), 1, OS_ERROR);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "setPriority: Failed to set new priority\n");
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_setPriority(newPriority);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 4/5: Failed to set priority - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 4/5: Failed to set new priority - message");
    
    /* ----- Test case #5: Succesfully set priority ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    newPriority = MEMORY_SYNC_PRIORITY_DEFAULT;
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, 99);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskSetPriority), 1, OS_SUCCESS);
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_setPriority(newPriority);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 5/5: New priority set, task running - return code");
    UtAssert_True(g_uiMemorySyncTaskPriority == newPriority, UT_MEMORY_SYNC_PRINT_SCOPE "setPriority - 5/5: New priority set, task running - priority value check");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_getPriority(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_getPriority(void)
{
    osal_id_t basePriority = 99;
    osal_id_t priority = 0;
    
    /* ----- Test case #1: Get task priority----- */
    /* Set additional inputs */
    g_uiMemorySyncTaskPriority = basePriority;
    /* Execute tests */
    priority = CFE_PSP_MEMORY_SYNC_getPriority();
    /* Verify results */
    UtAssert_True(priority == basePriority, UT_MEMORY_SYNC_PRINT_SCOPE "getPriority - 1/1: Get priority value");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_setFrequency(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_setFrequency(void)
{
    uint32 newFreq = 333;
    g_uiMemorySyncTime = 1;
    int32 iReturnCode = CFE_PSP_ERROR;
    
    /* ----- Test case #1: Set new frequency----- */
    /* Set additional inputs */
    /* Execute tests */
    iReturnCode = CFE_PSP_MEMORY_SYNC_setFrequency(newFreq);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "setFrequency - 1/1: Set new frequency - return code");
    UtAssert_True(g_uiMemorySyncTime == newFreq, UT_MEMORY_SYNC_PRINT_SCOPE "setFrequency - 1/1: Set new frequency - frequency value check");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_getFrequency(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_getFrequency(void)
{
    int32 retFreq = 33;
    
    /* ----- Test case #1: Get Frequency ----- */
    /* Set additional inputs */
    g_uiMemorySyncTime = 999;
    /* Execute tests */
    retFreq = CFE_PSP_MEMORY_SYNC_getFrequency();
    /* Verify results */
    UtAssert_True(g_uiMemorySyncTime == retFreq, UT_MEMORY_SYNC_PRINT_SCOPE "getFrequency - 1/1: Get frequency - frequency check");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_getStatus(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_getStatus(void)
{
    uint32 retStat = 99;
    
    /* ----- Test case #1: Get statistics ----- */
    /* Set additional inputs */
    g_uiMemorySyncStatistics = 333;
    /* Execute tests */
    retStat = CFE_PSP_MEMORY_SYNC_getStatistics();
    /* Verify results */
    UtAssert_True(retStat == g_uiMemorySyncStatistics, UT_MEMORY_SYNC_PRINT_SCOPE "getStatus - 1/1: Get statistics - stat check");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_Task(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_Task(void)
{
    char cMsg[256] = {};
    uint8 buf[33] = {};
    memset(buf, 0, 33);
    CFE_PSP_MemoryBlock_t baseMemBlock = {(void *)buf, 33};
    CFE_PSP_ReservedMemoryMap.ResetMemory = baseMemBlock;
    CFE_PSP_ReservedMemoryMap.CDSMemory = baseMemBlock;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory = baseMemBlock;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory = baseMemBlock;

    /* ----- Test case #1: Fail to take task semaphore ----- */
    /* Set additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Task: Failed to take TASK semaphore, status: %d\n", OS_SEM_FAILURE);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 1/7: Unable to take task semaphore - message");

    /* ----- Test case #2: Fail to take ENTRY semaphore ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Task: Failed to take ENTRY binary semaphore, status: %d\n", OS_SEM_FAILURE);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 2/7: Unable to take ENTRY semaphore - message");

    /* ----- Test case #3: Fail to write to FLASH, no flag change ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    g_bCDSUpdateFlag = true;
    UT_SetDeferredRetcode(UT_KEY(open), 1, -1);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_True(g_bCDSUpdateFlag == true, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 3/7: Write Failed, memory section update flag check");

    /* ----- Test case #4: Successfully write to FLASH, flag change -----*/
    /* Set additional inputs */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    g_bCDSUpdateFlag = true;
    UT_SetDeferredRetcode(UT_KEY(open), 1, 99);
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 33;
    UT_SetDeferredRetcode(UT_KEY(write), 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(close), 1, OS_SUCCESS);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_True(g_bCDSUpdateFlag == false, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 4/7: Write SUCCESS, memory section update flag check");

    /* ----- Test case #5: Fail to give ENTRY semaphore----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_bCDSUpdateFlag = false;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Task: Fail to give ENTRY binary semaphore, status: %d\n", OS_SEM_FAILURE);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 5/7: Unable to give ENTRY semaphore - message");

    /* ----- Test case #6: Fail to give task semaphore----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_bCDSUpdateFlag = false;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGive), 1, OS_SEM_FAILURE);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Task: Failed to give TASK semaphore, status: %d\n", OS_SEM_FAILURE);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 6/7: Unable to give TASK semaphore - message");

    /* ----- Test case #7: Fail to task delay----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_bCDSUpdateFlag = false;
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemTake), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemGive), OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelay), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskDelay), 1, OS_ERROR);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "Task: Failed to delay task, status: %d\n", OS_ERROR);
    /* Execute test */
    CFE_PSP_MEMORY_SYNC_Task();
    /* Verify results */
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "Task - 7/7: Unable to delay task - message");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_CDS_FPATH(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_CDS_FPATH(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - ----- */
    /* Additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(snprintf), 0);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_CDS_FPATH();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "GEN_CDS_FPATH - 1/1: Generate fpath - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_RESET_FPATH(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_RESET_FPATH(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - ----- */
    /* Additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(snprintf), 0);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_RESET_FPATH();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "GEN_RESET_FPATH - 1/1: Generate fpath - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_VOLATILEDISK_FPATH(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_VOLATILEDISK_FPATH(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - ----- */
    /* Additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(snprintf), 0);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_VOLATILEDISK_FPATH();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "GEN_VOLATILEDISK_FPATH - 1/1: Generate fpath - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_USERRESERVED_FPATH(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_USERRESERVED_FPATH(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* ----- Test case #1 - ----- */
    /* Additional inputs */
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(strncpy), 0);
    UT_SetDefaultReturnValue(UT_KEY(snprintf), 0);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_USERRESERVED_FPATH();
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "GEN_USERRESERVED_FPATH - 1/1: Generate fpath - return code");
}

/**********************************************************
 * void Ut_CFE_PSP_MEMORY_SYNC_GenerateFilepath(void); Testcases
 *********************************************************/
void Ut_CFE_PSP_MEMORY_SYNC_GenerateFilepath(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    char cMsg[256] = {};

    /* ----- Test case #1 - Fail to construct directory path ----- */
    /* Additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDefaultReturnValue(UT_KEY(snprintf), -5);
    // UT_SetDeferredRetcode(UT_KEY(snprintf), 1, -5);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath: Error assembling active path, error: %d\n", -5);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_GenerateFilepath(99);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 1/5: Fail to construct directory path - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 1/5: Fail to construct directory path - message");

    /* ----- Test case #2 - Fail to create direcotry ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(snprintf), 1, 0);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath: Error creating path\n");
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, -5);
    errno = 3;
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_GenerateFilepath(1);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 2/5: Fail to create directory path - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 2/5: Fail to create directory path - message");

    /* ----- Test case #3 - snprintf fail during complete constructions ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(snprintf), 1, 0);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath: ERROR: %d\n", -5);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(snprintf), 1, -5);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_GenerateFilepath(1);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 3/5: Fail to create FULL path - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 3/5: Fail to create FULL path - message");

    /* ----- Test case #4 - Reach defualt case ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    // UT_SetDeferredRetcode(UT_KEY(snprintf), 1, 0);
    UT_SetDefaultReturnValue(UT_KEY(snprintf), 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(strncpy), 0);
    sprintf(cMsg, MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath: Invalid Memory Section\n");
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_GenerateFilepath(99);
    printf("MATT: tc 4 - return code: %d\n", iReturnCode);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_ERROR, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 4/5: Invalid memory selection - return code");
    UtAssert_OS_print(cMsg, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 4/5: Invalid memory selection - message");

    /* ----- Test case #5 - Successful execution ----- */
    /* Additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(snprintf), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(mkdir), 1, 0);
    UT_SetDeferredRetcode(UT_KEY(snprintf), 1, 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_SymbolLookup), OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_MemValidateRange), CFE_PSP_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(strncpy), 0);
    /* Execute test */
    iReturnCode = CFE_PSP_MEMORY_SYNC_GenerateFilepath(1);
    /* Verify results */
    UtAssert_True(iReturnCode == CFE_PSP_SUCCESS, UT_MEMORY_SYNC_PRINT_SCOPE "GenerateFilepath - 5/5: Success - return code");
}

/*=======================================================================================
** End of file psp_memory_testcases.c
**=======================================================================================*/
