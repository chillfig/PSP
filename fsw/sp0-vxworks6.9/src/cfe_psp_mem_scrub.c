/**
 ** \file cfe_psp_mem_scrub.c
 **
 ** \brief API for Memory Scrubbing on SP0
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
 ** Implementation of Memory Scrubbing task using Aitech internal functions
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

#include <vxWorks.h>

/* Memory Scrubbing */
#include <mem_scrub.h>

/* Include PSP API */
#include "cfe_psp.h"

/* For CFE_PSP_SetTaskPrio() */
#include "psp_start.h"
#include "psp_mem_scrub.h"
#include "cfe_psp_config.h"

/* Defined in cfe_psp_memory.c */
extern uint32 g_uiEndOfRam;

/** \brief Task Priority of Memory Scrubbing Task */
static osal_priority_t g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;

/**
 ** \brief Contains the Active Memory Scrubbing Task ID
 ** \par Description:
 ** If 0, task is not running
 */
static uint32 g_uiMemScrubTaskId = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Start Address
 ** \par Description:
 ** The start address can be anything in the address space.
 */
static uint32 g_uiMemScrubStartAddr = 0;

/**
 ** \brief Contains the Active Memory Scrubbing End Address
 ** \par Description:
 ** End Address cannot be larger than the maximum RAM
 */
static uint32 g_uiMemScrubEndAddr = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Current Page
 ** \par Description:
 ** Current page that the task is working on. This value gets
 ** reset whenever task restart.
 */
static uint32 g_uiMemScrubCurrentPage = 0;

/**
 ** \brief Contains the Active Memory Scrubbing Total Pages
 ** \par Description:
 ** Total number of pages processed since the start of the task. This value gets
 ** reset whenever task restart.
 */
static uint32 g_uiMemScrubTotalPages = 0;

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
int32 CFE_PSP_MEM_SCRUB_Set(uint32 newStartAddr, uint32 newEndAddr, osal_priority_t task_priority)
{
    int32 iReturnCode = CFE_PSP_ERROR;

    /* Verify start and end address */
    if (newStartAddr < newEndAddr)
    {
        g_uiMemScrubStartAddr = newStartAddr;

        /* If top of memory has not been initialized, then initialize it */
        if (g_uiEndOfRam == 0)
        {
            g_uiEndOfRam = (uint32) sysPhysMemTop();
        }

        if (newEndAddr > g_uiEndOfRam)
        {
            g_uiMemScrubEndAddr = g_uiEndOfRam;
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Invalid newEndAddr. Range set to end of RAM\n");
        }
        else
        {
            g_uiMemScrubEndAddr = newEndAddr;
        }

        /*
        If the task priority is different from the currently set value,
        then change it as long as it is within the allowed range
        */
        if (task_priority != g_uiMemScrubTaskPriority)
        {
            /* If the Task Priority is set outside the range, use default range and 
            report error */
            if ((task_priority > MEMSCRUB_PRIORITY_UP_RANGE) || (task_priority < MEMSCRUB_PRIORITY_DOWN_RANGE))
            {
                /* Apply default priority */
                g_uiMemScrubTaskPriority = MEMSCRUB_DEFAULT_PRIORITY;
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Priority is outside range, using default `%u`\n",
                          g_uiMemScrubTaskPriority);
            }
            else
            {
                /* Apply new priority */
                g_uiMemScrubTaskPriority = task_priority;
            }

            iReturnCode = CFE_PSP_SetTaskPrio(MEMSCRUB_TASK_NAME, g_uiMemScrubTaskPriority);
        }
    }

    return iReturnCode;
} /* end CFE_PSP_MEM_SCRUB_Set */

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
** \param - None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Delete(void)
{
    if (OS_TaskDelete(g_uiMemScrubTaskId) == OS_SUCCESS)
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Memory Scrubbing Task Deleted\n");
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Memory Scrubbing Task Error\n");
    }
    /* Reset task stats and ID */
    g_uiMemScrubTotalPages = 0;
    g_uiMemScrubTaskId = 0;
}

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
void CFE_PSP_MEM_SCRUB_Status(void)
{
    if (g_uiMemScrubTaskId > 0)
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "StartAdr: 0x%08X - EndAdr: 0x%08X - CurrentPages: %u - TotalPages: %u\n",
                g_uiMemScrubStartAddr,
                g_uiMemScrubEndAddr,
                g_uiMemScrubCurrentPage,
                g_uiMemScrubTotalPages
                );
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task could not be found\n");
    }
}

