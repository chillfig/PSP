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
*   \file cfe_psp_start_stubs.c
*
*   \brief Header file for the PSP function prototypes in cfe_psp_start.c
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
*   PSP Startup API for Aitech SP0. Functions implemented in this file are
*   used to configure the SP0 target and the VxWorks OS, and gather information
*   on how the system is setup.
*
*   \par Limitations, Assumptions, External Events, and Notes:
*   None
*
**********************************************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_start header
 */

#include "cfe_psp_start.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_GetActiveCFSPartition()
 * ----------------------------------------------------
 */
int32 CFE_PSP_GetActiveCFSPartition(char *pBuffer, uint32 uBuffer_size)
{
    UT_GenStub_AddParam(CFE_PSP_GetActiveCFSPartition, char *, pBuffer);
    UT_GenStub_AddParam(CFE_PSP_GetActiveCFSPartition, uint32, uBuffer_size);

    UT_GenStub_Execute(CFE_PSP_GetActiveCFSPartition, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_GetActiveCFSPartition, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_GetRestartType()
 * ----------------------------------------------------
 */
uint32 CFE_PSP_GetRestartType(uint32 *resetSubType)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_GetRestartType, uint32);

    UT_GenStub_AddParam(CFE_PSP_GetRestartType, uint32 *, resetSubType);

    UT_GenStub_Execute(CFE_PSP_GetRestartType, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_GetRestartType, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_ProcessPOSTResults()
 * ----------------------------------------------------
 */
void CFE_PSP_ProcessPOSTResults(void)
{

    UT_GenStub_Execute(CFE_PSP_ProcessPOSTResults, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SetFileSysAddFixedMap()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SetFileSysAddFixedMap(osal_id_t *fs_id)
{
    // UT_GenStub_SetupReturnBuffer(CFE_PSP_SetFileSysAddFixedMap, static int32);
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SetFileSysAddFixedMap, int32);

    UT_GenStub_AddParam(CFE_PSP_SetFileSysAddFixedMap, osal_id_t *, fs_id);

    UT_GenStub_Execute(CFE_PSP_SetFileSysAddFixedMap, Basic, NULL);

    // return UT_GenStub_GetReturnValue(CFE_PSP_SetFileSysAddFixedMap, static int32);
    return UT_GenStub_GetReturnValue(CFE_PSP_SetFileSysAddFixedMap, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SetTaskPrio()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SetTaskPrio(const char *tName, uint8 tgtPrio)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SetTaskPrio, int32);

    UT_GenStub_AddParam(CFE_PSP_SetTaskPrio, const char *, tName);
    UT_GenStub_AddParam(CFE_PSP_SetTaskPrio, uint8, tgtPrio);

    UT_GenStub_Execute(CFE_PSP_SetTaskPrio, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SetTaskPrio, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_StartupClear()
 * ----------------------------------------------------
 */
int32 CFE_PSP_StartupClear(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_StartupClear, int32);

    UT_GenStub_Execute(CFE_PSP_StartupClear, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_StartupClear, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_StartupFailedRestartSP0_hook()
 * ----------------------------------------------------
 */
void CFE_PSP_StartupFailedRestartSP0_hook(osal_id_t timer_id)
{
    UT_GenStub_AddParam(CFE_PSP_StartupFailedRestartSP0_hook, osal_id_t, timer_id);

    UT_GenStub_Execute(CFE_PSP_StartupFailedRestartSP0_hook, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_StartupTimer()
 * ----------------------------------------------------
 */
int32 CFE_PSP_StartupTimer(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_StartupTimer, int32);

    UT_GenStub_Execute(CFE_PSP_StartupTimer, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_StartupTimer, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SuspendConsoleShellTask()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SuspendConsoleShellTask(bool suspend)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SuspendConsoleShellTask, int32);

    UT_GenStub_AddParam(CFE_PSP_SuspendConsoleShellTask, bool, suspend);

    UT_GenStub_Execute(CFE_PSP_SuspendConsoleShellTask, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SuspendConsoleShellTask, int32);
}
