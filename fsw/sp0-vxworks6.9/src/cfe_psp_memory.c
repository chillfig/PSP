/**
 ** \file cfe_psp_memory.c
 **
 ** \brief cFE PSP Memory related functions
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved. \n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that 
 ** agreement.
 **
 ** \par Description:
 ** This is the implementation of the cFE memory areas that have to be
 ** preserved, and the API that is designed to allow access to them.
 ** It also contains memory related routines to return the address of the
 ** kernel code used in the cFE checksum.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <moduleLib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <userReservedMem.h>
/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_memory.h"

#include <target_config.h>

/* Memory Scrubbing */
#include "mem_scrub.h"
#include "psp_mem_scrub.h"

/*
** Macro Definitions
*/

/** \brief Define cFE core loadable module name */
#define CFE_MODULE_NAME "cfe-core.o"

/*
**  External Declarations
*/
/** \brief External Kernel Function GetWrsKernelTextStart */
extern unsigned int GetWrsKernelTextStart(void);
/** \brief External Kernel Function GetWrsKernelTextEnd */
extern unsigned int GetWrsKernelTextEnd(void);

extern int32 CFE_PSP_SetTaskPrio(const char *tName, int32 tgtPrio);

/*
** Global variables
*/
/* Temporary solution which declared a CDS file to store all apps' CDS data 
   in. This solution is to take advance of BSP's Datalight File System. This
   file system will support read/write files to Flash at location from 
   128MB to 1GB.
*/

/**
 ** \brief CDS File name in File System
 ** \par Description:
 ** Fully qualified path of where the CDS file will be stored.
 */
char g_cCDSFilename[10] = CFE_PSP_CFE_FLASH_FILEPATH;

/* Set the default CDS reading method. It assumes the reserved CDS area on RAM is always correct */
/** \brief CDS Read Method */
uint8 g_uiCDSReadMethod = CFE_PSP_CDS_READ_METHOD_DEFAULT;

/* This static variable will store the calculated CRC for previous CDS changed data */
/** \brief CRC value of CDS content */
static uint32 g_uiCDSCrc = 0;

/** \brief Task Priority of Memory Scrubbing Task */
static osal_priority_t g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;


/**
 ** \brief Contains the Active Memory Scrubbing Task ID
 ** \par Description:
 ** If 0, task is not running
 */
static uint32 g_uiMemScrubTask_id = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Start Address
 ** \par Description:
 ** The start address can be anything in the address space.
 */
static uint32 g_uiMemScrubStartAddr = 0;

/**
 ** \brief Contains the Active Memory Scrubbing End Address
 ** \par Description:
 ** End Address cannot be larger than the maximum RAM
 */
static uint32 g_uiMemScrubEndAddr = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Current Page
 ** \par Description:
 ** Current page that the task is working on. This value gets
 ** reset whenever task restart.
 */
static uint32 g_uiMemScrubCurrentPage = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Total Pages
 ** \par Description:
 ** Total number of pages processed since the start of the task. This value gets
 ** reset whenever task restart.
 */
static uint32 g_uiMemScrubTotalPages = 0;

/**
 ** \brief Contains the address of the end of RAM
 ** \par Description:
 ** This variable is filled out once during boot and never changed again. Its
 ** value reflects the amount of RAM of the system. When moving cFS from SP0 to
 ** SP0-s, the value changes automatically. Value is also used for checking for
 ** out of range addresses.
 ** 
 */
static uint32 g_uiEndOfRam = 0;

/**
 ** \brief Pointer to the vxWorks USER_RESERVED_MEMORY area
 ** \par Description:
 ** The sizes of each memory area is defined in os_processor.h for this architecture.
 */
CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap; //UndCC_Line(SSET056)

/** \brief Pointer to the reserved memory block */
CFE_PSP_MemoryBlock_t g_ReservedMemBlock;

/*
*********************************************************************************
** CDS related functions
*********************************************************************************
*/

/**
** \func Get the size of the Critical Data Store memory area
**
** \par Description:
** This function fetches the size of the OS Critical Data Store memory area.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] SizeOfCDS - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetCDSSize(uint32 *SizeOfCDS)
{
    int32 return_code = CFE_PSP_ERROR;

    if (SizeOfCDS != NULL)
    {
        *SizeOfCDS =  CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
        return_code = CFE_PSP_SUCCESS;
    }
    return(return_code);
}

/**
** \func Set the CDS reading method
**
** \par Description:
** This function set the CDS reading method(use CRC, always read from Flash,
** or trust the CDS reserved memory in RAM is correct.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] ucMethod - Reading method
**
** \return None
*/
void CFE_PSP_SetReadCDSMethod(uint8 ucMethod)
{
    g_uiCDSReadMethod = ucMethod;
}

