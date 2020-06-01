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
#include <string.h>
#include <vxWorks.h>
#include <taskLib.h>
#include "osapi.h"
#include "cfe.h"
#include "cfe_psp.h"
#include "cfe_psp_memory.h"
#include "target_config.h"
#include "scratchRegMap.h"
#include <target_config.h>
#include "aimonUtil.h"



/*
** Macro Definitions
*/
#define CFE_PSP_TASK_PRIORITY    (30)
#define CFE_PSP_TASK_STACK_SIZE  (20 * 1024)

/* CFE_PSP_TIMER_PRINT_DBG
**
**   Debug flag
*/
#define CFE_PSP_TIMER_PRINT_DBG  FALSE

#define PSP_1HZ_INTERVAL 1000000

/*
**  External Function Prototypes
*/

/*
 * The preferred way to obtain the CFE tunable values at runtime is via
 * the dynamically generated configuration object.  This allows a single build
 * of the PSP to be completely CFE-independent.
 */

#define CFE_PSP_MAIN_FUNCTION        (*GLOBAL_CONFIGDATA.CfeConfig->SystemMain)
#define CFE_PSP_NONVOL_STARTUP_FILE  (GLOBAL_CONFIGDATA.CfeConfig->NonvolStartupFile)
#define CFE_PSP_1HZ_FUNCTION         (*GLOBAL_CONFIGDATA.CfeConfig->System1HzISR)

/*
**  Local Function Prototypes
*/
static int32 SetSysTasksPrio(void);
static int32 SetTaskPrio(const char* tName, int32 tgtPrio);
void CFE_PSP_Start(void);

/*
**  Local Global Variables
*/
static uint32 ResetType = 0;
static uint32 ResetSubtype = 0;
static USER_SAFE_MODE_DATA_STRUCT safeModeUserData;
static uint32 PSP_1Hz_TimerId = 0;
static uint32 PSP_1Hz_ClockAccuracy = 0;

/*
 * The list of VxWorks task to change the task priority
 * to before finishing initialization.
 */

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

/******************************************************************************
**  Function:  CFE_PSP_Main()
**
**  Purpose:
**    vxWorks/PSP Application entry point
**
**  Arguments:
**    None
**  Return:
**    None
******************************************************************************/
void CFE_PSP_Main(void)
{
    int32 root_task_id = 0;

    /* Need to add VX_FP_TASK (all tasks should just be created with it...
    ** to deal with an "SPE unknown exception" error as soon as this task
    ** would start on the SP0/PPC8548.
    */

    /* Note: When using e500vx_gnu tool chain, including VX_FP_TASK implicitly
    ** includes VX_SPE_TASK, which is needed when starting tasks that might
    ** use floating point on this processor.
    */
    OS_printf_enable();
    root_task_id = taskSpawn("PSP_START", CFE_PSP_TASK_PRIORITY,
                             VX_FP_TASK, CFE_PSP_TASK_STACK_SIZE,
                             (FUNCPTR) (void *)CFE_PSP_Start, 0,
                             0,0,0,0,0,0,0,0,0);

    if (root_task_id == ERROR)
    {
        OS_printf("CFE_PSP_Main() - ERROR - Unable to spawn PSP_START task!");
    }
}

