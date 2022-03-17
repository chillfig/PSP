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
#include <string.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <errno.h>
#include <ioLib.h>

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
#include "psp_mem_sync.h"
#include "psp_support.h"
#include "psp_mem_sync.h"

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

/** \brief Console Shell Task ID */
static TASK_ID g_uiShellTaskID = 0;

/** \brief Startup Info Structure */
CFE_PSP_Startup_structure_t g_StartupInfo;

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

/** \brief List Virtual FS Mappings
 ** \par Note:
 ** Values are defined in cfe_psp_config.h header.\n
 */
static CFE_PSP_SymbolicLinks_t g_SymbolicFolderLinks[] =
{
    OSAL_FS_SYMBOLIC_LINKS_MAPPING
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

/**********************************************************
 * 
 * Function: CFE_PSP_Main
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_Main(void)
{
    int32 iStatus = 0;
    iStatus = OS_BSPMain();

    /* If OS_BSPMain returns an error, then print a note */
    if (iStatus != 0)
    {
        /* At this point, CFS is closing and there is no guarantee that the OS_printf will work. */
        // UndCC_NextLine(SSET134)
        printf("\nPSP: Exiting CFE_PSP_Main() - OS_BSPMain application status [%d] \n", iStatus);
    }
}


/**********************************************************
 * 
 * Function: CFE_PSP_ProcessPOSTResults
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_ProcessPOSTResults(void)
{
    uint64 ulBitExecuted = 0ULL;
    uint64 ulBitResult   = 0ULL;
    uint32 uiIndex = 0;

    if ((aimonGetBITExecuted(&ulBitExecuted, 0) == OK) &&
        (aimonGetBITResults(&ulBitResult, 0) == OK))
    {
        /* Only log Power on Self Test that are valid*/
        for (uiIndex = 0; uiIndex < FWCH_TMR; uiIndex++)
        {
            if (CHECK_BIT(ulBitExecuted, uiIndex))
            {
                if (CHECK_BIT(ulBitResult, uiIndex))
                {
                    OS_printf("PSP: POST Test - FAILED - %s.\n",
                            AimonCompletionBlockTestIDStrings[uiIndex]);
                }
                else
                {
                    OS_printf("PSP: POST Test - PASSED - %s .\n",
                            AimonCompletionBlockTestIDStrings[uiIndex]);
                }
            }
            else
            {

                OS_printf("PSP: POST Test - Not Run - %s.\n",
                        AimonCompletionBlockTestIDStrings[uiIndex]);
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
** - g_StartupInfo.ResetType
** - g_StartupInfo.ResetSubtype
** Finally, function will print to console the reset type
**
** \par Assumptions, External Events, and Notes:
** Output defines are defined in Aitech file scratchRegMap.h
** SP0 target does not output a single reset type code, but actually they are
** combinations: i.e. RESET_SRC_POR | RESET_SRC_SWR.
**
** \param None
**
** \return A combination of RESET_SRC_POR, RESET_SRC_WDT, RESET_SRC_FWDT, 
**         RESET_SRC_CPCI, RESET_SRC_SWR 
*/
static RESET_SRC_REG_ENUM CFE_PSP_ProcessResetType(void)
{
    int32 iStatus = 0;
    RESET_SRC_REG_ENUM resetSrc = 0;

    /* Reset the content of the safe mode user data buffer */
    memset(&g_StartupInfo.safeModeUserData, 0, sizeof(g_StartupInfo.safeModeUserData));

    /* Default to CFE_PSP_RST_TYPE_PROCESSOR */
    g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_POWERON;
    /* g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_PROCESSOR; */

    /* Gather information */
    iStatus  = ReadResetSourceReg(&resetSrc, false);
    if (iStatus == OS_SUCCESS)
    {
        switch (resetSrc)
        {
            /* Power on Reset */
            case RESET_SRC_POR:
                OS_printf("PSP: POWERON Reset: Power Switch ON.\n");
                g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_POWER_CYCLE;
                break;
            /* Software Reset - detects user (including machine check) requested reset */
            case (RESET_SRC_POR | RESET_SRC_SWR):
                OS_printf("PSP: POWERON Reset: Software Hard Reset.\n");
                g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;

                /*
                Aitech manual says that it always returns OK, but we should check
                anyway in case a new BSP changes the return value.
                */
                if(ReadSafeModeUserData(&g_StartupInfo.safeModeUserData, false)!= OK)
                {
                    OS_printf("PSP: PROCESSOR Reset: failed to read safemode data.\n");
                }
                break;

            /* External FPGA Watchdog timer - detects primary EEPROM boot failure */                
            case (RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_WDT):
                OS_printf("PSP: PROCESSOR Reset: External FPGA Watchdog timer primary EEPROM boot failure.\n");
                g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                break;

            /* cPCI Reset - detects cPCI reset initiated by FPGA from remote SBC */        
            case (RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_CPCI):
                OS_printf("PSP: PROCESSOR Reset: cPCI Reset initiated by FPGA from remote SBC.\n");
                g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_RESET_COMMAND;
                break;

            /* Internal FPGA Watchdog timer - detects application SW failure */        
            case (RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_FWDT):
                OS_printf("PSP: PROCESSOR Reset: Internal FPGA Watchdog timer application SW failure.\n");
                g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_HW_WATCHDOG;
                break;

            default:
                OS_printf("PSP: POWERON Reset: UNKNOWN Reset.\n");
                g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_POWERON;
                g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET;
                break;
        }
    }
    else
    {
        OS_printf("PSP: POWERON Reset: UNKNOWN Reset. Reset source read failed.\n");
        g_StartupInfo.ResetType = CFE_PSP_RST_TYPE_POWERON;
        g_StartupInfo.ResetSubtype = CFE_PSP_RST_SUBTYPE_UNDEFINED_RESET;
    }

    return resetSrc;
}

/**********************************************************
 * 
 * Function: CFE_PSP_LogSoftwareResetType
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_LogSoftwareResetType(RESET_SRC_REG_ENUM resetSrc)
{
    uint8 ucIndex = 0u;
    uint8 ucMaxIterations = sizeof(g_pMachineCheckCause_msg) / sizeof(g_pMachineCheckCause_msg[0]);
    const char *pResetSrcString = NULL;
    
    switch (resetSrc)
    {
        case RESET_SRC_POR:
            pResetSrcString = "POR";
            break;
        case RESET_SRC_POR | RESET_SRC_SWR:
            pResetSrcString = "POR SWR";
            break;
        case RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_WDT:
            pResetSrcString = "POR SWR WDT";
            break;
        case RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_FWDT:
            pResetSrcString = "POR SWR FWDT";
            break;
        case RESET_SRC_POR | RESET_SRC_SWR | RESET_SRC_CPCI:
            pResetSrcString = "POR SWR CPCI";
            break;
        default:
            pResetSrcString = "Unknown";
            break;
    }
    if (g_StartupInfo.safeModeUserData.sbc == SM_LOCAL_SBC)
    {
        OS_printf("PSP: PROCESSOR rst Source = 0x%x = (%s) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
                resetSrc,
                pResetSrcString,
                g_StartupInfo.safeModeUserData.safeMode,
                "LOCAL",
                g_StartupInfo.safeModeUserData.reason,
                g_StartupInfo.safeModeUserData.mckCause);
    }
    else
    {
        OS_printf("PSP: PROCESSOR rst Source = 0x%x = (%s) Safe mode = %d, sbc = %s, reason = %d, cause = 0x%08x\n",
                resetSrc,
                pResetSrcString,
                g_StartupInfo.safeModeUserData.safeMode,
                "REMOTE",
                g_StartupInfo.safeModeUserData.reason,
                g_StartupInfo.safeModeUserData.mckCause);
    }

    /* If the reason for reset is Software, the mckCause will provide the cause */
    if (BITWISE_AND(resetSrc, RESET_SRC_SWR))
    {
        for (ucIndex = 0u; ucIndex < ucMaxIterations; ucIndex++)
        {
            if (CHECK_BIT((g_StartupInfo.safeModeUserData.mckCause), (ucIndex)))
            {
                OS_printf("PSP %s\n", g_pMachineCheckCause_msg[ucIndex]);
            }
        }
    }
}


/**********************************************************
 * 
 * Function: CFE_PSP_StartupFailedRestartSP0_hook
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_StartupFailedRestartSP0_hook(uint32 timer_id)
{
    /*
    This function is called when the process of starting up CFS is taking 
    longer time than 60 seconds. VSM is responsible for clearing the timer.
    
    Process:
    - Update fail.txt
    - Set Reset Memory
    - If CFS is restarting for 3 times, reboot with POWERON (Clear the User Reserved Memory)
    - If CFS is restarting for less than 3 times, reboot with PROCESSOR
    - If CFS is restarting for 1 time with POWERON, change Boot Startup String
    */
    int32   iFD = 0;
    CFE_PSP_Startup_structure_t startup_buffer = {'\0'};
    ssize_t iBytes_read = 0;
    ssize_t iBytes_wrote = 0;
    off_t   lOffset = 0;
    int32   iRet = 0;
    uint32  uiRestartType = CFE_PSP_RST_TYPE_PROCESSOR;
    bool    RemoveFile = false;

    if (g_StartupInfo.timerID == timer_id)
    {

        OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE "Startup Timer Expired!!\nRestarting\n");

        /* Append  */
        iFD = open(g_StartupInfo.fullpath_failed_startup_filename, O_CREAT | O_RDWR, 0644);
        if (iFD <= 0)
        {
            OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE
                      "Error, could not open `%s`\n",
                      g_StartupInfo.fullpath_failed_startup_filename
            );
            /*
            If we cannot open or create a file in the active CFS partition
            change partition.
            */
            uiRestartType = CFE_PSP_RST_TYPE_CFS_TOGGLE;
        }
        else
        {
            /* Created New, Wrong Size, or Right Size */
            iBytes_read = read(iFD, (char *)&startup_buffer, sizeof(startup_buffer));
            if (iBytes_read == 0)
            {
                /*
                This is the case when the file is empty or just created.
                Both startup_failed_attempts and startup_failed_reset_attempts
                are set to zero.
                */
                g_StartupInfo.startup_failed_attempts++;
            }
            else if ((iBytes_read != 0) && (iBytes_read != sizeof(startup_buffer)))
            {
                /*
                If we are reading less or more than the startup_buffer size, it means
                that the file is corrupted or the CFS instance has changed the definition
                of the g_StartupInfo structure.
                In any case, the file content cannot be read. All reset counters 
                are reset to zero just like the previous case.
                */
                OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE
                          "Error, could not read structure from `%s`, deleting file.\n",
                          g_StartupInfo.fullpath_failed_startup_filename
                );
                g_StartupInfo.startup_failed_attempts++;
            }
            else
            {
                startup_buffer.startup_failed_attempts++;

                if (startup_buffer.startup_failed_attempts == CFE_PSP_STARTUP_MAX_PROCESSOR_RESETS)
                {
                    startup_buffer.startup_failed_attempts = 0;
                    startup_buffer.startup_failed_reset_attempts++;

                    /* too many attempts, restart with POWERON to delete user reserved memory */
                    OS_printf("PSP DEBUG: iBytes_read = %d --> POWERON\n", iBytes_read);

                    /* If the previous reset was due to POWERON, switch boot partition */
                    if (g_StartupInfo.ResetType == CFE_PSP_RST_TYPE_POWERON)
                    {
                        /* Switch boot partition */
                        uiRestartType = CFE_PSP_RST_TYPE_CFS_TOGGLE;
                    }

                    uiRestartType = CFE_PSP_RST_TYPE_POWERON;
                }

                g_StartupInfo.startup_failed_attempts = startup_buffer.startup_failed_attempts;
                g_StartupInfo.startup_failed_reset_attempts = startup_buffer.startup_failed_reset_attempts;

            }

            /* Replace whatever is in the file */
            lOffset = lseek(iFD, 0, SEEK_SET);
            if (lOffset != 0)
            {
                /* If we cannot operate on the file, delte it */
                OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE
                          "Error, could not set the write pointer for `%s`\n",
                          g_StartupInfo.fullpath_failed_startup_filename
                );
                RemoveFile = true;
            }
            else
            {
                /* Write current startup info structure */
                iBytes_wrote = write(iFD,(char *) &g_StartupInfo, sizeof(g_StartupInfo));
                if (iBytes_wrote != sizeof(g_StartupInfo))
                {
                    OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE
                              "Error, could not write to `%s`, deleting file.\n",
                              g_StartupInfo.fullpath_failed_startup_filename
                    );
                }
            }
            /* Close file */
            iRet = close(iFD);
            if (iRet < 0)
            {
                OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE
                          "Error, could not close file `%s`\n",
                          g_StartupInfo.fullpath_failed_startup_filename
                );
            }

            if (RemoveFile)
            {
                remove(g_StartupInfo.fullpath_failed_startup_filename);
            }
        }

        /* Restart target */
        CFE_PSP_Restart(uiRestartType);
    }

    OS_printf("PSP: Error, Timer ID does not match `CFE_PSP_StartupFailedRestartSP0_hook`\n");
}

