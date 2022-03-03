/**
 ** \file  cfe_psp_support.c
 **
 ** \brief Contains glue routines between the cFE and the OS Board Support Package (BSP)
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
 ** The functions here allow the cFE to interface functions
 ** that are board and OS specific and usually don't fit well in the OS
 ** abstraction layer.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vxWorks.h>
#include <cacheLib.h>
#include <rebootLib.h>
#include <bootLib.h>

#include "common_types.h"
#include "target_config.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_memory.h"
#include "psp_support.h"
#include "psp_mem_scrub.h"
#include "psp_sp0_info.h"

/** \name Vehicle and Processor IDs */
/** \{ */
/** \brief CPU ID */
#define CFE_PSP_CPU_ID               (GLOBAL_CONFIGDATA.Default_CpuId)
/** \brief CPU NAME */
#define CFE_PSP_CPU_NAME             (GLOBAL_CONFIGDATA.Default_CpuName)
/** \brief SPACECRAFT ID */
#define CFE_PSP_SPACECRAFT_ID        (GLOBAL_CONFIGDATA.Default_SpacecraftId)
/** \} */

/**********************************************************
 * 
 * Function: CFE_PSP_Restart
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_Restart(uint32 resetType)
{
    if (resetType == CFE_PSP_RST_TYPE_POWERON)
    {
        CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_POWERON;
        /*Normally the cache would be flushed but the reserved memory is not cached
         * so the flush is not needed.
         */
        reboot(BOOT_CLEAR);
    }
    else
    {
        CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;
        /*Normally the cache would be flushed but the reserved memory is not cached
         * so the flush is not needed.
         */
        reboot(BOOT_NORMAL);
    }
}


/**********************************************************
 * 
 * Function: CFE_PSP_Panic
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_Panic(int32 errorCode)
{
    logMsg("CFE_PSP_Panic Called with error code = 0x%08X. Exiting.\n",
           errorCode, 0,0,0,0,0);

    /* Dump PSP SP0 Information */
    PSP_SP0_DumpData();

    /* Close Memory Scrubbing Task if still running */
    CFE_PSP_MEM_SCRUB_Delete();

    /* UndCC_NextLine(SSET_100, SSET110) */
    exit(-1);
}

/**********************************************************
 * 
 * Function: CFE_PSP_FlushCaches
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_FlushCaches(uint32 type, void *address, uint32 size)
{
    /*The cache is managed by the SP0 BSP/VxWorks OS. */
    OS_printf("PSP: FlushCache function not implemented. No need in SP0 target\n");
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetProcessorId
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
uint32 CFE_PSP_GetProcessorId(void)
{
    return CFE_PSP_CPU_ID;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetSpacecraftId
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
uint32 CFE_PSP_GetSpacecraftId(void)
{
    return CFE_PSP_SPACECRAFT_ID;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetProcessorName
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
//UndCC_NextLine(SSET_058)
const char *CFE_PSP_GetProcessorName(void)
{
    return CFE_PSP_CPU_NAME;
}

/**
 ** @internal
 **
 ** \func Gets current boot startup string
 **
 ** \par Description:
 ** Function gets the current target boot startup string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] startupBootString - Pointer to string where to save the boot string
 ** \param[in]  bufferSize - size of startupBootString array
 ** \param[in]  talkative - If true, print out the boot parameter structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 **
 ** @internal
 */
int32   CFE_PSP_GetBootStartupString(char *startupBootString, uint32 bufferSize, uint32 talkative)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    BOOT_PARAMS target_boot = {'\0'};

    /* Check startupBootString length */
    if ((startupBootString == NULL) || (bufferSize <= BOOT_FILE_LEN))
    {
        OS_printf("PSP: bufferSize to small, it needs to be %d bytes\n", BOOT_FILE_LEN);
        iRet_code = CFE_PSP_ERROR;
    }
    else
    {
        /* Get target parameters */
        if (CFE_PSP_GetBootStructure(&target_boot, talkative) == CFE_PSP_SUCCESS)
        {
            /* Save startupScript to provided pointer, but no more than provided bufferSize */
            strncpy(startupBootString, target_boot.startupScript, bufferSize);

            /* If talkative is turned ON, print the boot structure, and copied startup boot string */
            if (talkative > 0)
            {
                CFE_PSP_PrintBootParameters(&target_boot);
                OS_printf("PSP: startupBootString [%d]: `%s`\n",(uint32)strlen(startupBootString), startupBootString);
            }
        }
        else
        {
            OS_printf("PSP: ERROR, could not read the boot structure\n");
            iRet_code = CFE_PSP_ERROR;
        }
    }
    return iRet_code;
}

/**
 ** @internal
 **
 ** \func Sets new boot startup string
 **
 ** \par Description:
 ** Function sets a new target boot startup string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] startupScriptPath
 ** \param[in] talkative - If true, print out the boot parameter structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 **
 ** @internal
 */
int32   CFE_PSP_SetBootStartupString(char *startupScriptPath, uint32 talkative)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    BOOT_PARAMS target_boot = {'\0'};

    /* Check startupBootString length */
    /* We don't need to check for minimum length because there might be a 
    case when the startupScriptPath is just empty string */
    if ((startupScriptPath == NULL) || (memchr(startupScriptPath, (int) NULL, BOOT_FILE_LEN) == NULL))
    {
        OS_printf("PSP: Provided startup script path is NULL or cannot be longer than %d bytes\n", BOOT_FILE_LEN);
        iRet_code = CFE_PSP_ERROR;
    }
    else
    {
        /* Get current parameters */
        if (CFE_PSP_GetBootStructure(&target_boot, talkative) == CFE_PSP_SUCCESS)
        {
            /* If talkative is turned ON, print the boot structure, and copied startup boot string */
            if (talkative > 0)
            {
                CFE_PSP_PrintBootParameters(&target_boot);
            }

            /* Update local structure */
            strncpy(target_boot.startupScript, startupScriptPath, BOOT_FILE_LEN);

            /* Save new parameters to target */
            iRet_code = CFE_PSP_SetBootStructure(target_boot, talkative);

            if (iRet_code != CFE_PSP_SUCCESS)
            {
                OS_printf("PSP: Could not save new boot structure\n");
                CFE_PSP_PrintBootParameters(&target_boot);
                iRet_code = CFE_PSP_ERROR;
            }
        }
        else
        {
            OS_printf("PSP: ERROR, could not read the boot structure\n");
            iRet_code = CFE_PSP_ERROR;
        }

    }

    return iRet_code;
}

