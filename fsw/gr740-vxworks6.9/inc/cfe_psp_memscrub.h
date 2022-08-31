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
 ** This is specific to the LEON4 processor on GR740 running VxWorks 6.9 OS.
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
** \addtogroup psp_public_api_gr740vx69 PSP Public APIs - GR740-VxWorks6.9 Platform
** \{
*/

/**
 ** \brief Default Memory Scrubbing pre-print string 
 ** \par Description:
 ** This string is printed before every print related to Memory Scrubbing API.
*/
#define MEMSCRUB_PRINT_SCOPE       "PSP MEM SCRUB: "


/** \brief Size of a Page in function scrubMemory */
#define MEMSCRUB_PAGE_SIZE         4096

/**
** \brief MEMSCRUB Run Modes
*/
typedef enum
{
    /**
     ** \brief Idle Mode
     **
     ** \par Description
     ** A task is created that runs continuously in a while loop with low priority
     */
    MEMSCRUB_IDLE_MODE = 1,
    /**
     ** \brief Timed Mode
     **
     ** \par Description
     ** A task is created that runs every X number of seconds. The start and
     ** end address are scrubbed through in blocks.
     */
    MEMSCRUB_TIMED_MODE = 2,
    /**
     ** \brief Manual Mode
     **
     ** \par Description
     ** End user calls the mem scrub function when appropriate. The start and
     ** end address are scrubbed through in blocks.
     */
    MEMSCRUB_MANUAL_MODE = 3
} MEMSCRUB_RunMode_t;


/**
 ** \brief Memory Scrubbing configuration and information
 **
 */
typedef struct
{
    /**
     ** \brief Defines the Memory Scrub Run Method
     ** \par Description:
     ** Mem Scrub can be run in multiple modes depending on the PSP config.
     ** - Idle Mode
     **   - A task is created that runs continuously in a while loop with
     **     low priority
     ** - Timed Mode
     **   - A task is created that runs every X number of seconds. The start and
     **     end address are scrubbed through in blocks.
     ** - Manual Mode
     **   - End user calls the mem scrub function when appropriate. The start and
     **     end address are scrubbed through in blocks.
     */
    MEMSCRUB_RunMode_t  RunMode;
    /**
     ** \brief Defines the block size in pages (4096 bytes each) to scrub
     ** \par Description:
     ** This applies only for Timed and Manual Mode
     */
    uint32              uiBlockSize_Pages;
    /**
     ** \brief Defines the Start Address to scrub in Timed Mode
     ** \par Description:
     ** The start address is managed within the task
     */
    uint32              uiTimedStartAddr;
    /**
     ** \brief Defines the end Address to scrub in Timed Mode
     ** \par Description:
     ** The end address is managed within the task
     */
    uint32              uiTimedEndAddr;    
    /**
     ** \brief Defines the number of milliseconds to wait before scrubbing another block
     ** \par Description:
     ** This applies only for Timed Mode
     */
    uint32              uiTaskDelay_msec;
    /**
     ** \brief Defines the Start Address to scrub
     ** \par Description:
     ** The start address can be any value in the address space starting from 0
     */
    uint32              uiMemScrubStartAddr;
    /**
     ** \brief Defines the end Address to scrub
     ** \par Description:
     ** End Address cannot be larger than the maximum RAM
     */
    uint32              uiMemScrubEndAddr;
    /**
     ** \brief Contains the Active Memory Scrubbing Current Page
     ** \par Description:
     ** Current page that the task is working on. This value gets
     ** reset whenever task restart.
     */
    uint32              uiMemScrubCurrentPage;
    /**
     ** \brief Contains the Active Memory Scrubbing Total Pages
     ** \par Description:
     ** Total number of pages processed since the start of the task. This value gets
     ** reset whenever task restart.
     */
    uint32              uiMemScrubTotalPages;
    /** \brief Contains the Memory Scrubbing task priority
     **
     */
    osal_priority_t     opMemScrubTaskPriority;
    /**
     ** \brief Contains the Active Memory Scrubbing Task ID
     ** \par Description:
     ** If 0, task is not running
     */
    osal_id_t           uiMemScrubTaskId;
    /** \brief Contains the maximum allowed task priority
     **
     */
    osal_priority_t     uiMemScrub_MaxPriority;
    /** \brief Contains the minimum allowed task priority
     **
     */
    osal_priority_t     uiMemScrub_MinPriority;
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
** next call to the Activate Memory Scrubbing function.
** If newEndAddr is set to a value larger than the actual physical memory limit,
** the function will use the physical memory limit.
** Task priority can only be set between #MEMSCRUB_PRIORITY_UP_RANGE and 
** #MEMSCRUB_PRIORITY_DOWN_RANGE defined in cfe_psp_config.h. 
** If the scrubMemory function is called in a task that has a timing restriction, 
** use Timed or Manual Mode.
** 
** \param[in] pNewConfiguration - pointer to a Mem Scrub Configuration structure with new values
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration);

/**
** \func Check if the Memory Scrubbing task is running
** 
** \par Description:
** This function provides the status whether the Memory Scrubbing task is running.
**
** \par Assumptions, External Events, and Notes:
** This applies only for Idle and Timed Mode.
** See #uiRunMode for more details
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
** This function deletes the memory scrubbing task and the semaphore.
** Then, it resets all memory scrub related variables to default.
** 
** \par Assumptions, External Events, and Notes:
** Since the semaphore is deleted. The only way to restart Mem Scrub task is to 
** reinitialize via #CFE_PSP_MemScrubInit.
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
** last value of RAM address. Note that a page is 4096 bytes.
** 
** \param[out] pStatus - pointer to a Mem Scrub Configuration structure
** \param[in] talk - Print out the status values
**
** \return None
*/
void  CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pStatus, bool talk);

/**
** \func Initialize the Memory Scrubbing task
**
** \par Description:
** This function gets a semaphore and initialize variables. If set to start
** on startup and running in Idle or Timed Mode, function will starts the Mem Scrub task.
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
** \func Trigger the Memory Scrubbing task
**
** \par Description:
** This function runs Memory Scrubbing for Manual Mode. Task will
** exit immediately and auto advance to next memory block controlled by 
** #MEMSCRUB_BLOCKSIZE_PAGES
**
** \par Assumptions, External Events, and Notes:
** If the task is already running, do nothing.
**
** \param None
**
** \return #CFE_PSP_SUCCESS - If successfully started memory scrubbing task
** \return #CFE_PSP_ERROR - Could not start memory scrubbing task
*/
int32 CFE_PSP_MemScrubTrigger(void);

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
** \return #CFE_PSP_ERROR - Could not start memory scrubbing task
*/
int32  CFE_PSP_MemScrubEnable(void);


/**
 ** \func Get the memory error statistics
 **
 ** \par Description:
 ** This function will fill the provided CFE_PSP_MemScrubErrStats_t pointer with
 ** memory error statistics
 **
 ** \par Assumptions, External Events, and Notes:
 ** TBD what these individual values truly represent
 **
 ** \param errStats - Pointer to CFE_PSP_MemScrubErrStats_t structure
 ** \param talkative - Boolean to indicate if the ckCtrs should be called to print out statistics
 **
 ** \return None
 */
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, bool talkative);

/**
** \} <!-- End of group "psp_public_api_gr740vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_MEM_SCRUB_H_ */
