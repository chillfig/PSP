/**
 ** \file
 **
 ** \brief API header to control NTP Sync
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** This file contains the function prototypes that synchronize the cFE Time
 ** services to the NTP server. Note that the NTP server must be built into the
 ** kernel.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** The way this module updates the local time is by calling the CFE Time Service
 ** function CFE_TIME_SetTime(). The function changes the STCF value.\n
 **
 ** GSFC developers do not recommend to use this method of updating CFE time,
 ** but rather to use the function CFE_TIME_ExternalTime(). The only way to use
 ** this function is by building an app that will periodically (1Hz) get NTP
 ** time and publish it via Software Bus.
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
 * Auto-Generated stub implementations for functions defined in cfe_psp_timesync header
 */

#include "cfe_psp_timesync.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_GetOSTime()
 * ----------------------------------------------------
 */
int32 CFE_PSP_GetOSTime(CFE_TIME_SysTime_t *myT)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_GetOSTime, int32);

    UT_GenStub_AddParam(CFE_PSP_GetOSTime, CFE_TIME_SysTime_t *, myT);

    UT_GenStub_Execute(CFE_PSP_GetOSTime, Basic, NULL);

    UT_Stub_CopyToLocal(UT_KEY(CFE_PSP_GetOSTime), myT, sizeof(CFE_TIME_SysTime_t));

    return UT_GenStub_GetReturnValue(CFE_PSP_GetOSTime, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_NTPDaemonIsRunning()
 * ----------------------------------------------------
 */
int32 CFE_PSP_NTPDaemonIsRunning(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_NTPDaemonIsRunning, int32);

    UT_GenStub_Execute(CFE_PSP_NTPDaemonIsRunning, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_NTPDaemonIsRunning, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SetOSTime()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SetOSTime(const uint32 ts_sec, const uint32 ts_nsec)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SetOSTime, int32);

    UT_GenStub_AddParam(CFE_PSP_SetOSTime, const uint32, ts_sec);
    UT_GenStub_AddParam(CFE_PSP_SetOSTime, const uint32, ts_nsec);

    UT_GenStub_Execute(CFE_PSP_SetOSTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SetOSTime, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_StartNTPDaemon()
 * ----------------------------------------------------
 */
int32 CFE_PSP_StartNTPDaemon(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_StartNTPDaemon, int32);

    UT_GenStub_Execute(CFE_PSP_StartNTPDaemon, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_StartNTPDaemon, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_StopNTPDaemon()
 * ----------------------------------------------------
 */
int32 CFE_PSP_StopNTPDaemon(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_StopNTPDaemon, int32);

    UT_GenStub_Execute(CFE_PSP_StopNTPDaemon, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_StopNTPDaemon, int32);
}
