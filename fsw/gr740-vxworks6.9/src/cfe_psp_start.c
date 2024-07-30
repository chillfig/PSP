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
*   \file cfe_psp_start.c
*
*   \brief cFE PSP main entry point
*
*   \brief PSP Startup API for GR740. Functions implemented in this file are used to configure the GR740 
*   target, the VxWorks OS, and gather information on how the system is setup.
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
#include <taskLib.h>
#include <vxWorks.h>

/*
 ** cFE includes 
 */
#include "common_types.h"
/*
 * The preferred way to obtain the CFE tunable values at runtime is via
 * the dynamically generated configuration object.  This allows a single build
 * of the PSP to be completely CFE-independent.
 */
#include "target_config.h"
#include "osapi.h"

#include "cfe.h"
#include "cfe_es.h"
#include "cfe_time.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_start.h"
#include "cfe_psp_memory.h"
#include "cfe_psp_memscrub.h"
#include "cfe_psp_module.h"
#include "cfe_psp_timer.h"
#include "cfe_psp_gr740info.h"

/* To find number of CPU available */
#include <vxCpuLib.h>

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

/** \brief OSAL OS_BSPMain Entry Point */
extern int OS_BSPMain(void);
/** \brief Console Shell Task ID */
TASK_ID g_uiShellTaskID = (TASK_ID)0;

extern void CFE_PSP_InitLocalTime(void); // from cfe_psp_timer.c

/** \brief List Virtual FS Mappings
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 */
static CFE_PSP_SymbolicLinks_t g_SymbolicFolderLinks[] =
{
    CFE_PSP_OSAL_FS_SYMBOLIC_LINKS_MAPPING
};

/** \brief The list of VxWorks task to change the task priority to before finishing initialization.
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 ** The priority reassignment will be moved to kernel in a future release.
*/
static CFE_PSP_OS_Task_and_priority_t g_VxWorksTaskList[] =
{
    CFE_PSP_VXWORKS_TASK_PRIORITIES
};

/** \brief Total number of available processors
 **
 */
uint32 g_uiNumberOfProcessors = 0;

/** \brief The list of CFS tasks with associated processor number
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 */
static CFE_PSP_TaskAffinity_t g_CFSTaskAffinityList[] =
{
    CFE_PSP_CFS_TASK_AFFINITY
};

void CFE_PSP_Main(void)
{
    int32   iStatus = 0;
    TASK_ID iTaskID = 0;
    /* Add method to check if another CFS instance is already running then exit */
    iTaskID = taskNameToId((char *)"CFE_ES");
    if (iTaskID != TASK_ID_ERROR)
    {
        // UndCC_NextLine(SSET134)
        printf("Another instance of CFS is already running [%d]\n",iTaskID);
    }
    else
    {
        /* Start CFS */
        iStatus = OS_BSPMain();

        /* If OS_BSPMain returns an error, then print a note */
        if (iStatus != 0)
        {
            /* At this point, CFS is closing and there is no guarantee that the OS_printf will work. */
            // UndCC_NextLine(SSET134)
            printf("\nPSP: Exiting CFE_PSP_Main() - OS_BSPMain application status [%d]\n", iStatus);
        }
    }
}

/**
 ** \brief Change system task priorities
 **
 ** \par Description:
 ** This function changes the system task priorities so that they are 
 ** lower than CFS system task priorities.
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
int32 CFE_PSP_SetSysTasksPrio(void)
{
    int32 iStatus = CFE_PSP_SUCCESS;
    uint32 uiIndex = 0;
    int32 iRet_code = OS_ERROR;

    uint32 uiNumberOfTask = sizeof(g_VxWorksTaskList) / sizeof(CFE_PSP_OS_Task_and_priority_t);

    OS_printf("PSP: Setting system tasks' priorities for %d tasks.\n", uiNumberOfTask);

    for (uiIndex = 0; uiIndex < uiNumberOfTask; uiIndex++)
    {
        iRet_code = CFE_PSP_SetTaskPrio(g_VxWorksTaskList[uiIndex].VxWorksTaskName, 
                                  g_VxWorksTaskList[uiIndex].VxWorksTaskPriority);
        if (iRet_code != OS_SUCCESS)
        {
            iStatus = CFE_PSP_ERROR;
        }
    }

    return iStatus;
}

/**
 ** \brief Find the Task Name and return the related processor number
 **
 ** \par Description:
 ** 
 **
 ** \param pTaskName - Pointer to the name of the task
 **
 ** \return -1 - if Task Name was not found in the list
 ** \return The processor number to assign
 **
 */
