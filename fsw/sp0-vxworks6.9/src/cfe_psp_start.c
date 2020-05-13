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
#include <vxWorks.h>
#include <taskLib.h>
#include "userReservedMem.h"
#include "osapi.h"
#include "cfe.h"
#include "cfe_platform_cfg.h"  /* for processor ID */
#include "cfe_psp.h"
#include "cfe_psp_memory.h"
#include "target_config.h"
#include "scratchRegMap.h"



/*
** Macro Definitions
*/
#define CFE_PSP_TASK_PRIORITY    (30)
#define CFE_PSP_TASK_STACK_SIZE  (20 * 1024)

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

/*
**  Local Function Prototypes
*/
static void SetSysTasksPrio(void);
static void ResetSysTasksPrio(void);
static void SetTaskPrio(char* tName, const int32 tgtPrio);
void CFE_PSP_Start(void);

/*
**  Local Global Variables
*/
static uint32 ResetType = 0;
static uint32 ResetSubtype = 0;
static USER_SAFE_MODE_DATA_STRUCT safeModeUserData;


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
                ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
            }
            break;
            case RESET_SRC_FWDT:
            {
                OS_printf("CFE_PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
                ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
            }
            break;
            case RESET_SRC_CPCI:
            {
                OS_printf("CFE_PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
                ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
                ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;
            }
            break;
            case RESET_SRC_SWR:
            {

                /*
                ** For a Software hard reset, we want to look at the special
                ** BSP reset variable to determine if we wanted a
                ** Power ON or a Processor reset. Because the vxWorks sysToMonitor and
                ** reboot functions use this reset type, we want to use this for a software
                ** commanded processor or Power on reset.
                */
                if ( CFE_PSP_ReservedMemoryPtr->bsp_reset_type == CFE_PSP_RST_TYPE_POWERON)
                {
                    OS_printf("CFE_PSP: POWERON Reset: Software Hard Reset.\n");
                    ResetType = CFE_PSP_RST_TYPE_POWERON;
                    ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;
                }
                else
                {
                    OS_printf("CFE_PSP: PROCESSOR Reset: Software Hard Reset.\n");
                    ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
                    ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;
                }
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
        ResetType = CFE_PSP_ReservedMemoryPtr->bsp_reset_type;
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

    if (resetSrc == RESET_SRC_SWR)
    {

        CFE_ES_WriteToSysLog("CFE_PSP: PROCESSOR Reset: Safe mode = %d, sbc = %s, reset reason = %d, MCHK cause = 0x%08x\n",
                  (unsigned int)safeModeUserData.safeMode,
                  (safeModeUserData.sbc == SM_LOCAL_SBC)? (unsigned int)"LOCAL":(unsigned int)"REMOTE",
                  (unsigned int)safeModeUserData.reason,
                  (unsigned int)safeModeUserData.mckCause);
        if (safeModeUserData.mckCause != 0)
        {
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L1_ICHERR  =      (0x01) L1 instruction cache error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L1_DCHERR  =      (0x02) L1 data cache error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L1_DCHPERR =      (0x04) L1 data cache push error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_MULTERR =      (0x08) L2 multiple errors\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_TPARERR =      (0x10) L2 tag parity error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_MBEERR  =      (0x20) L2 multi-bit error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_SBEERR  =      (0x40) L2 single bit error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_L2_CFGERR  =      (0x80) L2 configuration error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_SDRAM_MBECC_ERR = (0x100) DDR multi-bit error\n");
            CFE_ES_WriteToSysLog("CFE_PSP: MCHK_OTHER_MCHK_ERR =  (0x200) Other machine check error\n");
        }
    }
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
    int32 status = 0;
    uint32 reserve_memory_size = 0;
    RESET_SRC_REG_ENUM resetSrc = 0;

    /* Initialize the hardware timer for the local time source */
    CFE_PSP_InitLocalTime();

    /* Initialize the OS API data structures */
    status = OS_API_Init();
    if (status != OS_SUCCESS)
    {
        OS_printf("CFE_PSP: CFE_PSP_Start() - OS_API_Init() failed (0x%X)\n",
               (unsigned int)status);
        goto CFE_PSP_Start_Exit_Tag;
    }

    /*
    ** Setup the pointer to the reserved area in vxWorks.
    ** This must be done before any of the reset variables are used.
    */
    userReservedGet((char**)&CFE_PSP_ReservedMemoryPtr, &reserve_memory_size);


    OS_printf("CFE_PSP: - Reserved Mem addr=0x%08X, CFE_PSP_ReserveMem size=0x%x vxWoks ReservedMem Size=0x%x\n",
           (uint32)CFE_PSP_ReservedMemoryPtr, sizeof(CFE_PSP_ReservedMemory_t), reserve_memory_size);
    if (reserve_memory_size < sizeof(CFE_PSP_ReservedMemory_t))
    {
            OS_printf("CFE_PSP: CFE_PSP_Start() - VxWorks Reserved Memory size smaller than CFE_PSP_ReserveMemory\n");
            goto CFE_PSP_Start_Exit_Tag;
    }

    /* Initialize the watchdog, it's left disabled */
    CFE_PSP_WatchdogInit();

    /* PSP Determine the reset type */
    resetSrc = CFE_PSP_ProcessResetType();
    /*TODO remove when new CFE exception code is picked up*/
    ResetSubtype = 0;
    ResetType = 0;
    /* Initialize the reserved memory */
    CFE_PSP_InitProcessorReservedMemory(ResetType);

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    SetSysTasksPrio();

    /* Call cFE entry point. This will return when cFE startup is complete. */
    CFE_PSP_MAIN_FUNCTION(ResetType, ResetSubtype, 1, CFE_PSP_NONVOL_STARTUP_FILE);


    /*Now that the system is initialized log software reset type to syslog*/
    CFE_PSP_LogSoftwareResetType(resetSrc);

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

    CFE_ES_WriteToSysLog("CFE_PSP: CFE_PSP_Start() done, exiting.\n");

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
                OS_printf("SetTaskPrio() - Setting %s priority from %d to %d\n",
                       (uint32)tName, curPrio, newPrio);

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
    CFE_ES_WriteToSysLog("\nResetting system tasks' priority to default\n");

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
    OS_printf("\nSetting system tasks' priorities\n");

    SetTaskPrio("tLogTask", 0);
    SetTaskPrio("tShell0", 201);
    SetTaskPrio("tWdbTask", 203);
    SetTaskPrio("tVxdbgTask", 200);
    SetTaskPrio("tNet0", 25);           /* Should be set right below CI/TO */
    SetTaskPrio("ipftps", 202);
    SetTaskPrio("ipcom_syslogd", 205);
    SetTaskPrio("ipcom_telnetd", 204);
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
    CFE_ES_WriteToSysLog("%s->%s<stub>:%d:\n", (uint32)__FILE__, (uint32)__func__, __LINE__);

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

