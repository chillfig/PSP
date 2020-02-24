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

#include "common_types.h"
#include "osapi.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

#include "cfe_psp.h"
#include "cfe_psp_memory.h"


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

/*
** Pointer to the vxWorks USER_RESERVED_MEMORY area
** The sizes of each memory area is defined in os_processor.h for this architecture.
*/
CFE_PSP_ReservedMemory_t *CFE_PSP_ReservedMemoryPtr;


/******************************************************************************
** CDS related functions
******************************************************************************/

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
int32 CFE_PSP_GetCDSSize(uint32 *pSizeOfCds)
{
    int32 retCode = CFE_PSP_ERROR;

    if (pSizeOfCds != NULL)
    {
        *pSizeOfCds = CFE_PSP_CDS_SIZE;
        retCode = CFE_PSP_SUCCESS;
    }

    return (retCode);
}


/******************************************************************************
**  Function: CFE_PSP_WriteToCDS
**
**  Purpose:
**    This function writes to the CDS Block.
**
**  Arguments:
**    Input - ptrToInputBuffer - Pointer to the input data buffer
**    Input - cdsOffset - Memory offset of CDS
**    Input - numBytes - Number of bytes to write to CDS
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_WriteToCDS(const void *ptrToInputBuffer, uint32 cdsOffset, uint32 numBytes)
{
    uint8 *copyPtr = NULL;
    int32 retCode = CFE_PSP_ERROR;

    if (ptrToInputBuffer != NULL)
    {
        if ((cdsOffset < CFE_PSP_CDS_SIZE) &&
            ((cdsOffset + numBytes) <= CFE_PSP_CDS_SIZE))
        {
            copyPtr = &(CFE_PSP_ReservedMemoryPtr->CDSMemory[cdsOffset]);
            memcpy(copyPtr, (char *)ptrToInputBuffer, numBytes);

            retCode = CFE_PSP_SUCCESS;
        }
    }

    return (retCode);
}


/******************************************************************************
**  Function: CFE_PSP_ReadFromCDS
**
**  Purpose:
**    This function reads from the CDS Block.
**
**  Arguments:
**    Output - ptrToOutputBuffer - Pointer to output data buffer
**    Input  - cdsOffset - Memory offset of CDS
**    Input  - numBytes - Number of bytes to read from CDS
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_ReadFromCDS(void *ptrToOutputBuffer, uint32 cdsOffset, uint32 numBytes)
{
    uint8 *copyPtr = NULL;
    int32 retCode = CFE_PSP_ERROR;

    if (ptrToOutputBuffer != NULL)
    {
        if ((cdsOffset < CFE_PSP_CDS_SIZE) &&
            ((cdsOffset + numBytes) <= CFE_PSP_CDS_SIZE))
        {
            copyPtr = &(CFE_PSP_ReservedMemoryPtr->CDSMemory[cdsOffset]);
            memcpy((char *)ptrToOutputBuffer, copyPtr, numBytes);

            retCode = CFE_PSP_SUCCESS;
        }
    }

    return (retCode);
}


/******************************************************************************
** ES Reset Area related functions
******************************************************************************/

/******************************************************************************
**  Function: CFE_PSP_GetResetArea
**
**  Purpose:
**     This function returns the location and size of the ES Reset information area.
**     This area is preserved during a processor reset and is used to store the
**     ER Log, System Log and reset related variables
**
**  Arguments:
**    Output - ptrToResetArea - Pointer to the reset memory address
**    Output - pSizeOfResetArea - Pointer to variable that stores size of memory
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetResetArea(cpuaddr *ptrToResetArea, 
                           uint32  *pSizeOfResetArea)
{
    int32 retCode = CFE_PSP_ERROR;

    if ((ptrToResetArea != NULL) && (pSizeOfResetArea != NULL))
    {
        *ptrToResetArea   = (cpuaddr)&(CFE_PSP_ReservedMemoryPtr->ResetMemory[0]);
        *pSizeOfResetArea = CFE_PSP_RESET_AREA_SIZE;

        retCode = CFE_PSP_SUCCESS;
    }

    return (retCode);
}


/******************************************************************************
** ES User Reserved Area related functions
******************************************************************************/

/******************************************************************************
**  Function: CFE_PSP_GetUserReservedArea
**
**  Purpose:
**    This function returns the location and size of the memory used for the cFE
**    user-reserved area.
**
**  Arguments:
**    Output - ptrToUserArea - Pointer to user-reserved memory address
**    Output - pSizeOfUserArea - Pointer to variable that stores size of memory
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetUserReservedArea(cpuaddr *ptrToUserArea, 
                                  uint32  *pSizeOfUserArea)
{
    int32 retCode = CFE_PSP_ERROR;

    if ((ptrToUserArea != NULL) && (pSizeOfUserArea != NULL))
    {
        *ptrToUserArea   = (cpuaddr)&(CFE_PSP_ReservedMemoryPtr->UserReservedMemory[0]);
        *pSizeOfUserArea = CFE_PSP_USER_RESERVED_SIZE;

        retCode = CFE_PSP_SUCCESS;
    }

    return (retCode);
}


/******************************************************************************
** ES Volatile disk memory related functions
******************************************************************************/