int32 CFE_PSP_FindProcessor(char *pTaskName)
{
    int32 iRetProcessorNumber = -1;
    uint32 uiIndex = 0;
    uint32 uiKeywordLength = 0;
    uint32 uiNumberOfTask = sizeof(g_CFSTaskAffinityList) / sizeof(CFE_PSP_TaskAffinity_t);

    for (uiIndex = 0; uiIndex < uiNumberOfTask; uiIndex++)
    {
        uiKeywordLength = strlen(g_CFSTaskAffinityList[uiIndex].pCFSTaskName);
        /* Check if the Task Name partially match the current indexed task name */
        if (strncmp(pTaskName, g_CFSTaskAffinityList[uiIndex].pCFSTaskName, uiKeywordLength) == 0)
        {
            iRetProcessorNumber = g_CFSTaskAffinityList[uiIndex].uiProcessorNumber;
            break;
        }
    }

    return iRetProcessorNumber;
}

/**
 ** \brief Task creation event handler
 **
 ** \par Description:
 ** In single core targets this function does nothing.
 ** In multi core targets this function assign tasks to specific cores.
 **  
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 ** \param event - 
 ** \param object_id - 
 ** \param pData - 
 **
 ** \return #OS_SUCCESS
 ** \return #OS_ERROR
 */
int32 CFE_PSP_OS_EventHandler(OS_Event_t event, osal_id_t object_id, void *pData)
{
    char      cTaskName[OS_MAX_API_NAME] = {0};
    int32     iProcessorNumber = -1;
    cpuset_t  cpuset = 0;

    /* osal_id_t osal_tid; */
    TASK_ID tid = 0;

    /* osal_tid = OS_OBJECT_ID_UNDEFINED; */

    memset(cTaskName, 0, sizeof(cTaskName));

    switch (event)
    {
        case OS_EVENT_RESOURCE_ALLOCATED:
            /* resource/id is newly allocated but not yet created.  Invoked within locked region. */
            break;
        case OS_EVENT_RESOURCE_CREATED:
            /* resource/id has been fully created/finalized.  Invoked outside locked region. */
            break;
        case OS_EVENT_RESOURCE_DELETED:
            /* resource/id has been deleted.  Invoked outside locked region. */
            break;
        case OS_EVENT_TASK_STARTUP:
        {
            /* New task is starting. Invoked from within the task context. */
            /* Get the name from OSAL and propagate to the pthread/system layer */
            if (OS_GetResourceName(object_id, cTaskName, sizeof(cTaskName)) == OS_SUCCESS)
            {
                /* Find if task name is listed in the affinity list */
                iProcessorNumber = CFE_PSP_FindProcessor(cTaskName);
                if ((iProcessorNumber > -1) && (iProcessorNumber < g_uiNumberOfProcessors))
                {
                    tid = taskIdSelf();
                    /* Assign the processor number identified in the affinity list */
                    CPUSET_ZERO(cpuset);
                    CPUSET_SET(cpuset, iProcessorNumber);

                    if (taskCpuAffinitySet(tid, cpuset) != OK)
                    {
                        OS_printf("Could not set task affinity\n");
                        taskCpuAffinityGet(tid, &cpuset);
                        OS_printf("- Current task affinity: [\"%s\" - TID: %p -> %04X]\n", cTaskName, (void*)tid, (unsigned int)cpuset);
                    }
                }
            }
            break;
        }

        default:
            break;
    }

    return OS_SUCCESS;
}

