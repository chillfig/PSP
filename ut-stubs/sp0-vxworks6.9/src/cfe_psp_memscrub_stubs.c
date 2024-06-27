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
*   \file cfe_psp_memscrub_stubs.c
*
*   \brief API header to control Memory Scrubbing
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
*   This file contains the function prototypes relating to memory scrubbing.
*   This is specific to the SP0-S processor running VxWorks 6.9 OS.
*
*   \par Limitations, Assumptions, External Events, and Notes:
*   None
*
**********************************************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_memscrub header
 */

#include "cfe_psp_memscrub.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubDelete()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubDelete(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemScrubDelete, int32);

    UT_GenStub_Execute(CFE_PSP_MemScrubDelete, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubDelete, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubDisable()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubDisable(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemScrubDisable, int32);

    UT_GenStub_Execute(CFE_PSP_MemScrubDisable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubDisable, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubEnable()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubEnable(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemScrubEnable, int32);

    UT_GenStub_Execute(CFE_PSP_MemScrubEnable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubEnable, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubErrStats()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, size_t iErrSize, bool talkative)
{
    UT_GenStub_AddParam(CFE_PSP_MemScrubErrStats, CFE_PSP_MemScrubErrStats_t *, errStats);
    UT_GenStub_AddParam(CFE_PSP_MemScrubErrStats, size_t, iErrSize);
    UT_GenStub_AddParam(CFE_PSP_MemScrubErrStats, bool, talkative);

    UT_GenStub_Execute(CFE_PSP_MemScrubErrStats, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubErrStats, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubInit()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemScrubInit, int32);

    UT_GenStub_Execute(CFE_PSP_MemScrubInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubIsRunning()
 * ----------------------------------------------------
 */
bool CFE_PSP_MemScrubIsRunning(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemScrubIsRunning, bool);

    UT_GenStub_Execute(CFE_PSP_MemScrubIsRunning, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubIsRunning, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubSet()
 * ----------------------------------------------------
 */
// int32 CFE_PSP_MemScrubSet(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority)
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_MemScrubSet, int32);

    UT_GenStub_AddParam(CFE_PSP_MemScrubSet, CFE_PSP_MemScrubStatus_t *, pNewConfiguration);

    UT_GenStub_Execute(CFE_PSP_MemScrubSet, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubSet, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubGet()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *mss_Status, size_t iConfigSize, bool talk)
{
    UT_GenStub_AddParam(CFE_PSP_MemScrubGet, CFE_PSP_MemScrubStatus_t *, mss_Status);
    UT_GenStub_AddParam(CFE_PSP_MemScrubGet, size_t, iConfigSize);
    UT_GenStub_AddParam(CFE_PSP_MemScrubGet, bool, talk);

    UT_GenStub_Execute(CFE_PSP_MemScrubGet, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubGet, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_MemScrubTrigger()
 * ----------------------------------------------------
 */
int32 CFE_PSP_MemScrubTrigger(void)
{
    UT_GenStub_Execute(CFE_PSP_MemScrubTrigger, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_MemScrubTrigger, int32);
}
