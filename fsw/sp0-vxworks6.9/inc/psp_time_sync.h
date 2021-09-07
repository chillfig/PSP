/**
 ** \file psp_time_sync.h
 **
 ** \brief API header to control NTP Sync
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved.\n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
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

/**
** \func Initialize the CFE PSP Time Task synchronizing with the NTP server
**
** \par Description:
** This function intializes the cFE PSP Time sync task with the NTP server.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] timer_frequency_sec - The update frequency, in seconds
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_TIME_Init(uint16 timer_frequency_sec);

/**
** \func Enable/disable time sync
**
** \par Description:
** This function sets the enabling/disabling of time sync.
**
** \par Assumptions, External Events, and Notes:
** When the flag is true, the NTP Sync task actively trys to sync clocks.
** When the flag is false, the NTP Sync task will remain active without sync.
**
** \param[in] enable - Boolean flag for sync or not sync
** 
** \return True - If synchronized
** \return False - If not synchronized
*/
int32  CFE_PSP_Sync_From_OS_Enable(bool enable);

/**
** \func Get the NTP daemon status
**
** \par Description:
** This function checks if the VxWorks NTP client task is running. It does not
** check if the task has successfully synchronized with an NTP server.
**
** \par Assumptions, External Events, and Notes:
** The task name for the VxWorks NTP client is the default "ipntpd".
**
** \param None
** 
** \return True - If NTP client task is running
** \return False - If NTP client task is not running
*/
bool  CFE_PSP_NTP_Daemon_Get_Status(void);

/**
** \func Gracefully shutdown NTP Sync Module 
** 
** \par Description:
** Function will attempt to delete the task. Usually this function will be 
** called when exiting cFS.
**
** \par Assumptions, External Events, and Notes:
** The task name for the VxWorks NTP client is the default "ipntpd".
**
** \param None
** 
** \return #CFE_PSP_SUCCESS 
** \return #CFE_PSP_ERROR
*/
int32 net_clock_vxworks_Destroy(void);

/**
** \func Change the sync frequency
**
** \par Description:
** This function updates the NTP time synchronization frequency, in seconds.
**
** \par Assumptions, External Events, and Notes:
** If 0 is passed in, the function returns the current frequency.
**
** \param[in] new_frequency_sec - The new frequency, in seconds
** 
** \return #CFE_PSP_SUCCESS - If successfully changed
** \return Current frequency - If passed in 0 for new_frequency_sec
*/
int32  CFE_PSP_Sync_From_OS_Freq(uint16 new_frequency_sec);

/**
** \func Set the OS time
**
** \par Description:
** This function sets the VxWorks OS time.
**
** \par Assumptions, External Events, and Notes:
** The changes do not occur if the NTP client is setup to synchrone with an NTP server.
** Set the OS CLOCK_REALTIME to a specified timestamp. Parameters are in UNIX time format,
** since Epoch 1/1/1970.
**
** \param[in] ts_sec - Time in seconds
** \param[in] ts_nsec - Time in nanoseconds
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_Set_OS_Time(const uint32 ts_sec, const uint32 ts_nsec);

/**
** \func Gets the current time from VxWorks OS
**
** \par Description:
** This function gets the current VxWorks OS time.
**
** \par Assumptions, External Events, and Notes:
** This function is used by the NTP Sync task to grab the current OS time.
** It uses CLOCK_REALTIME.
**
** \param[inout] myT - Pointer to the variable that stores the returned time value
** 
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_Get_OS_Time(CFE_TIME_SysTime_t *myT);

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
bool CFE_PSP_TimeService_Ready(void);

/**
** \func Update cFE time
** 
** \par Description:
** This function updates the time used by the cFE Time service.
**
** \par Assumptions, External Events, and Notes:
** This function will run forever until its task is deleted.
** 
** \param None
**
** \return None
*/
void  CFE_PSP_Update_OS_Time(void);

/**
** \func Start the NTP client
** 
** \par Description:
** This function starts the NTP client task, ipntpd, on VxWorks.
**
** \par Assumptions, External Events, and Notes:
** None
** 
** \param None
**
** \return NTP client Task ID
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_StartNTPDaemon(void);

/**
** \func Stop the NTP client
** 
** \par Description:
** This function stops the NTP client task, ipntpd, on VxWorks.
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
** \func Enable/disable the NTP client
** 
** \par Description:
** This function enables/disables the NTP client task, ipntpd, on VxWorks.
**
** \par Assumptions, External Events, and Notes:
** None
** 
** \param[in] enable - Boolean flag for enable or disable
**
** \return NTP client task ID - If successfully starts the NTP clien task
** \return #CFE_PSP_SUCCESS - If successfully stops the NTP client task
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_NTP_Daemon_Enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_TIME_SYNC_H_ */
