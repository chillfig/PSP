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
 * \file iodriver_impl.h
 *
 *  Created on: Oct 5, 2015
 *  Created by: joseph.p.hickey@nasa.gov
 *
 */

#ifndef IODRIVER_IMPL_H
#define IODRIVER_IMPL_H

#ifndef _CFE_PSP_MODULE_
#error "Do not include this file from outside the PSP"
#endif

#include "cfe_psp_module.h"
#include "iodriver_base.h"

/**
 * Macro to declare the global object for an IO device driver
 */
#define CFE_PSP_MODULE_DECLARE_IODEVICEDRIVER(name)         \
    static void         name##_Init(uint32 PspModuleId);    \
    CFE_PSP_ModuleApi_t CFE_PSP_##name##_API = {            \
        .ModuleType     = CFE_PSP_MODULE_TYPE_DEVICEDRIVER, \
        .OperationFlags = 0,                                \
        .Init           = name##_Init,                      \
        .ExtendedApi    = &name##_DevApi,                   \
    }

/** \brief Macro to define the mutex name of an IO device driver */
#define CFE_PSP_DRIVER_MUTEX_NAME  "DriverMutex-%02u"

/** \brief Verify that the driver mutex name is within required length */
CompileTimeAssert(sizeof(CFE_PSP_DRIVER_MUTEX_NAME) < OS_MAX_PATH_LEN, CFE_PSP_DRIVER_MUTEX_NAME_TOO_LONG);

/**
 * Prototype for a basic device command function
 * Implemented as a single API call with an extendible command code for device-specific ops.  This allows
 * a common API to be used while still allowing full freedom to handle many different device types.
 */
typedef int32 (*CFE_PSP_IODriver_ApiFunc_t)(uint32 CommandCode, uint16 Instance, uint16 SubChannel,
                                            CFE_PSP_IODriver_Arg_t arg);

typedef const struct
{
    CFE_PSP_IODriver_ApiFunc_t DeviceCommand;
    CFE_PSP_IODriver_ApiFunc_t DeviceMutex;
} CFE_PSP_IODriver_API_t;

osal_id_t CFE_PSP_IODriver_GetMutex(uint32 PspModuleId, int32 DeviceHash);
int32     CFE_PSP_IODriver_HashMutex(int32 StartHash, int32 Datum);

#endif /* IODRIVER_IMPL_H */
