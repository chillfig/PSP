/**
 ** \file  cfe_psp_exception.c
 **  
 ** \brief cFE PSP Exception related functions
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
 ** This is the implementation of the PSP Exception API. Functions defined here
 ** handles exceptions occurring during the execution of CFS.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** The following was found in the <tt>VxWorks 6.9 architecture supplement, pg 179,
 ** for PPC85xx:</tt>\n
 **
 ** <em>"Do not confuse the hardware floating-point provided by the FPU with that
 ** provided by the SPE (see 6.3.10 Signal Processing Engine Support, p.190).
 ** If using the e500v2diab or e500v2gnu toolchains, you must use the speSave()
 ** speSave() and speRestore() routines to save and restore floating-point
 ** context."</em> \n
 **
 ** The e500 core's SPE is a hardware double precision unit capable of both
 ** scalar and vector(SIMD) computation.
 **
 */


/*
**  Include Files
*/
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <excLib.h>
#include <taskLib.h>
#include <speLib.h>
#include <arch/ppc/vxPpcLib.h>
#include <arch/ppc/esfPpc.h>

/*
edrPolicyHandlerHookGet
edrPolicyHandlerHookAdd
edrPolicyHandlerHookRemove
*/
#include <edrLib.h>
#include <private/edrLibP.h>

/*
** cFE includes
*/
#include "common_types.h"
#include "target_config.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_exceptionstorage_types.h"
#include "cfe_psp_exceptionstorage_api.h"
#include "cfe_psp_memory.h"
#include "psp_exceptions.h"

/*
** Defines
*/

/**
 ** \brief Default NTP Sync pre-print string 
 ** \par Description:
 ** This string is printed before every print related to NTP Sync API.
*/
#define PSP_EXCEP_PRINT_SCOPE         "PSP EXC: "

/*
**  External Declarations
*/

/** \brief Declared in Aitech BSP 'bootrom.map' */
extern STATUS edrErrorPolicyHookRemove(void);

/**
 ** \brief This function allows the user to read data from the user area of the non-volatile boot EEPROM
 */
extern STATUS userNvRamGet (char *dat_ptr, int nbytes, int offset);
/**
 ** \brief This function allows the user to write data to the user area of the non-volatile boot EEPROM
 */
extern STATUS userNvRamSet (char *dat_ptr, int nbytes, int offset);

/*
** Global variables
*/

/**
 ** \brief g_ucOverRideDefaultedrPolicyHandlerHook
 ** 
 */
static BOOL g_ucOverRideDefaultedrPolicyHandlerHook = true;

/**
 ** \brief g_pDefaultedrPolicyHandlerHook
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** The EDR_POLICY_HANDLER_HOOK is a function pointer defined
 ** in VxWorks header file edrLibP.h.
 */
static EDR_POLICY_HANDLER_HOOK g_pDefaultedrPolicyHandlerHook = NULL;


/*
** Local Function Prototypes
*/

