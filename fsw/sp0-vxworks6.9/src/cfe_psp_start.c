/******************************************************************************
**
** File:  cfe_psp_start.c
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement.
**
** Purpose:
**   cFE PSP main entry point
**
******************************************************************************/

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "vxWorks.h"
#include "sysLib.h"
#include "taskLib.h"
#include "ramDrv.h"
#include "dosFsLib.h"
#include "xbdBlkDev.h"
#include "errnoLib.h"
#include "usrLib.h"
#include "cacheLib.h"
#include "userReservedMem.h"

#include "common_types.h"
#include "osapi.h"
#include "cfe.h"
#include "cfe_es.h"            /* For reset types */
#include "cfe_platform_cfg.h"  /* for processor ID */

#include "cfe_psp.h"
#include "cfe_psp_memory.h"


/*
** Macro Definitions
*/
#define CFE_PSP_TASK_PRIORITY    (30)
#define CFE_PSP_TASK_STACK_SIZE  (20 * 1024)


/*
**  External Function Prototypes
*/
extern void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId,
                        const char *StartFilePath);
extern void CFE_PSP_InitLocalTime(void);
extern void CFE_PSP_Init1HzTimer(void);

extern CFE_PSP_ReservedMemory_t* CFE_PSP_ReservedMemoryPtr;

extern int32 CFE_PSP_InitProcessorReservedMemory(uint32 RestartType);
extern void  CFE_PSP_WatchdogInit(void);


/*
**  Local Function Prototypes
*/
static void CFE_PSP_Start(uint32 ModeId, char *StartupFilePath);
static void CFE_PSP_SysInit(uint32* psp_reset_type, uint32* psp_reset_subtype,
                            uint32 last_bsp_reset_type);
static void SetSysTasksPrio(void);
static void ResetSysTasksPrio(void);
static void SetTaskPrio(char* tName, const int32 tgtPrio);

void CFE_PSP_Main(uint32 ModeId, char *StartupFilePath);
uint32 CFE_PSP_GetRestartType(uint32 *restartSubType);


/*
**  Local Global Variables
*/
static uint32 ResetType = 0;
static uint32 ResetSubtype = 0;


/******************************************************************************
**  Function:  CFE_PSP_Main()
**
**  Purpose:
**    vxWorks/PSP Application entry point
**
**  Arguments:
**    Input - modeId - Mode ID (not currently used)
**    Input - startupFilePath - Path to cFE startup file
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_Main(uint32 modeId, char *startupFilePath)
{
    int32 root_task_id = 0;

    if (startupFilePath != NULL)
    {
        /* Need to add VX_FP_TASK (all tasks should just be created with it...
        ** to deal with an "SPE unknown exception" error as soon as this task
        ** would start on the SP0/PPC8548.
        */

        /* Note: When using e500vx_gnu tool chain, including VX_FP_TASK implicitly
        ** includes VX_SPE_TASK, which is needed when starting tasks that might
        ** use floating point on this processor.
        */

        root_task_id = taskSpawn("PSP_START", CFE_PSP_TASK_PRIORITY,
                                 VX_FP_TASK, CFE_PSP_TASK_STACK_SIZE,
                                 (FUNCPTR) (void *)CFE_PSP_Start, modeId,
                                 (int)startupFilePath, 0,0,0,0,0,0,0,0);

        if (root_task_id == ERROR)
        {
            logMsg("CFE_PSP_Main() - ERROR - Unable to spawn PSP_START task!",
                   0,0,0,0,0,0);
        }
    }
}


/******************************************************************************
**  Function:  CFE_PSP_SysInit()
**
**  Purpose:
**    Initializes core PSP functionality.  This is called by both the primary
**    CFE_PSP_Main() startup, as well as the alternative initialization in the
**    Startup Manager (SM) component.  The SM uses the PSP and OSAL but does
**    not start cFE or initialize all components of the PSP.
**
**  Arguments:
**    Output - psp_reset_type - Reset type
**    Output - psp_reset_subtype - Reset sub-type
**    Input  - last_bsp_reset_type - Last reset type  //TODO: Do we need to keep this?
**
**  Return:
**    None
******************************************************************************/
static void CFE_PSP_SysInit(uint32* psp_reset_type, uint32* psp_reset_subtype,
                            uint32 last_bsp_reset_type)
{
    if ((psp_reset_type != NULL) && (psp_reset_subtype != NULL))
    {
        /* Delay for one second */
        taskDelay((int32)sysClkRateGet());

        /* Assign the reset cause - for now always start up in a power on reset */
        *psp_reset_type = CFE_PSP_RST_TYPE_POWERON;
        *psp_reset_subtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;

        /* Update the reset types */
        ResetType = *psp_reset_type;
        ResetSubtype = *psp_reset_subtype;
    }
}


