/**
 ** \file
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
int32 CFE_PSP_MemScrubGet(CFE_PSP_MemScrubStatus_t *pStatus, bool talk)
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
int32 CFE_PSP_MemScrubErrStats(CFE_PSP_MemScrubErrStats_t *errStats, bool talkative)
{
    OS_printf(MEM_SCRUB_PRINT_SCOPE "CFE_PSP_MemScrubErrStats not implemented in Linux OS");
    return (CFE_PSP_ERROR_NOT_IMPLEMENTED);
}