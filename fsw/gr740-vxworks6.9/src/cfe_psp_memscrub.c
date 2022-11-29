/**
 ** \file
 **
 ** \brief API for Memory Scrubbing on GR740
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
 ** Implementation of Memory Scrubbing
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** 
 ** 
 */

#include <vxWorks.h>

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_gr740.h"
#include "cfe_psp_memscrub.h"

/* Defined in cfe_psp_memory.c */
extern uint32 g_uiEndOfRam;

/** \brief Contains the boolean value if Mem Scrub should start at startup
 **
 */
static bool g_bMemScrubStartOnStartup = CFE_PSP_MEMSCRUB_TASK_START_ON_STARTUP;

/**
 ** \brief Mem Scrub Error Statistics
 */
static CFE_PSP_MemScrubErrStats_t g_MemScrub_Stats = 
{
	.uiCurrentPosition = 0,
	.uiRUNCOUNT = 0,
	.uiBLKCOUNT = 0,
	.uiCECNT = 0,
	.uiUECNT = 0
};

/**
 ** \brief Mem Scrub configuration and status structure
 */
static CFE_PSP_MemScrubStatus_t g_MemScrub_Status = 
{
/*
What are the MODEs for?
Operation Mode (MODE) - 00=Scrub, 01=Regenerate, 10=Initialize, 11=Undefined
This below is not the same!
*/
    .RunMode = CFE_PSP_MEMSCRUB_RUN_MODE,
    .uiMemScrubStartAddr = CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR,
    .uiMemScrubEndAddr = CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR
};

/**
 ** \brief Checks Mem Scrub configuration structure for errors
 **
 ** \par Description
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param pNewValues - Mem Scrub Status structure
 **
 ** \return #CFE_PSP_SUCCESS if all the variables pass checks
 ** \return #CFE_PSP_ERROR if any check fails
 */