/**********************************************************
 * 
 * Function: CFE_PSP_StartupClear
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_StartupClear(void)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    int32       iOSAL_ret = 0;
    STATUS      iRet = OK;
    uint32      uiWatchdogTimer_msec = 0;

    OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE "Deactivating Startup Timer\n");

    /* Delete existing timer */
    iOSAL_ret = OS_TimerDelete(g_StartupInfo.timerID);
    if (iOSAL_ret != OS_SUCCESS)
    {
        OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not delete timer [%d] errno [%d]\n", iOSAL_ret, errno);
        iRet_code = CFE_PSP_ERROR;
    }

    /* Delete status file saved in Flash */
    iRet = remove(g_StartupInfo.fullpath_failed_startup_filename);
    if (iRet != OK)
    {
        if (errno != EBADF)
        {
            OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE
                      "Error, could not delete failed startup file at `%s` errno [%d]\n",
                      g_StartupInfo.fullpath_failed_startup_filename,
                      errno
            );
            iRet_code = CFE_PSP_ERROR;
        }
    }

    /*
    Poor method to check if it is mine!!
    Deactivate Watchdog if it is ours
    */
    uiWatchdogTimer_msec = CFE_PSP_WatchdogGet();
    if (uiWatchdogTimer_msec == ((g_StartupInfo.uMaxWaitTime_sec + 10) * 1000))
    {
        CFE_PSP_WatchdogDisable();
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetActiveCFSPartition
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_GetActiveCFSPartition(char *pBuffer, uint32 uBuffer_size)
{
    int32   iReturn_code = CFE_PSP_ERROR;
    cpuaddr address_of_active_flash_partition_name = 0;
    char    cFallback_partition_name[] = "/ffx0";

    iReturn_code = OS_SymbolLookup(&address_of_active_flash_partition_name, "address_of_active_flash_partition_name");

    if ((iReturn_code == OS_SUCCESS) && (address_of_active_flash_partition_name != 0))
    {
        /* Validate Memory Pointers */
        iReturn_code = CFE_PSP_MemValidateRange(address_of_active_flash_partition_name, uBuffer_size, CFE_PSP_MEM_RAM);

        if (iReturn_code == CFE_PSP_SUCCESS)
        {
            /* Copy string to pBuffer */
            strncpy(pBuffer, (char *)address_of_active_flash_partition_name, uBuffer_size);
        }
        else
        {
            OS_printf("PSP: Error, address does not pass validation. Code [%d]\n", iReturn_code);
        }
    }
    else
    {
        OS_printf("PSP: Warning, kernel does not support reading the currently active CFS flash partition\n");
        strncpy(pBuffer, cFallback_partition_name, uBuffer_size);
    }
}

/**********************************************************
 * 
 * Function: CFE_PSP_StartupTimer
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_StartupTimer(void)
{
    int32       iRet_code = CFE_PSP_SUCCESS;
    int32       iOSAL_ret = 0;
    /* Accuracy of timer clock. */
    uint32      uiClock_accuracy = 0;

    /*
    Initialize startup structure
    g_StartupInfo.active_cfs_partition should have been updated by function
    CFE_PSP_GetActiveCFSPartition().
    */
    g_StartupInfo.timerID = 0;
    g_StartupInfo.uMaxWaitTime_sec = CFE_PSP_STARTUP_MAX_WAIT_TIME_SEC;
    strncpy(g_StartupInfo.failed_startup_filename, 
            CFE_PSP_STARTUP_FAILED_STARTUP_FILENAME,
            sizeof(g_StartupInfo.failed_startup_filename)
    );
    strncpy(g_StartupInfo.timer_name, "STARTUP_TIMER", sizeof(g_StartupInfo.timer_name));
    snprintf(
        g_StartupInfo.fullpath_failed_startup_filename,
        sizeof(g_StartupInfo.fullpath_failed_startup_filename),
        "%s/%s",
        g_StartupInfo.active_cfs_partition,
        g_StartupInfo.failed_startup_filename
    );
    /*
    This is unknown until we actually read the failed startup file and update it
    */
    g_StartupInfo.startup_failed_attempts = 0;
    g_StartupInfo.startup_failed_reset_attempts = 0;

    /* Create a timer to call CFE_PSP_StartupFailedRestartSP0_hook */
    iOSAL_ret = OS_TimerCreate(&g_StartupInfo.timerID,
                               g_StartupInfo.timer_name,
                               &uiClock_accuracy,
                               &CFE_PSP_StartupFailedRestartSP0_hook
    );
    if (iOSAL_ret != OS_SUCCESS)
    {
        OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not start the Startup Timer [%d]\n",iOSAL_ret);
        iRet_code = CFE_PSP_ERROR;
    }
    else
    {
        /* Set up a one-shot timer for uMaxWaitTime seconds */
        iOSAL_ret = OS_TimerSet(g_StartupInfo.timerID, g_StartupInfo.uMaxWaitTime_sec * 1000000, 0);
        if (iOSAL_ret != OS_SUCCESS)
        {
            OS_printf(PSP_STARTUP_TIMER_PRINT_SCOPE "Error, could not set the Startup Timer [%d]\n",iOSAL_ret);
            iRet_code = CFE_PSP_ERROR;
        }
    }

    /*
    Setup the backup Watchdog.
    This should be reconfigured automatically by HS app if it loads properly.
    The Watchdog is set to 10 seconds after the Startup Timer calls the restart
    function.
    */
    CFE_PSP_WatchdogInit();
    CFE_PSP_WatchdogSet((g_StartupInfo.uMaxWaitTime_sec + 10) * 1000);
    CFE_PSP_WatchdogEnable();

    return iRet_code;
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
    RESET_SRC_REG_ENUM resetSrc = 0;
    osal_id_t fs_id = 0;

    OS_printf_enable();

    /*
    Get currently active CFS partition, /ffx0 or /ffx1
    This function needs to run before any of the Startup APIs since they need
    the active CFS partition.
    */
    CFE_PSP_GetActiveCFSPartition(g_StartupInfo.active_cfs_partition, sizeof(g_StartupInfo.active_cfs_partition));
    
    /*
    Get current Boot Startup Filename from SP0 memory
    The string is used in case we need to switch to alterante CFS flash partition
    */
    CFE_PSP_GetBootStartupString(g_StartupInfo.boot_startup_filename,
                                 sizeof(g_StartupInfo.boot_startup_filename),
                                 false
    );

    /* Initialize the OS API data structures */
    iStatus = OS_API_Init();
    if (iStatus != OS_SUCCESS)
    {
        OS_printf("PSP: OS_Application_Startup() - OS_API_Init() failed (0x%X)\n", iStatus);

        /* Is OSAL fails, log the startup failure and restart target */
        CFE_PSP_StartupFailedRestartSP0_hook(g_StartupInfo.timerID);
    }

    /* Set up all the virtual FS mappings */
    iStatus = CFE_PSP_SetFileSysAddFixedMap(&fs_id);
    if (iStatus != CFE_PSP_SUCCESS)
    {
        /* Startup can continue, but it may fail later, depending on config */
        OS_printf("PSP: Some or all Virtual FS Mapping has failed\n");
    }

    /* Start Startup Timer */
    if (CFE_PSP_StartupTimer() != CFE_PSP_SUCCESS)
    {
        OS_printf("PSP: Could not start timer\n");
    }
    else
    {
        OS_printf("PSP: Startup Timer Started, timeout in %d sec\n", g_StartupInfo.uMaxWaitTime_sec);
    }

    /* 
    Collect SP0 information for Telemetry
    This function should run as early as possible to collect information from
    hardware and POST, and setup the task to dump the collected information 
    when abort is called.
    */
    if (PSP_SP0_GetInfo() != CFE_PSP_SUCCESS)
    {
        OS_printf("PSP: Error while collecting SP0 information\n");
    }

    CFE_PSP_SetupReservedMemoryMap();

    /* PSP Determine the reset type */
    resetSrc = CFE_PSP_ProcessResetType();

    /* Initialize the reserved memory */
    if (CFE_PSP_InitProcessorReservedMemory(g_StartupInfo.ResetType) != OS_SUCCESS)
    {
        OS_printf("PSP: OS_Application_Startup() - CFE_PSP_InitProcessorReservedMemory() failed (0x%x)\n", iStatus);

        /* Is OSAL fails, log the startup failure and restart target */
        CFE_PSP_StartupFailedRestartSP0_hook(g_StartupInfo.timerID);
    }

    /*
    ** Initialize the statically linked modules (if any)
    */
    CFE_PSP_ModuleInit();

    CFE_PSP_MEM_SCRUB_Init();
    CFE_PSP_MEMORY_SYNC_Init();

    /*
    ** Adjust system task priorities so that tasks such as the shell are
    ** at a lower priority that the CFS apps
    */
    iTaskSetStatus = CFE_PSP_SetSysTasksPrio();
    if (iTaskSetStatus != OS_SUCCESS)
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
    CFE_PSP_MAIN_FUNCTION(g_StartupInfo.ResetType, g_StartupInfo.ResetSubtype, 1, CFE_PSP_NONVOL_STARTUP_FILE);

    /*
    Deactivate Startup Timer and related Watchdog since no apps currently
    handles this task.
    */
    CFE_PSP_StartupClear();
}

