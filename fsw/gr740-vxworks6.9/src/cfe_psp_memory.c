/**
 ** \file
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

/******************************************************************************
** File:  cfe_psp_memory.c
**
**      GR740 vxWorks 6.x Version
**
** Purpose:
**   cFE PSP Memory related functions. This is the implementation of the cFE 
**   memory areas that have to be preserved, and the API that is designed to allow
**   access to them. It also contains memory related routines to return the
**   address of the kernel code used in the cFE checksum.
**
** History:
**   2006/09/29  A. Cudmore      | vxWorks 6.2 GR740 version
**   2016/04/07  M. Grubb        | Updated for PSP version 1.3
**   2020/06/03  J. Pham         | Modified for gr740 SPARC LEON4 board. Portions pulled from UT700 PSP
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
**    (none)
**
**  Return:
**    (none)
*/

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
**  Function: CFE_PSP_WriteToCDS
**
**  Purpose:
**    This function writes to the CDS Block.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
int32 CFE_PSP_WriteToCDS(const void *PtrToDataToWrite, uint32 CDSOffset, uint32 NumBytes)
{
   uint8 *CopyPtr;
   int32  return_code;
         
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
           memcpy(CopyPtr, (char *)PtrToDataToWrite,NumBytes);
          
           return_code = CFE_PSP_SUCCESS;
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
**   This function reads from the CDS Block
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/

int32 CFE_PSP_ReadFromCDS(void *PtrToDataToRead, uint32 CDSOffset, uint32 NumBytes)
{
   uint8 *CopyPtr;
   int32  return_code;
      
   if ( PtrToDataToRead == NULL )
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
           memcpy((char *)PtrToDataToRead,CopyPtr, NumBytes);
          
           return_code = CFE_PSP_SUCCESS;
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
**    (none)
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
*/
int32 CFE_PSP_GetResetArea(cpuaddr *PtrToResetArea, uint32 *SizeOfResetArea)
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
**     User reserved area.
**
**  Arguments:
**    (none)
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
*/
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
**    (none)
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
*/
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
    if(GR740_ReservedMemBlock.BlockSize > reserve_memory_size)
    {
        OS_printf("CFE_PSP: VxWorks Reserved Memory Block Size not large enough, "
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
            OS_printf("CFE_PSP: Clearing Processor Reserved Memory.\n");

            /* Initialize memory sections */
            memset(CFE_PSP_ReservedMemoryMap.ResetMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.ResetMemory.BlockSize);
            memset(CFE_PSP_ReservedMemoryMap.CDSMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.CDSMemory.BlockSize);
            memset(CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockPtr,
                    0, CFE_PSP_ReservedMemoryMap.VolatileDiskMemory.BlockSize);
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
    uint32      reserve_memory_size = 0;
    int32       iStatus = OS_SUCCESS;
    cpuaddr     end_addr;

    /* 
    ** Returns the address and size of the user-reserved region
    ** This is a Kernel function
    */
    userReservedGet((char **) &start_addr, &reserve_memory_size);
    memset(&CFE_PSP_ReservedMemoryMap, 0, sizeof(CFE_PSP_ReservedMemoryMap));
    g_uiEndOfRam = (uint32)sysPhysMemTop();

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

    OS_printf("CFE_PSP: GR740 Reserved Memory Block at 0x%08lx, Total Size = 0x%lx\n",
            (unsigned long)GR740_ReservedMemBlock.BlockPtr,
            (unsigned long)GR740_ReservedMemBlock.BlockSize);

    /*
    ** Set up the "RAM" entry in the memory table.
    **
    ** What is the purpose of this? Should we be more detailed with our set?
    ** We don't actually use all of RAM (nor should we at a cFS level?)
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
   int32 return_code = CFE_PSP_SUCCESS;
   cpuaddr StartAddress = 0;
   cpuaddr EndAddress = 0;
   
   if ((PtrToKernelSegment == NULL) || (SizeOfKernelSegment == NULL))
   {
      return_code = CFE_PSP_ERROR;
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
      
      return_code = CFE_PSP_SUCCESS;
   }
   
   return(return_code);
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

