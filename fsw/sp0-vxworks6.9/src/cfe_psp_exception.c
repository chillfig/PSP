/**
 ** \file
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
#include "cfe_psp_exception.h"

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
static bool g_ucOverRideDefaultedrPolicyHandlerHook = true;

/**
 ** \brief g_pDefaultedrPolicyHandlerHook
 ** 
 ** \par Assumptions, External Events, and Notes:
 ** The EDR_POLICY_HANDLER_HOOK is a function pointer defined
 ** in VxWorks header file edrLibP.h.
 */
static EDR_POLICY_HANDLER_HOOK g_pDefaultedrPolicyHandlerHook = NULL;


/**
 ** \brief User Reserved Memory Header
 ** 
 ** \par Description:
 ** This header is written on the SP0 simulated NVRAM. It contains the signature
 ** "URM" for identification followed by the number of bytes required to recover
 ** the EDR and BOOT structures.
 */
typedef struct {
    /* Signature for identification */
    char signature[3];

    /* Size of EDR and BOOT structure */
    int32 size;
} CFE_PSP_URM_EDR_t;

/*
** Local Function Prototypes
*/

/**********************************************************
 * 
 * Function: CFE_PSP_LoadFromNVRAM
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32   CFE_PSP_LoadFromNVRAM(void)
{
    int32       iStatus = OK;
    int32       iRet_code = CFE_PSP_ERROR;
    int32       iURM_size = 0;
    int32       edr_size = 0;
    const int32 boot_size = sizeof(CFE_PSP_ReservedMemoryBootRecord_t);
    const int32 urm_pack_size = sizeof(CFE_PSP_URM_EDR_t);
    const char  urm_word[] = "URM";
    uint32      num_exceptions_in_urm = 0;
    CFE_PSP_URM_EDR_t urm_header = {"\n", 0x00};

    /* Copy URM signature and size in local memory */
    iStatus = userNvRamGet((char *)&urm_header, urm_pack_size, 0);

    if (iStatus == OK)
    {
        /* Check if there is the URM word in memory. This allows to skip loading in case NVRAM is empty. */
        if (memcmp(urm_word, urm_header.signature, 3) == 0)
        {
            /* Extract URM size from signature */
            iURM_size = urm_header.size;

            /* Prints the Signature Pack for debugging */
            OS_printf(PSP_EXCEP_PRINT_SCOPE 
                    "URM Header found {0x%02X 0x%02X 0x%02X} with size 0x%08X\n",
                    urm_header.signature[0],
                    urm_header.signature[1],
                    urm_header.signature[2],
                    urm_header.size
            );

            if (iURM_size >= boot_size)
            {

                edr_size = iURM_size - boot_size;

                /* Load Boot Record data from EEPROM */
                iStatus = userNvRamGet((char *)CFE_PSP_ReservedMemoryMap.BootPtr, boot_size, urm_pack_size);

                /* Load data from EEPROM to ED&R pointer */
                iStatus = userNvRamGet(
                        (char *)CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr,
                        edr_size,
                        boot_size + urm_pack_size
                );

                if (iStatus == OK)
                {
                    /* Get the number of exceptions loaded from EEPROM */
                    num_exceptions_in_urm = CFE_PSP_Exception_GetCount();

                    OS_printf(PSP_EXCEP_PRINT_SCOPE "URM Data Recovered (%d bytes) - %u new exception(s)\n",
                            iURM_size,
                            num_exceptions_in_urm
                    );
                    iRet_code = CFE_PSP_SUCCESS;
                }
                else
                {
                    OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamGet ERROR, could not load URM Data\n");
                }
            }
            else
            {
                OS_printf(PSP_EXCEP_PRINT_SCOPE "URM Header found but data is empty\n");
            }
        }
        else
        {
            /* This section occurs when the NVRAM has not been initialized, or
            the data is corrupted. */
            OS_printf(PSP_EXCEP_PRINT_SCOPE "No URM Header found {0x%02X 0x%02X 0x%02X}\n", 
                      urm_header.signature[0],
                      urm_header.signature[1],
                      urm_header.signature[2]
            );
        }
    }
    else
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamGet ERROR, could not load URM Header\n");
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_SaveToNVRAM
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32   CFE_PSP_SaveToNVRAM(void)
{
    int32             iStatus = OK;
    int32             iRet_code = CFE_PSP_ERROR;
    CFE_PSP_URM_EDR_t urm_signature_pack = {"URM", 0x00};
    const int32       urm_pack_size = sizeof(urm_signature_pack);
    const int32       edr_size = sizeof(CFE_PSP_ExceptionStorage_t);
    const int32       boot_size = sizeof(CFE_PSP_ReservedMemoryBootRecord_t);

    /* Assign the urm size pointer */

    /*
    Get the size of ED&R structure and save it in the signature pack.
    The structure is defined in cfe_psp_exceptionstorage_types.h
    */
    urm_signature_pack.size = edr_size + boot_size;
    
    /* Prints the Signature Pack for debugging */
    OS_printf(PSP_EXCEP_PRINT_SCOPE "Saving URM Header {0x%02X 0x%02X 0x%02X} with size 0x%08X\n", 
              urm_signature_pack.signature[0],
              urm_signature_pack.signature[1],
              urm_signature_pack.signature[2],
              urm_signature_pack.size);

    /* Save the URM signature and size in memory */
    iStatus = userNvRamSet((char *)&urm_signature_pack, urm_pack_size, 0);
    
    if (iStatus == OK)
    {
        /* Save the Boot URM data to EEPROM */
        iStatus = userNvRamSet((char *)CFE_PSP_ReservedMemoryMap.BootPtr, boot_size, urm_pack_size);

        /* Save the EDR URM data to EEPROM */
        iStatus = userNvRamSet(
                  (char *)CFE_PSP_ReservedMemoryMap.ExceptionStoragePtr,
                  edr_size,
                  urm_pack_size + boot_size
        );

        if (iStatus == OK)
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "Saved URM Data to EEPROM (%u bytes)\n", urm_signature_pack.size);
            iRet_code = CFE_PSP_SUCCESS;
        }
        else
        {
            OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamSet ERROR, could not save URM Data\n");
        }
    }
    else
    {
        OS_printf(PSP_EXCEP_PRINT_SCOPE "userNvRamSet ERROR, could not save URM Header\n");
    }

    return iRet_code;
}


