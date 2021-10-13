/**
 ** \file  psp_mem_scrub.h
 **
 ** \brief API header to control Memory Scrubbing
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
 ** This file contains the function prototypes relating to memory scrubbing.
 ** This is specific to the SP0-S processor running VxWorks 6.9 OS.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

#ifndef _PSP_MEM_SCRUB_H_
#define _PSP_MEM_SCRUB_H_

#include "common_types.h"
#include "osapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 ** \brief Default Memory Scrubbing pre-print string 
 ** \par Description:
 ** This string is printed before every print related to Memory Scrubbing API.
*/
#define MEM_SCRUB_PRINT_SCOPE   "PSP MEM SCRUB: "

/**
** \func Set the Memory Scrubbing parameters
** 
** \par Description:
** This functions set the memory scrubbing parameters.
**
** \par Assumptions, External Events, and Notes:
** After calling this function, the new settings will be applied in the 
** next call to the Activate Memory Scrubbing funtion.
** If newEndAddr is set to a value larger than the actual physical memory limit,
** the function will use the phyisical memory limit.
** Task priority can only be set between #MEMSCRUB_PRIORITY_UP_RANGE and 
** #MEMSCRUB_PRIORITY_DOWN_RANGE defined in cfe_psp_config.h. 
** Default is set to #MEMSCRUB_DEFAULT_PRIORITY.
** 
** \param[in] newStartAddr - Memory address to start from, usually zero
** \param[in] newEndAddr - Memory address to end at, usually end of the physical RAM
** \param[in] task_priority - The task priority
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_MEM_SCRUB_Set(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority);

/**
** \func Check if the Memory Scrubbing task is running
** 
** \par Description:
** This function provides the status whether the Memory Scrubbing task is running.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return true - If task is running
** \return false - If task is not running
*/
bool  CFE_PSP_MEM_SCRUB_isRunning(void);

/**
** \func Stop the memory scrubbing task
**
** \par Description:
** This function deletes the Memory Scrubbing task.  
** The task is deleted and the statistics are reset.
** 
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void  CFE_PSP_MEM_SCRUB_Delete(void);

/**
** \func Print the Memory Scrubbing statistics
** 
** \par Description:
** This function outputs to the console the following Memory Scrubbing statistics:
** Start memory address, End memory address, current memory page and total memory pages
**
** \par Assumptions, External Events, and Notes:
** Start memory address is usually 0. End memory address is usually set to the 
** last value of RAM address. Note that a page is 4098 bytes.
**
** \param None
**
** \return None
*/
void  CFE_PSP_MEM_SCRUB_Status(void);

/**
** \func Memory Scrubbing task
**
** \par Description:
** This function performs the Memory Scrubbing steps.
**
** \par Assumptions, External Events, and Notes:
** The scrubMemory function implemented by AiTech may never return an error.
**
** \param None
**
** \return None
*/
void  CFE_PSP_MEM_SCRUB_Task(void);

/**
** \func Initialize the Memory Scrubbing task
**
** \par Description:
** This function starts the Memory Scrubbing task as a child thread.
**
** \par Assumptions, External Events, and Notes:
** The scrubMemory function implemented by AiTech may never return an error.
**
** \param None
**
** \return None
*/
void  CFE_PSP_MEM_SCRUB_Init(void);

/**
** \func Enable the Memory Scrubbing task
**
** \par Description:
** This function enables the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** If the task is already running, do nothing. If the task is not running,
** then start it.
**
** \param None
**
** \return None
*/
void  CFE_PSP_MEM_SCRUB_Enable(void);

/**
** \func Disable the Memory Scrubbing task
**
** \par Description:
** This function disables the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** If the task is already running, delete it. If the task is not running,
** then do nothing.
**
** \param None
**
** \return None
*/
void  CFE_PSP_MEM_SCRUB_Disable(void);

#ifdef __cplusplus
}
#endif

#endif /* _PSP_MEM_SCRUB_H_ */
