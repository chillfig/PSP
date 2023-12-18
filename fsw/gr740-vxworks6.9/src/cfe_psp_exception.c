/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file cfe_psp_exception.c
*
*   \brief cFE PSP exception-related functions
*
*   \brief This is the implementation of the PSP Exception API. Functions defined here handles exceptions 
*   occurring during the execution of CFS.
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
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <fppLib.h>
#include <excLib.h>
#include <taskLib.h>
#include <tickLib.h>

/*
edrPolicyHandlerHookGet
edrPolicyHandlerHookAdd
edrPolicyHandlerHookRemove
*/
#include <edrLib.h>
#include <private/edrLibP.h>

#include "arch/sparc/esfSparc.h"
#include "arch/sparc/fppSparcLib.h"
#include "arch/sparc/archSparc.h"

/*
** cFE includes
*/
#include "common_types.h"
#include <target_config.h>
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_exceptionstorage_types.h"
#include "cfe_psp_exceptionstorage_api.h"
#include "cfe_psp_memory.h"
#include "cfe_psp_timer.h"
/* #include "cfe_psp_exception.h" */

extern STATUS edrErrorPolicyHookRemove(void);

/*
** Defines
*/

/**
 ** \brief Default NTP Sync pre-print string 
 ** \par Description:
 ** This string is printed before every print related to NTP Sync API.
*/
#define PSP_EXCEP_PRINT_SCOPE         "PSP EXC: "

/**
 ** \brief g_ucOverRideDefaultedrPolicyHandlerHook
 ** 
 */
static bool g_ucOverRideDefaultedrPolicyHandlerHook = true;

/**
 ** \brief g_pDefaultedrPolicyHandlerHook
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** The EDR_POLICY_HANDLER_HOOK is a function pointer defined
 ** in VxWorks header file edrLibP.h.
 */
static EDR_POLICY_HANDLER_HOOK g_pDefaultedrPolicyHandlerHook = NULL;

/***************************************************************************
 **                        FUNCTIONS DEFINITIONS
 ***************************************************************************/

/**
 ** \brief Makes the proper call to CFE_ES_ProcessCoreException
 ** 
 ** \par Description:
 **
 **
 ** \par Assumptions, External Events, and Notes:
 ** Assuming the VxWorks OS will call this function with the right parameters.
 ** Thus, there is no check on the validity of the input parameters.
 ** When speSave() is called, it captures the last floating point
 ** context, which may not be valid. If a floating point exception
 ** occurs you can be almost 100% sure that this will reflect the
 ** proper context. But if another type of exception occurred then
 ** this has the possibility of not being valid. Specifically if a
 ** task that is not enabled for floating point causes a non-floating
 ** point exception, then the meaning of the floating point context
 ** will not be valid. If the task is enabled for floating point,
 ** then it will be valid.
 **
 ** \param[in] type - 
 **           EDR_FACILITY_KERNEL    - VxWorks kernel events
 **           EDR_FACILITY_INTERRUPT - interrupt handler events
 **           EDR_FACILITY_INIT      - system startup events
 **           EDR_FACILITY_BOOT      - system boot events
 **           EDR_FACILITY_REBOOT    - system restart events
 **           EDR_FACILITY_RTP       - RTP system events
 **           EDR_FACILITY_USER      - user generated events
 ** \param[in] pInfo_param - 
 **            A pointer to an architecture-specific EXC_INFO structure,
 **            in case of exceptions, with CPU exception information. The exception
 **            information is saved by the default VxWorks exception handler.
 **            The structure is defined for each architecture in one of these
 **            files:  target/h/arch/arch/excArchLib.h For example:  target/h/arch/ppc/excPpcLib.h
 ** \param[in] debug - 
 **            This flag indicates whether the ED&R system is in debug (also known as lab) mode,
 **            or in field (or deployed) mode.
 **
 ** \return True - Do not stop offending task
 ** \return False - Stop offending task
 */
