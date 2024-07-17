/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file cfe_psp_support.c
*
*   \brief Contains glue routines between the cFE and the OS Board Support Package (BSP)
*
*   \brief The functions here allow the cFE to interface functions that are board and OS specific and 
*   usually do not fit well in the OS abstraction layer.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vxWorks.h>
#include "sysLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "cacheLib.h"
#include "rebootLib.h"
#include "intLib.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_memory.h"

#include "target_config.h"

#ifdef _WRS_VX_SMP
#include "vxCpuLib.h"
#endif

/** \name Vehicle and Processor IDs */
/** \{ */
/** \brief CPU ID */
#define CFE_PSP_CPU_ID               (GLOBAL_CONFIGDATA.Default_CpuId)
/** \brief CPU NAME */
#define CFE_PSP_CPU_NAME             (GLOBAL_CONFIGDATA.Default_CpuName)
/** \brief SPACECRAFT ID */
#define CFE_PSP_SPACECRAFT_ID        (GLOBAL_CONFIGDATA.Default_SpacecraftId)
/** \} */

/*
 * Track the overall "reserved memory block" at the start of RAM.
 * This single large block is then subdivided into separate areas for CFE use.
 */
extern CFE_PSP_MemoryBlock_t GR740_ReservedMemBlock;

/**
 * To support HALO redundancy scheme, OR the CPU ID in targets.cmake w/ the results of CFE_SUPPORT_GetProcessorIdx from the kernel.
 * Then cache.
 */
extern uint32_t CFE_SUPPORT_GetProcessorIdx(void);

/**
 * Provides the address where the VxWorks Kernel boot string is located
 * REF: /users/acssl/vxworks/pne_vxworks_69/vxworks-6.9/target/config/gr_cpci_gr740/sysLib.c
 * REF: /users/acssl/vxworks/pne_vxworks_69/vxworks-6.9/target/config/gr_cpci_gr740/config.h
 */
// extern char *sysGetBootLine();

/**********************************************************
 * 
 * Function: CFE_PSP_Restart
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_Restart(uint32 reset_type)
{
    OS_printf("WARNING: PSP Restart called with %d\n", reset_type);

    /*
    ** Delay for second or two, allow the print statement to send
    */
   
    OS_TaskDelay(100);

#ifndef _WRS_VX_SMP /* Changed for SMP API compatability. */
    taskLock();
    intLock();
#else
    /* Note: This only locks the current CPU core.  Other cores
     *  are still active and may continue to access system resources
     *  or service the watchdog on an SMP system.
     */
    taskCpuLock();
    intCpuLock();
#endif

    //TODO Ensure this works w/ final GR740 NGINI board
    if ( reset_type == CFE_PSP_RST_TYPE_POWERON )
    {
       CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_POWERON;
       CFE_PSP_FlushCaches(1, GR740_ReservedMemBlock.BlockPtr, GR740_ReservedMemBlock.BlockSize);
       reboot(BOOT_CLEAR);
    }
    else
    {
       CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type = CFE_PSP_RST_TYPE_PROCESSOR;
       CFE_PSP_FlushCaches(1, GR740_ReservedMemBlock.BlockPtr, GR740_ReservedMemBlock.BlockSize);
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
    uint32 uiRestart_Type = CFE_PSP_RST_TYPE_PROCESSOR;

    OS_printf("WARNING: PSP Panic called with %d\n", errorCode);

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

    /*
    ** Debug Switch is not set, do a processor Reset
    */
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
   if (type == 1)
   {
      cacheTextUpdate(address, size);
   }

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
    static uint32_t processorId = 0xFFFFFFFF;
    if (processorId == 0xFFFFFFFF)
    {
        processorId = CFE_PSP_CPU_ID | ((CFE_SUPPORT_GetProcessorIdx() & 0b11) + 1);
    }
    return processorId;
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
const char *CFE_PSP_GetProcessorName(void)
{
    return CFE_PSP_CPU_NAME;
}
