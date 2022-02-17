/**
 ** \file cfe_psp_memory.c
 **
 ** \brief cFE PSP Memory related functions
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
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
#include "target_config.h"
#include "osapi.h"

/* Include PSP API */
#include "cfe_psp.h"

/* For CFE_PSP_SetTaskPrio() */
#include "psp_start.h"
#include "psp_exceptions.h"
#include "cfe_psp_memory.h"
#include "psp_flash.h"

/*
** Macro Definitions
*/

/** \brief Define cFE core loadable module name */
#define MEMORY_PRINT_SCOPE "PSP MEMORY: "

/*
** ENUM Definitions
*/
enum MEMORY_SECTION
    {OP_CDS = 0,
    OP_RESET = 1,
    OP_VOLATILEDISK = 2,
    OP_USERRESERVED = 3};

/*
**  External Declarations
*/
/** \brief External Kernel Function GetWrsKernelTextStart */
extern unsigned int GetWrsKernelTextStart(void);
/** \brief External Kernel Function GetWrsKernelTextEnd */
extern unsigned int GetWrsKernelTextEnd(void);

/*
** Static functions
**
** See function definitions for details
*/
static uint32 CFE_PSP_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC);
static int32 CFE_PSP_MEMORY_GetMemSize(uint32 *p_size, enum MEMORY_SECTION op);
static int32 CFE_PSP_MEMORY_GetMemArea(cpuaddr *p_area, uint32 *p_size, enum MEMORY_SECTION op);
static int32 CFE_PSP_MEMORY_ReadFromRAM(const void *p_data, uint32 offset, uint32 size, enum MEMORY_SECTION op);
static int32 CFE_PSP_MEMORY_WriteToRAM(const void *p_data, uint32 offset, uint32 size, enum MEMORY_SECTION op);
static int32 CFE_PSP_MEMORY_RestoreRESET(void);
static int32 CFE_PSP_MEMORY_RestoreCDS(void);
static int32 CFE_PSP_MEMORY_RestoreVOLATILEDISK(void);
static int32 CFE_PSP_MEMORY_RestoreUSERRESERVED(void);
static int32 CFE_PSP_MEMORY_RestoreDATA(enum MEMORY_SECTION op);
/*
** Global variables
*/
/* Temporary solution which declared a CDS file to store all apps' CDS data 
   in. This solution is to take advance of BSP's Datalight File System. This
   file system will support read/write files to Flash at location from 
   128MB to 1GB.
*/

/** \brief Track total amount of allocated memory */
static uint32 g_uiTotalReservedAllocSize = 0;

/**
 ** \brief Contains the address of the end of RAM
 ** \par Description:
 ** This variable is filled out once during boot and never changed again. Its
 ** value reflects the amount of RAM of the system. When moving cFS from SP0 to
 ** SP0-s, the value changes automatically. Value is also used for checking for
 ** out of range addresses.
 ** 
 */
uint32 g_uiEndOfRam = 0;

/**
 ** \brief Pointer to the vxWorks USER_RESERVED_MEMORY area
 ** \par Description:
 ** The sizes of each memory area is defined in os_processor.h for this architecture.
 */
CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap; //UndCC_Line(SSET056) Name format required by PSP API

/** \brief Pointer to the reserved memory block */
static CFE_PSP_MemoryBlock_t g_ReservedMemBlock;

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
 ** \param[in] DataPtr  - Pointer to the input data buffer
 ** \param[in] DataLength  - Data buffer length
 ** \param[in] InputCRC  - A starting value for use in the CRC calculation.
 **
 ** \return Calculated CRC value
 */
