/**
 ** \file cfe_psp_start.c
 **
 ** \brief cFE PSP main entry point
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
 ** PSP Startup API for Aitech SP0. Functions implemented in this file are
 ** used to configure the SP0 target and the VxWorks OS, and gather information
 ** on how the system is setup.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */


/*
**  Include Files
*/
#include <stdio.h>
#include <string.h>
#include <vxWorks.h>
#include <taskLib.h>

/* Aitech BSP Specific */
#include <scratchRegMap.h>
#include <aimonUtil.h>

/*
** cFE includes
*/
#include "common_types.h"
#include "target_config.h"
#include "osapi.h"
#include "cfe_es.h"

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_memory.h"
#include "cfe_psp_module.h"
#include "cfe_psp_config.h"
#include "psp_start.h"
#include "psp_mem_scrub.h"
#include "psp_sp0_info.h"
#include "psp_verify.h"

/*
**  External Function Prototypes
*/
/** \brief OSAL OS_BSPMain Entry Point */
extern int OS_BSPMain(void);

/** \name PSP Configuration 
 ** \par Description:
 ** The preferred way to obtain the CFE tunable values at runtime is via
 ** the dynamically generated configuration object.  This allows a single build
 ** of the PSP to be completely CFE-independent.
*/
/** \{ */
/** \brief PSP Main function pointer */
#define CFE_PSP_MAIN_FUNCTION        (*GLOBAL_CONFIGDATA.CfeConfig->SystemMain)
/** \brief PSP Non Volatile startup file */
#define CFE_PSP_NONVOL_STARTUP_FILE  (GLOBAL_CONFIGDATA.CfeConfig->NonvolStartupFile)
/** \} */

/* Local Global Variables */

/** \brief Reset Type */
static uint32 g_uiResetType = 0;
/** \brief Reset Sub Type */
static uint32 g_uiResetSubtype = 0;
/** \brief Safe Mode User Data */
static USER_SAFE_MODE_DATA_STRUCT g_safeModeUserData;
/** \brief Console Shell Task ID */
static TASK_ID g_uiShellTaskID = 0;

/**
 ** \brief List of MCHK Errors Messages
 */
const char *g_pMachineCheckCause_msg[] = {
    "L1 instruction cache error",
    "L1 data cache error error: reset",
    "L1 data cache push error error: reset",
    "L2 multiple errors",
    "L2 tag parity error",
    "L2 multi-bit error",
    "L2 single bit error",
    "L2 configuration error",
    "DDR multi-bit error: reset",
    "Other machine check error"
};


/** \brief The list of VxWorks task to change the task priority to before finishing initialization.
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 ** The priority reassignment will be moved to kernel in a future release.
*/
static CFE_PSP_OS_Task_and_priority_t g_VxWorksTaskList[] =
{
    VXWORKS_TASK_PRIORITIES
};

/**
** \func Main entry-point
**
** \par Description:
** This function is the entry point that the real time OS calls to start cFS.
** This function will do any BSP/OS-specific setup, then call the entry point
** of cFS, which is this function.
**
** \par Assumptions, External Events, and Notes:
** cFE should not call this function.  See the description.
**
** \param None
**
** \return None
*/
void CFE_PSP_Main(void)
{
    int32 status = 0;
    status = OS_BSPMain();

    /* If OS_BSPMain returns an error, then print a note */
    if (status != 0)
    {
        /* At this point, CFS is closing and there is no guarantee that the OS_printf will work. */
        // UndCC_NextLine(SSET134)
        printf("\nPSP: Exiting CFE_PSP_Main() - OS_BSPMain application status [%d] \n", status);
    }
}


