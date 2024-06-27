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
 * @file
 *
 * Auto-Generated stub implementations for functions defined in iodriver_base header
 */

#include "iodriver_base.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_IODriver_Command()
 * ----------------------------------------------------
 */
int32 CFE_PSP_IODriver_Command(const CFE_PSP_IODriver_Location_t *Location, uint32 CommandCode,
                               CFE_PSP_IODriver_Arg_t Arg)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_IODriver_Command, int32);

    UT_GenStub_AddParam(CFE_PSP_IODriver_Command, const CFE_PSP_IODriver_Location_t *, Location);
    UT_GenStub_AddParam(CFE_PSP_IODriver_Command, uint32, CommandCode);
    UT_GenStub_AddParam(CFE_PSP_IODriver_Command, CFE_PSP_IODriver_Arg_t, Arg);

    UT_GenStub_Execute(CFE_PSP_IODriver_Command, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_IODriver_Command, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_IODriver_FindByName()
 * ----------------------------------------------------
 */
int32 CFE_PSP_IODriver_FindByName(const char *DriverName, uint32 *PspModuleId)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_IODriver_FindByName, int32);

    UT_GenStub_AddParam(CFE_PSP_IODriver_FindByName, const char *, DriverName);
    UT_GenStub_AddParam(CFE_PSP_IODriver_FindByName, uint32 *, PspModuleId);

    UT_GenStub_Execute(CFE_PSP_IODriver_FindByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_IODriver_FindByName, int32);
}