/**
 ** @internal
 **
 ** \func Prints the boot paramters to console
 **
 ** \par Description:
 ** Prints the boot paramters to console
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] target_boot_parameters
 **
 ** \return None
 **
 ** @internal
 */
void    CFE_PSP_PrintBootParameters(BOOT_PARAMS *target_boot_parameters)
{
    OS_printf(
        "[\n"
        "bootDev: %s\n"
        "hostName: %s\n"
        "targetName: %s\n"
        "ead: %s\n"
        "bad: %s\n"
        "had: %s\n"
        "gad: %s\n"
        "bootFile: %s\n",
        target_boot_parameters->bootDev,
        target_boot_parameters->hostName,
        target_boot_parameters->targetName,
        target_boot_parameters->ead,
        target_boot_parameters->bad,
        target_boot_parameters->had,
        target_boot_parameters->gad,
        target_boot_parameters->bootFile
    );

    OS_printf(
        "startupScript: %s\n"
        "usr: %s\n"
        "passwd: %s\n"
        "other: %s\n"
        "procNum: %d\n"
        "flags: %d\n"
        "unitNum: %d\n"
        "]\n",
        target_boot_parameters->startupScript,
        target_boot_parameters->usr,
        target_boot_parameters->passwd,
        target_boot_parameters->other,
        target_boot_parameters->procNum,
        target_boot_parameters->flags,
        target_boot_parameters->unitNum
    );
}

/**
 ** @internal
 **
 ** \func Gets current boot parameter structure
 **
 ** \par Description:
 ** Function gets the target boot string and converts it to
 ** a BOOT_PARAM structure.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] target_boot_parameters
 ** \param[in]  talkative - If true, print out debug messages
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 **
 ** @internal
 */
int32   CFE_PSP_GetBootStructure(BOOT_PARAMS *target_boot_parameters, uint32 talkative)
{
    int32   iRet_code = CFE_PSP_ERROR;
    char    bootString[MAX_BOOT_LINE_SIZE] = {'\0'};
    char    *pRet_code = NULL;

    /* Get boot string */
    if (sysNvRamGet(bootString,MAX_BOOT_LINE_SIZE,0) == OK)
    {
        /* Convert boot string to structure */
        pRet_code = bootStringToStruct(bootString, target_boot_parameters);

        if (talkative > 0)
        {
            OS_printf("PSP: Boot String:\n");
            OS_printf("`%s`\n",bootString);
        }

        iRet_code = CFE_PSP_SUCCESS;
    }
    else
    {
        OS_printf("PSP: Could not get boot string\n");
    }

    return iRet_code;
}

/**
 ** @internal
 **
 ** \func Sets new boot parameter structure
 **
 ** \par Description:
 ** Function gets the target boot string and converts it to
 ** a BOOT_PARAM structure.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param[in] new_boot_parameters
 ** \param[in]  talkative - If true, print out debug messages
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 **
 ** @internal
 */
int32   CFE_PSP_SetBootStructure(BOOT_PARAMS new_boot_parameters, uint32 talkative)
{
    int32       iRet_code = CFE_PSP_ERROR;
    char        bootString[MAX_BOOT_LINE_SIZE] = {'\0'};

    /* Convert new parameters to a single boot string */
    if (bootStructToString(bootString, &new_boot_parameters) == OK)
    {
        /* Save it back on target */
        if (sysNvRamSet(bootString, (int)strlen(bootString), 0) == OK)
        {
            if (talkative > 0)
            {
                OS_printf("PSP: New Boot String:\n");
                OS_printf("`%s`\n",bootString);
            }
            iRet_code = CFE_PSP_SUCCESS;
        }
        else
        {
            OS_printf("PSP: Could not set new boot string\n");
        }
    }
    else
    {
        OS_printf("PSP: Could not convert structure to boot string\n");
    }

    return iRet_code;
}
