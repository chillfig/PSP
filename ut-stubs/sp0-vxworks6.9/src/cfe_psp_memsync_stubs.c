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
*   \file cfe_psp_memsync_stubs.c
*
*   \brief
*   API header to control memory sync to filesystem
*
*   \copyright
*   Copyright (c) 2019-2021 United States Government as represented by
*   the Administrator of the National Aeronautics and Space Administration.
*   All Rights Reserved.
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   \par Description:
*   This filel contains the function prototypes that synchronize data in
*   RAM/memory to a specific file on filesystem.
*
*   \par Limitations, Assumptions, External Events, and Notes:
*   None
**********************************************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_memsync header
 */

#include "cfe_psp_memsync.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncDestroy()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemSyncDestroy(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncDestroy, int32);

    UT_GenStub_Execute(CFE_PSP_MemSyncDestroy, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncDestroy, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncGetFrequency()
 * ----------------------------------------------------
 */
uint32 CFE_PSP_MemSyncGetFrequency(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncGetFrequency, uint32);

    UT_GenStub_Execute(CFE_PSP_MemSyncGetFrequency, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncGetFrequency, uint32);
}

/*
 * ----------------------------------------------------
 * Generated/Modified stub function for CFE_PSP_MemSyncGetPriority()
 * ----------------------------------------------------
 */
osal_priority_t CFE_PSP_MemSyncGetPriority(void)
{
    int32 iReturnValue = OS_ERROR;

    iReturnValue = UT_DEFAULT_IMPL(CFE_PSP_MemSyncGetPriority);

    return (iReturnValue);
}


/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncGetStatistics()
 * ----------------------------------------------------
 */
uint32 CFE_PSP_MemSyncGetStatistics(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncGetStatistics, uint32);

    UT_GenStub_Execute(CFE_PSP_MemSyncGetStatistics, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncGetStatistics, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncInit()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemSyncInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncInit, int32);

    UT_GenStub_Execute(CFE_PSP_MemSyncInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated/Modified stub function for CFE_PSP_MemSyncIsRunning()
 * ----------------------------------------------------
 */
bool CFE_PSP_MemSyncIsRunning(void)
{
    int32 iReturnValue = OS_ERROR;

    iReturnValue = UT_DEFAULT_IMPL(CFE_PSP_MemSyncIsRunning);

    return (iReturnValue);
}


/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncSetFrequency()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemSyncSetFrequency(uint32 sec_time)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncSetFrequency, int32);

    UT_GenStub_AddParam(CFE_PSP_MemSyncSetFrequency, uint32, sec_time);

    UT_GenStub_Execute(CFE_PSP_MemSyncSetFrequency, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncSetFrequency, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncSetPriority()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemSyncSetPriority(osal_priority_t priority)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncSetPriority, int32);

    UT_GenStub_AddParam(CFE_PSP_MemSyncSetPriority, osal_priority_t, priority);

    UT_GenStub_Execute(CFE_PSP_MemSyncSetPriority, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncSetPriority, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncStart()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemSyncStart(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncStart, int32);

    UT_GenStub_Execute(CFE_PSP_MemSyncStart, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncStart, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemSyncStop()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemSyncStop(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemSyncStop, int32);

    UT_GenStub_Execute(CFE_PSP_MemSyncStop, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemSyncStop, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_FlushToFLASH()
 * ----------------------------------------------------
 */
void CFE_PSP_FlushToFLASH(void)
{
    UT_GenStub_Execute(CFE_PSP_FlushToFLASH, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_CheckURMFilesExists()
 * ----------------------------------------------------
 */
int32 CFE_PSP_CheckURMFilesExists(void)
{
    UT_GenStub_Execute(CFE_PSP_CheckURMFilesExists, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_CheckURMFilesExists, int32);
}
