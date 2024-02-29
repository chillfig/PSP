/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file cfe_psp_timesync.h
*
*   \brief Header for API to control NTP sync
*
*   \brief This file contains the function prototypes that synchronize the cFE Time services to the NTP 
*   server. Note that the NTP server must be built into the kernel.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was cloned-n-owned at the NASA Johnson Space Center from the NASA cFE open source 
*   with license below.
**********************************************************************************************************/

/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

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
** This function will not check if the task has successfully synchronized with
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
** This function will only return a valid time if NTP sync has occurred and current 
** system time is greater than the CFE mission time epoch. Note that this function 
** utilizes the POSIX clock_gettime API with a CLOCK_REALTIME clock which is adjusted 
** by NTP and may include leap seconds depending on the NTP server configuration.
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