/**
** \func Print Power On Self Test (POST) results to the console
**
** \par Description:
** None
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void CFE_PSP_ProcessPOSTResults(void)
{
    uint64 bitExecuted = 0ULL;
    uint64 bitResult   = 0ULL;
    uint64 bitMask     = 0ULL;
    uint32 i = 0;

    if ((aimonGetBITExecuted(&bitExecuted, 0) == OK) &&
        (aimonGetBITResults(&bitResult, 0) == OK))
    {
        /* Only log Power on Self Test that are valid*/
        for (i = 0; i < FWCH_TMR; i++)
        {
            bitMask = (1ULL << i);
            if (bitExecuted & bitMask)
            {
                if (bitResult & bitMask)
                {
                    OS_printf("PSP: POST Test - FAILED - %s.\n",
                            AimonCompletionBlockTestIDStrings[i]);
                }
                else
                {
                    OS_printf("PSP: POST Test - PASSED - %s .\n",
                            AimonCompletionBlockTestIDStrings[i]);
                }
            }
            else
            {

                OS_printf("PSP: POST Test - Not Run - %s.\n",
                        AimonCompletionBlockTestIDStrings[i]);
            }
        }
    }
    else
    {
        OS_printf("PSP: POST aimonGetBITExecuted() or aimonGetBITResults() failed.\n");
    }
}

