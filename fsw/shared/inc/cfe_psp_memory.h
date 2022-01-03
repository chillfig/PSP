/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/**
 ** \file  cfe_psp_memory.h
 **
 ** \brief Header file for the Reserved Memory-related supporting functions
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved.\n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description:
 ** Header file containing the function declarations to initialize, manage, and 
 ** delete Reserved Memory
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

#ifndef _cfe_psp_memory_
#define _cfe_psp_memory_

/*
** Include Files
*/

#include "common_types.h"
#include "cfe_psp_config.h"
#include "cfe_psp_exceptionstorage_types.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

/**
** \brief Memory Table Type
*/
typedef struct
{
    /** \brief  Memory Type */
    uint32  MemoryType;
    /** \brief  Word Size  */
    size_t  WordSize;
    /** \brief  Start Address  */
    cpuaddr StartAddr;
    /** \brief  Size  */
    size_t  Size;
    /** \brief  Attributes */
    uint32  Attributes;

} CFE_PSP_MemTable_t;

/**
** \brief Memory Block Type
*/
typedef struct
{
    /** \brief  Block Pointer */
    void * BlockPtr;
    /** \brief  Block Size  */
    size_t BlockSize;

} CFE_PSP_MemoryBlock_t;

/**
** \brief Reserved Memory Map
*/
typedef struct
{
    /** \brief  Pointer to Reserved Memory Boot Record  */
    CFE_PSP_ReservedMemoryBootRecord_t *BootPtr;
    /** \brief  Pointer to Exception Storage  */
    CFE_PSP_ExceptionStorage_t *        ExceptionStoragePtr;
    /** \brief  Reset Memory  */
    CFE_PSP_MemoryBlock_t ResetMemory;
    /** \brief  Voltatile Disk Memory  */
    CFE_PSP_MemoryBlock_t VolatileDiskMemory;
    /** \brief  CDS Memory  */
    CFE_PSP_MemoryBlock_t CDSMemory;
    /** \brief  User Reservded Memory  */
    CFE_PSP_MemoryBlock_t UserReservedMemory;

    /**
     ** \brief The system memory table
     **
     ** \par Description:
     ** This is the table used for CFE_PSP_MemRangeGet/Set and related ops
     ** that allow CFE applications to query the general system memory map.
     */
    CFE_PSP_MemTable_t SysMemoryTable[CFE_PSP_MEM_TABLE_SIZE];

} CFE_PSP_ReservedMemoryMap_t;

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
void CFE_PSP_SetupReservedMemoryMap(void);

/**
 ** \func Initialize the processor's reserved memory
 **
 ** \par Description:
 ** This function initializes all of the memory in the BSP that is preserved on 
 ** a processor reset.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The memory includes the Critical Data Store, the ES Reset Area,
 ** the Volatile Disk Memory and the User Reserved Memory. Options include:
 ** \li #CFE_PSP_RST_TYPE_PROCESSOR
 ** \li #CFE_PSP_RST_TYPE_POWERON
 ** \li #CFE_PSP_RST_TYPE_MAX
 ** \n\n
 ** This initializes based on the reset type.  Typically, the information
 ** is preserved on a processor reset, and cleared/reinitialized on a power-on
 ** reset.
 **
 ** \param[in] RestartType - The reset type
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32  CFE_PSP_InitProcessorReservedMemory(uint32 RestartType);

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
void CFE_PSP_DeleteProcessorReservedMemory(void);

/*
** External variables
*/

/**
 ** \brief Map to the reserved memory area(s)
 ** Contains a pointer to each of the separate memory blocks
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 */
extern CFE_PSP_ReservedMemoryMap_t CFE_PSP_ReservedMemoryMap;

/**
** \} <!-- End of group "psp_public_api" -->
*/

#endif /* _cfe_psp_memory_ */