/**********************************************************
 * 
 * Function: OS_Application_Startup
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void OS_Application_Startup(void) //UndCC_Line(SSET106) Func. name part of PSP API, cannot change
{
    int32 iStatus = OS_SUCCESS;
    int32 iTaskSetStatus = OS_SUCCESS;
    osal_id_t fs_id = OS_OBJECT_ID_UNDEFINED;
    
    uint32 reset_type;
    uint32 reset_subtype;

    /*
     ** Initialize the hardware timer for the local time source
     ** On VxWorks, the default config sets the sysClk to 200 Hz (default 60Hz)
     ** OS_API_Init() calls OS_TimerAPIInit() which gets the
     ** clock resolution of the realtime clock, which is based on the sysClk
     ** and determines the clock accuracy which is used by the scheduler
     ** timers later.  sysClk needs to be at least 200Hz for a 100Hz minor
     ** frame rate.
     */
    CFE_PSP_InitLocalTime();

    /* Gets the number of available processors and sets the global variable */
    g_uiNumberOfProcessors = vxCpuConfiguredGet();

    OS_printf_enable();

    /* Initialize the OS API data structures */
    iStatus = OS_API_Init();
    if (iStatus != OS_SUCCESS)
    {
        OS_printf("PSP: OS_Application_Startup() - OS_API_Init() failed %d\n", iStatus);

        /* Is OSAL fails, log the startup failure and restart target */
        //CFE_PSP_StartupFailedRestartSP0_hook(g_StartupInfo.timerID);
        /* This call should never happen, but it aids the unit test */
        goto OS_Application_Startup_Exit_Tag;
    }

    /* Register event handler to assign tasks to processor cores */
    OS_RegisterEventHandler(CFE_PSP_OS_EventHandler);

    /* Set up all the virtual FS mappings */
    iStatus = CFE_PSP_SetFileSysAddFixedMap(&fs_id);
    if (iStatus != CFE_PSP_SUCCESS)
    {
        /* Startup can continue, but it may fail later, depending on config */
        OS_printf("PSP: Some or all Virtual FS Mapping has failed\n");
    }

    /* Initialize GR740 Temperature Sensor */
    CFE_PSP_TempSensorInit();
    /* 
    ** Collect GR740 information for Telemetry
    ** This functions should run as early as possible to collect information from
    ** hardware and POST, and setup the task to dump the collected information 
    ** when abort is called.
    */
    if (CFE_PSP_GR740CollectDynamicInfo() != CFE_PSP_SUCCESS)
    {
        OS_printf("PSP: Error while collecting dynamic GR740 information\n");
    }

    if (CFE_PSP_GR740CollectStaticInfo() != CFE_PSP_SUCCESS)
    {
        OS_printf("PSP: Error while collecting static GR740 information\n");
    }

    /*
     ** Setup the pointer to the reserved area in vxWorks.
     ** This must be done before any of the reset variables are used.
     */
    CFE_PSP_SetupReservedMemoryMap();

    /*
     ** Determine Reset type by reading the hardware reset register.
     */

    // For now, always assume POR
    reset_type = CFE_PSP_RST_TYPE_POWERON;
    reset_subtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;

    /*
    ** Initialize the statically linked modules (if any)
    */
    CFE_PSP_ModuleInit();

    /*
    ** Initialize the watchdog, it's left disabled
    */
    CFE_PSP_WatchdogInit();
    /*
    ** Initialize the watchdog, it's left disabled
    */
    CFE_PSP_MemScrubInit();

    /*
     ** Initialize the reserved memory 
     */
    if (CFE_PSP_InitProcessorReservedMemory(reset_type) != OS_SUCCESS)
    {
        OS_printf("PSP: CFS is rebooting due to not having enough User Reserved Memory\n");
        /* Is OSAL fails, log the startup failure and restart target */
        /* CFE_PSP_StartupFailedRestartSP0_hook(g_StartupInfo.timerID); */
        /* This call should never happen, but it aids the unit test */
        goto OS_Application_Startup_Exit_Tag;
    }

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    iTaskSetStatus = CFE_PSP_SetSysTasksPrio();
    if (iTaskSetStatus != OS_SUCCESS)
    {
        OS_printf("PSP: At least one vxWorks task priority set failed. System may have degraded performance.\n");
    }

    OS_printf("PSP: Application Startup Complete\n");

    /*
     ** Call cFE entry point. This will return when cFE startup
     ** is complete.
     */
    CFE_PSP_MAIN_FUNCTION(reset_type, reset_subtype, 1, CFE_PSP_NONVOL_STARTUP_FILE);

OS_Application_Startup_Exit_Tag:
    return;
}

