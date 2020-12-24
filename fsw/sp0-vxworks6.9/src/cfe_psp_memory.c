/******************************************************************************
** File:  cfe_psp_memory.c
**
**      Copyright (c) 2004-2011, United States Government as represented by
**      Administrator for The National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   cFE PSP Memory related functions
**
**   This is the implementation of the cFE memory areas that have to be
**   preserved, and the API that is designed to allow access to them.
**   It also contains memory related routines to return the address of the
**   kernel code used in the cFE checksum.
**
******************************************************************************/

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

/*
** Macro Definitions
*/

/* Define cFE core loadable module name*/
#define CFE_MODULE_NAME "cfe-core.o"

/*
**  External Declarations
*/
extern unsigned int GetWrsKernelTextStart(void);
extern unsigned int GetWrsKernelTextEnd(void);


/*
** Global variables
*/
/* Temporary solution which declared a CDS file to store all apps' CDS data 
   in. This solution is to take advance of BSP's Datalight File System. This
   file system will support read/write files to Flash at location from 
   128MB to 1GB.
*/
/* CDS File name */
char g_cCDSFilename[10] = "/ffx0/CDS";

/* Set the default CDS reading method. It assumes the reserved CDS area on RAM is always correct */
uint8 g_CDSReadMethod = CFE_PSP_CDS_READ_METHOD_DEFAULT;

/* This static variable will store the calculated CRC for previous CDS changed data */
static uint32  sg_uiCDSCrc = 0;

/*
** Pointer to the vxWorks USER_RESERVED_MEMORY area
** The sizes of each memory area is defined in os_processor.h for this architecture.
*/
CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap;

CFE_PSP_MemoryBlock_t PSP_ReservedMemBlock;
/*
*********************************************************************************
** CDS related functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetCDSSize
**
**  Purpose:
**    This function fetches the size of the OS Critical Data Store area.
**
**  Arguments:
**    Output - pSizeOfCds - Pointer to variable that stores size of CDS
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetCDSSize(uint32 *SizeOfCDS)
{
   int32 return_code;

   if ( SizeOfCDS == NULL )
   {
       return_code = CFE_PSP_ERROR;
   }
   else
   {
       *SizeOfCDS =  CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
       return_code = CFE_PSP_SUCCESS;
   }
   return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_SetReadCDSMethod
**
**  Purpose:
**    This function set the CDS reading method(use CRC, always read from Flash,
**    or trust the CDS reserved memory in RAM is correct.
**
**  Arguments:
**    Input - ucMethod - Reading method
**
**  Return: None
******************************************************************************/
void CFE_PSP_SetReadCDSMethod(uint8 ucMethod)
{
  g_CDSReadMethod = ucMethod;
}

/******************************************************************************
**  Function: CFE_PSP_GetReadCDSMethod
**
**  Purpose:
**    This function get the CDS reading method(use CRC, always read from Flash,
**    or trust the CDS reserved memory in RAM is correct.
**
**  Arguments: None
**
**  Return: 
**    CFE_PSP_CDS_READ_METHOD_DEFAULT: Trust the CDS data on RAM (no CRC or read from Flash)
**    CFE_PSP_CDS_READ_METHOD_CRC: Check the CRC first then read from Flash if CRC mis-matched
**    CFE_PSP_CDS_READ_METHOD_FLASH: Always read from Flash
******************************************************************************/
uint8 CFE_PSP_GetReadCDSMethod()
{
  return g_CDSReadMethod;
}

/******************************************************************************
**  Function: CFE_PSP_SetStaticCRC
**
**  Purpose:
**    This function change the previous calculated CRC value to new provided value.
**    This function is just for testing purpose by forcing the CRC mismatched and 
**    read CDS data from Flash.
**
**  Arguments:
**    Input - uiNewCRC - New force change value for static CRC variable
**
**  Return: None
******************************************************************************/
void CFE_PSP_SetStaticCRC(uint32 uiNewCRC)
{
  sg_uiCDSCrc = uiNewCRC;
}

