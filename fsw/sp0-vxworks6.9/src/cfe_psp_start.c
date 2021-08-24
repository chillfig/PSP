/**
 ** \file cfe_psp_start.c
 **
 ** \brief cFE PSP main entry point
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
 ** None
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

#include "target_config.h"
#include "scratchRegMap.h"
#include "aimonUtil.h"
#include "cfe_psp_config.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "psp_start.h"
#include "cfe_psp_memory.h"
#include "cfe_psp_module.h"
#include "psp_mem_scrub.h"
#include "psp_sp0_info.h"

/*
** Macro Definitions
*/


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

/*
**  Local Function Prototypes
*/


/* Local Global Variables */

/** \brief Reset Type */
static uint32 ResetType = 0;
/** \brief Reset Sub Type */
static uint32 ResetSubtype = 0;
/** \brief Safe Mode User Data */
static USER_SAFE_MODE_DATA_STRUCT safeModeUserData;
/** \brief Console Shell Task ID */
static TASK_ID sg_uiShellTaskID = 0;

/*
 * The list of VxWorks task to change the task priority
 * to before finishing initialization.
 */
/** \brief The list of VxWorks task to change the task priority to before finishing initialization. */
CFE_PSP_OS_Task_and_priority_t VxWorksTaskList[] =
{
    {"tLogTask", 0},
    {"tShell0", 201},
    {"tWdbTask", 203},
    {"tVxdbgTask", 200},
    {"tNet0", 25},         /* Should be set right below CI/TO */
    {"ipftps", 202},
    {"ipcom_syslogd", 205},
    {"ipcom_telnetd", 204}
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
    printf("\nPSP: Exiting CFE_PSP_Main() - OS_BSPMain application status [%d] \n",status);
}


/**
** \func Log the Power On Self Test (POST) results to the system log.
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
    uint32 i;

    if ((aimonGetBITExecuted(&bitExecuted, 0) == OK) &&
        (aimonGetBITResults(&bitResult, 0) == OK))
    {
        /* Only log Power on Self Test that are valid*/
        for (i=0; i < FWCH_TMR; i++)
        {
            if (bitExecuted & (1ULL << i))
            {
                if (bitResult & (1ULL << i))
                {
                    OS_printf("CFE_PSP: CFE_PSP_ProcessPOSTResults: Test - FAILED - %s.\n",
                            AimonCompletionBlockTestIDStrings[i]);
                }
                else
                {
                    OS_printf("CFE_PSP: CFE_PSP_ProcessPOSTResults: Test - PASSED - %s .\n",
                            AimonCompletionBlockTestIDStrings[i]);
                }
            }
            else
            {

                OS_printf("CFE_PSP: CFE_PSP_ProcessPOSTResults: Test - Not Run - %s.\n",
                        AimonCompletionBlockTestIDStrings[i]);
            }
        }
    }
    else
    {
        OS_printf("CFE_PSP: CFE_PSP_ProcessPOSTResults: aimonGetBITExecuted() or aimonGetBITResults() failed.");
    }
}