/**
 ** \func Load ED&R data from EEPROM
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R data in RAM is cleared. This function will
 ** first verify that the EEPROM user area contains valid ED&R data, then
 ** recover it. Data is saved to EEPROM using #CFE_PSP_edrSaveToEEPROM.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the onboard EEPROM user area with maximum usable
 ** space of 20 kB. Read and Write access is slow.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS - If the EDR signature and data is found
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_edrLoadFromEEPROM(void)
{
    int32   iStatus = OK;
    int32   iRet_code = CFE_PSP_SUCCESS;
    int     *pEDR_size = NULL;
    int     edr_pack_size = 7;
    char    edr_word[] = "EDR";
    uint32  num_exceptions_in_edr = 0;

    /* EDR Signature is defined as { 'E', 'D', 'R', 0x00, 0x00, 0x00, 0x00 } */
    char    edr_signature[7] = {};

    /* Copy EDR signature and size in local memory */
    iStatus = userNvRamGet(edr_signature, edr_pack_size, 0);

    if (iStatus == OK)
    {
        /* Check if there is the EDR word in memory. This allows to skip loading in case NVRAM is empty. */
        if (memcmp(edr_word, edr_signature, 3) == 0)
        {
            /* Extract EDR size from signature */
            pEDR_size = (int *)(edr_signature + 3);

            /* Prints the Signature Pack for debugging */
            OS_printf(PSP_EXCEP_PRINT_SCOPE 
                      "EDR Signature Pack found {0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X}\n",
                      edr_signature[0],
                      edr_signature[1],
                      edr_signature[2],
                      edr_signature[3],
                      edr_signature[4],
                      edr_signature[5],
                      edr_signature[6]
            );

            /* Load data from EEPROM to ED&R pointer */
            iStatus = userNvRamGet((char *)CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr, *pEDR_size, edr_pack_size);

            if (iStatus == OK)
            {
                /* Get the number of exceptions loaded from EEPROM */
                num_exceptions_in_edr = CFE_PSP_Exception_GetCount();

                OS_printf(PSP_EXCEP_PRINT_SCOPE "EDR Data Recovered (%d bytes) - %u new exception(s)\n",
                          *pEDR_size,
                          num_exceptions_in_edr
                );
            }
            else
            {
                OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamGet ERROR, could not load EDR Data\n");
                iRet_code = CFE_PSP_ERROR;
            }
        }
        else
        {
            /* This section occurs when the NVRAM has not been initialized, or
            the data is corrupted. */
            OS_printf(PSP_EXCEP_PRINT_SCOPE "No EDR Signature Pack found {0x%02X 0x%02X 0x%02X}\n", 
                      edr_signature[0],
                      edr_signature[1],
                      edr_signature[2]
            );
            iRet_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamGet ERROR, could not load EDR Signature Pack\n");
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}

/**
 ** \func Save RAM ED&R in EEPROM
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R data in RAM is cleared. This function will
 ** save the ED&R data in RAM to the EEPROM user area. It will also save a
 ** simple signature to allow the #CFE_PSP_edrLoadFromEEPROM to validate the data.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the onboard EEPROM user area with maximum usable
 ** space of 20 kB. Read and Write access is slow.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_edrSaveToEEPROM(void)
{
    int32   iStatus = OK;
    int32   iRet_code = CFE_PSP_SUCCESS;
    int     edr_pack_size = 0;
    char    edr_signature_pack[7] = { 'E', 'D', 'R', 0x00, 0x00, 0x00, 0x00 };
    int     *pEDR_size = NULL;

    /* Assign the edr size pointer */
    pEDR_size = (int *)(edr_signature_pack + 3);
    /* Get size of edr signature pack */
    edr_pack_size = sizeof(edr_signature_pack);

    /*
    Get the size of ED&R structure and save it in the signature pack.
    The structure is defined in cfe_psp_exceptionstorage_types.h
    */
    *pEDR_size = (int) sizeof(CFE_PSP_ExceptionStorage_t);

    /* Prints the Signature Pack for debugging */
    OS_printf(PSP_EXCEP_PRINT_SCOPE "Saving EDR Signature Pack {0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X}\n", 
              edr_signature_pack[0],
              edr_signature_pack[1],
              edr_signature_pack[2],
              edr_signature_pack[3],
              edr_signature_pack[4],
              edr_signature_pack[5],
              edr_signature_pack[6]);

    /* Save the ED&R signature and size in memory */
    iStatus = userNvRamSet(edr_signature_pack, edr_pack_size, 0);
    
    if (iStatus == OK)
    {
        /* Save the ED&R data to EEPROM */
        iStatus = userNvRamSet((char *)CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr, *pEDR_size, edr_pack_size);

        if (iStatus == OK)
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "Saving EDR Data to EEPROM (%u bytes)\n", *pEDR_size);
        }
        else
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamSet ERROR, could not save EDR Data\n");
            iRet_code = CFE_PSP_ERROR;
        }
    }
    else
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamSet ERROR, could not save EDR Signature\n");
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}


