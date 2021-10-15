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

#include "psp_time_sync.h"

/** \name NTP Sync Configuration */
/** \{ */
/**
 *  \brief Default NTP Sync Start/Stop on Startup
 * 
 *  \par Description:
 *      Enable or disable the Automatic time sync with the OS
 */
#ifndef CFE_MISSION_TIME_SYNC_OS_ENABLE
#define CFE_MISSION_TIME_SYNC_OS_ENABLE true
#endif

/**
 *  \brief Default Synchronization Frequency
 * 
 *  \par Description:
 *      Default number of seconds between time synchronizations.
 *      CFE Time Service updates MET and STCF from VxWorks OS.
 *      When set to zero, CFE Time will be synchronized only once during start.
 * 
 *  \par Limits
 *      Positive integer up to 255.
 *      If this value is too low, it will starve the other processes.
 */
#ifndef CFE_MISSION_TIME_SYNC_OS_SEC
#define CFE_MISSION_TIME_SYNC_OS_SEC 30
#endif

/** \brief Default NTP Sync Task Name */
#ifndef NTPSYNC_TASK_NAME
#define NTPSYNC_TASK_NAME           "PSPNTPSync"
#endif

/** \brief Default NTP Sync Task Priority */
#ifndef NTPSYNC_DEFAULT_PRIORITY
#define NTPSYNC_DEFAULT_PRIORITY    60
#endif
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

/* 
SetTime may not be necessary since the function CFE_TIME_SetTimeCmd in CFE module time
can take the ground command directly
*/
extern void CFE_TIME_SetTime(CFE_TIME_SysTime_t);

/**** Global variables ****/

/**
 * @brief Contains the NTP Sync Task ID
 *        If 0, task is not running
 */
static uint32 g_uiPSPNTPTask_id = 0;

static osal_priority_t g_ucNTPSyncTaskPriority = NTPSYNC_DEFAULT_PRIORITY;

/**
 * \brief Boolean variable to control if to synchronize CFE Time Service with OS
 * local time. True, synch will occur. False, timer will not be disabled, but 
 * sync will not execute.
 */
static bool g_iEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

/**
 * \brief Change how often to sync CFE Time Service with OS Local Time. OS local
 * time is synchronized to NTP server(s) automatically from within OS if 
 * enabled.
 */
static uint16 g_usOSTimeSync_Sec = CFE_MISSION_TIME_SYNC_OS_SEC;

/* Declare this file a PSP Module */
CFE_PSP_MODULE_DECLARE_SIMPLE(ntp_clock_linux);



/**
 * @brief Initialize the CFE PSP Time Task synchronizing with the NTP server
 * 
 * @param None
 * 
 * @return int32 - CFE_PSP_SUCCESS or CFE_PSP_ERROR
 */
int32 CFE_PSP_TIME_Init(void)
{
    /* Initialize */
    int32       status = OS_ERROR;
    
    /*Create the 1Hz task for synchronizing with OS time*/
    status = OS_TaskCreate(&g_uiPSPNTPTask_id,
                            NTPSYNC_TASK_NAME,
                            CFE_PSP_Update_OS_Time,
                            OSAL_TASK_STACK_ALLOCATE, 
                            OSAL_SIZE_C(1024),
                            g_ucNTPSyncTaskPriority,
                            0
                            );

    if (status != OS_SUCCESS)
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Failed to create task\n");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Task Initialized\n");
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
        OS_printf(NTPSYNC_PRINT_SCOPE "Sync with OS is enabled\n");
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Sync with OS is disabled\n");
    }

    /* Set flag */
    g_iEnableGetTimeFromOS_flag = enable;

    return (int32) g_iEnableGetTimeFromOS_flag;
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
    bool    return_code = true;
    
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_NTP_Daemon_Get_Status not implemented in Linux OS");

    return return_code;
}

/**
 * @brief 
 * 
 * @return int32 
 */