/******************************************************************************
**  Function: CFE_PSP_GetStaticCRC
**
**  Purpose:
**    This function get the previous calculated CRC value.
**
**  Arguments: None
**
**  Return:
**    The previous calculated CRC from last write or read from Flash
******************************************************************************/
uint32 CFE_PSP_GetStaticCRC()
{
  return sg_uiCDSCrc;
}

/******************************************************************************
**  Function: CFE_PSP_CalculateCRC
**
**  Purpose:
**    This function calculate 16 bits CRC from input data.
**
**  Arguments:
**    Input - DataPtr - Pointer to the input data buffer
**    Input - DataLength - Data buffer length
**    Input - InputCRC - A starting value for use in the CRC calculation.  
**                       This parameter allows the user to calculate the CRC 
**                       of non-contiguous blocks as a single value. 
**                       Nominally, the user should set this value to zero.
**
**  Return: 
**    The result of the CRC calculation on the specified memory block, 
**    or error code \ref CFEReturnCodes
******************************************************************************/
uint32 CFE_PSP_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC)
{
    uint32  i;
    int16  Index;
    int16  Crc = 0;
    const uint8 *BufPtr;
    uint8  ByteValue;

    static const uint16 CrcTable[256]=
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
    BufPtr = (const uint8 *)DataPtr;

    for (i = 0; i < DataLength; i++, BufPtr++)
    {
      /*
       * It is assumed that the supplied buffer is in a
       * directly-accessible memory space that does not
       * require special logic to access
       */
      ByteValue = *BufPtr;
      Index = (( Crc ^ ByteValue) & 0x00FF);
      Crc = ((Crc >> 8 ) & 0x00FF) ^ CrcTable[Index];
    }
     
    return(Crc);

} /* End of CFE_PSP_CalculateCRC() */

/******************************************************************************
**  Function: CFE_PSP_ReadCDSFromFlash
**
**  Purpose:
**    This function read the whole CDS data on Flash to reserved memory on RAM.
**    It took about 117ms to read 131072 bytes (128KB) whole CDS area from Flash. 
**
**  Arguments:
**    Output - puiReadBytes - Number of read bytes
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_ReadCDSFromFlash(uint32 *puiReadBytes)
{
  int32   iCDSFd = 0;
  int32   iReturnCode = CFE_PSP_SUCCESS;
  ssize_t readBytes = 0;
  #if defined(PRINT_DEBUG)
    OS_time_t localTime;
  #endif
  
  /* Return error if the output parameter wasn't allocated */
  if(puiReadBytes == NULL)
  {
    iReturnCode = CFE_PSP_ERROR;
  }
  else
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
        *puiReadBytes = readBytes;
      }
      /* Close the CDS file after opened and read */
      close(iCDSFd);
    }
  }

  return iReturnCode;
}

/******************************************************************************
**  Function: CFE_PSP_WriteCDSToFlash
**
**  Purpose:
**    This function write the whole CDS data from reserved memory on RAM to Flash.
**    It took about 117ms to write 131072 bytes (128KB) whole CDS data to Flash. 
**
**  Arguments:
**    Output - puiWroteBytes - Number of written bytes
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_WriteCDSToFlash(uint32 *puiWroteBytes)
{
  int32   iCDSFd = 0;
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
        *puiWroteBytes = wroteBytes;
      }
      /* Close the CDS file after opened and write */
      close(iCDSFd);
    }
  }

  return iReturnCode;
}