/**
 ** \func Clear EDR in EEPROM
 ** 
 ** \par Description:
 ** Function reset to \0 the edr signature in EEPROM.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_edrClearEEPROM(void)
{
    int32   iRet_code = CFE_PSP_SUCCESS;
    char    edr_signature_pack[7] = {'\0'};

    /* Write to EEPROM */
    if(userNvRamSet(edr_signature_pack, sizeof(edr_signature_pack), 0) != OK)
    {
        iRet_code = CFE_PSP_ERROR;
    }

    return iRet_code;
}

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
    EDR_TASK_INFO *pInfo = NULL;    //UndCC_Line(SSET_059_060_077_078)
    
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
        /*
        ** Save floating point registers
        */
        speSave(&pBuffer->context_info.fp);

        CFE_PSP_Exception_WriteComplete();

        /* After write complete, the EDR data in RAM is ready to be backup up on EEPROM */
        CFE_PSP_edrSaveToEEPROM();

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

/**
** \func Initialize exception handling
**
** \par Description:
** This function sets up the exception environment for a particular platform.
** It is called by CFE_ES_Main() in cfe_es_start.c 
** 
** \par Assumptions, External Events, and Notes:
** For VxWorks, this function initializes the EDR policy handling. The handler is 
** called for every exception that other handlers do not handle.  Note that the 
** floating point exceptions are handled by the default floating point exception 
** handler, which does a graceful recovery from floating point exceptions in the 
** file speExcLib.c.
**
** \param None
**
** \return None
*/
void CFE_PSP_AttachExceptions(void)
{
    /* The old PSP used excHookAdd instead of the EDR&R facility for handling
     * exceptions. The excHookAdd was replace with EDR&R because using the
     * exception hook interferes with the debugger. (breakpoints)
     */
    g_pDefaultedrPolicyHandlerHook = edrPolicyHandlerHookGet();

    /* If there is a handler function defined, then remove it */
    if (g_pDefaultedrPolicyHandlerHook != NULL)
    {
        /* The call to edrErrorPolicyHookRemove will return ERROR if the handler
         * is NULL otherwise it set the handler to NULL. No action was required but
         * ignoring an error is a bad practice.
         */
        if (edrErrorPolicyHookRemove() == ERROR) //UndCC_Line(SSET055) - returned by function
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE
                      "edrErrorPolicyHookRemove() failed for address 0x%x\n", 
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

/**
** \func Initialize default exception handling
**
** \par Description:
** This function sets up a default exception environment for a particular platform.
** 
** \par Assumptions, External Events, and Notes:
** For VxWorks, the exception environment is local to each task. Therefore, this
** must be called for each task that wants to do floating point and catch exceptions.
** Currently, this is automatically called from OS_TaskRegister() for every task.
**
** \param None
**
** \return None
*/
void CFE_PSP_SetDefaultExceptionEnvironment(void)
{
    /* Currently using the default VxWorks exception environment for the SP0 */
    OS_printf(PSP_EXCEP_PRINT_SCOPE "SetDefaultExceptionEnvironment not implemented\n");
}

/**
 ** \func Translate the exception context data into a string
 **
 ** \par Description:
 ** This function translates the exception context data into a user-friendly "reason" string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This is called in an application context to determine the cause of the exception.
 **
 ** \param[in] Buffer - Pointer to the Buffer Context data previously stored by ISR/signal handler
 ** \param[out] ReasonBuf - Buffer to store string
 ** \param[in] ReasonSize - Size of string buffer
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_ExceptionGetSummary_Impl(const CFE_PSP_Exception_LogData_t *Buffer, char *ReasonBuf, uint32 ReasonSize)
{
    int32       ret_code = CFE_PSP_SUCCESS;
    int         iRetChar = 0;
    const char  *pTaskName = NULL;

    if ((Buffer == NULL) || (ReasonBuf == NULL))
    {
        ret_code = CFE_PSP_ERROR;
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
            ret_code = CFE_PSP_ERROR;
        }
        if (iRetChar >= ReasonSize)
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "Could not save the whole exception reason string on buffer\n");
            ret_code = CFE_PSP_ERROR;
        }
    }

    return ret_code;
}