/**
** \func Get the CDS reading method
**
** \par Description:
** This function get the CDS reading method(use CRC, always read from Flash,
** or trust the CDS reserved memory in RAM is correct.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return #CFE_PSP_CDS_READ_METHOD_DEFAULT - Trust the CDS data on RAM (no CRC or read from Flash)
** \return #CFE_PSP_CDS_READ_METHOD_CRC - Check the CRC first then read from Flash if CRC mis-matched
** \return #CFE_PSP_CDS_READ_METHOD_FLASH - Always read from Flash
*/
uint8 CFE_PSP_GetReadCDSMethod()
{
    return g_uiCDSReadMethod;
}

/**
** \func Change the previous calculated CRC value to new provided value
**
** \par Description:
** This function change the previous calculated CRC value to new provided value.
** This function is just for testing purpose by forcing the CRC mismatched and 
** read CDS data from Flash.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param uiNewCRC - New CRC
**
** \return None
*/
void CFE_PSP_SetStaticCRC(uint32 uiNewCRC)
{
    g_uiCDSCrc = uiNewCRC;
}

/**
** \func Get the previous calculated CRC value
**
** \par Description:
** None
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return Calculated CRC value
*/
uint32 CFE_PSP_GetStaticCRC(void)
{
    return g_uiCDSCrc;
}

/**
 ** \func Calculate 16 bits CRC from input data
 **
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** InputCRC allows the user to calculate the CRC of non-contiguous blocks as a
 ** single value. Nominally, the user should set this value to zero.
 **
 ** CFE now includes a function to calculate the CRC.
 ** uint32 CFE_ES_CalculateCRC(void *pData, uint32 DataLength, uint32 InputCRC, uint32 TypeCRC);
 ** Only CFE_MISSION_ES_CRC_16 is implemented as the TypeCRC
 **
 ** \param[inout] DataPtr  - Pointer to the input data buffer
 ** \param[inout] DataLength  - Data buffer length
 ** \param[inout] InputCRC  - A starting value for use in the CRC calculation.
 **
 ** \return Calculated CRC value
 */
uint32 CFE_PSP_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC)
{
    uint32      i = 0;
    int16       Index = 0;
    int16       Crc = 0;
    const uint8 *pBufPtr = NULL;
    uint8       ByteValue = 0;

    static const uint16 CrcTable[256] = 
    {
        0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
        0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
        0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
        0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
        0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
        0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
        0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
        0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
        0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
        0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
        0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
        0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
        0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
        0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
        0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
        0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
        0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
        0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
        0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
        0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
        0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
        0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
        0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
        0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
        0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
        0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
        0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
        0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
        0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
        0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
        0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
        0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040

    };

    Crc =  (int16)(0xFFFF & InputCRC);
    pBufPtr = (const uint8 *) DataPtr;

    for (i = 0; i < DataLength; i++)
    {
        /*
        * It is assumed that the supplied buffer is in a
        * directly-accessible memory space that does not
        * require special logic to access
        */
        ByteValue = *pBufPtr;
        Index = (int16) (( Crc ^ ByteValue) & 0x00FF);
        Crc = (int16) (((Crc >> 8 ) & 0x00FF) ^ CrcTable[Index]);
        
        pBufPtr++;
    }
     
    return (uint32) (Crc);

} /* End of CFE_PSP_CalculateCRC() */

/**
** \func Read the whole CDS data from Flash
**
** \par Description:
** This function read the whole CDS data on Flash to reserved memory on RAM.
**
** \warning It took about 117ms to read 131072 bytes (128KB) whole CDS area from Flash. 
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param puiReadBytes - Number of read bytes
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_ReadCDSFromFlash(uint32 *puiReadBytes)
{
    int32   iCDSFd = -1;
    int32   iReturnCode = CFE_PSP_SUCCESS;
    ssize_t readBytes = 0;
    #if defined(PRINT_DEBUG)
        OS_time_t localTime;
    #endif
    
    /* Return error if the output parameter wasn't allocated */
    if(puiReadBytes != NULL)
    {
        /* Open the CDS file for read */
        iCDSFd = open(g_cCDSFilename, O_RDONLY, 0);
        if(iCDSFd < 0)
        {
            OS_printf("CFE_PSP: Failed to open the CDS file.\n");
            iReturnCode = CFE_PSP_ERROR;
        }
        else
        {
            #if defined(PRINT_DEBUG)
            CFE_PSP_GetTime(&localTime);
            OS_printf("CFE_PSP: Time before read (Second: %d, microSecond: %d)\n", 
                    localTime.seconds, localTime.microsecs);
            #endif
            readBytes = read(iCDSFd, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                             CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
            #if defined(PRINT_DEBUG)
            CFE_PSP_GetTime(&localTime);
            OS_printf("CFE_PSP: Time after read (Second: %d, microSecond: %d)\n", 
                    localTime.seconds, localTime.microsecs);
            #endif
            if(readBytes < 0)
            {
                OS_printf("CFE_PSP: Failed to read the CDS data on Flash.\n");

                /* Set the output parameter to 0 for read failed */
                *puiReadBytes = 0;
                /* Return error if read failed */
                iReturnCode = CFE_PSP_ERROR;
            }
            else
            {
                *puiReadBytes = (uint32) readBytes;
            }
            /* Close the CDS file after opened and read */
            close(iCDSFd);
        }
    }

    return iReturnCode;
}

