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

#ifndef PSP_TIME_SYNC_H
#define PSP_TIME_SYNC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osapi.h"

/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

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
** \return #CFE_PSP_SUCCESS - If NTP client task is running
** \return #CFE_PSP_ERROR - If NTP client task is not running
** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED - If function is not implemented
*/
int32  CFE_PSP_NTPDaemonIsRunning(void);

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
int32  CFE_PSP_SetOSTime(const uint32 ts_sec, const uint32 ts_nsec);

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
int32  CFE_PSP_GetOSTime(CFE_TIME_SysTime_t *myT);

/**
** \func Start the NTP client
** 
** \par Description:
** This function starts the NTP client task, ipntpd, on vxworks.
**
** \par Assumptions, External Events, and Notes:
** NTP Daemon will start by default (kernel config)
** 
** \param None
**
** \return NTP client Task ID
** \return IPCOM_ERR_NOT_STARTED
*/
int32  CFE_PSP_StartNTPDaemon(void);

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
int32  CFE_PSP_StopNTPDaemon(void);

/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_TIME_SYNC_H_ */
