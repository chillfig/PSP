/************************************************************************************************
**
** File: cfe_psp_timer.c
**
**      Copyright (c) 2004-2011, United States Government as represented by
**      Administrator for The National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package (BSP).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
*************************************************************************************************/


/* VxWorks IPCOM for Starting/Stopping NTP Daemon */
#define IP_PORT_VXWORKS 69
#include "ipcom_err.h"
#include "taskLib.h"

/* For supporting REALTIME clock */
#include <timers.h>

#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

/**
 *  \cfetimecfg Default EPOCH Values 
 * 
 *  \par Description:
 *      Enable or disable the Automatic time sync with the OS
 * 
 *  \par Limits
 *      Binary true or false
 */
#define CFE_MISSION_TIME_SYNC_OS_ENABLE true

/**
 *  \cfetimecfg Default EPOCH Values 
 * 
 *  \par Description:
 *      Default number of seconds between time synchronizations.
 *      CFE Time Service updates MET and STCF from VxWorks OS.
 *      When set to zero, CFE Time will be synchronized only once during start.
 * 
 *  \par Limits
 *      Positive integer
 */
#define CFE_MISSION_TIME_SYNC_OS_SEC 30

#define NTPSYNC_TASK_NAME           "PSPNTPSync"
#define NTPSYNC_DEFAULT_PRIORITY    60

#define PRE_PRINT_SCOPE             "PSP NTP SYNC: "

/* Below is define in CFE module core-api cfe_time_extern_typedefs.h */
typedef struct CFE_TIME_SysTime
{
    uint32 Seconds;    /**< \brief Number of seconds since epoch */
    uint32 Subseconds; /**< \brief Number of subseconds since epoch (LSB = 2^(-32) seconds) */
} CFE_TIME_SysTime_t;


/*
** External Function Prototypes
*/
extern TASK_ID taskNameToId (char * name);
extern IP_PUBLIC Ip_err ipcom_ipd_kill (const char *name );
extern IP_PUBLIC Ip_err ipcom_ipd_start (const char *name ); 

/* Below are defined in CFE module time cfe_time.h and cfe_time_utils.h */
extern uint32 CFE_TIME_Micro2SubSecs(uint32);

/* 
SetTime may not be necessary since the function CFE_TIME_SetTimeCmd in CFE module time
can take the ground command directly
*/
extern void CFE_TIME_SetTime(CFE_TIME_SysTime_t);

/*
** Function Prototypes
*/
int32 CFE_PSP_TIME_Init(uint16);
void CFE_PSP_Update_OS_Time(void);
int32 CFE_PSP_Get_OS_Time(CFE_TIME_SysTime_t *);

/*
** Global variables
*/

/**
 * @brief Contains the NTP Sync Task ID
 *        If 0, task is not running
 */
static uint32 sg_uiPSPNTPTask_id = 0;

static osal_priority_t sg_uiNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY;

/**
 * \brief Boolean variable to control if to synchronize CFE Time Service with OS
 * local time. True, synch will occur. False, timer will not be disabled, but 
 * sync will not execute.
 */
static bool sg_bEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

/**
 * \brief Change how often to sync CFE Time Service with OS Local Time. OS local
 * time is synchronized to NTP server(s) automatically from within OS if 
 * enabled.
 */
static uint16 sg_uiOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;

/* Declare this file a PSP Module */
CFE_PSP_MODULE_DECLARE_SIMPLE(ntp_clock_vxworks);




/**
 * @brief 
 * 
 * @param PspModuleId 
 */
void ntp_clock_vxworks_Init(uint32 PspModuleId)
{
    if (sg_bEnableGetTimeFromOS_flag)
    {
        CFE_PSP_TIME_Init(sg_uiOSTimeSync_Sec);
    }
}

/******************************************************************************
**  Function:  CFE_PSP_TIME_Init()
**
**  Purpose:
**    Initialize the CFE PSP Time Task synchronizing with the NTP server
**
**  Arguments:
**    timer_frequency_sec is the update frequency in seconds
**
**  Return:
**    int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
******************************************************************************/
int32 CFE_PSP_TIME_Init(uint16 timer_frequency_sec)
{
    /* Initialize */
    int32       status;
    
    /*Create the 1Hz task for synchronizing with OS time*/
    status = OS_TaskCreate(&sg_uiPSPNTPTask_id,
                            NTPSYNC_TASK_NAME,
                            CFE_PSP_Update_OS_Time,
                            OSAL_TASK_STACK_ALLOCATE, 
                            OSAL_SIZE_C(1024),
                            sg_uiNTPSyncTaskPriority,
                            0
                            );

    if (status != OS_SUCCESS)
    {
        printf(PRE_PRINT_SCOPE "Failed to create task\n");
    }
    else
    {
        printf(PRE_PRINT_SCOPE "Task Initialized\n");
    }

    return status;
}