/**
** \func Write the whole CDS data on Flash
**
** \par Description:
** This function write the whole CDS data from reserved memory on RAM to Flash.
**
** \par Assumptions, External Events, and Notes:
** It took about 117ms to write 131072 bytes (128KB) whole CDS data to Flash.
**
** \param puiWroteBytes - Number of written bytes
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_WriteCDSToFlash(uint32 *puiWroteBytes)
{
    int32   iCDSFd = -1;
    int32   iReturnCode = CFE_PSP_SUCCESS;
    ssize_t wroteBytes = 0;
    #if defined(PRINT_DEBUG)
        OS_time_t localTime;
    #endif
    
    /* Return error if the output parameter wasn't allocated */
    if(puiWroteBytes == NULL)
    {
        iReturnCode = CFE_PSP_ERROR;
    }
    else
    {
        /* Open the CDS file for read */
        iCDSFd = open(g_cCDSFilename, O_WRONLY, 0);
        if(iCDSFd < 0)
        {
            OS_printf("CFE_PSP: Failed to open the CDS file.\n");
            iReturnCode = CFE_PSP_ERROR;
        }
        else
        {
            #if defined(PRINT_DEBUG)
            CFE_PSP_GetTime(&localTime);
            OS_printf("CFE_PSP: Time before write (Second: %d, microSecond: %d)\n", 
                    localTime.seconds, localTime.microsecs);
            #endif
            wroteBytes = write(iCDSFd, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                               CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
            #if defined(PRINT_DEBUG)
            CFE_PSP_GetTime(&localTime);
            OS_printf("CFE_PSP: Time after write (Second: %d, microSecond: %d)\n", 
                    localTime.seconds, localTime.microsecs);
            #endif
            if(wroteBytes < 0)
            {
                OS_printf("CFE_PSP: Failed to write the CDS data to Flash.\n");

                /* Set the output parameter to 0 for write failed */
                *puiWroteBytes = 0;
                /* Return error if write failed */
                iReturnCode = CFE_PSP_ERROR;
            }
            else
            {
                *puiWroteBytes = (uint32) wroteBytes;
            }
            /* Close the CDS file after opened and write */
            close(iCDSFd);
        }
    }

    return iReturnCode;
}

/**
** \func Write to the Critical Data Store memory area
**
** \par Description:
** This function write the specified data to the specified memory area of the CDS.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] PtrToDataToWrite - Pointer to the data buffer to be written
** \param[in] CDSOffset - Memory offset from the beginning of the CDS block
** \param[in] NumBytes - Number of bytes to be written
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_WriteToCDS(const void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes)
{
    uint8 *pCopyPtr = NULL;
    int32  return_code = CFE_PSP_ERROR;
    int32  iWroteBytes = 0; 

    if (PtrToDataToWrite != NULL)
    {
        if ((CDSOffset >= CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize) ||
            ((CDSOffset + NumBytes) > CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize))
        {
            pCopyPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
            pCopyPtr += CDSOffset;

            /* Compare the original and user changed data. If not changes,
            no overwriting CDS block on memory and no updating Flash */
            if(memcmp((char *)pCopyPtr, (char *)PtrToDataToWrite, NumBytes) == 0)
            {
                return_code = CFE_PSP_SUCCESS;
            }
            else
            {
                /* Overwrite CFS block */
                memcpy(pCopyPtr, (char *)PtrToDataToWrite, NumBytes);

                /* Write whole CDS memory to Flash */
                return_code = CFE_PSP_WriteCDSToFlash(&iWroteBytes);
                if(return_code == CFE_PSP_SUCCESS)
                {
                    OS_printf("CFE_PSP: Wrote %d to CDS file.\n", iWroteBytes);
                    /* Calculate the CRC for whole CDS memory and write to static variable 
                    after updated new CDS data on Flash */
                    g_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                                      CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
                }
            }
        }
    } /* end if PtrToDataToWrite != NULL */

    return(return_code);
}