/**
** \func Main function for the Memory Scrubbing task
**
** \par Description:
** This is the main function for the Memory Scrubbing task.
**
** \par Assumptions, External Events, and Notes:
** The scrubMemory function implemented by AiTech may never return an error.
** The function may never exit, the task is meant to be deleted using 
** CFE_PSP_MEM_SCRUB_Delete
** 
** \param None
**
** \return None
*/
void CFE_PSP_MEM_SCRUB_Task(void)
{
    STATUS status = CFE_PSP_ERROR;

    if (g_uiMemScrubEndAddr <= g_uiEndOfRam)
    {
        while(1)
        {
            /* Call the active memory scrubbing function */
            status = scrubMemory(g_uiMemScrubStartAddr, g_uiMemScrubEndAddr, &g_uiMemScrubCurrentPage);

            if (status == CFE_PSP_ERROR)
            {
                OS_printf(MEM_SCRUB_PRINT_SCOPE "Unexpected ERROR during scrubMemory call\n");
            }
            else
            {
                g_uiMemScrubTotalPages += g_uiMemScrubCurrentPage;
            }
        }
    }
}

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
void CFE_PSP_MEM_SCRUB_Init(void)
{
    /* Initialize */
    int32 iStatus = CFE_PSP_ERROR;

    /* If task is not running, create task */
    if (CFE_PSP_MEM_SCRUB_isRunning() == false)
    {
        /* Set the scrub settings to default */
        CFE_PSP_MEM_SCRUB_Set(0, g_uiEndOfRam, g_uiMemScrubTaskPriority);

        OS_printf(MEM_SCRUB_PRINT_SCOPE "Starting Active Memory Scrubbing\n");

        iStatus = OS_TaskCreate(&g_uiMemScrubTaskId, 
                                MEMSCRUB_TASK_NAME, 
                                CFE_PSP_MEM_SCRUB_Task,
                                OSAL_TASK_STACK_ALLOCATE, 
                                OSAL_SIZE_C(1024), 
                                g_uiMemScrubTaskPriority, 
                                0
                                );
                                
        if (iStatus != OS_SUCCESS)
        {
            OS_printf(MEM_SCRUB_PRINT_SCOPE "Error creating task\n");
        }
    }
    else
    {
        /* If task is already runnning, notify user */
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Task already running\n");
    }
}

/**
** \func Check if the Memory Scrubbing task is running
** 
** \par Description:
** This function provides the status whether the Memory Scrubbing task is running.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param - None
**
** \return true - If task is running
** \return false - If task is not running
*/
bool CFE_PSP_MEM_SCRUB_isRunning(void)
{
    /* Initialize */
    bool        bReturnValue = true;
    int32       iStatus = OS_SUCCESS;
    osal_id_t   mem_scurb_id = 0;

    iStatus = OS_TaskGetIdByName(&mem_scurb_id, MEMSCRUB_TASK_NAME);

    if (iStatus == OS_ERR_NAME_NOT_FOUND)
    {
        bReturnValue = false;
    }

    return bReturnValue;
}

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
void CFE_PSP_MEM_SCRUB_Enable(void)
{
    /* If task is not running, initilize it */
    if (g_uiMemScrubTaskId > 0)
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task is already running\n");
    }
    else
    {
        CFE_PSP_MEM_SCRUB_Init();
    }
}

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
void CFE_PSP_MEM_SCRUB_Disable(void)
{
    /* If task is running, delete it */
    if (g_uiMemScrubTaskId > 0)
    {
        CFE_PSP_MEM_SCRUB_Delete();
    }
    else
    {
        OS_printf(MEM_SCRUB_PRINT_SCOPE "Active Memory Scrubbing task could not be found\n");
    }
}