/******************************************************************************
**  Function: CFE_PSP_GetVolatileDiskMem
**
**  Purpose:
**    This function returns the location and size of the memory used for the cFE
**     volatile disk.
**
**  Arguments:
**    Output - ptrToVolDisk - Pointer to the volatile disk memory address
**    Output - pSizeOfVolDisk - Pointer to variable that stores size of memory
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetVolatileDiskMem(cpuaddr *ptrToVolDisk, 
                                 uint32  *pSizeOfVolDisk)
{
    int32 retCode = CFE_PSP_ERROR;

    if ((ptrToVolDisk != NULL) && (pSizeOfVolDisk != NULL))
    {
        *ptrToVolDisk   = (cpuaddr)&(CFE_PSP_ReservedMemoryPtr->VolatileDiskMemory[0]);
        *pSizeOfVolDisk = CFE_PSP_VOLATILE_DISK_SIZE;

        retCode = CFE_PSP_SUCCESS;
    }

    return (retCode);
}


/******************************************************************************
** ES BSP Top Level Reserved memory initialization
******************************************************************************/

/******************************************************************************
**  Function: CFE_PSP_InitProcessorReservedMemory
**
**  Purpose:
**    This function performs the top level reserved memory initialization.
**
**  Arguments:
**    Input - restartType - type of re-start
**
**  Return:
**    CFE_PSP_SUCCESS
******************************************************************************/
int32 CFE_PSP_InitProcessorReservedMemory(uint32 restartType)
{
    if (restartType != CFE_ES_PROCESSOR_RESET)
    {
        logMsg("CFE_PSP: Clearing Processor Reserved Memory.\n", 0,0,0,0,0,0);
        memset((void *)CFE_PSP_ReservedMemoryPtr, 0, 
               sizeof(CFE_PSP_ReservedMemory_t));
    }

    CFE_PSP_ReservedMemoryPtr->bsp_last_reset_type = 
        CFE_PSP_ReservedMemoryPtr->bsp_reset_type;
    CFE_PSP_ReservedMemoryPtr->bsp_reset_type = CFE_ES_PROCESSOR_RESET;

    return (CFE_PSP_SUCCESS);
}


/******************************************************************************
** ES BSP kernel memory segment functions
******************************************************************************/

/******************************************************************************
**  Function: CFE_PSP_GetKernelTextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the kernel text segment.
**    It may not be implemented on all architectures.
**
**  Arguments:
**    Output - ptrToKernelSegment - Pointer to kernel segment memory address
**    Output - pSizeOfKernelSegment - Pointer to variable that stores memory size
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetKernelTextSegmentInfo(cpuaddr *ptrToKernelSegment, 
                                       uint32  *pSizeOfKernelSegment)
{
    int32 retCode = CFE_PSP_ERROR;
    cpuaddr startAddress = 0;
    cpuaddr endAddress = 0;

    if ((ptrToKernelSegment != NULL) && (pSizeOfKernelSegment != NULL))
    {
        /* Get the kernel start and end addresses from the BSP, because the
        ** symbol table does not contain the symbols we need for this. */
        startAddress = (cpuaddr)GetWrsKernelTextStart();
        endAddress = (cpuaddr)GetWrsKernelTextEnd();

        *ptrToKernelSegment   = (cpuaddr)startAddress;
        *pSizeOfKernelSegment = (uint32)(endAddress - startAddress);

        retCode = CFE_PSP_SUCCESS;
    }

    return (retCode);
}


/******************************************************************************
**  Function: CFE_PSP_GetCFETextSegmentInfo
**
**  Purpose:
**    This function returns the start and end address of the CFE text segment.
**    It may not be implemented on all architectures.
**
**  Arguments:
**    Output - ptrToCFESegment - Pointer to CFE segment memory address
**    Output - pSizeOfCFESegment - Pointer to variable that stores memory size
**
**  Return:
**    CFE_PSP_SUCCESS
**    CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_GetCFETextSegmentInfo(cpuaddr *ptrToCFESegment, 
                                    uint32  *pSizeOfCFESegment)
{
    int32 retCode = CFE_PSP_ERROR;
    MODULE_ID cFEModuleId;
    MODULE_INFO cFEModuleInfo;

    if ((ptrToCFESegment != NULL) && (pSizeOfCFESegment != NULL))
    {
        cFEModuleId = moduleFindByName(CFE_MODULE_NAME);
        if (cFEModuleId != NULL)
        {
            if (moduleInfoGet(cFEModuleId, &cFEModuleInfo) != ERROR)
            {
                *ptrToCFESegment   = (cpuaddr)(cFEModuleInfo.segInfo.textAddr);
                *pSizeOfCFESegment = (uint32)(cFEModuleInfo.segInfo.textSize);

                retCode = CFE_PSP_SUCCESS;
            }
        }
    }

    return (retCode);
}