/**
** \func Read from the Critical Data Store memory area
**
** \par Description:
** This function reads from the CDS memory area.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToDataToRead - Pointer to the data buffer that stores the read data
** \param[in] CDSOffset - Memory offset from the beginning of the CDS block
** \param[in] NumBytes - Number of bytes to be read
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_ReadFromCDS(void *PtrToDataToRead, uint32 CDSOffset, uint32 NumBytes)
{
    uint8   *pCopyPtr = NULL;
    int32   return_code = CFE_PSP_ERROR;
    uint32  uiTempCrc = 0;
    uint32  uiReadBytes = 0;
    
    if (PtrToDataToRead != NULL)
    {
        if ((CDSOffset < CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize) &&
            ((CDSOffset + NumBytes) <= CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize))
        {
            /* In the CRC mode */
            if(g_uiCDSReadMethod == CFE_PSP_CDS_READ_METHOD_CRC)
            {
                /* Calculate the CRC for whole CDS memory */
                uiTempCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                                 CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, uiTempCrc);

                /* CRC isn't match; Re-read CDS data from Flash to memory */
                if(uiTempCrc != g_uiCDSCrc)
                {
                    OS_printf("CFE_PSP: CRC mismatched (uiTempCrc = 0x%08X, g_uiCDSCrc = 0x%08X)\n", 
                              uiTempCrc, 
                              g_uiCDSCrc);
                    return_code = CFE_PSP_ReadCDSFromFlash(&uiReadBytes);
                    if(return_code == CFE_PSP_SUCCESS)
                    {
                        /* Update the static CRC variable with new read CDS data from Flash */
                        g_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                                          CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
                    }
                }
            }
            /* In the always read from Flash mode */
            if(g_uiCDSReadMethod == CFE_PSP_CDS_READ_METHOD_FLASH) 
            {
                return_code = CFE_PSP_ReadCDSFromFlash(&uiReadBytes);
                if(return_code == CFE_PSP_SUCCESS)
                {
                    /* Update the static CRC variable with new read CDS data from Flash */
                    g_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                                        CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
                }
            }

            /* Only copy the CDS data to user prefered pointer when the CDS memory in RAM is correct */
            if((g_uiCDSReadMethod == CFE_PSP_CDS_READ_METHOD_DEFAULT) || (return_code == CFE_PSP_SUCCESS))
            {
                pCopyPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
                pCopyPtr += CDSOffset;
                memcpy((char *)PtrToDataToRead,pCopyPtr, NumBytes);
                return_code = CFE_PSP_SUCCESS;
            }
        }

    } /* end if PtrToDataToWrite != NULL */

    return(return_code);
}

/*
*********************************************************************************
** ES Reset Area related functions
*********************************************************************************
*/

/**
** \func Get the location and size of the ES Reset memory area
**
** \par Description:
** This function returns the location and size of the ES Reset memory area. This area 
** is preserved during a processor reset and is used to store the ER Log, System Log 
** and reset related variables.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToResetArea - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfResetArea - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetResetArea (cpuaddr *PtrToResetArea, uint32 *SizeOfResetArea)
{
    int32   return_code = CFE_PSP_ERROR;

    if ((PtrToResetArea != NULL) && (SizeOfResetArea != NULL))
    {
        *PtrToResetArea = (cpuaddr)(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr);
        *SizeOfResetArea = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
        return_code = CFE_PSP_SUCCESS;
    }

    return(return_code);
}

/*
*********************************************************************************
** ES User Reserved Area related functions
*********************************************************************************
*/

/**
** \func Get the location and size of the cFE user-reserved memory area
**
** \par Description:
** This function returns the location and size of the cFE user-reserved memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToUserArea - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfUserArea - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetUserReservedArea(cpuaddr *PtrToUserArea, uint32 *SizeOfUserArea )
{
    int32   return_code = CFE_PSP_ERROR;

    if ((PtrToUserArea == NULL) || (SizeOfUserArea == NULL))
    {
        return_code = CFE_PSP_ERROR;
    }
    else
    {
        *PtrToUserArea = (cpuaddr)(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr);
        *SizeOfUserArea = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
        return_code = CFE_PSP_SUCCESS;
    }

    return(return_code);
    }

/*
*********************************************************************************
** ES Volatile disk memory related functions
*********************************************************************************
*/

