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
              CFE_PSP_ERROR_NOT_IMPLEMENTED beacuse this function has not
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
    struct timespec     unixTime;
    uint32              tv_msec = 0;
    int                 ret;
    int32               return_code = CFE_PSP_SUCCESS;

    if (myT != NULL)
    {
        /* Get real time clock from OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        if (ret == CFE_PSP_SUCCESS)
        {
            /* If the unix time has synchronzed with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT->Seconds = (uint32) unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32) unixTime.tv_nsec / 1000;
                myT->Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
            }
            else
            {
                /* OS has not sync with NTP server yet. */
                return_code = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "clock_gettime function failed\n");
            return_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_GetOSTime called without a proper argument\n");
        return_code = CFE_PSP_ERROR;
    }

    return return_code;
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
