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
*   \file cfe_psp_memory.c
*
*   \brief cFE PSP Memory related functions
*
*   \brief This is the implementation of the cFE memory areas that have to be preserved, and the API that
*   is designed to allow access to them. It also contains memory related routines to return the address 
*   of the kernel code used in the cFE checksum.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <moduleLib.h>
#include <userReservedMem.h>

/*
** cFE includes 
*/
#include "common_types.h"
#include "target_config.h"
#include "osapi.h"

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h" 
#include "cfe_psp_memory.h"           

/*
** Macro Definitions
*/

/** \name Define PRINT SCOPES */
/** \{ */
#define MEMORY_PRINT_SCOPE "PSP MEMORY: "
#define MEMORY_SYNC_PRINT_SCOPE "MEMORY SYNC: "
/** \} */

/*
** ENUM Definitions
*/

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
int32 CFE_PSP_GetMemSize(uint32 *p_size, MEMORY_SECTION_t op);
int32 CFE_PSP_GetMemArea(cpuaddr *p_area, uint32 *p_size, MEMORY_SECTION_t op);
int32 CFE_PSP_ReadFromRAM(const void *p_data, uint32 offset, uint32 size, MEMORY_SECTION_t op);
int32 CFE_PSP_WriteToRAM(const void *p_data, uint32 offset, uint32 size, MEMORY_SECTION_t op);

/*
** Global variables
*/

/** \brief Pointer and Size of overall user reserved memory */
CFE_PSP_MemoryBlock_t GR740_ReservedMemBlock;

/**
 ** \brief Contains the address of the end of RAM
 ** \par Description:
 ** This variable is filled out once during boot and never changed again. Its
 ** value reflects the amount of RAM of the system. Value is also used to check
 ** for out of range addresses.
 ** 
 */
uint32 g_uiEndOfRam = 0;

/**
 ** \brief Pointers and Sizes of individual user reserved areas
 ** \par Description:
 ** The sizes of each memory area is defined in os_processor.h for this architecture.
 */
CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap;

/*
*********************************************************************************
** ES BSP Top Level Reserved memory initialization
*********************************************************************************
*/

