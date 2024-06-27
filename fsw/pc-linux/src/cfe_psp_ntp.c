/************************************************************************************************
**
** File: cfe_psp_timer.c
**
** Copyright (c) 2019-2021 United States Government as represented by
** the Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package (BSP).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
*************************************************************************************************/

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

/* For supporting REALTIME clock */
#include <time.h>

/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"

#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

#include "cfe_psp_timesync.h"

/** \name NTP Sync Configuration - Linux */
/** \{ */

/** \} */

/**
 ** \brief Default NTP Sync pre-print string 
 ** \par Description:
 ** This string is printed before every print related to NTP Sync API.
*/
#define NTPSYNC_PRINT_SCOPE         "PSP NTP SYNC: "


/**** External Function Prototypes ****/

/* Below are defined in CFE module time cfe_time.h and cfe_time_utils.h */
extern uint32 CFE_TIME_Micro2SubSecs(uint32);

/**** Global variables ****/

/******************************************************************************
**  Function:  CFE_PSP_NTPDaemonIsRunning(void)
**
**  Purpose:
**    Get status of NTP daemon
**
**  Arguments:
**    none
**
**  Return:
**    int32 - CFE_PSP_SUCCESS if NTP Daemon is running, 
              CFE_PSP_ERROR if it is not running,
              CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not
              been implemented yet
******************************************************************************/
int32 CFE_PSP_NTPDaemonIsRunning(void)
{   
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/******************************************************************************
**  Function:  CFE_PSP_SetOSTime()
**
**  Purpose:
**    Set the OS CLOCK_REALTIME to a specified timestamp
**    IMPORTANT: If NTP daemon is enabled and it is capable of contacting an
**               NTP server, setting time does not work.
**
**  Arguments:
**    uint32 ts_sec - Seconds since Epoch 1/1/1970
**    uint32 ts_nsec - NanoSecond timestamp
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_SetOSTime(const uint32 ts_sec, const uint32 ts_nsec)
{
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_Set_OS_Time not implemented in Linux OS");

    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/*
 * Purpose: Get the current time from the linux OS
 * 
 * Param: [inout] myT Reference to CFE_TIME_SysTime_t time structure
 *
 * Return: CFE_PSP_SUCCESS
 *         CFE_PSP_ERROR
 */
int32 CFE_PSP_GetOSTime(CFE_TIME_SysTime_t *myT)
{
    struct timespec     unixTime = {0,0};
    uint32              tv_msec = 0;
    int                 iResult = CFE_PSP_ERROR;
    int32               iReturnStatus = CFE_PSP_SUCCESS;

    if (myT != NULL)
    {
        /* Get real time clock from OS */
        iResult = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        if (iResult == CFE_PSP_SUCCESS)
        {
            /* If the unix time has synchronized with NTP, it must be bigger than 
               CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS 
            */
            if (((unixTime.tv_sec == CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS) && 
                (unixTime.tv_nsec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF_MICROSECS*1000)) || 
                (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS))
            {
                /* Unix Time secs greater than or equal to epoch secs, but subsecs less than epoch subsecs */
                if (unixTime.tv_nsec < CFE_MISSION_TIME_EPOCH_UNIX_DIFF_MICROSECS*1000)
                {
                    tv_msec = (uint32) (unixTime.tv_nsec + 1000000000 - 
                        CFE_MISSION_TIME_EPOCH_UNIX_DIFF_MICROSECS*1000) / 1000;
                    myT->Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
                    myT->Seconds = unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS - 1;
                }
                /* Unix Time secs greater than or equal to epoch secs, and subsecs greater than
                   or equal to epoch subsecs 
                */
                else
                {
                    tv_msec = (uint32) (unixTime.tv_nsec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF_MICROSECS*1000) / 1000;
                    myT->Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
                    myT->Seconds = unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF_SECS;
                }
            }
            else
            {
                /* OS has not sync with NTP server yet. */
                iReturnStatus = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
            iReturnStatus = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_GetOSTime called without a proper argument\n");
        iReturnStatus = CFE_PSP_ERROR;
    }
    
    return iReturnStatus;
}

/******************************************************************************
**  Function:  CFE_PSP_StartNTPDaemon()
**
**  Purpose:
**    Start the NTP daemon on OS that sync with NTP server
**
**  Arguments:
**    none
**
**  Return:
**    int32 - Task ID or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_StartNTPDaemon(void)
{
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_StartNTPDaemon not implemented in Linux OS");

    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/******************************************************************************
**  Function:  CFE_PSP_StopNTPDaemon()
**
**  Purpose:
**    Stop the NTP daemon on OS that sync with NTP server
**
**  Arguments:
**    none
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
**            Note: NTP task already stopped, return CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_StopNTPDaemon(void)
{
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_StopNTPDaemon not implemented in Linux OS");

    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}