static uint32 CFE_PSP_CalculateCRC(const void *DataPtr, uint32 DataLength, uint32 InputCRC)
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
 ** the Volatile Disk Memory and the User Reserved Memory. Options include 
 ** #CFE_PSP_RST_TYPE_PROCESSOR, #CFE_PSP_RST_TYPE_POWERON, #CFE_PSP_RST_TYPE_MAX
 **
 ** \par Assumptions, External Events, and Notes:
 ** For SP0 target implementation, the User Reserved Memory is always preserved,
 ** independently from Reset Type.
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
    int32     iReturnCode = CFE_PSP_SUCCESS;
    int32     iCDSFd = -1;
    ssize_t   readBytes = 0;  
    int32     iStatus = 0;
    
    /* 
    ** Returns the address and size of the user-reserved region
    ** This is a Kernel function
    */
    userReservedGet((char **) &start_addr, &reserve_memory_size);

    /* 
    ** If the VxWorks reserved memory size is smaller than the 
    ** requested, print error
    */
    if(g_uiTotalReservedAllocSize > reserve_memory_size)
    {
        OS_printf("CFE_PSP: VxWorks Reserved Memory Block Size not large enough, "
                  "Total Size = 0x%lx, "
                  "VxWorks Reserved Size=0x%lx\n",
                  (unsigned long)g_uiTotalReservedAllocSize,
                  (unsigned long)reserve_memory_size);
        iReturnCode = CFE_PSP_ERROR;
    }

    /*
    ** Depending on the restart type, the User Reserved Memory is recovered or deleted
    ** from FLASH memory.
    **
    ** IF   the restart type is CFE_PSP_RST_TYPE_POWERON, erase all User Reserved Memory
    ** ELSE recover user reserved memory from FLASH
    */
    else if (RestartType == CFE_PSP_RST_TYPE_POWERON)
    {
        /*
        ** POWERON RESET Detected
        ** 
        ** Erase all user reserved memory.
        ** Clear from RAM, FLASH
        */

        /* Clear EDR data in EEPROM */
        CFE_PSP_edrClearEEPROM();

        /*
        ** Set the default reset type in case a watchdog reset occurs
        */
        CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;

        memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 
                0, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
        CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize,
                                    CFE_PSP_RESET_FLASH_FILEPATH);

        /* CDS MEMORY */
        memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                0, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
        CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize,
                                    CFE_PSP_CDS_FLASH_FILEPATH);

        /* VOLATILE DISK MEMORY */
        memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                0, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
        CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize,
                                    CFE_PSP_VOLATILEDISK_FLASH_FILEPATH);

        /* USER RESERVERED MEMORY */
        memset(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                0, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
        CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize,
                                    CFE_PSP_USERRESERVED_FLASH_FILEPATH);
    }
    else if (RestartType == CFE_PSP_RST_TYPE_PROCESSOR)
    {
        /*
        ** PROCESSOR RESET Detected
        ** 
        ** Volatile disk, Critical Data Store and User Reserved
        ** memory could still be valid
        **
        ** Attempt to restore above memory sections
        */
        
        /* Recover EDR data if available */
        CFE_PSP_edrLoadFromEEPROM();

        /* RESET MEMORY - Not sure if still valid */
        memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
                0, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
        CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 
                                    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize, 
                                    CFE_PSP_RESET_FLASH_FILEPATH);

        /* CDS MEMORY */
        iStatus = CFE_PSP_MEMORY_RestoreCDS();
        
        if (iStatus != CFE_PSP_SUCCESS)
        {
            OS_printf(MEMORY_PRINT_SCOPE "InitProcessorReservedMemory: Failed to restore CDS data\n");
            memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
            CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize,
                                    CFE_PSP_CDS_FLASH_FILEPATH);
        }

        /* VOLATILE DISK MEMORY */
        iStatus = CFE_PSP_MEMORY_RestoreVOLATILEDISK();

        if (iStatus != CFE_PSP_SUCCESS)
        {
            OS_printf(MEMORY_PRINT_SCOPE "InitProcessorReservedMemory: Failed to restore Volatile Disk data\n");
            memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
            CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize,
                                    CFE_PSP_VOLATILEDISK_FLASH_FILEPATH);
        }

        /* USER RESERVED MEMORY */
        iStatus = CFE_PSP_MEMORY_RestoreUSERRESERVED();

        if (iStatus != CFE_PSP_SUCCESS)
        {
            OS_printf(MEMORY_PRINT_SCOPE "InitProcessorReservedMemory: Failed to restore User Reserved data\n");
            memset(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
            CFE_PSP_FLASH_WriteToFLASH((uint32 *)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                                    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize,
                                    CFE_PSP_USERRESERVED_FLASH_FILEPATH);
        }
    }

   return(iReturnCode);
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
    uint32    reserve_memory_size = 0;
    int32     iStatus = OS_SUCCESS;
    STATUS    sStatus = OK;
    
    /* 
    ** Returns the address and size of the user-reserved region
    ** This is a Kernel function
    */
    userReservedGet((char **) &start_addr, &reserve_memory_size);
    memset(&CFE_PSP_ReservedMemoryMap, 0, sizeof(CFE_PSP_ReservedMemoryMap));
    g_uiEndOfRam = (uint32)sysPhysMemTop();

    /* There are a handful of reserved memory sections that 
    ** we need to setup, contained in (a struct) CFE_PSP_ReservedMemoryMap_t:
    **      1. (CFE_PSP_ReservedMemoryBootRecord_t *)   Boot Ptr
    **      2. (CFE_PSP_ExceptionStorage_t *)           Exception Storage Ptr
    **      3. (CFE_PSP_MemoryBlock_t)                  Reset Memory
    **      4. (CFE_PSP_MemoryBlock_t)                  Volatile Disk
    **      5. (CFE_PSP_MemoryBlock_t)                  CDS
    **      6. (CFE_PSP_MemoryBlock_t)                  User Reserved
    */

    /* 1. (CFE_PSP_ReservedMemoryBootRecord_t *) Boot Ptr */
    sStatus = userMemAlloc(   (uint32 *)&CFE_PSP_ReservedMemoryMap.BootPtr,
                                (uint32)sizeof(CFE_PSP_ReservedMemoryBootRecord_t), 
                                true);
    if (sStatus == OK)
    {
        g_uiTotalReservedAllocSize += sizeof(CFE_PSP_ReservedMemoryBootRecord_t);
    }
    else
    {
        OS_printf("usrMemAlloc Failed\n");
    }

    /* 2. (CFE_PSP_ExceptionStorage_t *) Exception Storage Ptr */
    sStatus = userMemAlloc(   (uint32 *)&CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr,
                                (uint32)sizeof(CFE_PSP_ExceptionStorage_t),
                                true);
    if (sStatus == OK)
    {
        g_uiTotalReservedAllocSize += sizeof(CFE_PSP_ExceptionStorage_t);
    }
    else
    {
        OS_printf("usrMemAlloc Failed\n");
    }
    
    /* 3. (CFE_PSP_MemoryBlock_t Reset Memory */
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->ResetAreaSize;
    sStatus = userMemAlloc(   (uint32 *)&CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
                                (uint32)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize,
                                true);
    if (sStatus == OK)
    {
        g_uiTotalReservedAllocSize += CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
    }
    else
    {
        OS_printf("usrMemAlloc Failed\n");
    }
    
    /* 4. (CFE_PSP_MemoryBlock_t) Volatile Disk */
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize =
        GLOBAL_CONFIGDATA.CfeConfig->RamDiskSectorSize * GLOBAL_CONFIGDATA.CfeConfig->RamDiskTotalSectors;
    sStatus = userMemAlloc(   (uint32 *)&CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                                (uint32)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize,
                                true);
    if (sStatus == OK)
    {
        g_uiTotalReservedAllocSize += CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
    }
    else
    {
        OS_printf("usrMemAlloc Failed\n");
    }

    /* 5. (CFE_PSP_MemoryBlock_t) CDS */
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->CdsSize;
    sStatus = userMemAlloc(   (uint32 *)&CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                                (uint32)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize,
                                true);
    if (sStatus == OK)
    {
        g_uiTotalReservedAllocSize += CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
    }
    else
    {
        OS_printf("usrMemAlloc Failed\n");
    }

    /* 6. (CFE_PSP_MemoryBlock_t) User Reserved */
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->UserReservedSize;
    sStatus = userMemAlloc(   (uint32 *)&CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                                (uint32)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize,
                                true);
    if (sStatus == OK)
    {
        g_uiTotalReservedAllocSize += CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
    }
    else
    {
        OS_printf("usrMemAlloc Failed\n");
    }
    
    /* Debug/Log printing */
    OS_printf(MEMORY_PRINT_SCOPE "SetupReservedMemoryMap Info:\n");
    OS_printf("\tRESET:\tBlock Ptr: 0x%08lx\tBlock Size: 0x%08lx\n",
                (unsigned long)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
                (unsigned long)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    OS_printf("\tCDS:\tBlock Ptr: 0x%08lx\tBlock Size: 0x%08lx\n",
                (unsigned long)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                (unsigned long)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    OS_printf("\tVoDi:\tBlock Ptr: 0x%08lx\tBlock Size: 0x%08lx\n",
                (unsigned long)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                (unsigned long)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    OS_printf("\tUsRe:\tBlock Ptr: 0x%08lx\tBlock Size: 0x%08lx\n",
                (unsigned long)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                (unsigned long)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
    
    /*
     * Set up the "RAM" entry in the memory table.
     *
     * What is the purpose of this? Should we be more detailed with our set?
     * We don't actually use all of RAM (nor should we at a cFS level?)
     */
    iStatus = CFE_PSP_MemRangeSet(
            0, 
            CFE_PSP_MEM_RAM, 
            0, 
            g_uiEndOfRam, 
            CFE_PSP_MEM_SIZE_DWORD, 
            CFE_PSP_MEM_ATTR_READWRITE
    );
    if (iStatus != OS_SUCCESS)
    {
        OS_printf(MEMORY_PRINT_SCOPE "SetupReservedMemory: MemRangeSet failed\n");
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
    /*
    ** NOTE: We don't actually have a way to 'de allocate' our allocated
    ** memory via kernel fucntion call
    **
    ** Just FLUSH and delete, but hold onto block Ptr.
    */

    OS_printf("CFE_PSP: Aitech does not support Non-Volatile User Reserved Memory\n");

    /* RESET */
    memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr, 0,
            CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
    CFE_PSP_FLASH_DeleteFile(CFE_PSP_RESET_FLASH_FILEPATH);

    /* CDS */
    memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr, 0,
            CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
    CFE_PSP_FLASH_DeleteFile(CFE_PSP_CDS_FLASH_FILEPATH);

    /* VOLATILE DISK */
    memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr, 0,
            CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
    CFE_PSP_FLASH_DeleteFile(CFE_PSP_CDS_FLASH_FILEPATH);

    /* User Reserved */
    memset(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr, 0,
            CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
    CFE_PSP_FLASH_DeleteFile(CFE_PSP_CDS_FLASH_FILEPATH);
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
    int32    iReturnCode = CFE_PSP_ERROR;
    cpuaddr  StartAddress = 0;
    cpuaddr  EndAddress = 0;

    if ((SizeOfKernelSegment == NULL) || (PtrToKernelSegment == NULL))
    {
        iReturnCode = CFE_PSP_ERROR;
    }
    else
    {
        /*
        ** Get the kernel start and end
        ** addresses from the BSP, because the
        ** symbol table does not contain the symbols we need for this
        */
        StartAddress = (cpuaddr) GetWrsKernelTextStart();
        EndAddress = (cpuaddr) GetWrsKernelTextEnd();

        *PtrToKernelSegment = StartAddress;
        *SizeOfKernelSegment = (uint32) (EndAddress - StartAddress);

        iReturnCode = CFE_PSP_SUCCESS;
    }

    return(iReturnCode);
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
    STATUS      status = OK;
    MODULE_ID   cFEModuleId = NULL;
    MODULE_INFO cFEModuleInfo = {};
    cpuaddr     GetModuleIdAddr = 0;
    MODULE_ID   (*GetModuldIdFunc)(void) = NULL;

    if ((PtrToCFESegment == NULL) || (SizeOfCFESegment == NULL))
    {
        return_code = CFE_PSP_ERROR;
    }
    else
    {
        /*
         * First attempt to call a function called GetCfeCoreModuleID().
         *
         * If CFE core was started via the "startCfeCore" routine, this
         * provides the actual module ID that was loaded by that routine,
         * no matter what it is actually named.  This is provided by the
         * support/integration code compiled directly into the VxWorks kernel
         * image.
         *
         * The prototype should be:
         *     MODULE_ID GetCfeCoreModuleID(void);
         */
        cFEModuleId     = NULL;
        GetModuleIdAddr = 0;
        return_code     = OS_SymbolLookup(&GetModuleIdAddr, "GetCfeCoreModuleID");
        if ((return_code == OS_SUCCESS) && (GetModuleIdAddr != 0))
        {
            GetModuldIdFunc = (MODULE_ID(*)(void))GetModuleIdAddr;
            cFEModuleId     = GetModuldIdFunc();
        }

        /*
         * If the above did not yield a valid module ID,
         * then attempt to find the module ID by name.
         * This assumes the core executable name as built by CMake
         */
        if (cFEModuleId == NULL)
        {
            cFEModuleId = moduleFindByName((char *)GLOBAL_CONFIGDATA.Default_CoreFilename);
        }

        if (cFEModuleId == NULL)
        {
            return_code = CFE_PSP_ERROR;
        }
        else
        {
            status = moduleInfoGet(cFEModuleId, &cFEModuleInfo);
            if (status == OK)
            {
                *PtrToCFESegment  = (cpuaddr)(cFEModuleInfo.segInfo.textAddr);
                *SizeOfCFESegment = (uint32)(cFEModuleInfo.segInfo.textSize);
                return_code       = CFE_PSP_SUCCESS;
            }
            else
            {
                return_code = CFE_PSP_ERROR;
            }
        }
    }

    return (return_code);
}

/**********************************************************
 * 
 * RESET MEMORY SECTION HANDLING
 * 
 **********************************************************/

/**********************************************************
 * 
 * Function: CFE_PSP_GetRESETSize
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetRESETSize(uint32 *size)
{
    return CFE_PSP_MEMORY_GetMemSize(size, OP_RESET);
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetResetArea
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetResetArea(cpuaddr *PtrToResetArea, uint32 *SizeOfResetArea)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    iReturnCode = CFE_PSP_MEMORY_GetMemArea(PtrToResetArea, SizeOfResetArea, OP_RESET);

    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;

}

/**********************************************************
 * 
 * Function: CFE_PSP_ReadFromRESET
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_MEMORY_ReadFromRESET(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_MEMORY_ReadFromRAM(p_data, offset, size, OP_RESET);
}

int32 CFE_PSP_MEMORY_WriteToRESET(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_MEMORY_WriteToRAM(p_data, offset, size, OP_RESET);
}

/*
** \brief Restore RESET data from FLASH
**
** \par Description:
** Restore RESET data from FLASH
**
** \par Assumptions, External Events, and Notes:
** None
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_RestoreRESET()
{
    return CFE_PSP_MEMORY_RestoreDATA(OP_RESET);
}

/**********************************************************
 * 
 * CDS MEMORY SECTION HANDLING
 * 
 **********************************************************/

/**********************************************************
 * 
 * Function: CFE_PSP_GetCDSSize
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetCDSSize(uint32 *SizeOfCDS)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    iReturnCode = CFE_PSP_MEMORY_GetMemSize(SizeOfCDS, OP_CDS);
    
    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetCDSArea
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetCDSArea(cpuaddr *p_area, uint32 *p_size)
{
    return CFE_PSP_MEMORY_GetMemArea(p_area, p_size, OP_CDS);
}

/**********************************************************
 * 
 * Function: CFE_PSP_ReadFromCDS
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_ReadFromCDS(void *PtrToDataFromRead, uint32 CDSOffset, uint32 NumBytes)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    iReturnCode = CFE_PSP_MEMORY_ReadFromRAM(PtrToDataFromRead, CDSOffset, NumBytes, OP_CDS);

    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_WriteToCDS
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_WriteToCDS(const void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    iReturnCode = CFE_PSP_MEMORY_WriteToRAM(PtrToDataToWrite, CDSOffset, NumBytes, OP_CDS);

    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}

/*
** \brief Restore CDS data from FLASH
**
** \par Description:
** Restore CDS data from FLASH (read)
**
** \par Assumptions, External Events, and Notes:
** None
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_RestoreCDS()
{
    return CFE_PSP_MEMORY_RestoreDATA(OP_CDS);
}

/**********************************************************
 * 
 * VOLATILE DISK MEMORY SECTION HANDLING
 * 
 **********************************************************/

/**********************************************************
 * 
 * Function: CFE_PSP_GetVOLATILEDISKSize
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetVOLATILEDISKSize(uint32 *size)
{
    return CFE_PSP_MEMORY_GetMemSize(size, OP_VOLATILEDISK);   
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetVolatileDiskMem
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetVolatileDiskMem(cpuaddr *PtrToVolDisk, uint32 *SizeOfVolDisk)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    iReturnCode = CFE_PSP_MEMORY_GetMemArea(PtrToVolDisk, SizeOfVolDisk, OP_VOLATILEDISK);
    
    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_ReadFromVOLATILEDISK
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_MEMORY_ReadFromVOLATILEDISK(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_MEMORY_ReadFromRAM(p_data, offset, size, OP_VOLATILEDISK);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MEMORY_WriteToVOLATILEDISK
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_MEMORY_WriteToVOLATILEDISK(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_MEMORY_WriteToRAM(p_data, offset, size, OP_VOLATILEDISK);
}

/*
** \brief Restore volatile disk data from FLASH
**
** \par Description:
** Restore volatile disk data from FLASH (read)
**
** \par Assumptions, External Events, and Notes:
** None
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_RestoreVOLATILEDISK()
{
    return CFE_PSP_MEMORY_RestoreDATA(OP_VOLATILEDISK);
}

/**********************************************************
 * 
 * USER RESERVED MEMORY SECTION HANDLING
 * 
 **********************************************************/

/**********************************************************
 * 
 * Function: CFE_PSP_GetUSERRESERVEDSize
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetUSERRESERVEDSize(uint32 *size)
{
    return CFE_PSP_MEMORY_GetMemSize(size, OP_USERRESERVED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetUserReservedArea
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_GetUserReservedArea(cpuaddr *PtrToUserArea, uint32 *SizeOfUserArea)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    iReturnCode = CFE_PSP_MEMORY_GetMemArea(PtrToUserArea, SizeOfUserArea, OP_USERRESERVED);

    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MEMORY_ReadFromUSERRESERVED
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_MEMORY_ReadFromUSERRESERVED(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_MEMORY_ReadFromRAM(p_data, offset, size, OP_USERRESERVED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MEMORY_WriteToUSERRESERVED
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_MEMORY_WriteToUSERRESERVED(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_MEMORY_WriteToRAM(p_data, offset, size, OP_USERRESERVED);
}

/*
** \brief Restore user reserved data from FLASH
**
** \par Description:
** Restore user reserved data from FLASH (read)
**
** \par Assumptions, External Events, and Notes:
** None
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_RestoreUSERRESERVED()
{
    return CFE_PSP_MEMORY_RestoreDATA(OP_USERRESERVED);
}

/**********************************************************
 * 
 * REUSABLE RESERVED MEMORY SECTION HANDLING
 * 
 **********************************************************/

/*
** \brief Get reserved memory size
**
** \par Description:
** Get reserved memory area size
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] p_size - Size of data
** \param[in] op - Reserved memory section option
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_GetMemSize(uint32 *p_size, enum MEMORY_SECTION op)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (p_size == NULL)
    {
        iReturnCode = CFE_PSP_INVALID_POINTER;
        OS_printf(MEMORY_PRINT_SCOPE "GetMemSize: NULL Pointer\n");
    }
    else
    {
        switch (op)
        {
            case OP_CDS:
                *p_size = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
                break;

            case OP_RESET:
                *p_size = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
                break;

            case OP_VOLATILEDISK:
                *p_size = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
                break;

            case OP_USERRESERVED:
                *p_size = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
                break;
            
            default:
                iReturnCode = CFE_PSP_ERROR;
                *p_size = (size_t)0x00000000;
                OS_printf(MEMORY_PRINT_SCOPE "GetMemSize: Invalid Memory Option\n");
                break;
        }
    }

    return iReturnCode;
}

/*
** \brief Get reserved memory area
**
** \par Description:
** Get reserved memory area size and pointer
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] p_area - Data pointer
** \param[out] p_size - Size of data
** \param[in] op - Reserved memory section option
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_GetMemArea(cpuaddr *p_area, uint32 *p_size, enum MEMORY_SECTION op)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if ((p_area == NULL) || (p_size == NULL))
    {
        iReturnCode = CFE_PSP_INVALID_POINTER;
        OS_printf(MEMORY_PRINT_SCOPE "GetMemArea: NULL Pointer\n");
    }
    else
    {
        switch (op)
        {
            case OP_CDS:
                *p_area = (cpuaddr)CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
                *p_size = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
                break;

            case OP_RESET:
                *p_area = (cpuaddr)CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
                *p_size = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
                break;

            case OP_VOLATILEDISK:
                *p_area = (cpuaddr)CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr;
                *p_size = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
                break;

            case OP_USERRESERVED:
                *p_area = (cpuaddr)CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr;
                *p_size = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
                break;
            
            default:
                iReturnCode = CFE_PSP_INVALID_MEM_TYPE;
                *p_area = (cpuaddr)0x00000000;
                *p_size = (size_t)0x00000000;
                OS_printf(MEMORY_PRINT_SCOPE "GetMemArea: Invalid Memory Option\n");
                break;
        }
    }

    return iReturnCode;
}

/*
** \brief Read data from RAM
**
** \par Description:
** Read data to a specific reserved memory section
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] p_data - Data buffer to receive data read from RAM
** \param[in] offset - Data offset for read location
** \param[in] size - Size of data read
** \param[in] op - Reserved memory section option
**
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_ReadFromRAM(const void *p_data, uint32 offset, uint32 size, enum MEMORY_SECTION op)
{
    uint8   *pCopyPtr       = NULL;
    int32   iReturnCode     = CFE_PSP_SUCCESS;
    uint32  uiReadBytes     = 0;

    CFE_PSP_MemoryBlock_t memBlock = {0, 0};

    if (p_data == NULL)
    {
        OS_printf(MEMORY_PRINT_SCOPE "ReadFromRAM: NULL Pointer\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        switch (op)
        {
            case OP_RESET:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
                break;

            case OP_CDS:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
                break;

            case OP_VOLATILEDISK:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
                break;

            case OP_USERRESERVED:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
                break;

            default:
                memBlock.BlockPtr = NULL;
                iReturnCode = CFE_PSP_INVALID_MEM_TYPE;
                break;
        }

        if (iReturnCode == CFE_PSP_INVALID_MEM_TYPE)
        {
            OS_printf(MEMORY_PRINT_SCOPE "ReadFromRAM: Invalid Memory Option\n");
        }
        else if (memBlock.BlockPtr == NULL)
        {
            iReturnCode = CFE_PSP_INVALID_POINTER;
        }
        else if ((offset + size) > memBlock.BlockSize)
        {
            iReturnCode = CFE_PSP_INVALID_MEM_RANGE;
        }
        else
        {
            iReturnCode = CFE_PSP_SUCCESS;

            pCopyPtr = memBlock.BlockPtr;
            pCopyPtr += offset;
            memcpy((char *)p_data, pCopyPtr, size);
        }
    }

    return iReturnCode;
}

/*
** \brief Write data to RAM
**
** \par Description:
** Write data to a specific reserved memory section
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] p_data - Data to write to RAM
** \param[in] offset - Data offset for write location
** \param[in] size - Size of data to write
** \param[in] op - Reserved memory section option
**
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_WriteToRAM(const void *p_data, uint32 offset, uint32 size, enum MEMORY_SECTION op)
{
    uint8 *pCopyPtr = NULL;
    int32 iReturnCode = CFE_PSP_ERROR;

    CFE_PSP_MemoryBlock_t memBlock = {0, 0};

    if (p_data == NULL)
    {
        OS_printf(MEMORY_PRINT_SCOPE "WriteToRAM: NULL Pointer\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        switch (op)
        {
            case OP_RESET:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
                break;

            case OP_CDS:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
                break;

            case OP_VOLATILEDISK:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
                break;

            case OP_USERRESERVED:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
                break;

            default:
                memBlock.BlockPtr = NULL;
                iReturnCode = CFE_PSP_INVALID_MEM_TYPE;
                break;
        }
        
        if (iReturnCode == CFE_PSP_INVALID_MEM_TYPE)
        {
            OS_printf(MEMORY_PRINT_SCOPE "WriteToRAM: Invalid Memory Selection\n");
        }
        else if (memBlock.BlockPtr == NULL)
        {
            iReturnCode = CFE_PSP_INVALID_POINTER;
        }
        else if ((offset + size) > memBlock.BlockSize)
        {
            iReturnCode = CFE_PSP_INVALID_MEM_RANGE;
            OS_printf(MEMORY_PRINT_SCOPE "WriteToRAM: Invalid Mem Range\n");
        }
        else
        {
            iReturnCode = CFE_PSP_SUCCESS;

            pCopyPtr = memBlock.BlockPtr;
            pCopyPtr += offset;

            /*
            ** Compare the original and user changed data. If no changes,
            ** don't do anything
            */
            if (memcmp((char *)pCopyPtr, (char *)p_data, size) != 0)
            {
                /* Overwrite memory (RAM) block */
                memcpy(pCopyPtr, (char *)p_data, size);
            }
        }
    }

    return iReturnCode;
}

/*
** \brief Restore DATA data from FLASH
**
** \par Description:
** Restore DATA data from FLASH
**
** \par Assumptions, External Events, and Notes:
** If file is not present on system, this function will 
** attempt to create the file and write current DATA data
** to file
**
** \param op - Memory selection
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
** \return #CFE_PSP_INVALID_MEM_TYPE
** \return #CFE_PSP_INVALID_POINTER
*/
static int32 CFE_PSP_MEMORY_RestoreDATA(enum MEMORY_SECTION op)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    char *p_caFilename = NULL;
    CFE_PSP_MemoryBlock_t memBlock = {0, 0};

    switch (op)
    {
        case OP_CDS:
            memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
            memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
            p_caFilename = CFE_PSP_CDS_FLASH_FILEPATH;
            break;

        case OP_RESET:
            memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
            memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
            p_caFilename = CFE_PSP_RESET_FLASH_FILEPATH;
            break;

        case OP_VOLATILEDISK:
            memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr;
            memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
            p_caFilename = CFE_PSP_VOLATILEDISK_FLASH_FILEPATH;
            break;

        case OP_USERRESERVED:
            memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr;
            memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
            p_caFilename = CFE_PSP_USERRESERVED_FLASH_FILEPATH;
            break;
        
        default:
            iReturnCode = CFE_PSP_INVALID_MEM_TYPE;
            memBlock.BlockPtr = (void *)0x00000000;
            memBlock.BlockSize = (size_t)0x00000000;
            OS_printf(MEMORY_PRINT_SCOPE "RestoreDATA: Invalid Memory Option\n");
            break;
    }

    if (p_caFilename == NULL)
    {
        iReturnCode = CFE_PSP_INVALID_MEM_TYPE;
    }
    else
    {
        /* We have detected a valid memory option */
        /* Check if file exists */
        if (CFE_PSP_FLASH_CheckFile(p_caFilename) == false)
        {
            /* File does not exist on system. Create file */
            iReturnCode = CFE_PSP_FLASH_CreateFile(p_caFilename);
            if (iReturnCode == CFE_PSP_SUCCESS)
            {
                /* Indicate that we need to write to FLASH */
                iReturnCode = CFE_PSP_MEM_INVALID;
                /*
                ** NOTE that a more appropriate code would
                ** be something like CFE_PSP_FLASH_SYNC_REQD
                */
            }
        }
        else
        {
            /* File does exist on system */
            if (CFE_PSP_FLASH_ReadFromFLASH(
                                (uint32 *)memBlock.BlockPtr,
                                memBlock.BlockSize,
                                p_caFilename) != CFE_PSP_SUCCESS)
            {
                /*
                ** Read error
                ** 
                ** Indicate we need to write current RAM
                ** data to FLASH
                */
                iReturnCode = CFE_PSP_MEM_INVALID;
                /*
                ** NOTE that a more appropriate code would
                ** be something like CFE_PSP_FLASH_SYNC_REQD
                */
            }
        }
        
        /* Check if we want to write to FLASH */
        if (iReturnCode == CFE_PSP_MEM_INVALID)
        {
            /* Write data to flash */
            iReturnCode = CFE_PSP_FLASH_WriteToFLASH(
                                (uint32 *)memBlock.BlockPtr,
                                memBlock.BlockSize,
                                p_caFilename);
        }
    }

    return iReturnCode;
}