BOOL CFE_PSP_edrPolicyHandlerHook(int type, void *pInfo_param, BOOL debug)
{
    CFE_PSP_Exception_LogData_t *pBuffer = NULL;
    BOOL returnStatus = false;
    EDR_TASK_INFO *pInfo = NULL;
    
    /*
    Assuming the VxWorks OS will call this function with the right parameters.
    Thus, there is no check on the validity of the input parameters.
    */
    pInfo = pInfo_param;

    /* Check if there is space in the Exception Storage Buffer. Only up to CFE_PSP_MAX_EXCEPTION_ENTRIES */
    pBuffer = CFE_PSP_Exception_GetNextContextBuffer();
    if (pBuffer != NULL)
    {
        /*
         * Immediately get a snapshot of the timebase when exception occurred
         *
         * This is because the remainder of exception processing might be done
         * in a cleanup job as a low priority background task, and might be
         * considerably delayed from the time the actual exception occurred.
         */
        vxTimeBaseGet(&pBuffer->context_info.timebase_upper, &pBuffer->context_info.timebase_lower);

        pBuffer->sys_task_id = pInfo->taskId;
        pBuffer->context_info.vector = pInfo->vector;

        /*
         * Save Exception Stack frame
         */
        memcpy(&pBuffer->context_info.esf, pInfo->pEsf, sizeof(pBuffer->context_info.esf));

        CFE_PSP_Exception_WriteComplete();

        /* After write complete, the EDR data in RAM is ready to be backup up on EEPROM */
        /* CFE_PSP_SaveToNVRAM(); */

        if (g_ucOverRideDefaultedrPolicyHandlerHook == false)
        {
            /*
            When using CFE_PSP_AttachExceptions, g_pDefaultedrPolicyHandlerHook is left to NULL
            */
            if (g_pDefaultedrPolicyHandlerHook != NULL)
            {
                returnStatus = g_pDefaultedrPolicyHandlerHook(type, pInfo, debug);
            }
            else
            {
                OS_printf(PSP_EXCEP_PRINT_SCOPE "Override enabled, but Handler Hook is not setup\n");
            }
        }
    }
    else
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE 
                  "No more storage available in exception buffer. Increase PSP_EXCEP_PRINT_SCOPE\n");
    }

    if (GLOBAL_CFE_CONFIGDATA.SystemNotify != NULL)
    {
        /* notify the CFE of the event */
        GLOBAL_CFE_CONFIGDATA.SystemNotify();
    }

    return returnStatus;
}


/**********************************************************
 * 
 * Function: CFE_PSP_AttachExceptions
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_AttachExceptions(void)
{
    /*
    The PSP implementation uses the private/edrLibP.h header. The BSP is in 
    charge of implementing the functions in the library (or some of them).
    GR740 BSP does not implement edrPolicyHandlerHookRemove().
    */
    g_pDefaultedrPolicyHandlerHook = edrPolicyHandlerHookGet();

    /* If there is a handler function defined, then remove it */
    if (g_pDefaultedrPolicyHandlerHook != NULL)
    {
        /* The call to edrErrorPolicyHookRemove will return ERROR if the handler
         * is NULL otherwise it set the handler to NULL. No action was required but
         * ignoring an error is a bad practice.
         */
        if (edrErrorPolicyHookRemove() != OK)
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE
                      "edrErrorPolicyHookRemove() failed for address 0x%p\n", 
                      g_pDefaultedrPolicyHandlerHook);

            g_pDefaultedrPolicyHandlerHook = NULL;
        }

    }

    /* Add our own handler function */
    if (edrPolicyHandlerHookAdd(CFE_PSP_edrPolicyHandlerHook) == CFE_PSP_ERROR)
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE
                  "edrPolicyHandlerHookAdd() failed for CFE_PSP_edrPolicyHandlerHook()\n");
    }
    else
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE "Attached cFE Exception Handler\n");
    }
}

/**********************************************************
 * 
 * Function: CFE_PSP_SetDefaultExceptionEnvironment
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /*
     ** Nothing to do here - BSP sets it up, From
     ** http://www.gaisler.com/doc/vxworks-bsps-6.9.pdf, section 6.2 Exception
     ** handling, Rev 2.1.3, February 2020
     ** The trap table is defined by the BSP in sysALib.s. The default action of
     ** unknown or unhandled traps is to enter excEnt().  The traps 0x02-0x04,
     ** 0x07-0x10 and 0x20-0x7f will cause a fatal exception - includes floating
     ** point and alignment exceptions.  It is possible to modify the the fatal
     ** exception handlers in target/config/comps/src/ edrStubs.c to change
     **  the default behavior.
     */
}

/**********************************************************
 * 
 * Function: CFE_PSP_ExceptionGetSummary_Impl
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_ExceptionGetSummary_Impl(const CFE_PSP_Exception_LogData_t* Buffer, char *ReasonBuf, uint32 ReasonSize)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    int32       iRetChar = 0;
    const char  *pTaskName = NULL;

    if ((Buffer == NULL) || (ReasonBuf == NULL))
    {
        iRet_code = CFE_PSP_ERROR;
    }
    else
    {
        /* Get the vxWorks task name */
        pTaskName = taskName(Buffer->sys_task_id);

        /* If the task is no longer available, return pointer to local "N/A" string */
        if (pTaskName == NULL)
        {
            pTaskName = "N/A";
        }

        /* 
        if pTaskName is longer than ReasonSize then ReasonBuf will be a truncated string 
        hiding part of the Task Name. No error will occur.
        */
        iRetChar = snprintf(ReasonBuf, ReasonSize, "Vector=0x%06X, Task ID=0x%08X, vxWorks Task Name=%s",
                            Buffer->context_info.vector,Buffer->sys_task_id,pTaskName);
        
        if (iRetChar < 0)
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "Could not save exception reason on buffer\n");
            iRet_code = CFE_PSP_ERROR;
        }
        if (iRetChar >= ReasonSize)
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "Could not save the whole exception reason string on buffer\n");
            iRet_code = CFE_PSP_ERROR;
        }
    }

    return iRet_code;
}