/**
** \func Get the location and size of the cFE volatile memory area
**
** \par Description:
** This function returns the location and size of the cFE volatile memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToVolDisk - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfVolDisk - Pointer to the variable that stores the returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetVolatileDiskMem(cpuaddr *PtrToVolDisk, uint32 *SizeOfVolDisk )
{
    int32   return_code = CFE_PSP_ERROR;

    if ((PtrToVolDisk == NULL) || (SizeOfVolDisk == NULL))
    {
        return_code = CFE_PSP_ERROR;
    }
    else
    {
        *PtrToVolDisk = (cpuaddr)(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr);
        *SizeOfVolDisk = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
        return_code = CFE_PSP_SUCCESS;
    }

    return(return_code);
}


/*
*********************************************************************************
** ES BSP Top Level Reserved memory initialization
*********************************************************************************
*/

/**
 ** \func Initialize the processor's reserved memory
 **
 ** \par Description:
 ** This function initializes all of the memory in the BSP that is preserved on 
 ** a processor reset. The memory includes the Critical Data Store, the ES Reset Area,
 ** the Volatile Disk Memory and the User Reserved Memory.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This initializes based on the reset type.  Typically, the information
 ** is preserved on a processor reset, and cleared/reinitialized on a power-on
 ** reset.
 **
 ** \param[in] RestartType - The reset type
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_InitProcessorReservedMemory( uint32 RestartType )
{
    cpuaddr   start_addr = 0;
    uint32    reserve_memory_size = 0;
    int32     return_code = CFE_PSP_SUCCESS;
    int32     iCDSFd = -1;
    ssize_t   readBytes = 0;  
    
    /* 
    Returns the address and size of the user-reserved region
    This is a Kernel function
     */
    userReservedGet((char **) &start_addr, &reserve_memory_size);

    /* If the VxWorks reserved memory size is smaller than the requested, print error */
    if(g_ReservedMemBlock.BlockSize > reserve_memory_size)
    {
        OS_printf("CFE_PSP: VxWorks Reserved Memory Block Size not large enough, "
                  "Total Size = 0x%lx, "
                  "VxWorks Reserved Size=0x%lx\n",
                  (unsigned long)g_ReservedMemBlock.BlockSize,
                  (unsigned long)reserve_memory_size);
        return_code = CFE_PSP_ERROR;

    }
    else if (RestartType != CFE_PSP_RST_TYPE_PROCESSOR)
    {
        OS_printf("CFE_PSP: Clearing Processor Reserved Memory.\n");
        memset(g_ReservedMemBlock.BlockPtr, 0, g_ReservedMemBlock.BlockSize);

        /*
        ** Set the default reset type in case a watchdog reset occurs
        */
        CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;
    }
    /**** CDS File ****/
    if(return_code == CFE_PSP_SUCCESS)
    {
        /* Open the CDS file */
        iCDSFd = open(g_cCDSFilename, O_RDONLY, 0);
        /* If the file was not opened, we need to create it */
        if(iCDSFd < 0)
        {
            /* If CDS file cannot be created, print error */
            if(creat(g_cCDSFilename, S_IRWXU) < 0)
            {
                OS_printf("CFE_PSP: Failed to create the CDS file(%s) on Flash.\n", 
                          g_cCDSFilename);
                return_code = CFE_PSP_ERROR;
            }
            /* If CDS file was create, print success message and close CDS file */
            else
            {
                OS_printf("CFE_PSP: Created the CDS file.\n");
                /* Close the CDS file after created */
                close(iCDSFd);
            }
        }
        /* If the CDS file was opened at first attempt, read content */
        else
        {
            readBytes = read(iCDSFd, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                            CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
            if(readBytes >= 0)
            {
                OS_printf("CFE_PSP: Read %d bytes of CDS data from Flash.\n", readBytes);  
                /* Calculate the CRC for whole CDS memory and write to static variable after read CDS from Flash */
                g_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                                  CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
            }
            else
            {
                OS_printf("CFE_PSP: Failed to read the CDS data on Flash.\n"); 
                /* Return error if read failed */
                return_code = CFE_PSP_ERROR;
            }
            /* Close the CDS file after opened and read */
            close(iCDSFd);
        }
    }

   return(return_code);
}

