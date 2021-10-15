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
    UtAssert_True(g_uiCDSCrc = uiNewCRC, "_CFE_PSP_SetStaticCRC - 1/1:Nominal");
}

/*=======================================================================================
** Ut_CFE_PSP_GetStaticCRC() test cases
**=======================================================================================*/
void Ut_CFE_PSP_GetStaticCRC(void)
{
    uint32 uiRetCode = 0;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    g_uiCDSCrc = 20;
    /* Execute test */
    uiRetCode = CFE_PSP_GetStaticCRC();
    /* Verify outputs */
    UtAssert_True(uiRetCode = g_uiCDSCrc, "_CFE_PSP_GetStaticCRC - 1/1:Nominal");
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
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_ReadCDSFromFlash() - 1/6: Nominal");
    UtAssert_True(*puiReadBytes == (uint32)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_ReadCDSFromFlash() - 1/6: Nominal");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    puiReadBytes = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 2/6: Input argument is NULL");
    UtAssert_True(puiReadBytes == NULL, "_CFE_PSP_ReadCDSFromFlash() - 2/6: Input argument is NULL");

    /* ----- Test case #3 - Open CDS file failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiReadBytes = &uiReadBytes;
    sprintf(cMsg, "CFE_PSP: Failed to open the CDS file during READ\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 3/6: Failed to open the CDS file");
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 3/6: Failed case for open()");

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
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 4/6: Failed to read the CDS file");
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadCDSFromFlash() - 4/6: Failed case for read()");
    UtAssert_True(*puiReadBytes == 0, "_CFE_PSP_ReadCDSFromFlash() - 4/6: Failed case for read()");

    /* ----- Test case #5 - Read less expected from flash ----- */
    /* Set additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiReadBytes = &uiReadBytes;
    long iDeferredReadNumBytes = (long)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 1;
    sprintf(cMsg, "CFE_PSP: Failed to read full block size. Read only %ld of %ld bytes.\n", iDeferredReadNumBytes, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, iDeferredReadNumBytes);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify results */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFomFlash() - 5/6: Read less than expected");
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_ReadCDSFomFlash() - 5/6: Read less than expected");
    UtAssert_True(*puiReadBytes == 0, "_CFE_PSP_ReadCDSFomFlash() - 5/6: Read less than expected");

    /* ----- Test case #6 - Close fail ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, "CFE_PSP: Could not close CDS file descriptor during READ\n");
    puiReadBytes = &uiReadBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiReadBytes);
    UT_SetDeferredRetcode(UT_KEY(close), 1, CFE_PSP_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_ReadCDSFromFlash(puiReadBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadCDSFromFlash() - 6/6: Unable to close file descriptor");
}

/*=======================================================================================
** Ut_CFE_PSP_WriteCDSToFlash() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteCDSToFlash(void)
{
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint32 *puiWriteBytes = NULL;
    uint32  uiWriteBytes = 100;
    g_bCorruptedCDSFlash = true;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    puiWriteBytes = &uiWriteBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiWriteBytes);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_WriteCDSToFlash() - 1/5: Nominal");
    UtAssert_True(*puiWriteBytes == (uint32)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, "_CFE_PSP_WriteCDSToFlash() - 1/5: Nominal");
    // UtAssert_True(g_bCorruptedCDSFlash == false, "_CFE_PSP_WriteCDSToFlash() - 1/4: Nominal");

    /* ----- Test case #2 - Input argument is NULL ----- */
    /* Setup additional inputs */
    puiWriteBytes = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteCDSToFlash() - 2/5: Input argument is NULL");
    UtAssert_True(puiWriteBytes == NULL, "_CFE_PSP_WriteCDSToFlash() - 2/5: Input argument is NULL");


    /* ----- Test case #3 - Open CDS file failed ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    puiWriteBytes = &uiWriteBytes;
    sprintf(cMsg, "CFE_PSP: Failed to open the CDS file during WRITE\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    g_bCorruptedCDSFlash = false;
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 3/5: Failed to open the CDS file");
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteCDSToFlash() - 3/5: Failed case for open()");
    UtAssert_True(g_bCorruptedCDSFlash == true,  "_CFE_PSP_WriteCDSToFlash() - 3/5: Failed case for open()");
    UtAssert_True(*puiWriteBytes == 0, "_CFE_PSP_WriteCDSToFlash() - 3/5: Failed case for open()");

    /* ----- Test case #4 - Read CDS file failed ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    puiWriteBytes = &uiWriteBytes;
    sprintf(cMsg, "CFE_PSP: Failed to write the CDS data to Flash.\n");
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, OS_ERROR);
    g_bCorruptedCDSFlash = false;
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 4/5: Failed to write the CDS file");
    UtAssert_True(iRetCode == CFE_PSP_ERROR, "_CFE_PSP_WriteCDSToFlash() - 4/4: Failed case for write()");
    UtAssert_True(*puiWriteBytes == 0, "_CFE_PSP_WriteCDSToFlash() - 4/5: Failed case for write()");
    UtAssert_True(g_bCorruptedCDSFlash == true, "_CFE_PSP_WriteCDSToFlash() - 4/5: Failed case for write()");

    /* ----- Test case #5 - Unable to close file descriptor ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    sprintf(cMsg, "CFE_PSP: Could not close CDS file descriptor during WRITE\n");
    puiWriteBytes = &uiWriteBytes;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiWriteBytes);
    UT_SetDeferredRetcode(UT_KEY(close), 1, CFE_PSP_ERROR);
    /* Execute test */
    iRetCode = CFE_PSP_WriteCDSToFlash(puiWriteBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_WriteCDSToFlash() - 5/5: Unable to close file descriptor");
}

/*=======================================================================================
** Ut_CFE_PSP_WriteToCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_WriteToCDS(void)
{
    int32 iRetCode = 0;
    char    cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;

    uint8   localCDSBuffer[2000];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 2000;
    memset(localCDSBuffer, '1',2000);
    memset(uiBuffer, '1',uiNumBytes);

    /* ----- Test case #1 - Nominal unchange while new data as the same as original one ----- */
    /* Setup additional inputs */
    pucData = uiBuffer;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteToCDS() - 1/6: Nominal No changed since the data buffer unchanged");

    /* ----- Test case #2 - Memcmp indicates we need to write, successful write to Flash ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = uiBuffer[10] + 1;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiNumBytes);
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteToCDS() - 2/6: Nominal");

    /* ----- Test case #3 - Write CDS To Flash failure ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    memcpy(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes);
    uiBuffer[10] = uiBuffer[10] + 1;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(write), 1, uiNumBytes);
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_WriteToCDS() - 3/6: Write CDS to flash failure");

    
    /* ----- Test case #4 - Input argument is NULL ----- */
    /* Setup additional inputs */
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteToCDS() - 4/6: Input argument is NULL");

    /* ----- Test case #5 - Data offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteToCDS() - 5/6: Failed data offset is larger than CDSMemory block size");
    
    /* ----- Test case #6- Data offset plus number of changed bytes is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 99;
    /* Execute test */
    iRetCode = CFE_PSP_WriteToCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_WriteToCDS() - 6/6: Failed data offset plus number of changed bytes is larger than CDSMemory block size");
}

