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
 ** This is specific to the Linux OS.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** These functions have not been implemented yet
 **
 */

#ifndef PSP_MEM_SCRUB_H
#define PSP_MEM_SCRUB_H

#include "common_types.h"
#include "osapi.h"


/**
 ** \brief Default Memory Scrubbing pre-print string 
 **
 ** \par Description:
 ** This string is printed before every print related to Memory Scrubbing API.
 */
#define MEM_SCRUB_PRINT_SCOPE           "PSP MEM SCRUB: "


/**
 ** \brief MEMSCRUB Run Modes
 **
 ** \par Note:
 ** This enum has not been fully implemented with the functions yet
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
 ** \par Note:
 ** This struct has not been fully implemented with the functions yet
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
 ** This struct has not been fully implemented with the functions yet
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
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 ** 
 ** \param[in] pNewConfiguration - pointer to a Mem Scrub Configuration structure with new values
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration);

/**
 ** \func Check if the Memory Scrubbing task is running
 ** 
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \param None
 **
 ** \return false because this function has not been implemented yet
 */
bool CFE_PSP_MemScrubIsRunning(void);

/**
 ** \func Stop the memory scrubbing task
 **
 ** \par Description:
 ** This function has not been implemented yet
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param - None
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32 CFE_PSP_MemScrubDelete(void);

/**
 ** \func Initialize the Memory Scrubbing task
 **
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pStatus, bool talk);

/**
 ** \func Initialize the Memory Scrubbing task
 **
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32 CFE_PSP_MemScrubInit(void);

/**
 ** \func Trigger the Memory Scrubbing task
 **
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32 CFE_PSP_MemScrubTrigger(void);

/**
 ** \func Enable the Memory Scrubbing task
 **
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32  CFE_PSP_MemScrubEnable(void);

/**
 ** \func Disable the Memory Scrubbing task
 **
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param None
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32  CFE_PSP_MemScrubDisable(void);

/**
 ** \func Get the memory error statistics
 **
 ** \par Description:
 ** This function has not been implemented yet
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function has not been implemented yet
 **
 ** \param errStats - Pointer to CFE_PSP_MemScrubErrStats_t structure
 ** \param talkative - Boolean to indicate if the ckCtrs should be called to print out statistics
 **
 ** \return #CFE_PSP_ERROR_NOT_IMPLEMENTED because this function has not been implemented yet
 */
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, bool talkative);

#endif /* _PSP_MEM_SCRUB_H_ */