static int32 CFE_PSP_MemScrubValidate(CFE_PSP_MemScrubStatus_t *pNewValues)
{
    int32 iRetCode = CFE_PSP_SUCCESS;

    /* check address range */
    if (pNewValues->uiMemScrubEndAddr < pNewValues->uiMemScrubStartAddr)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Start address after end address\n");
        iRetCode = CFE_PSP_ERROR;
    }

    /* If top of memory has not been initialized, then initialize it */
    if (g_uiEndOfRam == 0)
    {
        g_uiEndOfRam = (uint32) sysPhysMemTop();
    }

	/* check highest address */
    if (pNewValues->uiMemScrubEndAddr > g_uiEndOfRam)
    {
        OS_printf(MEMSCRUB_PRINT_SCOPE "Invalid new end address\n");
        iRetCode = CFE_PSP_ERROR;
    }

    return iRetCode;
} /* CFE_PSP_MemScrubValidate */

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubInit
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubInit(void)
{
	int32 iReturnCode = CFE_PSP_ERROR;
	
	/* Set Operation Mode */
	SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_OPERATION_MODE, 0x00);
	/* Disable Secondary Memory Range */
	SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SECONDARY_RANGE, 0);

    iReturnCode = CFE_PSP_MemScrubValidate(&g_MemScrub_Status);
    if (iReturnCode == CFE_PSP_SUCCESS)
    {
        /* Set Start and End addresses */
        MEMSCRUB_REG->range_low_addr = g_MemScrub_Status.uiMemScrubStartAddr;
        MEMSCRUB_REG->range_high_addr = g_MemScrub_Status.uiMemScrubEndAddr;
        /* Set Loop Mode */
        if (g_MemScrub_Status.RunMode == MEMSCRUB_AUTOMATIC_MODE)
        {
            SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_LOOP_MODE, 1);
        }
        else
        {
            SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_LOOP_MODE, 0);
        }
        
        if (g_bMemScrubStartOnStartup)
        {
            CFE_PSP_MemScrubEnable();
        }

        iReturnCode = CFE_PSP_SUCCESS;
    }
	
	return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubIsRunning
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
bool  CFE_PSP_MemScrubIsRunning(void)
{
	return (bool)GET_MEMSCRUB_STATUS(MEMSCRUB_STATUS_CURRENT_STATE);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubDisable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubDisable(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;

	/* Enable Memory Scrubber */
	SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SCRUBBER_ENABLE, 0);

	if (CFE_PSP_MemScrubIsRunning() == false)
    {
        iReturnCode = CFE_PSP_SUCCESS;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubEnable
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubEnable(void)
{
    int32 iReturnCode = CFE_PSP_ERROR;

	/* Enable Memory Scrubber */
	SET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_SCRUBBER_ENABLE, 1);

	if (CFE_PSP_MemScrubIsRunning() == true)
    {
        iReturnCode = CFE_PSP_SUCCESS;
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubSet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration)
{
	int32 iReturnCode = CFE_PSP_ERROR;
	bool uiMemScrubEnabled = 0;
	
    iReturnCode = CFE_PSP_MemScrubValidate(pNewConfiguration);

	/* Validate new configuration */
	if (iReturnCode == CFE_PSP_SUCCESS)
    {	
        /* If Scrubber is running, stop it */
        uiMemScrubEnabled = CFE_PSP_MemScrubIsRunning();
        if (uiMemScrubEnabled == true)
        {
            CFE_PSP_MemScrubDisable();
        }
        
        /* Set new values, addresses are masked [4:0] should be zeros for start, ones for end */
        g_MemScrub_Status.uiMemScrubEndAddr = ( pNewConfiguration->uiMemScrubEndAddr |= 0x0000001F );
        g_MemScrub_Status.uiMemScrubStartAddr = ( pNewConfiguration->uiMemScrubStartAddr &= 0xFFFFFFE0 );
        
        /* Set values in registers */
        MEMSCRUB_REG->range_low_addr = g_MemScrub_Status.uiMemScrubStartAddr;
	    MEMSCRUB_REG->range_high_addr = g_MemScrub_Status.uiMemScrubEndAddr;

        /* If the SCrubber was running, start it */
        if (uiMemScrubEnabled == true)
        {
            CFE_PSP_MemScrubEnable();
        }
    }
	return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubStatus
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pConfig, size_t iConfigSize, bool talk)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (sizeof(g_MemScrub_Status) > iConfigSize)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    if (iReturnCode == CFE_PSP_SUCCESS)
    {
        memcpy(pConfig, &g_MemScrub_Status, sizeof(g_MemScrub_Status));

        if (talk == true)
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Mode %u\nAddress Range [0x%08X-0x%08X]\n",
                    pConfig->RunMode,
                    pConfig->uiMemScrubStartAddr,
                    pConfig->uiMemScrubEndAddr
                    );
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubErrStats
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *pErrStats, size_t iErrSize, bool talkative)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    if (sizeof(g_MemScrub_Stats) > iErrSize)
    {
        iReturnCode = CFE_PSP_ERROR;
    }

    if (iReturnCode == CFE_PSP_SUCCESS)
    {
        /* Gather mem scrub statistics and saved them in CFE_PSP_MemScrubErrStats_t structure */
        g_MemScrub_Stats.uiCurrentPosition = MEMSCRUB_REG->position;
        g_MemScrub_Stats.uiRUNCOUNT = GET_MEMSCRUB_STATUS(MEMSCRUB_STATUS_RUN_ERRORS);
        g_MemScrub_Stats.uiBLKCOUNT = GET_MEMSCRUB_STATUS(MEMSCRUB_STATUS_BLOCK_ERRORS);
        g_MemScrub_Stats.uiCECNT = GET_MEMSCRUB_STATUS(MEMSCRUB_AHB_STATUS_CECNT);
        g_MemScrub_Stats.uiUECNT = GET_MEMSCRUB_STATUS(MEMSCRUB_AHB_STATUS_UECNT);
        
        memcpy(pErrStats, &g_MemScrub_Stats, sizeof(g_MemScrub_Stats));

        if (talkative == true)
        {
            OS_printf(MEMSCRUB_PRINT_SCOPE "Mem Scrub Statistics\n"
                "CurrentPosition = %u\n"
                "RUNCOUNT = %u\n"
                "BLKCOUNT = %u\n"
                "CECNT = %u\n"
                "UECNT = %u\n",
                g_MemScrub_Stats.uiCurrentPosition,
                g_MemScrub_Stats.uiRUNCOUNT,
                g_MemScrub_Stats.uiBLKCOUNT,
                g_MemScrub_Stats.uiCECNT,
                g_MemScrub_Stats.uiUECNT
                );
        }
    }

    return iReturnCode;
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubDelete
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubDelete(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;
    uint32_t uiMemScrubEnabled = 1;

   	/* If Scrubber is running, stop it */
	uiMemScrubEnabled = GET_MEMSCRUB_CONFIG(MEMSCRUB_CONFIG_LOOP_MODE);
	if (uiMemScrubEnabled == 1)
	{
		CFE_PSP_MemScrubDisable();
	}

    /* Reset all memory scrub related values to default */
    g_MemScrub_Status.RunMode = CFE_PSP_MEMSCRUB_RUN_MODE,
    g_MemScrub_Status.uiMemScrubStartAddr = CFE_PSP_MEMSCRUB_DEFAULT_START_ADDR;
    g_MemScrub_Status.uiMemScrubEndAddr = CFE_PSP_MEMSCRUB_DEFAULT_END_ADDR;

    return iReturnCode;
}