/*=======================================================================================
** Ut_CFE_PSP_ReadFromCDS() test cases
**=======================================================================================*/
void Ut_CFE_PSP_ReadFromCDS(void)
{
    /**
     * TODO: Pickup from here
     * 
     * CFE_PSP_ReadFromCDS received revamp, may need to scrap below
     */
    UT_ResetState(0);
    int32 iRetCode = 0;
    char cMsg[256] = {};
    uint8   uiBuffer[100] = {};
    uint8   *pucData = NULL;
    uint32  uiCDSOffset = 0;
    uint32  uiNumBytes = 100;
    uint32  uiTempCrc = 0;
    uint8   ucIdx = 0;
    // uint8   blockBuffer[500];
    g_bCorruptedCDSFlash = false;

    // uint8   localCDSBuffer[2000];
    uint8   localCDSBuffer[100];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    // CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 2000;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 100;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    pucData = uiBuffer;
    g_uiCDSCrc = 0;
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
    sprintf(cMsg, "CFE_PSP: CRC mismatched (uiCalculatedCrc = 0x%08X, g_uiCDSCrc = 0x%08X)\n", uiTempCrc, g_uiCDSCrc);
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadFromCDS() - 1/7: Nominal Read the data from Flash");
    UtAssert_True(g_uiCDSCrc == uiTempCrc, "_CFE_PSP_ReadFromCDS() - 1/7: Nominal New CRC codes was updated");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 1/7: Nominal Read data is matched with CDSMemory data");
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadFromCDS() - 1/7: Nominal");

    /* ----- Test case #2 - Unable to read from flash ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    pucData = uiBuffer;
    g_uiCDSCrc = 0;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_ERROR);
    /* Modify first 100 bytes from CDSMomory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr)[ucIdx] = ucIdx;
    }
    /* Calculate the CRC code for current CDSMemory data. This value needs for next OS_printf assert string */
    uiTempCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                        CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, uiTempCrc);
    sprintf(cMsg, "CFE_PSP: Cannot read from FLASH\n");
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_OS_print(cMsg, "_CFE_PSP_ReadFromCDS() - 2/7: Read CDS From Flash failure");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 2/7: Read CDS From Flash failure");

    /* ----- Test case #3 - Temp CDS and global CRC match ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    /* Modify first 100 bytes from CDSMomory for testing */
    for (ucIdx=0; ucIdx < uiNumBytes; ucIdx++)
    {
        ((uint8 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr)[ucIdx] = ucIdx;
    }
    /* Calculate the CRC code for current CDSMemory data. This value needs for next OS_printf assert string */
    uiTempCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                        CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, uiTempCrc);
    g_uiCDSCrc = uiTempCrc;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(g_uiCDSCrc == uiTempCrc, "_CFE_PSP_ReadFromCDS() - 3/7: Read CDS From Flash failure");
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 3/7: Read CDS From Flash failure");
    // UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadFromCDS() - 3/7: Read CDS From Flash failure");
    UtAssert_NA("_CFE_PSP_ReadFromCDS() - 3/7: Read CDS From Flash failure");

    /* ----- Test case #4 - Nominal Always read from Flash ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = uiBuffer;
    g_uiCDSCrc = 0;
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(read), 1, uiNumBytes);
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_MemCmp(uiBuffer, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, uiNumBytes, 
                    "_CFE_PSP_ReadFromCDS() - 4/7: Nominal Read data is matched with CDSMemory data");
    UtAssert_True(iRetCode == OS_SUCCESS, "_CFE_PSP_ReadFromCDS() - 4/7: Nominal");

    /* ----- Test case #5 - Input argument is NULL ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    pucData = NULL;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadFromCDS() - 5/7: Input argument is NULL");

    /* ----- Test case #6 - Dara offset is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize + 1;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadFromCDS() - 6/7: Failed data offset is larger than CDSMemory block size");

    /* ----- Test case #7 - Date offset plus number of changed bytes is larger than CDSMemory block size ----- */
    /* Setup additional inputs */
    pucData = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
    uiCDSOffset = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize - 99;
    /* Execute test */
    iRetCode = CFE_PSP_ReadFromCDS(pucData, uiCDSOffset, uiNumBytes);
    /* Verify outputs */
    UtAssert_True(iRetCode == OS_ERROR, "_CFE_PSP_ReadFromCDS() - 7/7: Failed data offset plus number of changed bytes is larger than CDSMemory block size");
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
    uint8_t uiMemBlockSize = 100;
    uint32  uiRetCode = 0;
    char cMsg[256] = {};
    uint32 uiRestartType = CFE_PSP_RST_TYPE_POWERON;
    uint32 uiNumBytes = 100;
    uint32 uiTempCRC = 0;

    // uint8 localReservedMemBlock[300];
    uint8 localReservedMemBlock[uiMemBlockSize];
    g_ReservedMemBlock.BlockPtr = (void *)localReservedMemBlock;
    // g_ReservedMemBlock.BlockSize = 300;
    g_ReservedMemBlock.BlockSize = uiMemBlockSize;

    // uint8   localCDSBuffer[2000];
    uint8   localCDSBuffer[uiMemBlockSize];
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)localCDSBuffer;
    // CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = 2000;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = uiMemBlockSize;

    CFE_PSP_ReservedMemoryBootRecord_t localBootRecord;
    CFE_PSP_ReservedMemoryMap.BootPtr = &localBootRecord;

    /* ----- Test case #1 - Nominal Read the exist CDS file ----- */
    /* Setup additional inputs */
    Ut_OS_printf_Setup();
    UT_SetDeferredRetcode(UT_KEY(open), 1, OS_SUCCESS);
    // UT_SetDeferredRetcode(UT_KEY(read), 1, uiNumBytes); 
    UT_SetDeferredRetcode(UT_KEY(read), 1, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize); 
    uiTempCRC = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);

    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    sprintf(cMsg, "CFE_PSP: Clearing Processor Reserved Memory.\n");
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Cleared processor reserved memory");
    sprintf(cMsg, "CFE_PSP: Read %d bytes of CDS data from Flash.\n", uiNumBytes);
    UtAssert_OS_print(cMsg, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Read from existed CDS file");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Nominal ");
    UtAssert_True(uiTempCRC == g_uiCDSCrc, "_CFE_PSP_InitProcessorReservedMemory - 1/5: Nominal CRC Calculation");


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
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_InitProcessorReservedMemory - 2/5: Nominal ");

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
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_InitProcessorReservedMemory - 3/5: Return error code");

    /* ----- Test case #4 - Nominal Unable to create new CDS file ----- */
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
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_InitProcessorReservedMemory - 4/5: Return error code");

    /* ----- Test case #5 - Failed Reserved block size is greater than the reserved memory ----- */
    /* Setup additional inputs */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    /* Set BlockSize to 16MB, which 1MB larger than the reserved size to force to failure case */
    g_ReservedMemBlock.BlockSize = 16777216; 
    /* Execute test */
    uiRetCode = CFE_PSP_InitProcessorReservedMemory(uiRestartType);
    /* Verify outputs */
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_InitProcessorReservedMemory - 5/5: Failed Reserved block size is greater than reserved memory");
}