/******************************************************************************
**  Function:  CFE_PSP_Sync_From_OS_Enable(bool)
**
**  Purpose:
**    Enable or Disable CFE_PSP_Get_OS_Time
**
**  Arguments:
**    bool - When True, CFE_PSP_Get_OS_Time will be called to synchronize time
**
**  Return:
**    int32 - input argument
******************************************************************************/
int32 CFE_PSP_Sync_From_OS_Enable(bool enable)
{
    
    if (enable)
    {
        printf(PRE_PRINT_SCOPE "Sync with OS is enabled\n");
    }
    else
    {
        printf(PRE_PRINT_SCOPE "Sync with OS is disabled\n");
    }

    /* Set flag */
    sg_bEnableGetTimeFromOS_flag = enable;

    return (int32) sg_bEnableGetTimeFromOS_flag;
}

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Get_Status(void)
**
**  Purpose:
**    Get status of NTP daemon
**
**  Arguments:
**    none
**
**  Return:
**    bool - True if NTP Daemon is running, False if it is not running
******************************************************************************/
bool CFE_PSP_NTP_Daemon_Get_Status(void)
{
    bool       return_code = true;
    TASK_ID     ret;
    
    ret = taskNameToId("ipntpd");
    if (ret == ERROR)
    {
        return_code = false;
    }
    return return_code;
}

/**
 * @brief 
 * 
 * @return int32 
 */
int32 net_clock_vxworks_Destroy()
{
    int32       return_value = CFE_PSP_SUCCESS;
    int32       ret;

    /* Disable time update */
    sg_bEnableGetTimeFromOS_flag = false;
    
    /* Delete task */
    ret = OS_TaskDelete(sg_uiPSPNTPTask_id);

    if (ret == OS_SUCCESS)
    {
        sg_uiPSPNTPTask_id = 0;
    }
    else if (ret == OS_ERR_INVALID_ID)
    {
        printf(PRE_PRINT_SCOPE "Could not kill the NTP Sync task\n");
        return_value == CFE_PSP_ERROR;
    }
    else
    {
        printf(PRE_PRINT_SCOPE "Could not kill the NTP Sync task\n");
        return_value == CFE_PSP_ERROR;
    }

    return return_value;
}

/******************************************************************************
**  Function:  CFE_PSP_Sync_From_OS_Freq(uint16_t)
**
**  Purpose:
**    Changes the synchronication frequency
**
**  Arguments:
**    uint16 - seconds between updates. If zero, returns the current value
**
**  Return:
**    int - CFE_PSP_SUCCESS or the current sg_uiOSTimeSync_Sec value
******************************************************************************/
int32 CFE_PSP_Sync_From_OS_Freq(uint16 new_frequency_sec)
{
    int32       return_value = CFE_PSP_SUCCESS;
    int32       ret;

    if (new_frequency_sec == 0)
    {
        /* Return the value of sg_uiOSTimeSync_Sec */
        return_value = (int32)sg_uiOSTimeSync_Sec;
    }
    else
    {
        /* Set a new value of sg_uiOSTimeSync_Sec */
        /* Update frequency with new value */
        sg_uiOSTimeSync_Sec = new_frequency_sec;

        net_clock_vxworks_Destroy();

        if (return_value != CFE_PSP_ERROR)
        {
            /* Reinitialize timer with new updated value */
            ntp_clock_vxworks_Init((uint32)0);
        }
    }

    return return_value;
}

/******************************************************************************
**  Function:  CFE_PSP_Set_OS_Time()
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
int32 CFE_PSP_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec)
{
    struct timespec     unixTime;
    int                 ret;
    int32               return_status = CFE_PSP_SUCCESS;

    if (ts_sec > 0 && ts_nsec >=0)
    {
        unixTime.tv_sec = ts_sec;
        unixTime.tv_nsec = (long) ts_nsec;

        ret = clock_settime(CLOCK_REALTIME, &unixTime);
        if (ret == OK)
        {
            printf(PRE_PRINT_SCOPE "Clock set");
        }
        else
        {
            printf(PRE_PRINT_SCOPE "ERROR Clock not set");
            return_status = CFE_PSP_ERROR;
        }
    }
    else
    {
        printf(PRE_PRINT_SCOPE "ERROR: Invalid timestamp");
        return_status = CFE_PSP_ERROR;
    }

    return return_status;
}

/**
 * @brief Function get the current time from the VxWorks OS
 * 
 * @param myT Reference to CFE_TIME_SysTime_t time structure
 * @return int32 CFE_PSP_SUCCESS or CFE_PSP_ERROR
 */
