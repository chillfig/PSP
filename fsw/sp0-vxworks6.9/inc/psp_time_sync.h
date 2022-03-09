/**
 ** \file psp_time_sync.h
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

#ifndef _PSP_TIME_SYNC_H_
#define _PSP_TIME_SYNC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "osapi.h"

/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
** \func Initialize the CFE PSP Time Task synchronizing with the NTP server
**
** \par Description:
** This function intializes the cFE PSP Time sync task with the NTP server.
**
** \par Assumptions, External Events, and Notes:
** Function will return CFE_PSP_SUCCESS if there is already an NTP Sync
** task running and will NOT attempt to start another
**
** \param None
** 
** \return #CFE_PSP_SUCCESS - If successfully started NTP Sync task
** \return #CFE_PSP_ERROR - If unsuccessfully started NTP Sync task
*/
int32  CFE_PSP_TIME_NTPSync_Task_Enable(void);

/**
** \func Disables the CFE PSP Time Task synchronizing with the NTP server
**
** \par Description:
** This function disable the cFE PSP Time sync task with the NTP server.
**
** \par Assumptions, External Events, and Notes:
** Function will return CFE_PSP_SUCCESS if there is no NTP Sync
** task running
**
** \param None
** 
** \return #CFE_PSP_SUCCESS - If successfully started NTP Sync task
** \return #CFE_PSP_ERROR - If unsuccessfully started NTP Sync task
*/
int32  CFE_PSP_TIME_NTPSync_Task_Disable(void);

/** 
 ** \func Check if the NTP Sync task is running
 **
 ** \par Description:
 ** This function checks on whether or not
 ** the NTP Sync task is running
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will check via OS_TaskGetIdByName using NTPSYNC_TASK_NAME
 ** as the name of NTP sync task
 **
 ** \param None
 **
 ** \return true - If NTP Sync Task is running
 ** \return false - If NTP Sync Task is not running
 */
bool CFE_PSP_TIME_NTPSync_Task_isRunning(void);

/**
 ** \func Set the NTP Sync task priority
 **
 ** \par Description:
 ** This function sets the NTP Sync task priority
 **
 ** \par Assumptions, External Events, and Notes:
 ** New priority must be between NTPSYNC_PRIORITY_DOWN_RANGE
 ** and NTPSYNC_PRIORITY_UP_RANGE. If the new priority is not
 ** within this range, the default priority will be assigned.
 **
 ** \param[in] opPriority - The new task priority
 **
 ** \return #CFE_PSP_SUCCESS - If successfully set new priority
 ** \return #CFE_PSP_ERROR - If unsuccessfully set new priority
 */
int32  CFE_PSP_TIME_NTPSync_Task_Priority_Set(osal_priority_t opPriority);

/**
** \func Check if the NTP Daemon is running
**
** \par Description:
** This function checks if the vxworks NTP client task is running
**
** \par Assumptions, External Events, and Notes:
** This function will not check if the task has successfully syncronized with
** an NTP server
**
** \param None
** 
** \return True - If NTP client task is running
** \return False - If NTP client task is not running
*/
bool  CFE_PSP_TIME_NTP_Daemon_isRunning(void);

/**
 ** \func Gracefully shutdown NTP Sync Module 
 ** 
 ** \par Description:
 ** Function will attempt to delete the NTP Sync task
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** When this function is called, no matter what it's return status is,
 ** the g_bEnableGetTimeFromOS_flag will be set to false.
 ** Intended only for use upon module initialization, not for
 ** 'normal' use during starting/stopping of NTP Sync task
 ** 
 ** \return #CFE_PSP_SUCCESS 
 ** \return #CFE_PSP_ERROR
 */
int32 ntp_clock_vxworks_Destroy(void);

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
uint16 CFE_PSP_TIME_NTPSync_GetFreq(void);

/**
** \func Change the sync frequency
**
** \par Description:
** This function updates the NTP time synchronization frequency, in seconds.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] uiNewFreqSec - The new frequency, in seconds
*/
void CFE_PSP_TIME_NTPSync_SetFreq(uint16 uiNewFreqSec);

/**
** \func Set the OS time
**
** \par Description:
** This function sets the vxworks OS time.
**
** \par Assumptions, External Events, and Notes:
** The changes do not occur if the NTP client is setup to synchronize with an NTP server.
** Set the OS CLOCK_REALTIME to a specified timestamp. Parameters are in UNIX time format,
** since Epoch 1/1/1970.
**
** \param[in] ts_sec - Time in seconds
** \param[in] ts_nsec - Time in nanoseconds
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_TIME_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec);

/**
** \func Gets the current time from vxworks OS
**
** \par Description:
** This function gets the current vxworks OS time.
**
** \par Assumptions, External Events, and Notes:
** This function is used by the NTP Sync task to grab the current OS time.
** It uses CLOCK_REALTIME.\n
** NTP Sync will not occur if NTP time is less than CFE_MISSION_TIME_EPOCH_UNIX_DIFF
**
** \param[out] myT - Pointer to the variable that stores the returned time value
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_TIME_Get_OS_Time(CFE_TIME_SysTime_t *myT);

/**
** \brief Check if CFS Time Service is up and running
** 
** \par Description:
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return true - CFE Time Service is ready
** \return false - CFE Time Service is not ready
*/
bool CFE_PSP_TIME_CFETimeService_isRunning(void);

/**
** \func Start the NTP client
** 
** \par Description:
** This function starts the NTP client task, ipntpd, on vxworks.
**
** \par Assumptions, External Events, and Notes:
** None
** 
** \param None
**
** \return NTP client Task ID
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_TIME_StartNTPDaemon(void);

/**
** \func Stop the NTP client
** 
** \par Description:
** This function stops the NTP client task, ipntpd, on vxworks.
**
** \par Assumptions, External Events, and Notes:
** None
** 
** \param None
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_TIME_StopNTPDaemon(void);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_TIME_SYNC_H_ */