/**
 ** \func Initialize the CFE_PSP_ReservedMemoryMap global object
 **
 ** \par Description:
 ** This function initializes the CFE_PSP_ReservedMemoryMap global object.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function must be called by the startup code before the map is accessed.
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_SetupReservedMemoryMap(void)
{
    cpuaddr   start_addr = 0;
    cpuaddr   end_addr = 0;
    uint32    reserve_memory_size = 0;
    int32     status = OS_SUCCESS;

    /*
    ** Setup the pointer to the reserved area in vxWorks.
    ** This must be done before any of the reset variables are used.
    */
    /*
    ** Note: this uses a "cpuaddr" (integer address) as an intermediate
    ** to avoid warnings about alignment.  The output of userReservedGet()
    ** should be aligned to hold any data type, being the very start
    ** of the memory space.
    */
    
    /* 
    Returns the address and size of the user-reserved region
    This is a Kernel function
    */
    userReservedGet((char **) &start_addr, &reserve_memory_size);

    end_addr = start_addr;

    g_uiEndOfRam = (uint32)sysPhysMemTop();

    memset(&CFE_PSP_ReservedMemoryMap, 0, sizeof(CFE_PSP_ReservedMemoryMap));

    CFE_PSP_ReservedMemoryMap.BootPtr = (CFE_PSP_ReservedMemoryBootRecord_t *)end_addr;
    end_addr += sizeof(CFE_PSP_ReservedMemoryBootRecord_t);
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & (~CFE_PSP_MEMALIGN_MASK);

    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr = (CFE_PSP_ExceptionStorage_t *)end_addr;
    end_addr += sizeof(CFE_PSP_ExceptionStorage_t);
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & (~CFE_PSP_MEMALIGN_MASK);

    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void *) end_addr;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->ResetAreaSize;
    end_addr += CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & (~CFE_PSP_MEMALIGN_MASK);

    OS_printf("CFE_PSP: Reset Memory Block at 0x%08lx, Total Size = 0x%lx\n",
              (unsigned long)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
              (unsigned long)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);

    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = (void *) end_addr;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize =
        GLOBAL_CONFIGDATA.CfeConfig->RamDiskSectorSize * GLOBAL_CONFIGDATA.CfeConfig->RamDiskTotalSectors;
    end_addr += CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & (~CFE_PSP_MEMALIGN_MASK);
    
    OS_printf("CFE_PSP: Volatile Disk Memory Block at 0x%08lx, Total Size = 0x%lx\n",
              (unsigned long)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
              (unsigned long)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);

    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void *) end_addr;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->CdsSize;
    end_addr += CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & (~CFE_PSP_MEMALIGN_MASK);

    OS_printf("CFE_PSP: CDS Memory Block at 0x%08lx, Total Size = 0x%lx\n",
              (unsigned long)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
              (unsigned long)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);

    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = (void *) end_addr;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->UserReservedSize;
    end_addr += CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & (~CFE_PSP_MEMALIGN_MASK);

    /* The total size of the entire block is the difference in address */
    g_ReservedMemBlock.BlockPtr = (void *) start_addr;
    g_ReservedMemBlock.BlockSize =  end_addr - start_addr;

    OS_printf("CFE_PSP: Reserved Memory Block at 0x%08lx with size 0x%lx, Total VxWorks Reserved Size=0x%lx\n",
              (unsigned long)g_ReservedMemBlock.BlockPtr,
              (unsigned long)g_ReservedMemBlock.BlockSize,
              (unsigned long)reserve_memory_size);

    /*
     * Set up the "RAM" entry in the memory table.
     */
    status = CFE_PSP_MemRangeSet(
            0, 
            CFE_PSP_MEM_RAM, 
            0, 
            g_uiEndOfRam, 
            CFE_PSP_MEM_SIZE_DWORD, 
            CFE_PSP_MEM_ATTR_READWRITE
    );
    if (status != OS_SUCCESS)
    {
        OS_printf("CFE_PSP_MemRangeSet returned error\n");
    }
}

/**
 ** \func Delete the processor's reserved memory
 **
 ** \par Description:
 ** This function unlinks the memory segments within the CFE_PSP_ReservedMemoryMap
 ** global object.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is only relevant on systems where the objects are implemented
 ** as kernel shared memory segments.  The segments will be marked for deletion
 ** but the local maps remain usable until the process ends.
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_DeleteProcessorReservedMemory(void)
{
}

/*
*********************************************************************************
** ES BSP kernel memory segment functions
*********************************************************************************
*/

/**
** \func Get the location and size of the kernel text segment
**
** \par Description:
** This function returns the location and size of the kernel text segment of the memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToKernelSegment - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfKernelSegment - Pointer to the variable that stores returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetKernelTextSegmentInfo(cpuaddr *PtrToKernelSegment, uint32 *SizeOfKernelSegment)
{
    int32    return_code = CFE_PSP_ERROR;
    cpuaddr  StartAddress = 0;
    cpuaddr  EndAddress = 0;

    if (SizeOfKernelSegment == NULL)
    {
        return_code = CFE_PSP_ERROR;
    }
    else
    {
        /*
        ** Get the kernel start and end
        ** addresses from the BSP, because the
        ** symbol table does not contain the symbls we need for this
        */
        StartAddress = (cpuaddr) GetWrsKernelTextStart();
        EndAddress = (cpuaddr) GetWrsKernelTextEnd();

        *PtrToKernelSegment = StartAddress;
        *SizeOfKernelSegment = (uint32) (EndAddress - StartAddress);

        return_code = CFE_PSP_SUCCESS;
    }

    return(return_code);
}

