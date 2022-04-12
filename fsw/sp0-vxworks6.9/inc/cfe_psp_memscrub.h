/**
 ** \file
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

#ifndef PSP_MEM_SCRUB_H
#define PSP_MEM_SCRUB_H

#include "common_types.h"
#include "osapi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \brief Default Memory Scrubbing pre-print string 
 ** \par Description:
 ** This string is printed before every print related to Memory Scrubbing API.
*/
#define MEM_SCRUB_PRINT_SCOPE           "PSP MEM SCRUB: "

/**
 ** \brief Start mem scrub on startup option 
 ** \par Description:
 ** This option can be set to indicate if PSP should start 
 ** mem scrub task on startup.
 ** 0 = Do not start task during startup
 ** 1 = Start task during startup
*/
#define MEM_SCRUB_TASK_START_ON_STARTUP      true

/**
 ** \brief Memory Scrubbing information struct
 ** 
 ** \par Description:
 ** Memory scrubbing struct containing useful information:
 **     - uiMemScrubStartAddr
 **     - uiMemScrubEndAddr
 **     - uiMemScrubCurrentPage
 **     - uiMemScrubTotalPages
 **     - opMemScrubTaskPriority
 **     - bIsRunning
*/
typedef struct
{
    uint32              uiMemScrubStartAddr;
    uint32              uiMemScrubEndAddr;
    uint32              uiMemScrubCurrentPage;
    uint32              uiMemScrubTotalPages;
    osal_priority_t     opMemScrubTaskPriority;
} CFE_PSP_MemScrubStatus_t;

/**
 ** \brief Memory Error Statistics struct
 **
 ** \par Description:
 ** Returns a structure containing information about
 ** memory errors:
 **     - uil2errTotal
 **     - uil2errMult
 **     - uil2errTagPar
 **     - uil2errMBECC
 **     - uil2errSBECC
 **     - uil2errCfg
 **     - uimchCause
 **     - uimchkHook
 **
 ** \par Assumptions, External Events, and Notes:
 ** From sysLib.c: "The machine check ISR will update these counters"
 */
typedef struct
{
    uint32              uil2errTotal;
    uint32              uil2errMult;
    uint32              uil2errTagPar;
    uint32              uil2errMBECC;
    uint32              uil2errSBECC;
    uint32              uil2errCfg;
    uint32              uimchCause;
    uint32              uimchkHook;
} CFE_PSP_MemScrubErrStats_t;

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
** Default is set to #MEMSCRUB_DEFAULT_PRIORITY.\n
** If the scrubMemory function is called in a task that has a timing restriction, 
** the scrub range (i.e. endAddr - startAddr) should be adjusted to a small 
** value but should be a multiple of the page size (4096 bytes).
** 
** \param[in] newStartAddr - Memory address to start from, usually zero
** \param[in] newEndAddr - Memory address to end at, usually end of the physical RAM
** \param[in] task_priority - The task priority
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_MemScrubSet(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority);

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
bool  CFE_PSP_MemScrubIsRunning(void);

/**
** \func Stop the memory scrubbing task
**
** \par Description:
** This function resets all memory scrub related variables,
** then call CFE_PSP_MemScrubDisable to delete the memory scrubbing
** task.
** 
** \par Assumptions, External Events, and Notes:
** This function should only be used for shutdown/reset. To stop/delete
** memory scrub task for other situations, use SCRUB_Disable
**
** \param - None
**
** \return #CFE_PSP_SUCCESS - If successfully deleted
** \return #CFE_PSP_ERROR - If unsuccessfully deleted
*/
int32  CFE_PSP_MemScrubDelete(void);

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
** \param[out] mss_Status - Pointer to struct containing mem scrub info
** \param[in] talk - Print out the status values
**
** \return None
*/
void  CFE_PSP_MemScrubStatus(CFE_PSP_MemScrubStatus_t *mss_Status, bool talk);

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
** \return #CFE_PSP_SUCCESS - If successful initialization
** \return #CFE_PSP_ERROR - If unsuccessful initialization
*/
int32  CFE_PSP_MemScrubInit(void);

/**
** \func Enable the Memory Scrubbing task
**
** \par Description:
** This function enables (starts) the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** If the task is already running, do nothing. If the task is not running,
** then start it.
**
** \param None
**
** \return #CFE_PSP_SUCCESS - If successfully started memory scrubbing task
** \return #CFE_PSP_ERROR - If unsuccesffuly started memory scrubbing task
*/
int32  CFE_PSP_MemScrubEnable(void);

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
** \return #CFE_PSP_SUCCESS - If successfully disabled memory scrub task
** \return #CFE_PSP_ERROR - If unsuccessfully disabled memory scrub task
*/
int32  CFE_PSP_MemScrubDisable(void);

/**
 ** \func Get the memory error statistics
 **
 ** \par Description:
 ** This function will fill the provided CFE_PSP_MemScrubErrStats_t pointer with
 ** memory error statistics
 **
 ** \par Assumptions, External Events, and Notes:
 ** TBD what these individual values truely represent
 **
 ** \param errStats - Pointer to CFE_PSP_MemScrubErrStats_t structure
 ** \param talkative - Boolean to indicate if the ckCtrs should be called to print out statistics
 **
 ** \return None
 */
void CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, bool talkative);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_MEM_SCRUB_H_ */
