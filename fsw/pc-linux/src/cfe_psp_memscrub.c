/**
 ** \file cfe_psp_memscrub.c
 **
 ** \brief API for Memory Scrubbing on Linux
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
 ** Implementation of Memory Scrubbing task
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** These functions have not been implemented yet
 */

/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_memscrub.h"


/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubSet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubSet(CFE_PSP_MemScrubStatus_t *pNewConfiguration)
{
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubSet not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubIsRunning
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
bool CFE_PSP_MemScrubIsRunning(void)
{
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubDelete not implemented in Linux OS");
    return false;
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
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubDelete not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubGet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pConfig, size_t iConfigSize, bool talk)
{
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubGet not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubInit
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubInit(void)
{
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubInit not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemScrubTrigger
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemScrubTrigger(void)
{
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubTrigger not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
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
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubEnable not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
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
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubDisable not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
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
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubErrStats not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}