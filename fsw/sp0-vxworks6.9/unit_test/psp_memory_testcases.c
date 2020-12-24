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
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "CFE_PSP_GetCDSSize() - 1/2: Argument is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_SetReadCDSMethod() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetReadCDSMethod(void)
{
    uint8 ucMethod = 10;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    CFE_PSP_SetReadCDSMethod(ucMethod);
    /* Verify outputs */
    UtAssert_True(g_CDSReadMethod = ucMethod, "_CFE_PSP_SetReadCDSMethod - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetReadCDSMethod() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetReadCDSMethod(void)
{
    uint8 ucRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    g_CDSReadMethod = 20;
    /* Execute test */
    ucRetCode = CFE_PSP_GetReadCDSMethod();
    /* Verify outputs */
    UtAssert_True(ucRetCode = g_CDSReadMethod, "_CFE_PSP_GetReadCDSMethod - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_SetStaticCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetStaticCRC(void)
{
    uint32 uiNewCRC = 10;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    CFE_PSP_SetStaticCRC(uiNewCRC);
    /* Verify outputs */
    UtAssert_True(sg_uiCDSCrc = uiNewCRC, "_CFE_PSP_SetStaticCRC - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetStaticCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetStaticCRC(void)
{
    uint32 uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    sg_uiCDSCrc = 20;
    /* Execute test */
    uiRetCode = CFE_PSP_GetStaticCRC();
    /* Verify outputs */
    UtAssert_True(uiRetCode = sg_uiCDSCrc, "_CFE_PSP_GetStaticCRC - 1/1:Nominal");
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
    UtAssert_True(uiRetCode = uiCrcCode, "_CFE_PSP_CalculateCRC - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_ReadCDSFromFlash() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ReadCDSFromFlash(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint32 *puiReadBytes = NULL;
    uint32  uiReadBytes = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiReadBytes = &uiReadBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiReadBytes);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadCDSFromFlash() - 1/4: Nominal");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    puiReadBytes = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 2/4: Input argument is NULL");

    /* ----- Test case #3 - Open CDS file failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    puiReadBytes = &uiReadBytes;
    sprintf(cMsg, "CFE_PSP: Failed to open the CDS file.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 3/4: Failed to open the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 3/4: Failed case for open()");

    /* ----- Test case #4 - Read CDS file failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiReadBytes = &uiReadBytes;
    sprintf(cMsg, "CFE_PSP: Failed to read the CDS data on Flash.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 4/4: Failed to read the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 4/4: Failed case for read()");
}

/*=======================================================================================
** Ut_CFE_PSP_WriteCDSToFlash() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteCDSToFlash(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint32 *puiWriteBytes = NULL;
    uint32  uiWriteBytes = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiWriteBytes = &uiWriteBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiWriteBytes);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteCDSToFlash() - 1/4: Nominal");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    puiWriteBytes = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteCDSToFlash() - 2/4: Input argument is NULL");

    /* ----- Test case #3 - Open CDS file failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    puiWriteBytes = &uiWriteBytes;
    sprintf(cMsg, "CFE_PSP: Failed to open the CDS file.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 3/4: Failed to open the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteCDSToFlash() - 3/4: Failed case for open()");

    /* ----- Test case #4 - Read CDS file failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiWriteBytes = &uiWriteBytes;
    sprintf(cMsg, "CFE_PSP: Failed to write the CDS data to Flash.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 4/4: Failed to write the CDS file");
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteCDSToFlash() - 4/4: Failed case for write()");
}


/*=======================================================================================
** Ut_CFE_PSP_WriteToCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteToCDS(void)
{
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;

    /* ----- Test case #1 - Nominal unchange while new data as the same as original one ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteToCDS() - 1/5: Nominal No changed since the data buffer unchanged");

    // /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 100);
    pucData = uiBuffer;
    uiBuffer[10] = uiBuffer[10] + 1;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiNumBytes);
    sprintf(cMsg, "CFE_PSP: Wrote %d to CDS file.\n", uiNumBytes); 
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteToCDS() - 2/5: Nominal Write data to Flash");
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteToCDS() - 2/5: Nominal");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteToCDS() - 3/5: Input argument is NULL");

    /* ----- Test case #4 - Dara offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteToCDS() - 4/5: Failed data offset is larger than CDSMemory block size");

    /* ----- Test case #5 - Date offset plus number of changed bytes is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteToCDS() - 5/5: Failed data offset plus number of changed bytes is larger than CDSMemory block size");
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
    uint32  uiTempCrc = 0;
    uint8   ucIdx = 0;

    /* ----- Test case #1 - Nominal Using CRC check mode ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    pucData = uiBuffer;
    g_CDSReadMethod = CFE_PSP_CDS_READ_METHOD_CRC;
    sg_uiCDSCrc = 0;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiNumBytes);
    /* Modify first 100 bytes from CDSMomory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr)[ucIdx] = ucIdx;
    }
    /* Calculate the CRC code for current CDSMemory data. This value needs for next OS_printf assert string */
    uiTempCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                        CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, uiTempCrc);
    sprintf(cMsg, "CFE_PSP: CRC mismatched (uiTempCrc = 0x%08X, sg_uiCDSCrc = 0x%08X)\n", uiTempCrc, sg_uiCDSCrc); 
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadFromCDS() - 1/5: Nominal Read the data from Flash");
    UtAssert_True(sg_uiCDSCrc == uiTempCrc, "_CFE_PSP_ReadFromCDS() - 1/5: Nominal New CRC codes was updated");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 1/5: Nominal Read data is matched with CDSMemory data");
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadFromCDS() - 1/5: Nominal");

    /* ----- Test case #2 - Nominal Always read from Flash ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    g_CDSReadMethod = CFE_PSP_CDS_READ_METHOD_FLASH;
    sg_uiCDSCrc = 0;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiNumBytes);
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 2/5: Nominal Read data is matched with CDSMemory data");
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadFromCDS() - 2/5: Nominal");

    /* ----- Test case #3 - Input argument is NULL ----- */
    /* Setup additional inputs */
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadFromCDS() - 3/5: Input argument is NULL");

    /* ----- Test case #4 - Dara offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadFromCDS() - 4/5: Failed data offset is larger than CDSMemory block size");

    /* ----- Test case #5 - Date offset plus number of changed bytes is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 99;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadFromCDS() - 5/5: Failed data offset plus number of changed bytes is larger than CDSMemory block size");
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

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(uiBlockAddress, (uint32)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 0, 
                           "_CFE_PSP_GetResetArea - 1/3:Nominal ResetMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiBlockSize, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, 0, 
                           "_CFE_PSP_GetResetArea - 1/3:Nominal ResetMemory block size is matched");
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetResetArea - 1/3:Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = NULL;
    puiBlockSize = &uiBlockSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetResetArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiBlockAddress = &uiBlockAddress;
    puiBlockSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiBlockAddress, puiBlockSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetResetArea - 3/3: Failed block size pointer is NULL");
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
                           "_CFE_PSP_GetUserReservedArea - 1/3:Nominal UserReservedMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiUserAreaSize, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize, 0, 
                           "_CFE_PSP_GetUserReservedArea - 1/3:Nominal UserReservedMemory block size is matched");
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetUserReservedArea - 1/3:Nominal");

    /* ----- Test case #2 - UserArea address pointer is NULL ----- */
    /* Setup additional inputs */
    puiUserArea = NULL;
    puiUserAreaSize = &uiUserAreaSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetUserReservedArea - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - UserArea size pointer is NULL ----- */
    /* Setup additional inputs */
    puiUserArea = &uiUserArea;
    puiUserAreaSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetUserReservedArea(puiUserArea, puiUserAreaSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetUserReservedArea - 3/3: Failed block size pointer is NULL");
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
                           "_CFE_PSP_GetVolatileDiskMem - 1/3:Nominal VolatileDiskMemory block address is matched");
    UtAssert_IntegerCmpAbs(uiVolDiskSize, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize, 0, 
                           "_CFE_PSP_GetVolatileDiskMem - 1/3:Nominal VolatileDiskMemory block size is matched");
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetUserReservedArea - 1/3:Nominal");

    /* ----- Test case #2 - Block address pointer is NULL ----- */
    /* Setup additional inputs */
    puiVolDisk = NULL;
    puiVolDiskSize = &uiVolDiskSize;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetVolatileDiskMem - 2/3: Failed block address pointer is NULL");

    /* ----- Test case #3 - Block size pointer is NULL ----- */
    /* Setup additional inputs */
    puiVolDisk = &uiVolDisk;
    puiVolDiskSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetVolatileDiskMem(puiVolDisk, puiVolDiskSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetVolatileDiskMem - 3/3: Failed block size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_InitProcessorReservedMemory() test cases
**=======================================================================================*/
void Ut_CFE_PSP_InitProcessorReservedMemory(void)
{
    uint32  uiRetCode = 0;
    char cMsg[256] = {};
    uint32 uiRestartType = CFE_PSP_RST_TYPE_POWERON;
    uint32 uiNumBytes = 100;

    /* ----- Test case #1 - Nominal Read the exist CDS file ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiNumBytes); 
    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    sprintf(cMsg, "CFE_PSP: Clearing Processor Reserved Memory.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Cleared processor reserved memory");
    sprintf(cMsg, "CFE_PSP: Read %d bytes of CDS data from Flash.\n", uiNumBytes);
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Read from existed CDS file");
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Nominal ");

    /* ----- Test case #2 - Nominal Create new CDS file ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, CFE_PSP_SUCCESS); 
    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    sprintf(cMsg, "CFE_PSP: Clearing Processor Reserved Memory.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 2/5: Cleared processor reserved memory");
    sprintf(cMsg, "CFE_PSP: Created the CDS file.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 2/5: Created new CDS file");
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory - 2/5: Nominal ");

    /* ----- Test case #3 - Failed Read the exist CDS file ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, OS_ERROR); 
    uiRestartType = CFE_PSP_RST_TYPE_PROCESSOR;
    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    sprintf(cMsg, "CFE_PSP: Clearing Processor Reserved Memory.\n");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 3/5: No clear processor reserved memory");
    sprintf(cMsg, "CFE_PSP: Failed to read the CDS data on Flash.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 3/5: Failed to read from existed CDS file");
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_InitProcessorReservedMemory - 3/5: Return error code");

    /* ----- Test case #4 - Nominal Create new CDS file ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(creat), 1, OS_ERROR); 
    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    sprintf(cMsg, "CFE_PSP: Clearing Processor Reserved Memory.\n");
    UtAssert_NoOS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 4/5: No clear processor reserved memory");
    sprintf(cMsg, "CFE_PSP: Failed to create the CDS file(/ffx0/CDS) on Flash.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 4/5: Failed to create new CDS file");
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_InitProcessorReservedMemory - 4/5: Return error code");

    /* ----- Test case #5 - Failed Reserved block size is greater than the reserved memory ----- */
    /* Setup additional inputs */
    /* Set BlockSize to 16MB, which 1MB larger than the reserved size to force to failure case */
    PSP_ReservedMemBlock.BlockSize = 16777216; 
    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_InitProcessorReservedMemory - 5/5: Failed Reserved block size is greater than reserved memory");
}

/*=======================================================================================
** Ut_CFE_PSP_SetupReservedMemoryMap() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetupReservedMemoryMap(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    /* Execute test */
    uiRetCode = CFE_PSP_SetupReservedMemoryMap();
    /* Verify outputs */
    UtAssert_True(TRUE, "_CFE_PSP_SetupReservedMemoryMap - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_DeleteProcessorReservedMemory() test cases
**=======================================================================================*/
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void)
{
    /* Nothing to test */
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
    uiRetCode = CFE_PSP_GetResetArea(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetKernelTextSegmentInfo - 1/2:Nominal");

    /* ----- Test case #2 - Kernel segement size pointer is NULL ----- */
    /* Setup additional inputs */
    puiKernelSegment = &uiKernelSegment;
    puiKernelSegmentSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetKernelTextSegmentInfo - 2/2: Failed Kernel segement size pointer is NULL");
}

/*=======================================================================================
** Ut_CFE_PSP_GetCFETextSegmentInfo() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetCFETextSegmentInfo(void)
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
    uiRetCode = CFE_PSP_GetResetArea(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_SUCCESS, "_CFE_PSP_GetKernelTextSegmentInfo - 1/3:Nominal");

    /* ----- Test case #3 - Kernel segement size pointer is NULL ----- */
    /* Setup additional inputs */
    puiKernelSegment = &uiKernelSegment;
    puiKernelSegmentSize = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetResetArea(puiKernelSegment, puiKernelSegmentSize);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetKernelTextSegmentInfo - 3/3: Failed Kernel segement size pointer is NULL");
}


/*=======================================================================================
** End of file psp_memory_testcases.c
**=======================================================================================*/
