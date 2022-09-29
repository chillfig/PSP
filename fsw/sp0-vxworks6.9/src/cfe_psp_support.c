/**
 ** \file
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
#include <ioLib.h>
#include <unistd.h>
#include <vxWorks.h>
#include <cacheLib.h>
#include <rebootLib.h>
#include <bootLib.h>
/* For manipulating target catalog */
#include <bflashCt.h>
/* For loading a new kernel on catalog */
#include <progUtil.h>

#include "common_types.h"
#include "target_config.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_memory.h"
#include "cfe_psp_start.h"
#include "cfe_psp_memsync.h"
#include "cfe_psp_support.h"
#include "cfe_psp_exception.h"
#include "cfe_psp_memscrub.h"
#include "cfe_psp_sp0info.h"
#include "cfe_psp_timesync.h"
#include "cfe_psp_flash.h"

/** \name Vehicle and Processor IDs */
/** \{ */
/** \brief CPU ID */
#define CFE_PSP_CPU_ID               (GLOBAL_CONFIGDATA.Default_CpuId)
/** \brief CPU NAME */
#define CFE_PSP_CPU_NAME             (GLOBAL_CONFIGDATA.Default_CpuName)
/** \brief SPACECRAFT ID */
#define CFE_PSP_SPACECRAFT_ID        (GLOBAL_CONFIGDATA.Default_SpacecraftId)
/** \} */

/** \brief List of available CFS partitions in Flash memory */
static char g_cAvailable_cfs_partitions[][CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH] = CFE_PSP_STARTUP_AVAILABLE_PARTITIONS;

extern CFE_PSP_Startup_structure_t g_StartupInfo;
extern void CFE_PSP_FlushToFLASH(void);

/**********************************************************
 * 
 * Function: CFE_PSP_Restart
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_Restart(uint32 resetType)
{
    char    cStartupString[BOOT_FILE_LEN] = {'\0'};

    OS_printf("WARNING: PSP Restart called with %d\n", resetType);

    /* This cases are PSP specific */
    switch (resetType)
    {
        case CFE_PSP_RST_TYPE_SHELL:
            /*
            VxWorks will automatically boot CFS if a boot startup string is provided.
            By clearing the boot startup string, VxWorks will fall back to shell.
            */
            CFE_PSP_SetBootStartupString(cStartupString, false);
            resetType = CFE_PSP_RST_TYPE_PROCESSOR;
            break;

        case CFE_PSP_RST_TYPE_CFS_TOGGLE:
            /*
            Find next available CFS partition, and change to it
            Reboot with PROCESSOR to keep the 
            */
            CFE_PSP_ToggleCFSBootPartition();
            resetType = CFE_PSP_RST_TYPE_PROCESSOR;
            break;

        default:
            break;
    }

    if (resetType == CFE_PSP_RST_TYPE_POWERON)
    {
        CFE_PSP_MemSyncStop();
        /*
        When we delete all URM files, the reset type gets deleted too.
        This means that we have no way to tell CFS what was the reset type when
        it starts up again. CFE_PSP_InitProcessorReservedMemory will check for
        the URM files, and if it does not find them, it will assume that it is a
        reset type POWERON, else assume PROCESSOR.
         */
        CFE_PSP_DeleteProcessorReservedMemory();
        CFE_PSP_ClearNVRAM();
    }
    else
    {
        if (CFE_PSP_ReservedMemoryMap.BootPtr != NULL)
        {
            CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = resetType;
        }

        /* Stop Mem Sync and Synch URM to Flash */
        CFE_PSP_FlushToFLASH();

        CFE_PSP_SaveToNVRAM();
    }

    /* Delay to let console catch up on printing logs. */
    OS_TaskDelay(1000);

    reboot(BOOT_CLEAR);
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
    uint32 uiRestart_Type = CFE_PSP_RST_TYPE_PROCESSOR;

    OS_printf("WARNING: PSP Panic called with %d\n", errorCode);

    /** Clean up memory as much as possible before rebooting **/

    /* Dump PSP SP0 Information */
    CFE_PSP_SP0DumpData();

    /* Close Memory Scrubbing Task if still running */
    CFE_PSP_MemScrubDelete();

    /** Attempt to clean up memory before restarting target **/

    /*
    CFS calls this function when its own OS_* functions returns some errors
    due to errors returned by VxWorks functions. Because the errorCode actually
    tells us what failed, we can attempt to fix it and then reboot it. If we
    cannot fix it, we exit to console.
    Error Codes defined in cfe_psp.h
    */
    switch (errorCode)
    {
        case CFE_PSP_PANIC_MEMORY_ALLOC:
            /* Memory Allocation */
        case CFE_PSP_PANIC_VOLATILE_DISK:
            /* Volatile Disk */
        case CFE_PSP_PANIC_NONVOL_DISK:
            /* Nonvolatile Disk */

            /* Target will not recover User Reserved Memory on startup */
            uiRestart_Type = CFE_PSP_RST_TYPE_POWERON;
            break;
        /*
        Catch the rest of the error codes
        CFE_PSP_PANIC_STARTUP
        CFE_PSP_PANIC_STARTUP_SEM
        CFE_PSP_PANIC_CORE_APP
        CFE_PSP_PANIC_GENERAL_FAILURE
        */
        default:
            /* PROCESSOR restart */
            break;
    }

    /* Restart CFS */
    CFE_PSP_Restart(uiRestart_Type);
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