/**********************************************************
 * 
 * Function: CFE_PSP_InitProcessorReservedMemory
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_InitProcessorReservedMemory( uint32 RestartType )
{
    cpuaddr   start_addr = 0;
    size_t    reserve_memory_size = 0;
    int32     iReturnCode = CFE_PSP_SUCCESS;
 
    /* 
    ** Returns the address and size of the user-reserved region
    ** This is a Kernel function
    */
    userReservedGet((char **) &start_addr, &reserve_memory_size);

    /* 
    ** If the VxWorks reserved memory size is smaller than the 
    ** requested, print error
    */
    if (GR740_ReservedMemBlock.BlockSize > reserve_memory_size)
    {
        OS_printf(MEMORY_PRINT_SCOPE "VxWorks Reserved Memory Block Size not large enough, "
                  "Total Size = 0x%lx, "
                  "VxWorks Reserved Size=0x%lx\n",
                  (unsigned long)GR740_ReservedMemBlock.BlockSize,
                  (unsigned long)reserve_memory_size);
        iReturnCode = CFE_PSP_ERROR;
    }

    /*
    ** If any of the below fail, we can restart. But if the above
    ** fail, we need to have some sort of more meaningful return code
    ** and not restart as it is likely a configuration failure
    */
    if (iReturnCode != CFE_PSP_ERROR)
    {
        if ( RestartType == CFE_PSP_RST_TYPE_POWERON )
        {
            /*
            POWERON:
            In this case, the User Reserved Memory is cleared
            */
            OS_printf(MEMORY_PRINT_SCOPE "Clearing Processor Reserved Memory.\n");

            /* Initialize memory sections */
            memset(CFE_PSP_ReservedMemoryMap.BootPtr,
                    0, sizeof(CFE_PSP_ReservedMemoryBootRecord_t));            
            memset(CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr,
                    0, sizeof(CFE_PSP_ExceptionStorage_t));
            memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
            memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
            memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
            memset(CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize);
        }
        else
        {
            /*
            PROCESSOR:
            In this case, the User Reserved Memory is preserved/recovered
            */
        }
    }
    /*
    ** Set the default reset type in case a watchdog reset occurs 
    */
    CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;

    return(iReturnCode);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SetupReservedMemoryMap
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
void CFE_PSP_SetupReservedMemoryMap(void)
{
    cpuaddr     start_addr;
    size_t      reserve_memory_size = 0;
    cpuaddr     end_addr;

    /* 
    ** Returns the address and size of the user-reserved region
    ** This is a Kernel function
    */
    userReservedGet((char **) &start_addr, &reserve_memory_size);
    memset(&CFE_PSP_ReservedMemoryMap, 0, sizeof(CFE_PSP_ReservedMemoryMap));
    g_uiEndOfRam = (cpuaddr)sysPhysMemTop();

    end_addr = start_addr;

    /* There are a handful of reserved memory sections that 
    ** we need to setup, contained in (a struct) CFE_PSP_ReservedMemoryMap_t:
    **      1. (CFE_PSP_ReservedMemoryBootRecord_t *)   Boot Ptr
    **      2. (CFE_PSP_ExceptionStorage_t *)           Exception Storage Ptr
    **      3. (CFE_PSP_MemoryBlock_t)                  Reset Memory
    **      4. (CFE_PSP_MemoryBlock_t)                  Volatile Disk
    **      5. (CFE_PSP_MemoryBlock_t)                  CDS
    **      6. (CFE_PSP_MemoryBlock_t)                  User Reserved
    **
    */

    /* 1. (CFE_PSP_ReservedMemoryBootRecord_t *) Boot Ptr */
    CFE_PSP_ReservedMemoryMap.BootPtr = (CFE_PSP_ReservedMemoryBootRecord_t *)end_addr;
    end_addr += sizeof(CFE_PSP_ReservedMemoryBootRecord_t);
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* 2. (CFE_PSP_ExceptionStorage_t *) Exception Storage Ptr */
    CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr = (CFE_PSP_ExceptionStorage_t *)end_addr;
    end_addr += sizeof(CFE_PSP_ExceptionStorage_t);
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* 3. (CFE_PSP_MemoryBlock_t Reset Memory */
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->ResetAreaSize;
    end_addr += CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* 4. (CFE_PSP_MemoryBlock_t) Volatile Disk */
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize =
        GLOBAL_CONFIGDATA.CfeConfig->RamDiskSectorSize * GLOBAL_CONFIGDATA.CfeConfig->RamDiskTotalSectors;
    end_addr += CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* 5. (CFE_PSP_MemoryBlock_t) CDS */
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->CdsSize;
    end_addr += CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* 6. (CFE_PSP_MemoryBlock_t) User Reserved */
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockPtr = (void*)end_addr;
    CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize = GLOBAL_CONFIGDATA.CfeConfig->UserReservedSize;
    end_addr += CFE_PSP_ReservedMemoryMap.UserReservedMemory.BlockSize;
    end_addr = (end_addr + CFE_PSP_MEMALIGN_MASK) & ~CFE_PSP_MEMALIGN_MASK;

    /* The total size of the entire block is the difference in address */
    GR740_ReservedMemBlock.BlockPtr = (void*)start_addr;
    GR740_ReservedMemBlock.BlockSize =  end_addr - start_addr;

    OS_printf(MEMORY_PRINT_SCOPE "GR740 Reserved Memory Block at 0x%08lx, Total Size = 0x%lx\n",
            (unsigned long)GR740_ReservedMemBlock.BlockPtr,
            (unsigned long)GR740_ReservedMemBlock.BlockSize);

    /*
    ** Set up the "RAM" entry in the memory table.
    **
    ** What is the purpose of this? Should we be more detailed with our set?
    ** We don't actually use all of RAM (nor should we at a cFS level?)
    */
    CFE_PSP_MemRangeSet(0, CFE_PSP_MEM_RAM, 0, g_uiEndOfRam, CFE_PSP_MEM_SIZE_DWORD, CFE_PSP_MEM_ATTR_READWRITE);
}

/******************************************************************************
 * Function: CFE_PSP_DeleteProcessorReservedMemory
 *
 * No action on GR740 - reserved block is statically allocated at sysMemTop.
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

/**********************************************************
 * 
 * Function: CFE_PSP_GetKernelTextSegmentInfo
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_GetKernelTextSegmentInfo(cpuaddr *PtrToKernelSegment, uint32 *SizeOfKernelSegment)
{
   int32 iReturnCode = CFE_PSP_SUCCESS;
   cpuaddr StartAddress = 0;
   cpuaddr EndAddress = 0;
   
   if ((PtrToKernelSegment == NULL) || (SizeOfKernelSegment == NULL))
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

/**********************************************************
 * 
 * Function: CFE_PSP_GetCFETextSegmentInfo
 * 
 * Description: See function declaration for info
 * 
 *********************************************************/
int32 CFE_PSP_GetCFETextSegmentInfo(cpuaddr *PtrToCFESegment, uint32 *SizeOfCFESegment)
{
    int32       return_code = CFE_PSP_ERROR;
    STATUS      status = OK;
    MODULE_ID   cFEModuleId = NULL;
    MODULE_INFO cFEModuleInfo = {};
    cpuaddr     GetModuleIdAddr = 0;
    MODULE_ID   (*GetModuleIdFunc)(void) = NULL;

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
            GetModuleIdFunc = (MODULE_ID(*)(void))GetModuleIdAddr;
            cFEModuleId     = GetModuleIdFunc();
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
    return CFE_PSP_GetMemSize(size, OP_RESET);
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

    iReturnCode = CFE_PSP_GetMemArea(PtrToResetArea, SizeOfResetArea, OP_RESET);

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
int32 CFE_PSP_ReadFromRESET(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_ReadFromRAM(p_data, offset, size, OP_RESET);
}

int32 CFE_PSP_WriteToRESET(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_WriteToRAM(p_data, offset, size, OP_RESET);
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

    iReturnCode = CFE_PSP_GetMemSize(SizeOfCDS, OP_CDS);
    
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
    return CFE_PSP_GetMemArea(p_area, p_size, OP_CDS);
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

    iReturnCode = CFE_PSP_ReadFromRAM(PtrToDataFromRead, CDSOffset, NumBytes, OP_CDS);

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

    iReturnCode = CFE_PSP_WriteToRAM(PtrToDataToWrite, CDSOffset, NumBytes, OP_CDS);

    if (iReturnCode != CFE_PSP_SUCCESS)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
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
    return CFE_PSP_GetMemSize(size, OP_VOLATILEDISK);   
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
    return CFE_PSP_GetMemArea(PtrToVolDisk, SizeOfVolDisk, OP_VOLATILEDISK);
}

/**********************************************************
 * 
 * Function: CFE_PSP_ReadFromVOLATILEDISK
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_ReadFromVOLATILEDISK(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_ReadFromRAM(p_data, offset, size, OP_VOLATILEDISK);
}

/**********************************************************
 * 
 * Function: CFE_PSP_WriteToVOLATILEDISK
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_WriteToVOLATILEDISK(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_WriteToRAM(p_data, offset, size, OP_VOLATILEDISK);
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
    return CFE_PSP_GetMemSize(size, OP_USERRESERVED);
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
    return CFE_PSP_GetMemArea(PtrToUserArea, SizeOfUserArea, OP_USERRESERVED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_ReadFromUSERRESERVED
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_ReadFromUSERRESERVED(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_ReadFromRAM(p_data, offset, size, OP_USERRESERVED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_WriteToUSERRESERVED
 * 
 * Description: See function declaration for info
 * 
 **********************************************************/
int32 CFE_PSP_WriteToUSERRESERVED(const void *p_data, uint32 offset, uint32 size)
{
    return CFE_PSP_WriteToRAM(p_data, offset, size, OP_USERRESERVED);
}

/**********************************************************
 * 
 * REUSABLE RESERVED MEMORY SECTION HANDLING
 * 
 **********************************************************/

/**
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
int32 CFE_PSP_GetMemSize(uint32 *p_size, MEMORY_SECTION_t op)
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

/**
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
int32 CFE_PSP_GetMemArea(cpuaddr *p_area, uint32 *p_size, MEMORY_SECTION_t op)
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

/**
 ** \brief Read data from RAM
 **
 ** \par Description:
 ** Read data to a specific reserved memory section
 **
 ** \par Assumptions, External Events, and Notes:
 ** No assumptions can be made in the type of data that is being copied
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
int32 CFE_PSP_ReadFromRAM(const void *p_data, uint32 offset, uint32 size, MEMORY_SECTION_t op)
{
    uint8                   *pCopyPtr       = NULL;
    int32                   iReturnCode     = CFE_PSP_SUCCESS;
    CFE_PSP_MemoryBlock_t   memBlock        = {0, 0};

    if (p_data == NULL)
    {
        OS_printf(MEMORY_PRINT_SCOPE "ReadFromRAM: NULL Pointer\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        switch (op)
        {
            case OP_CDS:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
                break;

            case OP_RESET:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
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
            /* Copy binary data to provided buffer */
            memcpy((void *)p_data, pCopyPtr, size);
        }
    }

    return iReturnCode;
}

/**
 ** \brief Write data to RAM
 **
 ** \par Description:
 ** Write data to a specific reserved memory section
 **
 ** \par Assumptions, External Events, and Notes:
 ** No assumptions can be made in the type of data that is being copied
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
int32 CFE_PSP_WriteToRAM(const void *p_data, uint32 offset, uint32 size, MEMORY_SECTION_t op)
{
    uint8                   *pCopyPtr       = NULL;
    int32                   iReturnCode     = CFE_PSP_ERROR;
    CFE_PSP_MemoryBlock_t   memBlock        = {0, 0};

    if (p_data == NULL)
    {
        OS_printf(MEMORY_PRINT_SCOPE "WriteToRAM: NULL Pointer\n");
        iReturnCode = CFE_PSP_INVALID_POINTER;
    }
    else
    {
        switch (op)
        {
            case OP_CDS:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize;
                break;

            case OP_RESET:
                memBlock.BlockPtr = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr;
                memBlock.BlockSize = CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize;
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
            pCopyPtr = memBlock.BlockPtr;
            pCopyPtr += offset;

            /* Copy binary data to RAM */
            memcpy(pCopyPtr, p_data, size);

            iReturnCode = CFE_PSP_SUCCESS;
        }
    }

    return iReturnCode;
}