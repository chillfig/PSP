/**
 ** \file  cfe_psp_support.c
 **
 ** \brief Contains glue routines between the cFE and the OS Board Support Package (BSP)
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved. \n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that 
 ** agreement.
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

#include "common_types.h"
#include "target_config.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_memory.h"
#include "psp_mem_scrub.h"
#include "psp_sp0_info.h"

/** \name Macros */
/** \{ */
/** \brief CPU ID */
#define CFE_PSP_CPU_ID               (GLOBAL_CONFIGDATA.Default_CpuId)
/** \brief CPU NAME */
#define CFE_PSP_CPU_NAME             (GLOBAL_CONFIGDATA.Default_CpuName)
/** \brief SPACECRAFT ID */
#define CFE_PSP_SPACECRAFT_ID        (GLOBAL_CONFIGDATA.Default_SpacecraftId)
/** \} */

/**
 ** \func Re-start
 **
 ** \par Description:
 ** This function is the entry point back to the BSP to restart the processor.
 ** cFE calls this function to restart the processor.\n
 ** Depending on the resetType, the function will clear the memory or not.
 **
 ** \par Assumptions, External Events, and Notes:
 ** system restart types defined in sysLib.h:\n
 ** - BOOT_NORMAL _"normal reboot with countdown, memory is not cleared"_\n
 ** - BOOT_NO_AUTOBOOT _"no autoboot if set, memory is not cleared"_\n
 ** - BOOT_CLEAR _"clear memory"_
 ** - BOOT_QUICK_AUTOBOOT _"fast autoboot, memory is not cleared"_
 **
 ** \param[in] resetType - Type of cFE reset
 **
 ** \return None
 */
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


/**
 ** \func Abort cFE startup
 **
 ** \par Description:
 ** This function provides the mechanism to abort the cFE startup process and 
 ** returns back to the OS.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function should not be called by the cFS applications.
 **
 ** \param[in] errorCode - Error code that causes the exit
 **
 ** \return None
 */
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


/**
 ** \func Flush memory caches
 **
 ** \par Description:
 ** This function flushes the processor caches. This function is in the PSP
 ** because it is sometimes implemented in hardware and sometimes taken 
 ** care of by the OS.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function is not implemented for the SP0-vxworks6.9 PSP since it is 
 ** managed by the SP0 BSP/VxWorks OS.
 **
 ** \param[in] type - Cache memory type
 ** \param[in] address - Pointer to the cache memory address
 ** \param[in] size - Cache memory size
 **
 ** \return None
 */
void CFE_PSP_FlushCaches(uint32 type, void *address, uint32 size)
{
    /*The cache is managed by the SP0 BSP/VxWorks OS. */
}


/**
 ** \func Get the CPU ID
 **
 ** \par Description:
 ** This function returns the CPU ID as pre-defined by the cFE for
 ** specific board and BSP.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The macro is defined in cfe_platform_cfg.h.
 **
 ** \param None
 **
 ** \return #CFE_PSP_CPU_ID
 */
uint32 CFE_PSP_GetProcessorId(void)
{
    return CFE_PSP_CPU_ID;
}


/**
 ** \func Get the spacecraft ID
 **
 ** \par Description:
 ** This function returns the spacecraft ID as pre-defined by the cFE.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The macro is defined in cfe_platform_cfg.h.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SPACECRAFT_ID
 */
uint32 CFE_PSP_GetSpacecraftId(void)
{
    return CFE_PSP_SPACECRAFT_ID;
}

/**
 ** \func Get the processor name
 **
 ** \par Description:
 ** This function returns the CPU name as pre-defined by the cFE.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The macro is defined in cfe_platform_cfg.h.
 **
 ** \param None
 **
 ** \return #CFE_PSP_CPU_NAME
 */
//UndCC_NextLine(SSET_058)
const char *CFE_PSP_GetProcessorName(void)
{
    return CFE_PSP_CPU_NAME;
}