/**********************************************************
 * 
 * Function: CFE_PSP_ToggleCFSBootPartition
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_ToggleCFSBootPartition(void)
{
    int32   iRetCode = CFE_PSP_SUCCESS;
    int32   iChars = 0;
    uint8   ucIndex = 0u;
    uint8   ucEffectiveIndex = 0U;
    uint8   ucMaxIterations = sizeof(g_cAvailable_cfs_partitions) / sizeof(g_cAvailable_cfs_partitions[0]);
    char    cBootString[MAX_BOOT_LINE_SIZE] = {'\0'};
    
    for (ucIndex = 0; ucIndex < ucMaxIterations; ucIndex++)
    {
        if (memcmp(g_StartupInfo.active_cfs_partition,
                g_cAvailable_cfs_partitions[ucIndex],
                CFE_PSP_ACTIVE_PARTITION_MAX_LENGTH) == 0)
        {
            /* If this is the last record, restart from zero */
            if (ucIndex == (ucMaxIterations - 1))
            {
                ucEffectiveIndex = 0;
            }
            else
            {
                /* Set index to next one */
                ucEffectiveIndex = ucIndex;
                ucEffectiveIndex++;
            }
            
            /* Build string */
            iChars = snprintf(
                     cBootString,
                     sizeof(cBootString),
                     "%s/%s",
                     g_cAvailable_cfs_partitions[ucEffectiveIndex],
                     CFE_PSP_STARTUP_FILENAME
            );
            if ((iChars > sizeof(cBootString)) || (iChars < 3))
            {
                OS_printf("PSP: Could not construct new boot startup string.\n`%s`/`%s`\n",
                          g_cAvailable_cfs_partitions[ucEffectiveIndex],
                          CFE_PSP_STARTUP_FILENAME
                );
            }

            break;
        }
    }

    /* Set Boot Startup String */
    iRetCode = CFE_PSP_SetBootStartupString(cBootString, false);
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetBootStartupString
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_GetBootStartupString(char *startupBootString, uint32 bufferSize, uint32 talkative)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    BOOT_PARAMS target_boot = {};

    /* Check startupBootString length */
    if ((startupBootString == NULL) || (bufferSize < BOOT_FILE_LEN))
    {
        OS_printf("PSP: bufferSize too small, it needs to be %d bytes\n", BOOT_FILE_LEN);
        iRet_code = CFE_PSP_ERROR;
    }
    else
    {
        /* Get target parameters */
        if (CFE_PSP_GetBootStructure(&target_boot, talkative) == CFE_PSP_SUCCESS)
        {
            /* Save startupScript to provided pointer, but no more than provided bufferSize */
            strncpy(startupBootString, target_boot.startupScript, bufferSize);

            /* If talkative is turned ON, print the boot structure startup boot string */
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

/**********************************************************
 * 
 * Function: CFE_PSP_SetBootStartupString
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SetBootStartupString(char *startupScriptPath, uint32 talkative)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    BOOT_PARAMS target_boot = {};

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
            memcpy(target_boot.startupScript, startupScriptPath, BOOT_FILE_LEN);

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

/**********************************************************
 * 
 * Function: CFE_PSP_PrintBootParameters
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_PrintBootParameters(BOOT_PARAMS *target_boot_parameters)
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

/**********************************************************
 * 
 * Function: CFE_PSP_GetBootStructure
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_GetBootStructure(BOOT_PARAMS *target_boot_parameters, uint32 talkative)
{
    int32   iRet_code = CFE_PSP_ERROR;
    char    cBootString[MAX_BOOT_LINE_SIZE] = {'\0'};
    char    *pRet_code = NULL;

    /* Get boot string */
    if (sysNvRamGet(cBootString,MAX_BOOT_LINE_SIZE,0) == OK)
    {
        /* Convert boot string to structure */
        pRet_code = bootStringToStruct(cBootString, target_boot_parameters);

        if (talkative > 0)
        {
            OS_printf("PSP: Boot String:\n");
            OS_printf("`%s`\n",cBootString);
        }

        iRet_code = CFE_PSP_SUCCESS;
    }
    else
    {
        OS_printf("PSP: Could not get boot string\n");
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SetBootStructure
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SetBootStructure(BOOT_PARAMS new_boot_parameters, uint32 talkative)
{
    int32       iRet_code = CFE_PSP_ERROR;
    char        cBootString[MAX_BOOT_LINE_SIZE] = {'\0'};

    /* Convert new parameters to a single boot string */
    if (bootStructToString(cBootString, &new_boot_parameters) == OK)
    {
        /* Save it back on target */
        if (sysNvRamSet(cBootString, (int)strlen(cBootString) + 1, 0) == OK)
        {
            if (talkative > 0)
            {
                OS_printf("PSP: New Boot String:\n");
                OS_printf("`%s`\n",cBootString);
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

/**********************************************************
 * 
 * Function: CFE_PSP_KernelGetCRC
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
uint32 CFE_PSP_KernelGetCRC(char *pCatalogEntryName, bool bFirstCatalog)
{
    int32_t iNumCatEntries = 0;
    uint8_t uiIndex = 0;
    uint32_t uiCRC = 0;
    cat_struct_type *pCatStruct = NULL;

    /* If the catalog entry name is not set and the catalog number is outside the required range [1, 2] skip */
    if (pCatalogEntryName != NULL)
    {
        /* Get pointer to the first catalog */
        pCatStruct = getCatalogPointer(bFirstCatalog);

        if (pCatStruct != NULL)
        {
            /* Get number of entries in the catalog */
            iNumCatEntries = getCatalogEntryCount(pCatStruct);

            if (iNumCatEntries < MAX_CATALOG_ENTRIES)
            {
                /* Loop through all catalog entries */
                for (uiIndex = 0; uiIndex < iNumCatEntries; uiIndex++)
                {
                    /* Check if name of catalog entry is the same as our target */
                    if (strncmp(pCatStruct->fcatalog_s[uiIndex].id, pCatalogEntryName, 8) == 0)
                    {
                        /* Save locally the pre computed CRC */
                        uiCRC = pCatStruct->fcatalog_s[uiIndex].crc;
                        break;
                    }
                }
            }
        }
    }
    return uiCRC;
}

/**********************************************************
 * 
 * Function: CFE_PSP_KernelLoadNew
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_KernelLoadNew(char *pKernelPath, char *pKernelCatalogName)
{
    int32 iRetCode = CFE_PSP_ERROR;
    /* Buffer Size for Programming */
    const uint32_t uiBufferSize = 0x1000000;
    /* RAM address where kernel will start from */
    const uint32_t uiRAMAddress = 0x00200000;

    /* Create Buffer for kernel writing */
    if (CreateProgrammingBuffer(uiBufferSize) == OK)
    {
        /* Flash kernel file and automatically set it to default */
        if (flashProgFile(pKernelPath, pKernelCatalogName, uiRAMAddress, 1) == OK)
        {
            iRetCode = CFE_PSP_SUCCESS;
        }
        else
        {
            OS_printf("PSP: Could not flash new kernel\n");
        }
    }
    else
    {
        OS_printf("PSP: Could not pre-allocate buffer for new kernel\n");
    }
    
    return iRetCode;
}
