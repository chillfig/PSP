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
*   \file cfe_psp_memory.h
*
*   \brief Header file for the Reserved Memory-related supporting functions
*
*   \brief Header file containing the function declarations to initialize, manage, and delete Reserved Memory
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was cloned-n-owned at the NASA Johnson Space Center from the NASA cFE open source 
*   with license below.
**********************************************************************************************************/

/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

#ifndef CFE_PSP_MEMORY_H
#define CFE_PSP_MEMORY_H

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
    /** \brief  Volatile Disk Memory  */
    CFE_PSP_MemoryBlock_t VolatileDiskMemory;
    /** \brief  CDS Memory  */
    CFE_PSP_MemoryBlock_t CDSMemory;
    /** \brief  User Reserved Memory  */
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
 ** \brief Memory sections
 **/
typedef enum
{
    /** \brief Critical Device Storage section*/
    OP_CDS          = 0,
    /** \brief RESET section*/
    OP_RESET        = 1,
    /** \brief Volatile Disk section*/
    OP_VOLATILEDISK = 2,
    /** \brief User Reserved section*/
    OP_USERRESERVED = 3,
    /** \brief Boot Record section*/
    OP_BOOTRECORD = 4,
    /** \brief Exception Storage section*/
    OP_EXCEPTION = 5,
    /** \brief Not Applicable section. Used as a No-Op */
    OP_NA           = 6
} MEMORY_SECTION_t;

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

/**
 ** \func Get the size of the RESET memory area
 **
 ** \par Description:
 ** This function fetches the size of the RESET memory area.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] size - Pointer to the variable that stores the returned memory size
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR;
 */
int32 CFE_PSP_GetRESETSize(uint32 *size);

/**
 ** \func Write to the RESET memory area
 **
 ** \par Description:
 ** This function write the specified data to the specified memory area of the RESET.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] p_data - Pointer to the data buffer to be written
 ** \param[in] offset - Memory offset from the beginning of the memory block
 ** \param[in] size - Number of bytes to be written
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_INVALID_MEM_RANGE
 */
int32 CFE_PSP_WriteToRESET(const void *p_data, uint32 offset, uint32 size);

/**
 ** \func Read from the RESET memory area
 **
 ** \par Description:
 ** Read the specified data from the specified memory area of the RESET.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Assume that data read here is correct
 **
 ** \param[in] p_data - Pointer to store read data
 ** \param[in] offset - Memory offset from the beginning of the memory block
 ** \param[in] size - Number of bytes to be read
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_INVALID_MEM_RANGE
 */
int32 CFE_PSP_ReadFromRESET(const void *p_data, uint32 offset, uint32 size);

/**
 ** \func Get the size of the VOLATILEDISK memory area
 **
 ** \par Description:
 ** This function fetches the size of the VOLATILEDISK memory area.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] size - Pointer to the variable that stores the returned memory size
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR;
 */
int32 CFE_PSP_GetVOLATILEDISKSize(uint32 *size);

/**
 ** \func Write to the VOLATILEDISK memory area
 **
 ** \par Description:
 ** This function write the specified data to the specified memory area of the VOLATILEDISK.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] p_data - Pointer to the data buffer to be written
 ** \param[in] offset - Memory offset from the beginning of the memory block
 ** \param[in] size - Number of bytes to be written
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_INVALID_MEM_RANGE
 */
int32 CFE_PSP_WriteToVOLATILEDISK(const void *p_data, uint32 offset, uint32 size);

/**
 ** \func Read from the VOLATILEDISK memory area
 **
 ** \par Description:
 ** Read the specified data from the specified memory area of the VOLATILEDISK.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Memory Scrubbing only fixes 1 error per block, and no more than that. 
 ** In case there are more than 1 errors in the RMA memory used by the User Reserved Memory, 
 ** reading using CFE_PSP_ReadFromVOLATILEDISK will produce incorrect data.
 **
 ** \param[in] p_data - Pointer to store read data
 ** \param[in] offset - Memory offset from the beginning of the memory block
 ** \param[in] size - Number of bytes to be read
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_INVALID_MEM_RANGE
 */
int32 CFE_PSP_ReadFromVOLATILEDISK(const void *p_data, uint32 offset, uint32 size);

/**
 ** \func Get the size of the USERRESERVED memory area
 **
 ** \par Description:
 ** This function fetches the size of the USERRESERVED memory area.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] size - Pointer to the variable that stores the returned memory size
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR;
 */
int32 CFE_PSP_GetUSERRESERVEDSize(uint32 *size);

/**
 ** \func Write to the USERRESERVED memory area
 **
 ** \par Description:
 ** This function write the specified data to the specified memory area of the USERRESERVED.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] p_data - Pointer to the data buffer to be written
 ** \param[in] offset - Memory offset from the beginning of the memory block
 ** \param[in] size - Number of bytes to be written
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_INVALID_MEM_RANGE
 */
int32 CFE_PSP_WriteToUSERRESERVED(const void *p_data, uint32 offset, uint32 size);

/**
 ** \func Read from the USERRESERVED memory area
 **
 ** \par Description:
 ** Read the specified data from the specified memory area of the USERRESERVED.
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** Assume that data read here is correct
 **
 ** \param[in] p_data - Pointer to store read data
 ** \param[in] offset - Memory offset from the beginning of the memory block
 ** \param[in] size - Number of bytes to be read
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_INVALID_MEM_RANGE
 */
int32 CFE_PSP_ReadFromUSERRESERVED(const void *p_data, uint32 offset, uint32 size);

/**
 ** \func Get pointer ot the CDS memory area
 **
 ** \par Description:
 ** This function fetches the area of the CDS memory
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] p_area - Pointer to the variable that stores returned memory address
 ** \param[out] p_size - Size of data
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_INVALID_POINTER
 ** \return #CFE_PSP_ERROR;
 */
int32 CFE_PSP_GetCDSArea(cpuaddr *p_area, uint32 *p_size);

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

#endif