/******************************************************************************
**  Function:  CFE_PSP_Start()
**
**  Purpose:
**    Initialize the PSP and start cFE
**
**  Arguments:
**    Input - modeId - Used to indicate which bank of non-volatile memory
**                     was used to boot.  If the bank used to boot this time
**                     is different from the previous boot, then we re-initialize
**                     reserved memory.
**    Input - startupFilePath - path to cFE startup file to use
**
**  Return:
**    None
******************************************************************************/
static void CFE_PSP_Start(uint32 modeId, char *startupFilePath)
{
    int32 status = 0;
    uint32 reset_type = 0;
    uint32 reset_subtype = 0;
    uint32 reserve_memory_size = 0;

    if (startupFilePath == NULL)
    {
        goto CFE_PSP_Start_Exit_Tag;
    }

    /* Initialize the hardware timer for the local time source */
    
    /* 
    ** On VxWorks, the default config sets the SysClk to 200Hz (default 60Hz).
    **
    ** OS_API_Init() calls OS_TimerAPIInit(), which gets the clock resolution
    ** of the realtime clock, which is based on the SysClk, and determines
    ** the clock accuracy, which is used by the scheduler timer later.
    **
    ** SysClk needs to be at least 200Hz for a 100Hz minor frame rate.
    */
    CFE_PSP_InitLocalTime();

    /* Initialize the OS API data structures */
    status = OS_API_Init();
    if (status != OS_SUCCESS)
    {
        logMsg("CFE_PSP_Start() - OS_API_Init() failed (0x%X)\n",
               (unsigned int)status, 0,0,0,0,0);
    }

    /*
    ** Setup the pointer to the reserved area in vxWorks.
    ** This must be done before any of the reset variables are used.
    */
    userReservedGet((char**)&CFE_PSP_ReservedMemoryPtr, &reserve_memory_size);
  

    logMsg("CFE_PSP_Main() - Reserved Memory address=0x%08X, CFE_PSP_ReserveMemory size=%d vxWoks ReservedMemory Size %d\n",
           (uint32)CFE_PSP_ReservedMemoryPtr, sizeof(CFE_PSP_ReservedMemory_t), reserve_memory_size,
            0,0,0);
    if (reserve_memory_size < sizeof(CFE_PSP_ReservedMemory_t))
    {
            logMsg("CFE_PSP_Main() - VxWorks Reserved Memory size smaller than CFE_PSP_ReserveMemory\n",
                   0,0,0,0,0,0); 
            goto CFE_PSP_Start_Exit_Tag;
    }
    /* PSP System Initialization */
    CFE_PSP_SysInit(&reset_type, &reset_subtype,
                    CFE_PSP_ReservedMemoryPtr->bsp_reset_type);

    /* Initialize the watchdog, it's left disabled */
    CFE_PSP_WatchdogInit();

    /* Initialize the reserved memory */
    CFE_PSP_InitProcessorReservedMemory(reset_type);

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    SetSysTasksPrio();

    /* Call cFE entry point. This will return when cFE startup is complete. */
    CFE_ES_Main(reset_type, reset_subtype, modeId, (char *)startupFilePath);

    /*
    ** Initializing the 1Hz timer connects the cFE 1Hz ISR for providing the
    ** CFS 1Hz time sync, sync the scheduler's 1Hz major frame start to the
    ** 1Hz timer. 
    */

    /* 
    ** This call can only occur after CFE_ES_Main() because the 1Hz ISR uses
    ** a semaphore that is created when timer services are initialized.
    */
    CFE_PSP_Init1HzTimer();

    logMsg("CFE_PSP_Start done, exiting.\n", 0,0,0,0,0,0);

CFE_PSP_Start_Exit_Tag:
    return;
}