/******************************************************************************
**  Function: CFE_PSP_WriteToCDS
**
**  Purpose:
**    This function writes to the CDS Block.
**
**  Arguments:
**    Input - PtrToDataToWrite - Pointer to the input data buffer
**    Input - CDSOffset - Memory offset of CDS
**    Input - NumBytes - Number of bytes to write to CDS
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_WriteToCDS(const void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes)
{
  uint8 *CopyPtr;
  int32  return_code;
  int32  iWroteBytes = 0; 

  if ( PtrToDataToWrite == NULL )
  {
     return_code = CFE_PSP_ERROR;
  }
  else
  {
    if ( (CDSOffset < CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize ) &&
           ( (CDSOffset + NumBytes) <= CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize ))
    {
      CopyPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
      CopyPtr += CDSOffset;

      /* Compare the original and user changed data. There is no overwriting CDS block on memory
         and updating to Flash if they are matched */
      if(memcmp((char *)CopyPtr, (char *)PtrToDataToWrite, NumBytes) == 0)
      {
        return_code = CFE_PSP_SUCCESS;
      } 
      else
      {      
        memcpy(CopyPtr, (char *)PtrToDataToWrite, NumBytes);

        /* Write whole CDS memory to Flash */
        return_code = CFE_PSP_WriteCDSToFlash(&iWroteBytes);
        if(return_code == CFE_PSP_SUCCESS)
        {
          OS_printf("CFE_PSP: Wrote %d to CDS file.\n", iWroteBytes);
          /* Calculate the CRC for whole CDS memory and write to static variable after updated new CDS data on Flash */
          sg_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                            CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
        }          
      }
    }
    else
    {
      return_code = CFE_PSP_ERROR;
    }

  } /* end if PtrToDataToWrite == NULL */

  return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_ReadFromCDS
**
**  Purpose:
**    This function reads from the CDS Block.
**
**  Arguments:
**    Output - PtrToDataToRead - Pointer to output data buffer
**    Input  - CDSOffset - Memory offset of CDS
**    Input  - NumBytes - Number of bytes to read from CDS
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/

int32 CFE_PSP_ReadFromCDS(void *PtrToDataToRead, uint32 CDSOffset, uint32 NumBytes)
{
  uint8   *CopyPtr;
  int32   return_code = CFE_PSP_SUCCESS;
  uint32  uiTempCrc = 0;
  uint32  uiReadBytes = 0;
  
  if ( PtrToDataToRead == NULL )
  {
     return_code = CFE_PSP_ERROR;
  }
  else
  {
    if ((CDSOffset < CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize) &&
           ((CDSOffset + NumBytes) <= CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize))
    {
      /* In the CRC mode */
      if(g_CDSReadMethod == CFE_PSP_CDS_READ_METHOD_CRC)
      {
        /* Calculate the CRC for whole CDS memory */
        uiTempCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                        CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, uiTempCrc);

        /* CRC isn't match; Re-read CDS data from Flash to memory */
        if(uiTempCrc != sg_uiCDSCrc)
        {
          OS_printf("CFE_PSP: CRC mismatched (uiTempCrc = 0x%08X, sg_uiCDSCrc = 0x%08X)\n", uiTempCrc, sg_uiCDSCrc);
          return_code = CFE_PSP_ReadCDSFromFlash(&uiReadBytes);
          if( return_code == CFE_PSP_SUCCESS)
          {
            /* Update the static CRC variable with new read CDS data from Flash */
            sg_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                              CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
          }        
        }
      }
      /* In the always read from Flash mode */
      else if(g_CDSReadMethod == CFE_PSP_CDS_READ_METHOD_FLASH) 
      {
        return_code = CFE_PSP_ReadCDSFromFlash(&uiReadBytes);
      }

      /* Only copy the CDS data to user prefered pointer when the CDS memory in RAM is correct */
      if(return_code == CFE_PSP_SUCCESS)
      {
        CopyPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
        CopyPtr += CDSOffset;
        memcpy((char *)PtrToDataToRead,CopyPtr, NumBytes);
      }
    }
    else
    {
      return_code = CFE_PSP_ERROR;
    }

  } /* end if PtrToDataToWrite == NULL */

  return(return_code);
}