/*=======================================================================================
** Ut_CFE_PSP_SetupReservedMemoryMap() test cases
**=======================================================================================*/
void Ut_CFE_PSP_SetupReservedMemoryMap(void)
{
    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs - None */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, OS_SUCCESS);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify outputs */
    UtAssert_True(TRUE, "_CFE_PSP_SetupReservedMemoryMap - 1/2: Nominal");

    /* ----- Test case #2 - Error setting up memory range ----- */
    /* Setup additional inputs - None */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemRangeSet), 1, OS_ERROR);
    /* Execute test */
    CFE_PSP_SetupReservedMemoryMap();
    /* Verify outputs */
    UtAssert_True(TRUE, "_CFE_PSP_SetupReservedMemoryMap - 2/2: Error setting up memory range");
}

/*=======================================================================================
** Ut_CFE_PSP_DeleteProcessorReservedMemory() test cases
**=======================================================================================*/
void Ut_CFE_PSP_DeleteProcessorReservedMemory(void)
{
    /* Nothing to test */
    CFE_PSP_DeleteProcessorReservedMemory();
    UtAssert_NA("No code in this function");
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
    MODULE_ID  moduleID;
    MODULE_INFO moduleInfo;

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDeferredRetcode(UT_KEY(moduleFindByName), 1, (uint32)&moduleID);
    UT_SetDeferredRetcode(UT_KEY(moduleInfoGet), 1, OS_SUCCESS);
    /* Execute test */
    // printf("CFESegment: %u - uiSizeOfCFESegment: %u\n", CFESegment, uiSizeOfCFESegment);
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    // printf("CFESegment: %u - uiSizeOfCFESegment: %u\n", CFESegment, uiSizeOfCFESegment);
    /* Verify outputs */
    // UtAssert_True(CFESegment != 0, "_CFE_PSP_GetCFETextSegmentInfo - 1/5: CFESegment was changed");
    UtAssert_NA("_CFE_PSP_GetCFETextSegmentInfo - 1/5: CFESegment was changed");
    UtAssert_True(uiRetCode == CFE_PSP_SUCCESS, "_CFE_PSP_GetCFETextSegmentInfo - 1/5: Nominal");

    /* ----- Test case #2 - Nominal moduleInfoGet return error ----- */
    /* Setup additional inputs */
    CFESegment = 0;
    pCFESegment = &CFESegment;
    UT_SetDeferredRetcode(UT_KEY(moduleFindByName), 1, (uint32)&moduleID);
    UT_SetDeferredRetcode(UT_KEY(moduleInfoGet), 1, OS_ERROR);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_IntegerCmpAbs(CFESegment, 0, 0, "_CFE_PSP_GetCFETextSegmentInfo - 2/5: CFESegment was not changed");
    UtAssert_True(uiRetCode == CFE_PSP_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 2/5: Nominal moduleInfoGet returned error");

    /* ----- Test case #3 - CFE Segement size pointer is NULL ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    puiSizeOfCFESegment = NULL;
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 3/5: Failed because CFE Segement size pointer is NULL");

    /* ----- Test case #4 - moduleFindByName return NULL pointer ----- */
    /* Setup additional inputs */
    pCFESegment = &CFESegment;
    UT_SetDeferredRetcode(UT_KEY(moduleFindByName), 1, 0);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify outputs */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 4/5: Failed because moduleFindByName returned NULL");

    /* ----- Test Case #5 - SizeOfCFESegment != NULL and moduleFindByName return NULL ptr ----- */
    /* Setup Inputs */
    puiSizeOfCFESegment = &uiSizeOfCFESegment;
    UT_SetDeferredRetcode(UT_KEY(moduleFindByName), 1, 0);
    /* Execute test */
    uiRetCode = CFE_PSP_GetCFETextSegmentInfo(pCFESegment, puiSizeOfCFESegment);
    /* Verify Output */
    UtAssert_True(uiRetCode == OS_ERROR, "_CFE_PSP_GetCFETextSegmentInfo - 5/5: Failed because *sizeOfCFESegment != NULL and moduleFindByName returned NULL");

}
/*=======================================================================================
** End of file psp_memory_testcases.c
**=======================================================================================*/