/**
** \func Get the location and size of the cFE text segment
**
** \par Description:
** This function returns the location and size of the cFE text segment of the memory area.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] PtrToCFESegment - Pointer to the variable that stores the returned memory address
** \param[out] SizeOfCFESegment - Pointer to the variable that stores returned memory size
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_GetCFETextSegmentInfo(cpuaddr *PtrToCFESegment, uint32 *SizeOfCFESegment)
{
    int32       return_code = CFE_PSP_ERROR;
    STATUS      status = CFE_PSP_ERROR;
    MODULE_ID   cFEModuleId = NULL;
    MODULE_INFO cFEModuleInfo = {};

    if (SizeOfCFESegment == NULL)
    {
        return_code = CFE_PSP_ERROR;
    }
    else
    {
        cFEModuleId = moduleFindByName(CFE_MODULE_NAME);

        if (cFEModuleId == NULL)
        {
            return_code = CFE_PSP_ERROR;
        }
        else
        {
            status = moduleInfoGet(cFEModuleId, &cFEModuleInfo);
            if (status != CFE_PSP_ERROR)
            {
                *PtrToCFESegment = (cpuaddr)(cFEModuleInfo.segInfo.textAddr);
                *SizeOfCFESegment = (uint32) (cFEModuleInfo.segInfo.textSize);
                return_code = CFE_PSP_SUCCESS;
            }
            else
            {
                return_code = CFE_PSP_SUCCESS;
            }
        }
    }

    return(return_code);
}

/******** PSP Active Memory Scrubbing ********/

/**
** \func Set the Memory Scrubbing parameters
** 
** \par Description:
** This functions set the memory scrubbing parameters.
**
** \par Assumptions, External Events, and Notes:
** After calling this function, the new settings will be applied in the 
** next call to the Activate Memory Scrubbing funtion.
** If newEndAddr is set to a value larger than the actual physical memory limit,
** the function will use the phyisical memory limit.
** Task priority can only be set between #MEMSCRUB_PRIORITY_UP_RANGE and 
** #MEMSCRUB_PRIORITY_DOWN_RANGE defined in cfe_psp_config.h. 
** Default is set to #MEMSCRUB_DEFAULT_PRIORITY.
** 
** \param[in] newStartAddr - Memory address to start from, usually zero
** \param[in] newEndAddr - Memory address to end at, usually end of the physical RAM
** \param[in] task_priority - The task priority
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Set(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority)
{
    /* If top of memory has not been initialized, then initialize it */
    if (g_uiEndOfRam == 0)
    {
        g_uiEndOfRam = (uint32) sysPhysMemTop();
    }

    /* Verify Start and End Addresses */
    g_uiMemScrubStartAddr = newStartAddr;
    if (newEndAddr > (g_uiEndOfRam - 1))
    {
        g_uiMemScrubEndAddr = g_uiEndOfRam - 1;
    }
    else
    {
        g_uiMemScrubEndAddr = newEndAddr;
    }

    /*
    If the task priority is different from the currently set value,
    then change it as long as it is within the allowed range
    */
    if (task_priority != g_uiMemScrubTaskPriority)
    {
        /* If the Task Priority is set outside the range, use default range and 
        report error */
        if ((task_priority > MEMSCRUB_PRIORITY_UP_RANGE) || (task_priority < MEMSCRUB_PRIORITY_DOWN_RANGE))
        {
            /* Apply default priority */
            g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
            OS_printf("PSP MEM SCRUB: Priority is outside range, using default `%u`\n",g_uiMemScrubTaskPriority);
        }
        else
        {
            /* Apply new priority */
            g_uiMemScrubTaskPriority = task_priority;
        }

        CFE_PSP_SetTaskPrio(MEMSCRUB_TASK_NAME, g_uiMemScrubTaskPriority);
    }
} /* end CFE_PSP_MEM_SCRUB_Set */

/**
** \func Stop the memory scrubbing task
**
** \par Description:
** This function deletes the Memory Scrubbing task.  
** The task is deleted and the statistics are reset.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param - None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Delete(void)
{
    if (OS_TaskDelete(g_uiMemScrubTask_id) == OS_SUCCESS)
    {
        OS_printf("PSP MEM SCRUB: Memory Scrubbing Task Deleted\n");
    }
    /* Reset task stats and ID */
    g_uiMemScrubTotalPages = 0;
    g_uiMemScrubTask_id = 0;
}

