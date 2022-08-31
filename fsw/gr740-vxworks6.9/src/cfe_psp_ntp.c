/**
 **
 ** \file cfe_psp_ntp.c
 **
 ** \brief API to control NTP Sync
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
 ** \par Purpose:
 ** This file contains the function declaration that synchronize the cFE Time
 ** services to the NTP server. Note that the NTP server must be built into the
 ** kernel.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** In function #CFE_PSP_GetOSTime the OS time is compared with the mission epoch
 ** time #CFE_MISSION_TIME_EPOCH_UNIX_DIFF for validity. This might not be the 
 ** most appropriate way.
 */


#include <vxWorks.h>
/**
 ** \brief Endian information is required by VxWorks IPCOM for Starting/Stopping NTP Daemon
 */
#if (_BYTE_ORDER == _LITTLE_ENDIAN )
#define IP_LITTLE_ENDIAN
#elif (_BYTE_ORDER == _BIG_ENDIAN )
#define IP_BIG_ENDIAN
#endif
#include <ipcom_err.h>
#include <taskLib.h>

/* For supporting REALTIME clock */
#include <timers.h>

/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"

#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

#include "cfe_psp_timesync.h"

/**
 ** \brief Default NTP Sync pre-print string 
 ** \par Description:
 ** This string is printed before every print related to NTP Sync API.
*/
#define NTPSYNC_PRINT_SCOPE         "PSP NTP SYNC: "


/* Functions provided by Kernel */
/** \brief VxWorks function to get ID of running task */
extern TASK_ID taskNameToId (char *name);
/** \brief VxWorks function to kill a running service */
extern IP_PUBLIC Ip_err ipcom_ipd_kill (const char *name );
/** \brief VxWorks function to start a service */
extern IP_PUBLIC Ip_err ipcom_ipd_start (const char *name ); 

/** \brief Convert micro seconds in subseconds
 ** \par Description:
 ** Defined in CFE module time cfe_time.h
 */
extern uint32 CFE_TIME_Micro2SubSecs(uint32);


/**********************************************************
 * 
 * Function: CFE_PSP_NTPDaemonIsRunning
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_NTPDaemonIsRunning(void)
{
    int32       iReturnCode = CFE_PSP_SUCCESS;
    TASK_ID     tidTaskId   = 0;

    tidTaskId = taskNameToId((char *)NTP_DAEMON_TASK_NAME);
    if (tidTaskId == CFE_PSP_ERROR)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SetOSTime
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SetOSTime(const uint32 ts_sec, const uint32 ts_nsec)
{
    struct timespec     unixTime = {0,0};
    int32               iResult = CFE_PSP_ERROR;
    int32               iReturnStatus = CFE_PSP_SUCCESS;

    unixTime.tv_sec = ts_sec;
    unixTime.tv_nsec = (long) ts_nsec;

    iResult = clock_settime(CLOCK_REALTIME, &unixTime);
    if (iResult == OK)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Clock set");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "ERROR Clock not set");
        iReturnStatus = CFE_PSP_ERROR;
    }

    return iReturnStatus;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetOSTime
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
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
        
        if (iResult == OK)
        {
            /* If the unix time has synchronized with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT->Seconds = unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32)unixTime.tv_nsec / 1000;
                myT->Subseconds = CFE_TIME_Micro2SubSecs(tv_msec);
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

/**********************************************************
 * 
 * Function: CFE_PSP_StartNTPDaemon
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_StartNTPDaemon(void)
{
    int32       iStatus = CFE_PSP_ERROR;
    int32       tidTaskId = 0;

    iStatus = ipcom_ipd_start(NTP_SERVICE_NAME);
    if (iStatus == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully\n");
        tidTaskId = taskNameToId((char *)NTP_DAEMON_TASK_NAME);
    }
    else if (iStatus == IPCOM_ERR_ALREADY_STARTED)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already started\n");
        tidTaskId = taskNameToId((char *)NTP_DAEMON_TASK_NAME);
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)\n", iStatus);
        tidTaskId = CFE_PSP_ERROR;
    }

    return tidTaskId;
}

/**********************************************************
 * 
 * Function: CFE_PSP_StopNTPDaemon
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_StopNTPDaemon(void)
{
    int32       iReturnCode = CFE_PSP_SUCCESS;
    int32       iIPCOMStatus = CFE_PSP_ERROR;

    iIPCOMStatus = ipcom_ipd_kill(NTP_SERVICE_NAME);
    if (iIPCOMStatus == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully\n");
    }
    else
    {
        /* Although the name, OS user guide describes IPCOM_ERR_NOT_STARTED as "Daemon is not running" */
        if (iIPCOMStatus == IPCOM_ERR_NOT_STARTED)
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already stopped\n");
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)\n",iIPCOMStatus);
        }
        iReturnCode = CFE_PSP_ERROR;
    }

    return iReturnCode;
}
