/*
 *  Copyright (c) 2015, United States government as represented by the
 *  administrator of the National Aeronautics Space Administration.
 *  All rights reserved. This software was created at NASA Glenn
 *  Research Center pursuant to government contracts.
 */

/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/**
 * \file
 *
 * I/O adapter for memory device access
 */

#ifndef CFE_PSP_IODRIVER_MEMORY_IO_H
#define CFE_PSP_IODRIVER_MEMORY_IO_H

/* Include all base definitions */
#include "iodriver_base.h"

/**
 * API container for memory write commands.
 *
 * Associates a device address, buffer pointer and a buffer size.
 */
typedef struct
{
    uint32      DeviceAddress;
    uint32      BufferSize;
    const void *BufferMem;
} CFE_PSP_IODriver_WriteMemoryBuffer_t;

/**
 * API container for memory write commands.
 *
 * Associates a device address, buffer pointer and a buffer size.
 */
typedef struct
{
    uint32 DeviceAddress;
    uint32 BufferSize;
    void * BufferMem;
} CFE_PSP_IODriver_ReadMemoryBuffer_t;

/**
 * Opcodes specific to memory devices or other direct register-oriented interfaces
 */
enum
{
    CFE_PSP_IODriver_MEMORY_IO_NOOP = CFE_PSP_IODriver_MEMORY_IO_CLASS_BASE,

    CFE_PSP_IODriver_MEMORY_IO_READ_32,    /**< CFE_PSP_IODriver_ReadMemoryBuffer_t argument, use 32 bit access */
    CFE_PSP_IODriver_MEMORY_IO_WRITE_32,   /**< CFE_PSP_IODriver_WriteMemoryBuffer_t argument, use 32 bit access */
    CFE_PSP_IODriver_MEMORY_IO_READ_16,    /**< CFE_PSP_IODriver_ReadMemoryBuffer_t argument, use 16 bit access */
    CFE_PSP_IODriver_MEMORY_IO_WRITE_16,   /**< CFE_PSP_IODriver_WriteMemoryBuffer_t argument, use 16 bit access */
    CFE_PSP_IODriver_MEMORY_IO_READ_8,     /**< CFE_PSP_IODriver_ReadMemoryBuffer_t argument, use 8 bit access */
    CFE_PSP_IODriver_MEMORY_IO_WRITE_8,    /**< CFE_PSP_IODriver_WriteMemoryBuffer_t argument, use 8 bit access */
    CFE_PSP_IODriver_MEMORY_IO_READ_BLOCK, /**< CFE_PSP_IODriver_ReadMemoryBuffer_t argument, use any appropriate access
                                              cycle (generic) */
    CFE_PSP_IODriver_MEMORY_IO_WRITE_BLOCK, /**< CFE_PSP_IODriver_WriteMemoryBuffer_t argument, use any appropriate
                                               access cycle (generic) */

    CFE_PSP_IODriver_MEMORY_IO_MAX
};

#endif /* CFE_PSP_IODRIVER_MEMORY_IO_H */