/**
** \func Print the Memory Scrubbing statistics
** 
** \par Description:
** This function outputs to the console the following Memory Scrubbing statistics:
** Start memory address, End memory address, current memory page and total memory pages
**
** \par Assumptions, External Events, and Notes:
** Start memory address is usually 0. End memory address is usually set to the 
** last value of RAM address. Note that a page is 4098 bytes.
**
** \param None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Status(void)
{
    if (g_uiMemScrubTask_id > 0)
    {
        OS_printf("PSP MEM SCRUB: MemScrub -> StartAdr: 0x%08X - EndAdr: 0x%08X - CurrentPages: %u - TotalPages: %u\n",
                g_uiMemScrubStartAddr,
                g_uiMemScrubEndAddr,
                g_uiMemScrubCurrentPage,
                g_uiMemScrubTotalPages
                );
    }
    else
    {
        OS_printf("Active Memory Scrubbing task could not be found\n");
    }
}

/**
** \func Main function for the Memory Scrubbing task
**
** \par Description:
** This is the main function for the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** The scrubMemory function implemented by AiTech may never return an error.
**
** \param None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Task(void)
{
    STATUS status = CFE_PSP_ERROR;

    if (g_uiMemScrubEndAddr <= g_uiEndOfRam)
    {
        while(1)
        {
            /* Call the active memory scrubbing function */
            status = scrubMemory(g_uiMemScrubStartAddr, g_uiMemScrubEndAddr, &g_uiMemScrubCurrentPage);

            if (status == CFE_PSP_ERROR)
            {
                OS_printf("PSP MEM SCRUB: Unexpected ERROR during scrubMemory call\n");
            }
            else
            {
                g_uiMemScrubTotalPages += g_uiMemScrubCurrentPage;
            }
        }
    }
}

/**
** \func Initialize the Memory Scrubbing task
**
** \par Description:
** This function starts the Memory Scrubbing task as a child thread.
**
** \par Assumptions, External Events, and Notes:
** The scrubMemory function implemented by AiTech may never return an error.
**
** \param None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Init(void)
{
    /* Initialize */
    int32 status = CFE_PSP_ERROR;

    /* If task is not running, create task */
    if (CFE_PSP_MEM_SCRUB_isRunning() == false)
    {
        /* Set the scrub settings to default */
        CFE_PSP_MEM_SCRUB_Set(0, g_uiEndOfRam, g_uiMemScrubTaskPriority);

        OS_printf("PSP MEM SCRUB: Starting Active Memory Scrubbing\n");

        status = OS_TaskCreate(&g_uiMemScrubTask_id, 
                                MEMSCRUB_TASK_NAME, 
                                CFE_PSP_MEM_SCRUB_Task,
                                OSAL_TASK_STACK_ALLOCATE, 
                                OSAL_SIZE_C(1024), 
                                g_uiMemScrubTaskPriority, 
                                0
                                );
                                
        if (status != OS_SUCCESS)
        {
            OS_printf("PSP MEM SCRUB: Error creating PSPMemScrub\n");
        }
    }
    else
    {
        /* If task is already runnning, notify user */
        OS_printf("PSP MEM SCRUB: Task already running\n");
    }
}

/**
** \func Check if the Memory Scrubbing task is running
** 
** \par Description:
** This function provides the status whether the Memory Scrubbing task is running.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param - None
**
** \return true - If task is running
** \return false - If task is not running
*/
bool CFE_PSP_MEM_SCRUB_isRunning(void)
{
    /* Initialize */
    bool        ret = true;
    int32       status = OS_SUCCESS;
    osal_id_t   mem_scurb_id = 0;

    status = OS_TaskGetIdByName(&mem_scurb_id, MEMSCRUB_TASK_NAME);

    if (status == OS_ERR_NAME_NOT_FOUND)
    {
        ret = false;
    }

    return ret;
}


/**
** \func Enable the Memory Scrubbing task
**
** \par Description:
** This function enables the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** If the task is already running, do nothing. If the task is not running,
** then start it.
**
** \param None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Enable(void)
{
    /* If task is not running, initilize it */
    if (g_uiMemScrubTask_id > 0)
    {
        OS_printf("Active Memory Scrubbing task is already running\n");
    }
    else
    {
        CFE_PSP_MEM_SCRUB_Init();
    }
}

/**
** \func Disable the Memory Scrubbing task
**
** \par Description:
** This function disables the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** If the task is already running, delete it. If the task is not running,
** then do nothing.
**
** \param None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Disable(void)
{
    /* If task is running, delete it */
    if (g_uiMemScrubTask_id > 0)
    {
        CFE_PSP_MEM_SCRUB_Delete();
    }
    else
    {
        OS_printf("Active Memory Scrubbing task could not be found\n");
    }
}