/**********************************************************
 * 
 * Function: CFE_PSP_ClearNVRAM
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32   CFE_PSP_ClearNVRAM(void)
{
    int32             iRet_code = CFE_PSP_SUCCESS;
    CFE_PSP_URM_EDR_t urm_signature_pack = {"", 0x00};

    /* Write to EEPROM */
    if(userNvRamSet((char *) &urm_signature_pack, sizeof(urm_signature_pack), 0) != OK)
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
        /*
        ** Save floating point registers
        */
        speSave(&pBuffer->context_info.fp);

        CFE_PSP_Exception_WriteComplete();

        /* After write complete, the EDR data in RAM is ready to be backup up on EEPROM */
        CFE_PSP_SaveToNVRAM();

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
    References:
    vxworks-6.9/target/h/arch/ppc/archPpc.h
    vxworks-6.9/target/h/arch/ppc/ppc85xx.h
    vxworks-6.9/target/h/arch/ppc/vxPpcLib.h
    PDF: PowerPC™ e500 Core Family Reference Manual, E500CORERM.pdf, page 122, Table 2-35
    */
    
    vxMsrSet(vxMsrGet()   | 
             /* enable the external interrupt */
             _PPC_MSR_EE  | 
             /* enable floating point */
             _PPC_MSR_SPE | 
             /* machine check enable */
             _PPC_MSR_ME
    );

    vxSpefscrSet(vxSpefscrGet()     | 
                 /* Embedded floating-point invalid operation/input error exception enable */
                 _PPC_SPEFSCR_FINVE | 
                 /* Embedded floating-point overflow exception enable */
                 _PPC_SPEFSCR_FOVFE | 
                 /* Embedded floating-point divide by zero exception enable */
                 _PPC_SPEFSCR_FDBZE | 
                 /* Embedded floating-point round (inexact) exception enable */
                 /* _PPC_SPEFSCR_FINXE |  */
                 /* Embedded floating-point underflow exception enable */
                 _PPC_SPEFSCR_FUNFE | 
                 /* Integer overflow. An overflow occurred in the lower half of the register
                 while a SPE integer instruction is being executed. */
                 _PPC_SPEFSCR_OV
    );
}

/**********************************************************
 * 
 * Function: CFE_PSP_ExceptionGetSummary_Impl
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_ExceptionGetSummary_Impl(const CFE_PSP_Exception_LogData_t *Buffer, char *ReasonBuf, uint32 ReasonSize)
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