/*
*********************************************************************************
** ES Reset Area related functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetResetArea
**
**  Purpose:
**     This function returns the location and size of the ES Reset information area.
**     This area is preserved during a processor reset and is used to store the
**     ER Log, System Log and reset related variables
**
**  Arguments:
**    Output - PtrToResetArea - Pointer to the reset memory address
**    Output - SizeOfResetArea - Pointer to variable that stores size of memory
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetResetArea (cpuaddr *PtrToResetArea, uint32 *SizeOfResetArea)
{
   int32   return_code;

   if ((PtrToResetArea == NULL) || (SizeOfResetArea == NULL))
   {
      return_code = CFE_PSP_ERROR;
   }
   else
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

/******************************************************************************
**  Function: CFE_PSP_GetUserReservedArea
**
**  Purpose:
**    This function returns the location and size of the memory used for the cFE
**    user-reserved area.
**
**  Arguments:
**    Output - PtrToUserArea - Pointer to user-reserved memory address
**    Output - SizeOfUserArea - Pointer to variable that stores size of memory
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetUserReservedArea(cpuaddr *PtrToUserArea, uint32 *SizeOfUserArea )
{
   int32   return_code;

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

/******************************************************************************
**  Function: CFE_PSP_GetVolatileDiskMem
**
**  Purpose:
**    This function returns the location and size of the memory used for the cFE
**     volatile disk.
**
**  Arguments:
**    Output - PtrToVolDisk - Pointer to the volatile disk memory address
**    Output - SizeOfVolDisk - Pointer to variable that stores size of memory
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetVolatileDiskMem(cpuaddr *PtrToVolDisk, uint32 *SizeOfVolDisk )
{
   int32   return_code;

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

/******************************************************************************
**  Function: CFE_PSP_InitProcessorReservedMemory
**
**  Purpose:
**    This function performs the top level reserved memory initialization.
**
**  Arguments:
**    Input - RestartType - type of re-start
**
**  Return:
**    CFE_PSP_SUCCESS
******************************************************************************/
int32 CFE_PSP_InitProcessorReservedMemory( uint32 RestartType )
{
    cpuaddr start_addr;
    uint32 reserve_memory_size = 0;
    int32 return_code = CFE_PSP_SUCCESS;
    int32  iCDSFd = -1;
    ssize_t readBytes = 0;  

    userReservedGet((char**)&start_addr, &reserve_memory_size);

    if(PSP_ReservedMemBlock.BlockSize > reserve_memory_size)
    {
      OS_printf("CFE_PSP: VxWorks Reserved Memory Block Size not large enough, Total Size = 0x%lx, VxWorks Reserved Size=0x%lx\n",
               (unsigned long)PSP_ReservedMemBlock.BlockSize,
               (unsigned long)reserve_memory_size);
      return_code = OS_ERROR;

    }
    else if ( RestartType != CFE_PSP_RST_TYPE_PROCESSOR )
    {
      OS_printf("CFE_PSP: Clearing Processor Reserved Memory.\n");
      memset(PSP_ReservedMemBlock.BlockPtr, 0, PSP_ReservedMemBlock.BlockSize);

      /*
      ** Set the default reset type in case a watchdog reset occurs
      */
      CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;

    }

    if(return_code == CFE_PSP_SUCCESS)
    {
      /* Make sure the CDS file was created */
      iCDSFd = open(g_cCDSFilename, O_RDONLY, 0);
      if(iCDSFd < 0)
      {
        /* Create the CDS file afer failed to open it */
        if(creat(g_cCDSFilename, S_IRWXU) < 0)
        {
          OS_printf("CFE_PSP: Failed to create the CDS file(%s) on Flash.\n", 
                     g_cCDSFilename);
          return_code = OS_ERROR;
        }
        else
        {
          OS_printf("CFE_PSP: Created the CDS file.\n");
          /* Close the CDS file after created */
          close(iCDSFd);
        }
      }
      else
      {
        readBytes = read(iCDSFd, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                        CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
        if(readBytes >= 0)
        {
          OS_printf("CFE_PSP: Read %d bytes of CDS data from Flash.\n", readBytes);  
          /* Calculate the CRC for whole CDS memory and write to static variable after read CDS from Flash */
          sg_uiCDSCrc = CFE_PSP_CalculateCRC(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 
                                             CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize, 0);
        }
        else
        {
          OS_printf("CFE_PSP: Failed to read the CDS data on Flash.\n"); 
          /* Return error if read failed */
          return_code = OS_ERROR;
        }
        /* Close the CDS file after opened and read */
        close(iCDSFd);
      }
    }

   return(return_code);
}

/******************************************************************************
**  Function: CFE_PSP_SetupReservedMemoryMap
**
**  Purpose:
**    Set up the CFE_PSP_ReservedMemoryMap global data structure
**    This only sets the pointers, it does not initialize the data.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
void CFE_PSP_SetupReservedMemoryMap(void)
{
    cpuaddr start_addr;
    cpuaddr end_addr;
    uint32 reserve_memory_size = 0;

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
    userReservedGet((char**)&start_addr, &reserve_memory_size);

    end_addr = start_addr;

    memset(&CFE_PSP_ReservedMemoryMap, 0, sizeof(CFE_PSP_ReservedMemoryMap));

    CFE_PSP_ReservedMemoryMap.BootPtr = (CFE_PSP_ReservedMemoryBootRecord_t *)end_addr;
    end_addr += sizeof(CFE_PSP_ReservedMemoryBootRecord_t);
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr = (CFE_PSP_ExceptionStorage_t *)end_addr;
    end_addr += sizeof(CFE_PSP_ExceptionStorage_t);
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->ResetAreaSize;
    end_addr += CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize =
        GLOBAL_CONFIGDATA.CfeConfig->RamDiskSectorSize * GLOBAL_CONFIGDATA.CfeConfig->RamDiskTotalSectors;
    end_addr += CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;
    
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->CdsSize;
    end_addr += CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->UserReservedSize;
    end_addr += CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* The total size of the entire block is the difference in address */
    PSP_ReservedMemBlock.BlockPtr = (void*)start_addr;
    PSP_ReservedMemBlock.BlockSize =  end_addr - start_addr;

    OS_printf("CFE_PSP: Reserved Memory Block at 0x%08lx, Total Size = 0x%lx, VxWorks Reserved Size=0x%lx\n",
            (unsigned long)PSP_ReservedMemBlock.BlockPtr,
            (unsigned long)PSP_ReservedMemBlock.BlockSize,
            (unsigned long)reserve_memory_size);
}

