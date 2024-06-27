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
 * I/O adapter for discrete (digitial gpio) interfaces
 */

#ifndef CFE_PSP_IODRIVER_DISCRETE_IO_H
#define CFE_PSP_IODRIVER_DISCRETE_IO_H

/**
 * Type abstraction for expressing digital logic levels.
 *
 * This value will be filled starting with the LSB. A typical GPIO logic channel is 1 bit, so
 * only the LSB is signficiant and the other bits are not used.
 *
 * This allows single channels up to 8 bits wide, but multiple "channels" could be concatenated
 * using a multi-read/write opcode to allow atomic access to any number of bits.
 */
typedef uint8 CFE_PSP_IODriver_GpioLevel_t;

/**
 * Enumerated names for typical digital 1-bit logic channel states.
 *
 * For convenience / code readability.
 */
enum
{
    CFE_PSP_IODriver_GPIO_LOGIC_LOW  = 0,
    CFE_PSP_IODriver_GPIO_LOGIC_HIGH = 1
};

/**
 * Complete API container for gpio read/write commands.
 * This allows reading/writing multiple channels at once with a single entry into the API.
 * As each entry into the API needs to acquire a mutex for serialization, this can be much
 * more efficient to read channels through this means rather than single channel read/write.
 */
typedef struct
{
    uint16 NumChannels;                    /**<  Number of channels in the i/o structure (length of "samples" array) */
    CFE_PSP_IODriver_GpioLevel_t *Samples; /**<  Array for digital logic levels */
} CFE_PSP_IODriver_GpioRdWr_t;

/**
 * Opcodes specific to digital GPIO devices
 */
enum
{
    CFE_PSP_IODriver_DISCRETE_IO_NOOP = CFE_PSP_IODriver_DISCRETE_IO_CLASS_BASE,

    CFE_PSP_IODriver_DISCRETE_IO_READ_CHANNELS,  /**< CFE_PSP_IODriver_GpioRdWr_t argument */
    CFE_PSP_IODriver_DISCRETE_IO_WRITE_CHANNELS, /**< CFE_PSP_IODriver_GpioRdWr_t argument */

    CFE_PSP_IODriver_DISCRETE_IO_MAX
};

#endif /* CFE_PSP_IODRIVER_DISCRETE_IO_H */