/******************************************************************************
**  Function:  CFE_PSP_ProcessPOSTResults()
**
**  Purpose:
**    The Power on Self Test results are logged to the system log.
**
**  Arguments:
**    None
**
**  Return:
**    None
******************************************************************************/
void CFE_PSP_ProcessPOSTResults(void)
{
    uint64 bitExecuted = 0ULL;
    uint64 bitResult   = 0ULL;
    uint32 i = 0;

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
                    CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_ProcessPOSTResults: Test - FAILED - %s.\n",
                            AimonCompletionBlockTestIDStrings[i]);
                }
                else
                {
                    CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_ProcessPOSTResults: Test - PASSED - %s .\n",
                            AimonCompletionBlockTestIDStrings[i]);
                }
            }
            else
            {

                CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_ProcessPOSTResults: Test - Not Run - %s.\n",
                        AimonCompletionBlockTestIDStrings[i]);
            }
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_ProcessPOSTResults: aimonGetBITExecuted() or aimonGetBITResults() failed.");
    }
}
/******************************************************************************
**  Function:  CFE_PSP_ProcessResetType()
**
**  Purpose:
**     Determines the reset type and logs off nominal resets.
**
**  Arguments:
**    None
**
**  Return:
**    rest_type - Reset type
******************************************************************************/
static RESET_SRC_REG_ENUM CFE_PSP_ProcessResetType(void)
{
    int32 status = 0;
    RESET_SRC_REG_ENUM resetSrc = 0;
    uint32 talkative = 1;

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

/******************************************************************************
**  Function:  CFE_PSP_LogSoftwareResetType()
**
**  Purpose:
**     Determines if started in safe mode and logs off nominal resets.
**
**  Arguments:
**    None
**
**  Return:
**    rest_type - Reset type
******************************************************************************/

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
    CFE_ES_WriteToSysLog("CFE_PSP: PROCESSOR Reset Source = 0x%x = (%s) Safe mode = %d, sbc = %s, reset reason = %d, MCHK cause = 0x%08x\n",
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
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L1_DCHERR  =      (0x02) L1 data cache error error: reset\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L1_DCHPERR =      (0x04) L1 data cache push error error: reset\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_MULTERR =      (0x08) L2 multiple errors\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_TPARERR =      (0x10) L2 tag parity error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_MBEERR  =      (0x20) L2 multi-bit error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_SBEERR  =      (0x40) L2 single bit error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_CFGERR  =      (0x80) L2 configuration error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_SDRAM_MBECC_ERR = (0x100) DDR multi-bit error: reset\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_OTHER_MCHK_ERR =  (0x200) Other machine check error\n");
        }
    }
    CFE_PSP_ProcessPOSTResults();
}
/******************************************************************************
**  Function:  CFE_PSP_Start()
**
**  Purpose:
**    Application startup entry point from OSAL BSP.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
void CFE_PSP_Start(void)
{
    int32 status = OS_SUCCESS;
    int32 taskSetStatus = OS_SUCCESS;
    RESET_SRC_REG_ENUM resetSrc = 0;

    /* Initialize the OS API data structures */
    status = OS_API_Init();
    if (status != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: CFE_PSP_Start() - OS_API_Init() failed (0x%X)\n",
               status);
        goto CFE_PSP_Start_Exit_Tag;
    }

    CFE_PSP_SetupReservedMemoryMap();

    /* Initialize the watchdog, it's left disabled */
    CFE_PSP_WatchdogInit();

    /* PSP Determine the reset type */
    resetSrc = CFE_PSP_ProcessResetType();

    /* Initialize the reserved memory */
    if (CFE_PSP_InitProcessorReservedMemory(ResetType) != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: CFE_PSP_Start() - CFE_PSP_InitProcessorReservedMemory() failed (0x%x)\n",
                  status);
        goto CFE_PSP_Start_Exit_Tag;
    }

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    taskSetStatus = SetSysTasksPrio();

    /* Call cFE entry point. This will return when cFE startup is complete. */
    CFE_PSP_MAIN_FUNCTION(ResetType, ResetSubtype, 1, CFE_PSP_NONVOL_STARTUP_FILE);


    /*Now that the system is initialized log software reset type to syslog*/
    CFE_PSP_LogSoftwareResetType(resetSrc);

    OS_Application_Run();

    if (taskSetStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_Start() At least one vxWorks task priority set failed. System may have degraded performance.\n");
    }

    CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_Start() done.\n");

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
static int32 SetTaskPrio(const char* tName, int32 tgtPrio)
{
    int32 tid = 0;
    int32 curPrio = 0;
    int32 newPrio = 0;
    int32 status = OS_SUCCESS;

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
        if (tid != ERROR)
        {
            if (taskPriorityGet(tid, (int *)&curPrio) != ERROR)
            {
                OS_printf("PSP: SetTaskPrio() - Setting %s priority from %d to %d\n",
                       tName, curPrio, newPrio);

                if (taskPrioritySet(tid, newPrio) == ERROR)
                {
                        OS_printf("PSP: taskPrioritySet() - Failed for %s priority from %d to %d\n",
                                  tName, curPrio, newPrio);
                        status = OS_ERROR;
                }

            }
        }
    }

    return status;
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
static int32 SetSysTasksPrio(void)
{
    int32 status = OS_SUCCESS;
    int32 index = 0;

    int32 numberOfTask = sizeof(VxWorksTaskList)/sizeof(CFE_PSP_OS_Task_and_priority_t);

    OS_printf("\nSetting system tasks' priorities for %d tasks.\n", numberOfTask);

    for (index = 0; index < numberOfTask; index++)
    {
        if (SetTaskPrio(VxWorksTaskList[index].VxWorksTaskName, VxWorksTaskList[index].VxWorksTaskPriority) != OS_SUCCESS)
        {
            status = OS_ERROR;
        }
    }

    return status;
}


/******************************************************************************
**  Function:  PSP_1HzLocalCallback()
**
**  Purpose:
**    The 1Hz call back handler calls the cfe 1Hz routine
**
**  Arguments:
**    Input TimerId - Id for 1Hz timer
**
**  Return:
**    (none)
*/
void PSP_1HzLocalCallback(uint32 TimerId)
{
#if CFE_PSP_TIMER_PRINT_DBG == TRUE
      OS_time_t LocalTime;
      CFE_PSP_GetTime(&LocalTime);
#endif
      CFE_PSP_1HZ_FUNCTION();
#if CFE_PSP_TIMER_PRINT_DBG == TRUE

      OS_printf("PSP_1HzLocalCallback: PSP Local Time: %d.%d\n", LocalTime.seconds,LocalTime.microsecs);
#endif
}
/******************************************************************************
**  Function:  OS_Application_Run()
**
**  Purpose:
**    Initializes the 1Hz timer connects the cFE 1Hz ISR for providing the
**    CFS 1Hz time sync, sync the scheduler's 1Hz major frame start to the
**    1Hz timer.
**
**  Arguments:
**    (none)
**
**  Return:
**    (none)
*/
void OS_Application_Run(void)
{
   int32  Status    = CFE_SUCCESS;

   /*Create the 1Hz timer for synchronizing the major frame*/
   Status = OS_TimerCreate(&PSP_1Hz_TimerId,
                            "PSP_1HZ_TIMER",
                            &PSP_1Hz_ClockAccuracy,
                            PSP_1HzLocalCallback);
   if (Status != CFE_SUCCESS)
   {
       CFE_ES_WriteToSysLog("Failed to create OS_Timer for 1Hz local time.\n");
   }
   else
   {
       /*Set the interval to one second in microseconds.*/
       Status = OS_TimerSet(PSP_1Hz_TimerId, PSP_1HZ_INTERVAL, PSP_1HZ_INTERVAL);
       if (Status != CFE_SUCCESS)
       {
           CFE_ES_WriteToSysLog("Failed to set OS_Timer for 1Hz local time.\n");
       }
   }

}

/*TODO have osal add conditional compile when SPE preset instead of FPU
 * Once that has occurred we can remove vxFpscrGet and vxFpscrSet
 * */
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
    CFE_ES_WriteToSysLog("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__);

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
    CFE_ES_WriteToSysLog("%s->%s<stub>:%d:\n", __FILE__, __func__, __LINE__);
}