/******************************************************************************
 * Function: CFE_PSP_DeleteProcessorReservedMemory
 *
 * No action on SP0 - reserved block is statically allocated as user reserved
 * memory.
 * Implemented for API consistency with other PSPs.
 */
void CFE_PSP_DeleteProcessorReservedMemory(void)
{
}

/*
*********************************************************************************
** ES BSP kernel memory segment functions
*********************************************************************************
*/

/******************************************************************************
**  Function: CFE_PSP_GetKernelTextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the kernel text segment.
**     It may not be implemented on all architectures.
**
**  Arguments:
**    Output - PtrToKernelSegment - Pointer to kernel segment memory address
**    Output - SizeOfKernelSegment - Pointer to variable that stores memory size
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetKernelTextSegmentInfo(cpuaddr *PtrToKernelSegment, uint32 *SizeOfKernelSegment)
{
   int32 return_code;
   cpuaddr StartAddress;
   cpuaddr EndAddress;

   if ( SizeOfKernelSegment == NULL )
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

/******************************************************************************
**  Function: CFE_PSP_GetCFETextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the CFE text segment.
**     It may not be implemented on all architectures.
**
**  Arguments:
**    Output - PtrToCFESegment - Pointer to CFE segment memory address
**    Output - SizeOfCFESegment - Pointer to variable that stores memory size
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetCFETextSegmentInfo(cpuaddr *PtrToCFESegment, uint32 *SizeOfCFESegment)
{
   int32       return_code;
   STATUS      status;
   MODULE_ID   cFEModuleId;
   MODULE_INFO cFEModuleInfo;

   if ( SizeOfCFESegment == NULL )
   {
      return_code = CFE_PSP_ERROR;
   }
   else
   {
      cFEModuleId = moduleFindByName(CFE_MODULE_NAME);

      if ( cFEModuleId == NULL )
      {
         return_code = CFE_PSP_ERROR;
      }
      else
      {
         status = moduleInfoGet(cFEModuleId, &cFEModuleInfo);
         if ( status != ERROR )
         {
            *PtrToCFESegment = (cpuaddr) (cFEModuleInfo.segInfo.textAddr);
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



