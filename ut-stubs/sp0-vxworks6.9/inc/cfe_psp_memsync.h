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
*   \file cfe_psp_memsync.h
*
*   \brief
*   API header to control memory sync to filesystem
*
*   \copyright
*   Copyright (c) 2019-2021 United States Government as represented by
*   the Administrator of the National Aeronautics and Space Administration.
*   All Rights Reserved.
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   \par Description:
*   This file contains the function prototypes that synchronize data in 
*   RAM/memory to a specific file on filesystem. 
*
*   \par Limitations, Assumptions, External Events, and Notes:
*   None
**********************************************************************************************************/

#ifndef PSP_MEM_SYNC_H
#define PSP_MEM_SYNC_H

#include "common_types.h"
#include "osapi.h"
#include "cfe_psp_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
 ** \{
 */

/**
 ** \brief Initialize data for memory sync task
 **
 ** \par Description:
 ** Initialize data required by the memory sync task
 **
 ** \par Assumptions, External Events, and Notes:
 ** Memory Sync task will not start unless data is 
 ** properly initialized
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_MemSyncInit(void);

/**
 ** \brief Destroy data for memory sync task
 **
 ** \par Description:
 ** Destroy data required by the memory sync task
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will only affect data initialized in ...SYNC_Init.
 ** Task must be stopped for destructions to occur.
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_MemSyncDestroy(void);

/**
 ** \brief Start Memory Sync Task
 **
 ** \par Description:
 ** This function will start memory syncing task
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will check to make sure memory sync task is not already running
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS - If task successfully started
 ** \return #CFE_PSP_ERROR - If task unsuccessfully started
 */
int32 CFE_PSP_MemSyncStart(void);

/**
 ** \brief Stop memory sync Task
 **
 ** \par Description:
 ** Stop and delete memory sync task, but don't destroy data
 **
 ** \par Assumptions, External Events, and Notes:
 ** Will wait for current write/sync to finish, if applicable
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS - If successfully stopped task
 ** \return #CFE_PSP_ERROR - If unsuccessfully stopped task
 */
int32 CFE_PSP_MemSyncStop(void);

/**
 ** \brief Check if memory sync task is running
 **
 ** \par Description:
 ** Check if memory sync task is running
 **
 ** \par Assumptions, External Events, and Notes:
 ** True indicates task is running, false indicates task is not running
 **
 ** \param None
 **
 ** \return true - If task is running
 ** \return false - If task is not running
 */
bool CFE_PSP_MemSyncIsRunning(void);

/**
 ** \brief Set new memory sync task priority
 **
 ** \par Description:
 ** Set a new memory sync task priority
 **
 ** \par Assumptions, External Events, and Notes:
 ** New memory sync task priority must be within configured range
 **
 ** \param[in] priority - New sync task priority
 **
 ** \return #CFE_PSP_SUCCESS - If successfully set new priority
 ** \return #CFE_PSP_ERROR - Could not set new priority
 */
int32 CFE_PSP_MemSyncSetPriority(osal_priority_t priority);

/**
 ** \brief Get the memory sync task priority
 **
 ** \par Description:
 ** Get the memory sync task priority
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return osal_priority_t Memory sync task priority
 */
osal_priority_t CFE_PSP_MemSyncGetPriority(void);

/**
 ** \brief Set the time between memory syncs in seconds
 **
 ** \par Description:
 ** Set the time between memory syncs in seconds
 **
 ** \par Assumptions, External Events, and Notes:
 ** New time bust be between configured values
 **
 ** \param[in] sec_time - New time
 **
 ** \return #CFE_PSP_SUCCESS - If successfully set new time
 ** \return #CFE_PSP_ERROR - If unsuccessfully set new time
 */
int32 CFE_PSP_MemSyncSetFrequency(uint32 sec_time);

/**
 ** \brief Get the time between memory syncs in seconds
 **
 ** \par Description:
 ** Get the time between memory syncs in seconds
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return uint32 Time between memory sync
 */
uint32 CFE_PSP_MemSyncGetFrequency(void);

/**
 ** \brief Return stat tracking
 **
 ** \par Description:
 ** Returns any statistic tracking for memory sync
 **
 ** \par Assumptions, External Events, and Notes:
 ** For now, we are only returning the total number
 ** of memory syncs that occur. This mainly serves
 ** as a placeholder until we decide what information
 ** would be useful to track
 **
 ** \param None
 **
 ** \return Statistic (currently, number of syncs that occur)
 */
uint32 CFE_PSP_MemSyncGetStatistics(void);

/**
 ** \brief Flush the User Reserved Memory to its filepath
 **
 ** \par Description:
 ** This function will perform a flush of all the data from User Reserved Memory
 ** to their respective filenames in Flash memory
 **
 ** \par Assumptions, External Events, and Notes:
 ** Function is supposed to be called right before target restart
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_FlushToFLASH(void);

/**
 ** \brief Check that if the User Reserved Memory file exists
 **
 ** \par Description:
 ** This function will check if the user reserved memory file exits in the path
 ** constructed.
 **
 ** \par Assumptions, External Events, and Notes:
 ** Checking only for a single file, not all 4. Deleting the Reset file will
 ** produce a POWERON restart type.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS - When the file does exist
 ** \return #CFE_PSP_ERROR - When the file does not exist
 */
int32 CFE_PSP_CheckURMFilesExists(void);

/**
 ** \} <!-- End of group "psp_public_api_sp0vx69" -->
 */

#ifdef __cplusplus
}
#endif

#endif /* PSP_MEM_SYNC_H */