int32 net_clock_linux_Destroy()
{
    int32       return_value = CFE_PSP_SUCCESS;
    int32       ret = CFE_PSP_ERROR;

    /* Disable time update */
    g_iEnableGetTimeFromOS_flag = false;
    
    /* Delete task */
    ret = OS_TaskDelete(g_uiPSPNTPTask_id);

    if (ret == OS_SUCCESS)
    {
        g_uiPSPNTPTask_id = 0;
    }
    else
    {
        OS_printf(NTPSYNC_PRINT_SCOPE "Could not kill the NTP Sync task\n");
        return_value = CFE_PSP_ERROR;
    }

    return return_value;
}

/**
 * @brief 
 * 
 * @param PspModuleId 
 */
void ntp_clock_linux_Init(uint32 PspModuleId)
{
    g_iEnableGetTimeFromOS_flag = CFE_MISSION_TIME_SYNC_OS_ENABLE;

    if (g_iEnableGetTimeFromOS_flag)
    {
        CFE_PSP_TIME_Init();
    }
}

/**
** \func Get the currently set sync frequency
**
** \par Description:
** This function returns the NTP time synchronization frequency, in seconds.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
** 
** \return Current frequency
*/
uint16 CFE_PSP_Sync_From_OS_GetFreq(void)
{
    /* Return the value of g_usOSTimeSync_Sec */
    return g_usOSTimeSync_Sec;
}

/**
** \func Change the sync frequency
**
** \par Description:
** This function updates the NTP time synchronization frequency, in seconds.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] new_frequency_sec - The new frequency, in seconds
** 
** \return #CFE_PSP_SUCCESS - If successfully changed
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_Sync_From_OS_SetFreq(uint16 new_frequency_sec)
{
    int32       return_value = CFE_PSP_SUCCESS;

    /* Set a new value of g_usOSTimeSync_Sec */
    /* Update frequency with new value */
    g_usOSTimeSync_Sec = new_frequency_sec;

    /* Kill the NTP Sync task */
    return_value = net_clock_linux_Destroy();

    if (return_value != CFE_PSP_ERROR)
    {
        /* Reinitialize timer with new updated value */
        /* The ModuleID is not used in the SP0 implementation */
        ntp_clock_linux_Init((uint32)0);

        /* If the task did not start, report error */
        if (g_uiPSPNTPTask_id == 0)
        {
            return_value = CFE_PSP_ERROR;
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
    int32               return_status = CFE_PSP_SUCCESS;

    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_Set_OS_Time not implemented in Linux OS");

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
        OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_Get_OS_Time called without a proper argument\n");
        return_code = CFE_PSP_ERROR;
    }

    return return_code;
}

/******************************************************************************
**  Function:  CFE_PSP_Get_OS_Time()
**
**  Purpose:
**    Syncronize CFE Time Service with OS local time. OS time is 
**    automatically syncronized to NTP server
**    Declaration is dictated by OSAL, see osapi-os-timer.h OS_TimerCreate info
**
**    IMPORTANT: Function declaration is dictated by OS_TimerCreate function.
**
**  Arguments:
**    void
**
**  Return:
**    void
******************************************************************************/
void CFE_PSP_Update_OS_Time(void)
{
    CFE_TIME_SysTime_t  myT;
    int32               ret;
    uint32              sleep_time;

    while (1)
    {
        sleep_time = g_usOSTimeSync_Sec * 1000U;
        ret = OS_TaskDelay(sleep_time);

        if (ret == OS_SUCCESS)
        {
            /* If the flag is enabled */
            if (g_iEnableGetTimeFromOS_flag)
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
                    OS_printf(NTPSYNC_PRINT_SCOPE "OS has not sync with NTP server yet, trying again later.\n");
                }
            }
        }
        else
        {
            OS_printf(NTPSYNC_PRINT_SCOPE "OS_TaskDelay error\n");
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
    
    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_StartNTPDaemon not implemented in Linux OS");

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

    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_StopNTPDaemon not implemented in Linux OS");

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

    OS_printf(NTPSYNC_PRINT_SCOPE "CFE_PSP_NTP_Daemon_Enable not implemented in Linux OS");

    return return_code;
}