/******************************************************************************
**  Function:  CFE_PSP_GetRestartType()
**
**  Purpose:
**    Provides the CFE PSP reset type and subtype
**
**  Arguments:
**    Output - resetSubType - Reset subtype
**
**  Return:
**    Reset type
******************************************************************************/
uint32 CFE_PSP_GetRestartType(uint32 *resetSubType)
{
    if (resetSubType != NULL)
    {
        *resetSubType = ResetSubtype;
    }

    return (ResetType);
}


/******************************************************************************
**  Function:  SetTaskPrio()
**
**  Purpose:
**    Changes default task priority to a given priority
**
**  Arguments:
**    Input - tName - Task name
**    Input - tgtPrio - New task priority
**
**  Return:
**     None
******************************************************************************/
static void SetTaskPrio(char* tName, const int32 tgtPrio)
{
    int32 tid = 0;
    int32 curPrio = 0;
    int32 newPrio = 0;

    if ((tName != NULL) && (strlen((const char*) tName) > 0))
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

        tid = taskNameToId(tName);
        if (tid != ERROR)
        {
            if (taskPriorityGet(tid, (int *)&curPrio) != ERROR)
            {
                logMsg("SetTaskPrio() - Setting %s priority from %d to %d\n",
                       (uint32)tName, curPrio, newPrio, 0,0,0);

                taskPrioritySet(tid, newPrio);
            }
        }
    }
}


/******************************************************************************
**  Function:  ResetSysTasksPrio()
**
**  Purpose:
**    Resets changed task priorities back to defaults
**
**  Arguments:
**    None
**
**  Return:
**    None
******************************************************************************/
static void ResetSysTasksPrio(void)
{
    logMsg("\nResetting system tasks' priority to default\n", 0,0,0,0,0,0);

    SetTaskPrio("tLogTask", 0);
    SetTaskPrio("tShell0", 1);
    SetTaskPrio("tWdbTask", 3);
    SetTaskPrio("tVxdbgTask", 25);
    SetTaskPrio("tNet0", 50);
    SetTaskPrio("ipftps", 50);
    SetTaskPrio("ipcom_syslogd", 50);
    SetTaskPrio("ipcom_telnetd", 50);
}


/******************************************************************************
**  Function:  SetSysTasksPrio()
**
**  Purpose:
**    Changes system task priorities so that they are lower than CFS system
**    task priorities
**
**    Note: tNet0 priority should be adjusted to be right below what ever
**    gets defined for CI/TO apps in your system if using the network
**    interface CCSDS/UDP for CI/TO apps.
**
**  Arguments:
**    None
**
**  Return:
**    None
******************************************************************************/
static void SetSysTasksPrio(void)
{
    logMsg("\nSetting system tasks' priorities\n", 0,0,0,0,0,0);

    SetTaskPrio("tLogTask", 0);
    SetTaskPrio("tShell0", 201);
    SetTaskPrio("tWdbTask", 203);
    SetTaskPrio("tVxdbgTask", 200);
    SetTaskPrio("tNet0", 25);           /* Should be set right below CI/TO */
    SetTaskPrio("ipftps", 202);
    SetTaskPrio("ipcom_syslogd", 205);
    SetTaskPrio("ipcom_telnetd", 204);
}


/******************************************************************************
**  Function:  vxFpscrGet()
**
**  Purpose:
**    Provides stub function for FPU exception handler, vxFpscrGet()
**
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
**  Arguments:
**    None
**
**  Return:
**    0
*******************************************************************************/
unsigned int vxFpscrGet(void)
{
    logMsg("%s->%s<stub>:%d:\n", (uint32)__FILE__, (uint32)__func__, __LINE__, 0,0,0);

    return (0);
}


/******************************************************************************
**  Function:  vxFpscrSet()
**
**  Purpose:
**    Provides stub function for FPU exception handler, vxFpscrSet()
**
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
**  Arguments:
**    Input - x - TODO: Describe x
**
**  Return:
**    None
*******************************************************************************/
void vxFpscrSet(unsigned int x)
{
    logMsg("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__, 0,0,0);
}