/**********************************************************
 * 
 * Function: OS_Application_Run
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
#if CFE_PSP_OVERRIDE_OSAL_OS_APPLICATION_RUN
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

/**********************************************************
 * 
 * Function: CFE_PSP_SetTaskPrio
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SetTaskPrio(const char *tName, uint8 tgtPrio)
{
    TASK_ID iTID = TASK_ID_ERROR;
    int32 iCurPriority = 0;
    uint8 ucCurPrio = 0;
    uint8 ucNewPrio = 0;
    int32 iStatus = CFE_PSP_ERROR;

    if ((tName != NULL) && (memchr(tName, (cpuaddr) NULL, CFE_PSP_MAXIMUM_TASK_LENGTH) != NULL))
    {
        ucNewPrio = tgtPrio;

        iTID = taskNameToId((char *)tName);
        if (iTID != TASK_ID_ERROR)
        {
            if (taskPriorityGet(iTID, &iCurPriority) == OK)
            {
                ucCurPrio = (uint8) iCurPriority ;
                if (taskPrioritySet(iTID, ucNewPrio) == OK)
                {
                    OS_printf("PSP: Setting %s priority from %u to %u\n",
                            tName, ucCurPrio, ucNewPrio);
                    iStatus = CFE_PSP_SUCCESS;
                }
                else
                {
                    OS_printf("PSP: taskPrioritySet() - Failed for %s priority from %u to %u\n",
                              tName, ucCurPrio, ucNewPrio);
                }
            }
        }
        else
        {
            OS_printf("PSP: Could not find task %s \n", tName);
        }
    }

    return iStatus;
}


/******************************************************************************
 **  Function:  CFE_PSP_ResetSysTasksPrio()
 **
 **  Copied from UT700 PSP, NOSA licensed
 **
 **  Purpose:
 **    reset changed task priorities back to defaults
 **
 **  Arguments: none
 **
 **  Return: none
 **
 */
void CFE_PSP_ResetSysTasksPrio(void)
{
    OS_printf("\nResetting system tasks' priority to default\n");
    CFE_PSP_SetTaskPrio("tLogTask", 0);
    CFE_PSP_SetTaskPrio("tShell0", 1);
    CFE_PSP_SetTaskPrio("tWdbTask", 3);
    CFE_PSP_SetTaskPrio("tVxdbgTask", 25);
    CFE_PSP_SetTaskPrio("tNet0", 50);
    CFE_PSP_SetTaskPrio("ipftps", 50);
    CFE_PSP_SetTaskPrio("ipcom_syslogd", 50);
    CFE_PSP_SetTaskPrio("ipcom_telnetd", 50);
    CFE_PSP_SetTaskPrio("tWvrBuffMgr", 100);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SuspendConsoleShellTask
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SuspendConsoleShellTask(bool suspend)
{
    int32 iStatus = CFE_PSP_ERROR;

    /* Get the Shell Task ID if we have not done it already */
    if (g_uiShellTaskID == 0)
    {
        g_uiShellTaskID = taskNameToId((char *)"tShell0");
    }

    if (suspend)
    {
        iStatus = taskSuspend(g_uiShellTaskID);
        if (iStatus == OK)
        {
            OS_printf("Shell Task Suspended [%p]\n", (void*)g_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be suspended [%p]\n", (void*)g_uiShellTaskID);
        }
    }
    else
    {
        iStatus = taskResume(g_uiShellTaskID);
        if (iStatus == OK)
        {
            OS_printf("Shell Task Resumed [%p]\n", (void*)g_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be resumed [%p]\n", (void*)g_uiShellTaskID);
        }
    }

    return iStatus;
}

/**
 ** \func Add a list of symbolic link mappings 
 **
 ** \par Description:
 ** This function simply calls the OS_FileSysAddFixedMap multiple times
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] fs_id - Buffer to store the ID of the file system mapping
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetFileSysAddFixedMap(osal_id_t *fs_id)
{
    int32   iRet_code      = CFE_PSP_SUCCESS;
    int32   iStatus        = OS_SUCCESS;
    int32   index          = 0;
    int8    cNumberOfLinks = 0;

    /* Get the number of structures in the symbolic links array */
    cNumberOfLinks = sizeof(g_SymbolicFolderLinks) / sizeof(CFE_PSP_SymbolicLinks_t);

    for (index = 0; index < cNumberOfLinks; index++)
    {
        /* Set up the virtual FS mapping */
        iStatus = OS_FileSysAddFixedMap(fs_id,
                                       g_SymbolicFolderLinks[index].fullpath_real,
                                       g_SymbolicFolderLinks[index].fullpath_map);
        if (iStatus != OS_SUCCESS)
        {
            /*
            Print for informational purposes --
            startup can continue, but something else may fail later, depending on config.
            */
            OS_printf("PSP: OS_FileSysAddFixedMap() failure: %d\n", (int32)iStatus);
            iRet_code = CFE_PSP_ERROR;
        }
    }

    if (iRet_code == CFE_PSP_SUCCESS)
    {
        OS_printf("PSP: Set %d Virtual Path(s)\n", cNumberOfLinks);
    }

    return iRet_code;
}