/**********************************************************
 * 
 * Function: OS_Application_Run
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
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
        g_uiShellTaskID = taskNameToId("tShell0");
    }

    if (suspend)
    {
        iStatus = taskSuspend(g_uiShellTaskID);
        if (iStatus == OK)
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
        iStatus = taskResume(g_uiShellTaskID);
        if (iStatus == OK)
        {
            OS_printf("Shell Task Resumed [0x%08X]\n",g_uiShellTaskID);
        }
        else
        {
            OS_printf("Shell Task could not be resumed [0x%08X]\n",g_uiShellTaskID);
        }
    }

    return iStatus;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetRestartType
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
uint32 CFE_PSP_GetRestartType(uint32 *resetSubType)
{
    if (resetSubType != NULL)
    {
        *resetSubType = g_StartupInfo.ResetSubtype;
    }

    return (g_StartupInfo.ResetType);
}

/**********************************************************
 * 
 * Function: CFE_PSP_SestTaskPrio
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_SetTaskPrio(const char *tName, uint8 tgtPrio)
{
    int32 iTID = CFE_PSP_ERROR;
    int32 iCurPriority = 0;
    uint8 ucCurPrio = 0;
    uint8 ucNewPrio = 0;
    int32 iStatus = CFE_PSP_ERROR;

    if ((tName != NULL) && (memchr(tName, (int) NULL, CFE_PSP_MAXIMUM_TASK_LENGTH) != NULL))
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

/**********************************************************
 * 
 * Function: CFE_PSP_SetSysTasksPrio
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
static int32 CFE_PSP_SetSysTasksPrio(void)
{
    int32 iStatus = CFE_PSP_SUCCESS;
    int32 iIndex = 0;
    int32 iRet_code = OS_ERROR;

    int32 iNumberOfTask = sizeof(g_VxWorksTaskList) / sizeof(CFE_PSP_OS_Task_and_priority_t);

    OS_printf("PSP: Setting system tasks' priorities for %d tasks.\n", iNumberOfTask);

    for (iIndex = 0; iIndex < iNumberOfTask; iIndex++)
    {
        iRet_code = CFE_PSP_SetTaskPrio(g_VxWorksTaskList[iIndex].VxWorksTaskName, 
                                  g_VxWorksTaskList[iIndex].VxWorksTaskPriority);
        if (iRet_code != OS_SUCCESS)
        {
            iStatus = CFE_PSP_ERROR;
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
static int32 CFE_PSP_SetFileSysAddFixedMap(osal_id_t *fs_id)
{
    int32   iRet_code      = CFE_PSP_SUCCESS;
    int32   iStatus        = OS_SUCCESS;
    int32   index         = 0;
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
            OS_printf("PSP: OS_FileSysAddFixedMap() failure: %d\n", (int)iStatus);
            iRet_code = CFE_PSP_ERROR;
        }
    }

    if (iRet_code == CFE_PSP_SUCCESS)
    {
        OS_printf("PSP: Set %d Virtual Path(s)\n", cNumberOfLinks);
    }

    return iRet_code;
}

/**********************************************************
 * 
 * Function: CFE_PSP_InitSSR
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32  CFE_PSP_InitSSR(uint32 bus, uint32 device, char *DeviceName )
{
    /* Function not implemented for the SP0 */
    OS_printf("PSP: CFE_PSP_InitSSR function not implemented for SP0\n");
    return CFE_PSP_ERROR_NOT_IMPLEMENTED;

}
