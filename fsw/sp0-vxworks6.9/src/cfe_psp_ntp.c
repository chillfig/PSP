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
*   \file cfe_psp_ntp.c
*
*   \brief Implementation of PSP NTP functions
*
*   \brief This file contains the function declaration that synchronize the cFE Time services to the NTP 
*   server. Note that the NTP server must be built into the kernel.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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

    tidTaskId = taskNameToId((char *)CFE_PSP_NTP_DAEMON_TASK_NAME);
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
        OS_printf(NTPSYNC_PRINT_SCOPE "Clock set\n");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "ERROR Clock not set\n");
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
                   or equal to epoch subsecs */
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

    iStatus = ipcom_ipd_start(CFE_PSP_NTP_SERVICE_NAME);
    if (iStatus == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Started Successfully\n");
        tidTaskId = taskNameToId((char *)CFE_PSP_NTP_DAEMON_TASK_NAME);
    }
    else if (iStatus == IPCOM_ERR_ALREADY_STARTED)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon already started\n");
        tidTaskId = taskNameToId((char *)CFE_PSP_NTP_DAEMON_TASK_NAME);
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

    iIPCOMStatus = ipcom_ipd_kill(CFE_PSP_NTP_SERVICE_NAME);
    if (iIPCOMStatus == IPCOM_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "NTP Daemon Stopped Successfully\n");
    }
    else
    {
        /*
        Although the name implied an IPCOM error, 
        VxWorks OS user guide describes IPCOM_ERR_NOT_STARTED as "Daemon is not running"
        */
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