/**
** \func Determines the reset type and logs off nominal resets.
**
** \par Description:
** Reset Types are defined in Aitech headers
**
** \par Assumptions, External Events, and Notes:
** None
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
    bool talkative = 1;

    memset(&safeModeUserData, 0, sizeof(safeModeUserData));

    status  = ReadResetSourceReg(&resetSrc,talkative);
    if (status == OS_SUCCESS )
    {
        OS_printf("CFE_PSP: Reset Register = %02X\n",resetSrc);
        switch (resetSrc)
        {
            case RESET_SRC_POR:
            {
                OS_printf("CFE_PSP: POWERON Reset: Power Switch ON.\n");
                ResetType = CFE_PSP_RST_TYPE_POWERON;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;
            }
            break;
            case RESET_SRC_WDT:
            {
                OS_printf("CFE_PSP: PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.\n");
                /* The ResetType should be CFE_PSP_RST_TYPE_PROCESSOR but
                 * the SP0 FPGA resets the reserved memory. The ResetType
                 * is forced to a CFE_PSP_RST_TYPE_POWERON (power on)
                 */
                ResetType = CFE_PSP_RST_TYPE_POWERON;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
            }
            break;
            case RESET_SRC_FWDT:
            {
                OS_printf("CFE_PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
                /* The ResetType should be CFE_PSP_RST_TYPE_PROCESSOR but
                 * the SP0 FPGA resets the reserved memory. The ResetType
                 * is forced to a CFE_PSP_RST_TYPE_POWERON (power on)
                 */
                ResetType = CFE_PSP_RST_TYPE_POWERON;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
            }
            break;
            case RESET_SRC_CPCI:
            {
                OS_printf("CFE_PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
                /* The ResetType should be CFE_PSP_RST_TYPE_PROCESSOR but
                 * the SP0 FPGA resets the reserved memory. The ResetType
                 * is forced to a CFE_PSP_RST_TYPE_POWERON (power on)
                 */
                ResetType = CFE_PSP_RST_TYPE_POWERON;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;
            }
            break;
            case RESET_SRC_SWR:
            {
                OS_printf("CFE_PSP: POWERON Reset: Software Hard Reset.\n");
                ResetType = CFE_PSP_RST_TYPE_POWERON;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;

                /*
                Aitech manual says that it always returs OK, but we should check
                anyway in case a new BSP changes the return value.
                */
                if(ReadSafeModeUserData(&safeModeUserData, talkative)!= OK)
                {
                    OS_printf("CFE_PSP: PROCESSOR Reset: failed to read safemode data.\n");
                }
            }
            break;
            default:
            {
                OS_printf("CFE_PSP: POWERON Reset: UNKNOWN Reset.\n");
                ResetType = CFE_PSP_RST_TYPE_POWERON;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET;
            }
            break;
        }
    }
    else
    {
        OS_printf("CFE_PSP: POWERON Reset: UNKNOWN Reset. Reset source read failed.\n");
        ResetType = CFE_PSP_RST_TYPE_POWERON;
        ResetSubtype = CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET;
    }

    if (ResetType == 0)
    {
        ResetType = CFE_PSP_ReservedMemoryMap.BootPtr->bsp_reset_type;
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
    const char* resetSrcString = NULL;
    switch (resetSrc)
    {
        case RESET_SRC_POR:
        {
            resetSrcString = "RESET_SRC_POR";
        }
        break;
        case RESET_SRC_WDT:
        {
            resetSrcString = "RESET_SRC_WDT";
        }
        break;
        case RESET_SRC_FWDT:
        {
            resetSrcString = "RESET_SRC_FWDT";
        }
        break;
        case RESET_SRC_CPCI:
        {
            resetSrcString = "RESET_SRC_CPCI";
        }
        break;
        case RESET_SRC_SWR:
        {
            resetSrcString = "RESET_SRC_SWR";
        }
        break;
        default:
        {
            resetSrcString = "RESET_SRC_POR";
        }
        break;
    }
    OS_printf("CFE_PSP: PROCESSOR rst Source = 0x%x = (%s) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
              resetSrc,
              resetSrcString,
              safeModeUserData.safeMode,
              (safeModeUserData.sbc == SM_LOCAL_SBC)? "LOCAL":"REMOTE",
              safeModeUserData.reason,
              safeModeUserData.mckCause);

    if (resetSrc == RESET_SRC_SWR)
    {
        if (safeModeUserData.mckCause != 0)
        {
            OS_printf("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n");
            OS_printf("CFE_PSP: MCHK_L1_DCHERR  =      (0x02) L1 data cache error error: reset\n");
            OS_printf("CFE_PSP: MCHK_L1_DCHPERR =      (0x04) L1 data cache push error error: reset\n");
            OS_printf("CFE_PSP: MCHK_L2_MULTERR =      (0x08) L2 multiple errors\n");
            OS_printf("CFE_PSP: MCHK_L2_TPARERR =      (0x10) L2 tag parity error\n");
            OS_printf("CFE_PSP: MCHK_L2_MBEERR  =      (0x20) L2 multi-bit error\n");
            OS_printf("CFE_PSP: MCHK_L2_SBEERR  =      (0x40) L2 single bit error\n");
            OS_printf("CFE_PSP: MCHK_L2_CFGERR  =      (0x80) L2 configuration error\n");
            OS_printf("CFE_PSP: MCHK_SDRAM_MBECC_ERR = (0x100) DDR multi-bit error: reset\n");
            OS_printf("CFE_PSP: MCHK_OTHER_MCHK_ERR =  (0x200) Other machine check error\n");
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
void OS_Application_Startup(void)
{
    int32 status = OS_SUCCESS;
    int32 taskSetStatus = OS_SUCCESS;
    RESET_SRC_REG_ENUM resetSrc = 0;
    osal_id_t fs_id;

    OS_printf_enable();

    /* Initialize the OS API data structures */
    status = OS_API_Init();
    if (status != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n",
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
        OS_printf("CFE_PSP: OS_FileSysAddFixedMap() failure: %d\n", (int)status);
    }

    /* 
    Collect SP0 information for Telemetry
    This function should run as early as possible to collect information from
    hardware and POST, and setup the task to dump the collected information 
    when abort is called.
    */
    getSP0Info();

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
    if (CFE_PSP_InitProcessorReservedMemory(ResetType) != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: OS_Application_Startup() - CFE_PSP_InitProcessorReservedMemory() failed (0x%x)\n",
                  status);
        goto OS_Application_Startup_Exit_Tag;
    }

    CFE_PSP_MEM_SCRUB_Init();

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    taskSetStatus = SetSysTasksPrio();
    if (taskSetStatus != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    }

    /* Print software reset type */
    CFE_PSP_LogSoftwareResetType(resetSrc);

    /* Print all POST results */
    CFE_PSP_ProcessPOSTResults();

    OS_printf("CFE_PSP: PSP Application Startup Complete\n");

    /* Call cFE entry point. This will return when cFE startup is complete. */
    CFE_PSP_MAIN_FUNCTION(ResetType, ResetSubtype, 1, CFE_PSP_NONVOL_STARTUP_FILE);

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
**
** \param None
**
** \return None
*/
void OS_Application_Run(void)
{
    /*
    The declaration of PSP OS_Application_Run is necessary to avoid running the
    OSAL OS_Application_Run which forces the tShell0 task, the VxWorks console 
    shell, into suspend mode.
    */
}

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
    int32 status;

    /* Get the Shell Task ID if we have not done it already */
    if (sg_uiShellTaskID == 0)
    {
        sg_uiShellTaskID = taskNameToId("tShell0");
    }

    if (suspend)
    {
        status = taskSuspend(sg_uiShellTaskID);
        if (status == OK)
        {
            OS_printf("Shell Task Suspended [0x%08X]\n",sg_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be suspended [0x%08X]\n",sg_uiShellTaskID);
        }
    }
    else
    {
        status = taskResume(sg_uiShellTaskID);
        if (status == OK)
        {
            OS_printf("Shell Task Resumed [0x%08X]\n",sg_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be resumed [0x%08X]\n",sg_uiShellTaskID);
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
** Right now the reset types are application-specific. For the cFE, they
** are defined in the cfe_es.h file.
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
        *resetSubType = ResetSubtype;
    }

    return (ResetType);
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
static int32 SetTaskPrio(const char* tName, int32 tgtPrio)
{
    int32 tid;
    int32 curPrio = 0;
    int32 newPrio;
    int32 status = CFE_PSP_SUCCESS;

    if ((tName != NULL) && (strlen(tName) > 0))
    {
        newPrio = tgtPrio;
        if (newPrio < 0)
        {
            newPrio = 0;
        }
        else if (newPrio > 255)
        {
            newPrio = 255;
        }

        tid = taskNameToId((char*)tName);
        if (tid != CFE_PSP_ERROR)
        {
            if (taskPriorityGet(tid, (int *)&curPrio) != CFE_PSP_ERROR)
            {
                OS_printf("PSP: SetTaskPrio() - Setting %s priority from %d to %d\n",
                       tName, curPrio, newPrio);

                if (taskPrioritySet(tid, newPrio) == CFE_PSP_ERROR)
                {
                        OS_printf("PSP: taskPrioritySet() - Failed for %s priority from %d to %d\n",
                                  tName, curPrio, newPrio);
                        status = CFE_PSP_ERROR;
                }

            }
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
static int32 SetSysTasksPrio(void)
{
    int32 status = CFE_PSP_SUCCESS;
    int32 index = 0;

    int32 numberOfTask = sizeof(VxWorksTaskList)/sizeof(CFE_PSP_OS_Task_and_priority_t);

    OS_printf("\nSetting system tasks' priorities for %d tasks.\n", numberOfTask);

    for (index = 0; index < numberOfTask; index++)
    {
        if (SetTaskPrio(VxWorksTaskList[index].VxWorksTaskName, VxWorksTaskList[index].VxWorksTaskPriority) != OS_SUCCESS)
        {
            status = CFE_PSP_ERROR;
        }
    }

    return status;
}


/**
 ** \func Provides stub function for FPU exception handler, vxFpscrGet()
 **
 ** \par Description:
 **    Added this function here so that the code can compile & run without error.
 **
 **    If there's code that calls these functions, we will get a message like so,
 **       > ld < cfe-core.o
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrGet.
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrSet.
 **
 **    These do not seem to be included in 85xx build, but are defined as
 **       "defined(_PPC_) && CPU != PPC440" in vxWorks osapi.c, line 2707, v4.2.1a
 **
 **    If this function is not used, stub it out like below. Otherwise, define it.
 **
 ** \par Assumptions, External Events, and Notes:
 ** If still relevant, have OSAL add conditional compile when SPE preset instead of FPU
 ** Once that has occurred we can remove vxFpscrGet and vxFpscrSet
 **
 ** \param None
 **
 ** \return 0 - Integer Zero
 */
unsigned int vxFpscrGet(void)
{
    OS_printf("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__);

    return (0);
}

/**
 ** \func Provides stub function for FPU exception handler, vxFpscrSet()
 **
 ** \par Description:
 **    Added this function here so that the code can compile & run without error.
 **
 **    If there's code that calls these functions, we will get a message like so,
 **       > ld < cfe-core.o
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrGet.
 **       Warning: module 0x461d010 holds reference to undefined symbol vxFpscrSet.
 **
 **    These do not seem to be included in 85xx build, but are defined as
 **       "defined(_PPC_) && CPU != PPC440" in vxWorks osapi.c, line 2707, v4.2.1a
 **
 **    If this function is not used, stub it out like below. Otherwise, define it.
 **
 ** \par Assumptions, External Events, and Notes:
 ** If still relevant, have OSAL add conditional compile when SPE preset instead of FPU
 ** Once that has occurred we can remove vxFpscrGet and vxFpscrSet
 **
 ** \param x - Unused
 **
 ** \return None
 */
void vxFpscrSet(unsigned int x)
{
    OS_printf("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__);
}