/**
** \func Determines the reset type and subtype
**
** \par Description:
** Reset Types are defined in Aitech headers
** Function will save reset types to the respective global static variables:
** - g_uiResetType
** - g_uiResetSubtype
** Finally, function will print to console the reset type
**
** \par Assumptions, External Events, and Notes:
** Output defines are defined in Aitech file scratchRegMap.h
**
** \param None
**
** \return RESET_SRC_POR 
** \return RESET_SRC_WDT 
** \return RESET_SRC_FWDT 
** \return RESET_SRC_CPCI 
** \return RESET_SRC_SWR 
*/
static RESET_SRC_REG_ENUM CFE_PSP_ProcessResetType(void)
{
    int32 status = 0;
    RESET_SRC_REG_ENUM resetSrc = 0;

    memset(&g_safeModeUserData, 0, sizeof(g_safeModeUserData));

    status  = ReadResetSourceReg(&resetSrc, false);
    if (status == OS_SUCCESS)
    {
        OS_printf("PSP: Reset Register = %02X\n",resetSrc);
        switch (resetSrc)
        {
            case RESET_SRC_POR:
                OS_printf("PSP: POWERON Reset: Power Switch ON.\n");
                g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
                g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;
                break;
            case RESET_SRC_WDT:
                OS_printf("PSP: PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.\n");
                /* The g_uiResetType should be CFE_PSP_RST_TYPE_PROCESSOR but
                * the SP0 FPGA resets the reserved memory. The g_uiResetType
                * is forced to a CFE_PSP_RST_TYPE_POWERON (power on)
                */
                g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
                g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                break;
            case RESET_SRC_FWDT:
                OS_printf("PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
                /* The g_uiResetType should be CFE_PSP_RST_TYPE_PROCESSOR but
                * the SP0 FPGA resets the reserved memory. The g_uiResetType
                * is forced to a CFE_PSP_RST_TYPE_POWERON (power on)
                */
                g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
                g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                break;
            case RESET_SRC_CPCI:
                OS_printf("PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
                /* The g_uiResetType should be CFE_PSP_RST_TYPE_PROCESSOR but
                * the SP0 FPGA resets the reserved memory. The g_uiResetType
                * is forced to a CFE_PSP_RST_TYPE_POWERON (power on)
                */
                g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
                g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;
                break;
            case RESET_SRC_SWR:
                OS_printf("PSP: POWERON Reset: Software Hard Reset.\n");
                g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
                g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;

                /*
                Aitech manual says that it always returs OK, but we should check
                anyway in case a new BSP changes the return value.
                */
                if(ReadSafeModeUserData(&g_safeModeUserData, false)!= OK)
                {
                    OS_printf("PSP: PROCESSOR Reset: failed to read safemode data.\n");
                }
                break;
            default:
                OS_printf("PSP: POWERON Reset: UNKNOWN Reset.\n");
                g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
                g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET;
                break;
        }
    }
    else
    {
        OS_printf("PSP: POWERON Reset: UNKNOWN Reset. Reset source read failed.\n");
        g_uiResetType = CFE_PSP_RST_TYPE_POWERON;
        g_uiResetSubtype = CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET;
    }

    return resetSrc;
}

/**
** \func Determines if started in safe mode and logs off nominal resets.
**
** \par Description:
** None
**
** \par Assumptions, External Events, and Notes:
** RESET_SRC_REG_ENUM is defined in Aitech file scratchRegMap.h
**
** \param resetSrc - Reset Type RESET_SRC_REG_ENUM
**
** \return None
*/
void CFE_PSP_LogSoftwareResetType(RESET_SRC_REG_ENUM resetSrc)
{
    uint8 ucIndex = 0u;
    uint8 ucMaxIterations = sizeof(g_pMachineCheckCause_msg) / sizeof(g_pMachineCheckCause_msg[0]);
    const char *pResetSrcString = NULL;
    
    switch (resetSrc)
    {
        case RESET_SRC_POR:
            pResetSrcString = "RESET_SRC_POR";
            break;
        case RESET_SRC_WDT:
            pResetSrcString = "RESET_SRC_WDT";
            break;
        case RESET_SRC_FWDT:
            pResetSrcString = "RESET_SRC_FWDT";
            break;
        case RESET_SRC_CPCI:
            pResetSrcString = "RESET_SRC_CPCI";
            break;
        case RESET_SRC_SWR:
            pResetSrcString = "RESET_SRC_SWR";
            break;
        default:
            pResetSrcString = "RESET_SRC_POR";
            break;
    }
    if (g_safeModeUserData.sbc == SM_LOCAL_SBC)
    {
        OS_printf("PSP: PROCESSOR rst Source = 0x%x = (%s) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
                resetSrc,
                pResetSrcString,
                g_safeModeUserData.safeMode,
                "LOCAL",
                g_safeModeUserData.reason,
                g_safeModeUserData.mckCause);
    }
    else
    {
        OS_printf("PSP: PROCESSOR rst Source = 0x%x = (%s) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
                resetSrc,
                pResetSrcString,
                g_safeModeUserData.safeMode,
                "REMOTE",
                g_safeModeUserData.reason,
                g_safeModeUserData.mckCause);
    }

    /* If the reason for reset is Software, the mckCause will provide the cause */
    if (resetSrc == RESET_SRC_SWR)
    {
        for (ucIndex = 0u; ucIndex < ucMaxIterations; ucIndex++)
        {
            if (CHECK_BIT((g_safeModeUserData.mckCause), (ucIndex)))
            {
                OS_printf("PSP %s\n", g_pMachineCheckCause_msg[ucIndex]);
            }
        }
    }
}

/**
** \func Application startup entry point from OSAL BSP.
**
** \par Description:
** SP0 Implementation Specific
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void OS_Application_Startup(void) //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    int32 status = OS_SUCCESS;
    int32 taskSetStatus = OS_SUCCESS;
    RESET_SRC_REG_ENUM resetSrc = 0;
    osal_id_t fs_id = 0;

    OS_printf_enable();

    /* Initialize the OS API data structures */
    status = OS_API_Init();
    if (status != OS_SUCCESS)
    {
        OS_printf("PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n",
               status);
        goto OS_Application_Startup_Exit_Tag;
    }

    /*
    ** Set up the virtual FS mapping for the "/cf" directory
    ** On this platform it is will use the /ram0/cf physical device.
    */
    status = OS_FileSysAddFixedMap(&fs_id, "/ram0/cf", "/cf");
    if (status != OS_SUCCESS)
    {
        /* Print for informational purposes --
         * startup can continue, but loads may fail later, depending on config. */
        OS_printf("PSP: OS_FileSysAddFixedMap() failure: %d\n", (int)status);
    }

    /* 
    Collect SP0 information for Telemetry
    This function should run as early as possible to collect information from
    hardware and POST, and setup the task to dump the collected information 
    when abort is called.
    */
    PSP_SP0_GetInfo();

    CFE_PSP_SetupReservedMemoryMap();

    /*
    ** Initialize the statically linked modules (if any)
    */
    CFE_PSP_ModuleInit();

    /* Initialize the watchdog, it's left disabled */
    CFE_PSP_WatchdogInit();

    /* PSP Determine the reset type */
    resetSrc = CFE_PSP_ProcessResetType();

    /* Initialize the reserved memory */
    if (CFE_PSP_InitProcessorReservedMemory(g_uiResetType) != OS_SUCCESS)
    {
        OS_printf("PSP: OS_Application_Startup() - CFE_PSP_InitProcessorReservedMemory() failed (0x%x)\n", status);
        goto OS_Application_Startup_Exit_Tag;
    }

    CFE_PSP_MEM_SCRUB_Init();

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    taskSetStatus = CFE_PSP_SetSysTasksPrio();
    if (taskSetStatus != OS_SUCCESS)
    {
        OS_printf("PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    }

    /* Print software reset type */
    CFE_PSP_LogSoftwareResetType(resetSrc);

    /* Print all POST results */
    CFE_PSP_ProcessPOSTResults();

    /* Set the resource configuration flags */
    OS_BSP_SetResourceTypeConfig(OS_OBJECT_TYPE_OS_TASK, VX_SPE_TASK);


    OS_printf("PSP: PSP Application Startup Complete\n");

    /* Call cFE entry point. This will return when cFE startup is complete. */
    CFE_PSP_MAIN_FUNCTION(g_uiResetType, g_uiResetSubtype, 1, CFE_PSP_NONVOL_STARTUP_FILE);

    OS_Application_Startup_Exit_Tag:
    return;
}

/**
** \func Application Run entry point from OSAL BSP.
**
** \par Description:
** SP0 Implementation Specific
**
** \par Assumptions, External Events, and Notes:
** This function is declared but empty so that we don't run the default OSAL
** equivalent function. The latter will actively suspend the console shell.
** Replication of the behaviour to suspend the shell is performed via API 
** function CFE_PSP_SuspendConsoleShellTask().
**
** \param None
**
** \return None
*/
#if OVERRIDE_OSAL_OS_APPLICATION_RUN
void OS_Application_Run(void) //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    /*
    The declaration of PSP OS_Application_Run is necessary to avoid running the
    OSAL OS_Application_Run which forces the tShell0 task, the VxWorks console 
    shell, into suspend mode.
    Replication of the behaviour to suspend the shell is performed via API function
    CFE_PSP_SuspendConsoleShellTask().
    */
}
#endif

/**
 ** \func Function Suspend/Resume the Console Shell Task.
 **
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] suspend - True to suspend task, False to resume task
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SuspendConsoleShellTask(bool suspend)
{
    int32 status = CFE_PSP_ERROR;

    /* Get the Shell Task ID if we have not done it already */
    if (g_uiShellTaskID == 0)
    {
        g_uiShellTaskID = taskNameToId("tShell0");
    }

    if (suspend)
    {
        status = taskSuspend(g_uiShellTaskID);
        if (status == OK)
        {
            OS_printf("Shell Task Suspended [0x%08X]\n",g_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be suspended [0x%08X]\n",g_uiShellTaskID);
        }
    }
    else
    {
        status = taskResume(g_uiShellTaskID);
        if (status == OK)
        {
            OS_printf("Shell Task Resumed [0x%08X]\n",g_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be resumed [0x%08X]\n",g_uiShellTaskID);
        }
    }

    return status;
}

/**
** \func Get restart type
**
** \par Description:
** This function returns the last reset type.  If a pointer to a valid
** memory space is passed in, it returns the reset sub-type in that memory.
** Right now the reset types are application-specific.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[out] resetSubType - Pointer to the variable that stores the returned reset sub-type
**
** \return Last reset type
*/
uint32 CFE_PSP_GetRestartType(uint32 *resetSubType)
{
    if (resetSubType != NULL)
    {
        *resetSubType = g_uiResetSubtype;
    }

    return (g_uiResetType);
}

/**
 ** \func Changes default task priority to a given priority
 **
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] tName - Task name
 ** \param[in] tgtPrio - New task priority
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetTaskPrio(const char *tName, uint8 tgtPrio)
{
    int32 tid = CFE_PSP_ERROR;
    int32 curPriority = 0;
    uint8 curPrio = 0;
    uint8 newPrio = 0;
    int32 status = CFE_PSP_ERROR;

    if ((tName != NULL) && (memchr(tName, (int) NULL, CFE_PSP_MAXIMUM_TASK_LENGTH) != NULL))
    {
        newPrio = tgtPrio;

        tid = taskNameToId((char *)tName);
        if (tid != TASK_ID_ERROR)
        {
            if (taskPriorityGet(tid, &curPriority) != ERROR) //UndCC_Line(SSET055) - returned by external function
            {
                curPrio = (uint8) curPriority;
                if (taskPrioritySet(tid, newPrio) == OK)
                {
                    OS_printf("PSP: Setting %s priority from %u to %u\n",
                            tName, curPrio, newPrio);
                    status = CFE_PSP_SUCCESS;
                }
                else
                {
                    OS_printf("PSP: taskPrioritySet() - Failed for %s priority from %u to %u\n",
                              tName, curPrio, newPrio);
                }
            }
        }
        else
        {
            OS_printf("PSP: Could not find task %s \n", tName);
        }
    }

    return status;
}


/**
 ** \func Changes system task priorities so that they are lower than CFS system
 ** task priorities
 **
 ** \par Description:
 ** None
 **
 ** \par Assumptions, External Events, and Notes:
 ** tNet0 priority should be adjusted to be right below what ever
 ** gets defined for CI/TO apps in your system if using the network
 ** interface CCSDS/UDP for CI/TO apps.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
static int32 CFE_PSP_SetSysTasksPrio(void)
{
    int32 status = CFE_PSP_SUCCESS;
    int32 index = 0;
    int32 ret = OS_ERROR;

    int32 numberOfTask = sizeof(g_VxWorksTaskList) / sizeof(CFE_PSP_OS_Task_and_priority_t);

    OS_printf("PSP: Setting system tasks' priorities for %d tasks.\n", numberOfTask);

    for (index = 0; index < numberOfTask; index++)
    {
        ret = CFE_PSP_SetTaskPrio(g_VxWorksTaskList[index].VxWorksTaskName, 
                                  g_VxWorksTaskList[index].VxWorksTaskPriority);
        if (ret != OS_SUCCESS)
        {
            status = CFE_PSP_ERROR;
        }
    }

    return status;
}

/**
** \func Initialize the Solid State Recorder
**
** \par Description:
** This function configures and initializes the Solid State Recorder for 
** a particular platform.
**
** \par Assumptions, External Events, and Notes:
** This function is not implemented for the SP0-vxworks6.9 PSP since SSR is not used.
**
** \param[in] bus - ATA controller number
** \param[in] device - ATA drive number
** \param[in] DeviceName - Name of the XBD device to create
**
** \return #CFE_PSP_SUCCESS
** \return #CFE_PSP_ERROR
*/
int32  CFE_PSP_InitSSR(uint32 bus, uint32 device, char *DeviceName )
{
    /* Function not implemented for the SP0 */
    OS_printf("PSP: CFE_PSP_InitSSR function not implemented for SP0\n");
    return CFE_PSP_ERROR_NOT_IMPLEMENTED;

}