int32 CFE_PSP_Get_OS_Time(CFE_TIME_SysTime_t *myT)
{
    struct timespec     unixTime;
    uint32              tv_sec = 0;
    uint32              tv_msec = 0;
    int                 ret;
    int32               return_code = CFE_PSP_SUCCESS;

    if (myT != NULL)
    {
        /* Get real time clock from OS */
        ret = clock_gettime(CLOCK_REALTIME, &unixTime);
        
        /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
        if (ret == OK)
        {
            /* If the unix time has synchronzed with NTP, it must be bigger than CFE_MISSION_TIME_EPOCH_UNIX_DIFF */
            if (unixTime.tv_sec > CFE_MISSION_TIME_EPOCH_UNIX_DIFF)
            {
                myT->Seconds = unixTime.tv_sec - CFE_MISSION_TIME_EPOCH_UNIX_DIFF;
                tv_msec = (uint32)unixTime.tv_nsec / 1000;
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
            printf(PRE_PRINT_SCOPE "clock_gettime function failed\n");
            return_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        printf(PRE_PRINT_SCOPE "CFE_PSP_Get_OS_Time called without a proper argument\n");
        return_code = CFE_PSP_ERROR;
    }
    
    return return_code;
}

/**
 * @brief Get OS time and update CFE Time Service. Function will run forever 
 * until task is deleted.
 * 
 */
void CFE_PSP_Update_OS_Time(void)
{
    CFE_TIME_SysTime_t  myT;
    int32               ret;
    uint32              sleep_time;

    while (1)
    {
        sleep_time = sg_uiOSTimeSync_Sec * 1000U;
        ret = OS_TaskDelay(sleep_time);

        if (ret == OS_SUCCESS)
        {
            /* If the flag is enabled */
            if (sg_bEnableGetTimeFromOS_flag)
            {
                /* Get real time clock from OS */
                ret = CFE_PSP_Get_OS_Time(&myT);
                
                /* If there are no errors, save the time in CFE Time Service using CFE_TIME_SetTime() */
                if (ret == CFE_PSP_SUCCESS)
                {
                    CFE_TIME_SetTime(myT);
                }
                else
                {
                    /* OS has not sync with NTP server yet. */
                    printf(PRE_PRINT_SCOPE "OS has not sync with NTP server yet, trying again later.\n");
                }
            }
        }
        else
        {
            printf(PRE_PRINT_SCOPE "OS_TaskDelay error\n");
        }
    }
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
    int32       return_code = 0;
    int32       ret;
    
    ret = ipcom_ipd_start("ipntp");
    if (ret == IPCOM_SUCCESS)
    {
        printf(PRE_PRINT_SCOPE "NTP Daemon Started Successfully");
        return_code = taskNameToId("ipntpd");
    }
    else
    {
        if (ret == IPCOM_ERR_ALREADY_STARTED)
        {
            printf(PRE_PRINT_SCOPE "NTP Daemon already started");
            return_code = taskNameToId("ipntpd");
        }
        else
        {
            printf(PRE_PRINT_SCOPE "ERROR NTP Daemon did not Start (ip_err = %d)",ret);
            return_code = CFE_PSP_ERROR;
        }
    }
    return return_code;
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
    int32       return_code = CFE_PSP_SUCCESS;
    int32       ret;

    ret = ipcom_ipd_kill("ipntp");
    if (ret == IPCOM_SUCCESS)
    {
        printf(PRE_PRINT_SCOPE "NTP Daemon Stopped Successfully");
    }
    else
    {
        /* Although the name, OS user guide describes IPCOM_ERR_NOT_STARTED as "Dameon is not running" */
        if (ret == IPCOM_ERR_NOT_STARTED)
        {
            printf(PRE_PRINT_SCOPE "NTP Daemon already stopped");
        }
        else
        {
            printf(PRE_PRINT_SCOPE "ERROR NTP Daemon did not stop (ip_err = %d)",ret);
        }
        return_code = CFE_PSP_ERROR;
    }

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_NTP_Daemon_Enable(bool)
**
**  Purpose:
**    Enable or Disable OS NTP dameon process
**
**  Arguments:
**    bool - When True starts NTP daemon, else stop process
**
**  Return:
**    int32 - Task ID or CFE_PSP_SUCCESS or CFE_PSP_ERROR
**            Task ID is returned when successfully starting NTP process
**            CFE_PSP_SUCCESS is returned when successfully stopping NTP process
******************************************************************************/
int32 CFE_PSP_NTP_Daemon_Enable(bool enable)
{
    int32   return_code = CFE_PSP_SUCCESS;

    if (enable)
    {
        return_code = CFE_PSP_StartNTPDaemon();
    }
    else
    {
        return_code = CFE_PSP_StopNTPDaemon();
    }

    return return_code;
}