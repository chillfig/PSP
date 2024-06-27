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
*   \file psp_ft.c
*
*   \brief PSP Functional tests
*
*   \brief Functions are tested without OSAL and CFE, and they run on target. Standard VxWorks functions 
*   must be used in place of OSAL.
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
#include <ioLib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <vxWorks.h>
#include <edrLib.h>
#include <usrLib.h>

#include <vxbTimerLib.h>
#include <wdb/wdbLib.h>

#include <math.h>

#include "common_types.h"
#include "osapi.h"
/* Provides the definition of CFE_TIME_SysTime_t */
#include "cfe_time_extern_typedefs.h"
#include "cfe_config.h"
#include "cfe_config_core_internal.h"

#include "cfe.h"
#include "cfe_psp_exceptionstorage_api.h"
#include "cfe_psp_exceptionstorage_types.h"
#include "cfe_psp.h"
#include "cfe_psp_memory.h"
/* #include "cfe_psp_memsync.h" */
#include "cfe_psp_config.h"
#include "cfe_psp_start.h"
/* #include "cfe_psp_exception.h" */
/* #include "cfe_psp_support.h" */
#include "cfe_psp_memscrub.h"
#include "cfe_psp_gr740info.h"
#include "cfe_psp_timesync.h"
#include "cfe_psp_timer.h"
#include "psp_version.h"

#include "psp_ft.h"

#define CFE_1HZ_TASK_NAME                   "TIME_1HZ_TASK"

/** \brief OSAL OS_BSPMain Entry Point */
extern int OS_BSPMain(void);

/* Global count of pass and fail */
uint16_t cnt_tests = 0;
uint16_t cnt_pass = 0;
uint16_t cnt_fail = 0;

/* Helper functions */
bool check_file_exists(char *filename)
{
    int32_t f_ret = 0;
    bool return_value = false;

    f_ret = open(filename,O_RDONLY,0644);
    if (f_ret != ERROR)
    {
        close(f_ret);
        return_value = true;
    }
    return return_value;
}

int32_t get_file_content(char *filename, char *buffer, uint16_t buffer_size)
{
    int32_t     f_ret = 0;
    uint32_t    uSize = 0;
    uint32_t    return_value = CFE_PSP_ERROR;

    f_ret = open(filename,O_RDONLY,0644);
    if (f_ret != ERROR)
    {
        uSize = read(f_ret, buffer, buffer_size);
        if (uSize != -1)
        {
            return_value = CFE_PSP_SUCCESS;
        }
        close(f_ret);
    }
    return return_value;
}

/**
 ** \brief Check if a value is within percent range
 ** 
 ** \param[in] current_value 
 ** \param[in] right_value 
 ** \param[in] vt is a value type enum
 ** \param[in] positive_value 
 ** \param[in] negative_value 
 ** \return true current value within range
 ** \return false current value is outside range
 */
bool check_range_value(uint32_t current_value, uint32_t positive_value, uint32_t negative_value)
{
    if ((current_value < positive_value) && (current_value > negative_value))
    {
        return true;
    }
    return false;
}

/* Main entry point */
void PSP_FT_Run(void)
{
    /* Setup environment */
    PSP_FT_Setup();
    
    /* Introduce a delay to let NTP sync its time */
    OS_TaskDelay(1000);

    /* Run functional test */
    PSP_FT_Start();
}

/* Setup environment */
void PSP_FT_Setup(void)
{
    /* char buffer[10]; */

    /** Create an empty cfe_es_startup to remove the error **/
    int32_t tmp_fd;
    tmp_fd = creat("/ram0/cf/cfe_es_startup.scr",O_RDWR);
    close(tmp_fd);

    /* Clear flash from backups */
    /*
    TODO: Add code here to clear any permanent memory locations or files 
    that could affect the test.
    */

    /* Setup OSAL and call the OSAL version of OS_Application_Startup */
    CFE_PSP_Main();
}

/**
 ** \brief Special function to send WDB Event Message to calling WTX Script
 ** 
 */
void PSP_FT_SendEndTestEvent(void)
{
    char cEventMessage[] = "End of Tests";
    if (wdbUserEvtPost (cEventMessage) != OK) {
        OS_printf ("Can't send message to host tools");
        CFE_Config_Init();
        CFE_Config_GetString(CFE_CONFIGID_CORE_BUILDINFO_DATE);
        CFE_Config_IterateAll(NULL, 0);
    }
}

/**
 ** \brief Functional test main function
 ** 
 */
void PSP_FT_Start(void)
{
    /* Reset test counters */
    cnt_tests = 0;
    cnt_pass = 0;
    cnt_fail = 0;

    OS_printf("[START PSP Functional Test]\n\n");

    ft_exception();

    ft_gr740_info();

    ft_memory();

    ft_mem_scrub();

    ft_ntp();

    ft_ssr();

    ft_start();

    ft_support();

    ft_timer();

    ft_watchdog();

    OS_printf("[END PSP Functional Test]\n\n");

    OS_printf("[RESULTS]\n"
              "Passed: %u - "
              "Failed: %u\n",
              cnt_pass, cnt_fail);

    PSP_FT_SendEndTestEvent();
}

void ft_support(void)
{
    // int32_t  ret_code = 0;
    // char     bs_original[200] = {'\0'};
    // char     bs_new[200] = "/ram0/test";
    // char     bs_confirm[200] = {'\0'};
    // char     cKernelName[] = "AITECH20";
    // char     cKernelNameBad[] = "IEH0";
    // uint32_t uiCRC = 0;

    OS_printf("[SUPPORT START]\n");

    // /* Get startup script */
    // ret_code = CFE_PSP_GetBootStartupString(bs_original, sizeof(bs_original), 0);
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Boot Startup String Successful")

    // /* Set new startup script */
    // ret_code = CFE_PSP_SetBootStartupString(bs_new, 0);
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Setting new Boot Startup String Successful")
    
    // ret_code = CFE_PSP_GetBootStartupString(bs_confirm, sizeof(bs_confirm), 0);
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Boot Startup String Successful")
    // /* The string comparison is using memcmp because I need to compare the null character too in case 
    // bs_original is 0 length */
    // FT_Assert_True(memcmp(bs_confirm, bs_original, strlen(bs_original)+1) != 0, "Confirming string boot has changed compared to original")
    // FT_Assert_True(memcmp(bs_confirm, bs_new, strlen(bs_new)+1) == 0, "Confirming string boot is the new string")

    // /* Restore original value */
    // ret_code = CFE_PSP_SetBootStartupString(bs_original, 0);
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Setting original Boot Startup String Successful")

    // /* Read again and confirm that it is the original string */
    // ret_code = CFE_PSP_GetBootStartupString(bs_confirm, sizeof(bs_confirm), 0);
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Boot Startup String Successful")
    // FT_Assert_True(memcmp(bs_confirm, bs_original, strlen(bs_original)+1) == 0, "Confirming string boot has been restored to original")

    // /* Read CRC of AITECH20 kernel */
    // uiCRC = CFE_PSP_KernelGetCRC(cKernelName, true);
    // FT_Assert_True((uiCRC != 0), "Get `AITECH20` CRC");

    // /* Read CRC of random name kernel */
    // uiCRC = CFE_PSP_KernelGetCRC(cKernelNameBad, true);
    // FT_Assert_True((uiCRC == 0), "Could not get `IEH0` CRC");

    OS_printf("[SUPPORT END]\n\n");
}

void ft_exception(void)
{
    uint32_t  exceptionCount = 0;
    char      ReasonBuf[128];
    uint32_t  LogId = 0;
    osal_id_t TaskId;
    int32_t   ret_code = 0;
    CFE_PSP_Exception_LogData_t *pBuffer = NULL;

    OS_printf("[EXCEPTION START]\n");

    /*
    At this point of the functional test, the exception hook function has already
    being assigned and the default exception environment set up.
    CFE_PSP_edrPolicyHandlerHook
    CFE_PSP_AttachExceptions
    CFE_PSP_SetDefaultExceptionEnvironment
    */

    // ret_code = CFE_PSP_SaveToNVRAM();
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Saving ED&R Data to EEPROM was Successful")

    // ret_code = CFE_PSP_LoadFromNVRAM();
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Loading ED&R Data from EEPROM returned Success")

    // ret_code = CFE_PSP_ClearNVRAM();
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Clearing ED&R Data in EEPROM returned Success")

    // ret_code = CFE_PSP_LoadFromNVRAM();
    // FT_Assert_True(ret_code == CFE_PSP_ERROR, "Loading ED&R Data from EEPROM after Clear EDR returned Error")

    EDR_USER_FATAL_INJECT(true, "Test Fatal EDR Inject");

    pBuffer = CFE_PSP_Exception_GetNextContextBuffer();
    pBuffer->sys_task_id = 0;
    CFE_PSP_Exception_WriteComplete();

    /* NEED TO INDUCE AN EXCEPTION */
    exceptionCount = CFE_PSP_Exception_GetCount();
    FT_Assert_True(exceptionCount == 1, "Exception Count is one")

    ret_code = CFE_PSP_Exception_GetSummary(&LogId, &TaskId, ReasonBuf, sizeof(ReasonBuf));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Summary of Exception Data returned Success")

    OS_printf("[EXCEPTION END]\n\n");
}

/*
Functional Tests:
- 
*/
void ft_memory(void)
{
    int32_t     ret_code = 0;

    cpuaddr     ptrResetAddr = 0;
    cpuaddr     ptrUserAddr = 0;
    cpuaddr     ptrVolAddr = 0;
    cpuaddr     ptrKernelSeg = 0;
    cpuaddr     ptrCFETextSeg = 0;

    uint32_t    size_of_resetArea = 0;
    uint32_t    size_of_UserArea = 0;
    uint32_t    size_of_VolArea = 0;
    uint32_t    size_of_KernelSeg = 0;
    uint32_t    size_of_CFETestSeg = 0;

    uint32_t    uCDSSize;
    int32_t     char_data = 0x11;
    char        data_buffer[100] = {};
    char        data_buffer_readback[100] = {};
    
    OS_printf("[MEMORY START]\n");

    /* Check the CDS Size in User Memory RAM */
    ret_code = CFE_PSP_GetCDSSize(&uCDSSize);
    FT_Assert_True(uCDSSize > 1, "CDS Size larger than zero (%d bytes)",uCDSSize)

    /* Set the local data memory */
    memset(data_buffer, char_data, sizeof(data_buffer));

    ret_code = CFE_PSP_WriteToCDS(data_buffer, 0, sizeof(data_buffer));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToCDS returned SUCCESS")

    ret_code = CFE_PSP_ReadFromCDS(data_buffer_readback, 0, sizeof(data_buffer_readback));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadToCDS returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(data_buffer));
    FT_Assert_True(ret_code == 0, "ReadCDS returned the same data as the WriteCDS")

    /* Set the local data memory */
    memset(data_buffer, char_data+1, sizeof(data_buffer));

    ret_code = CFE_PSP_WriteToCDS(data_buffer, 0, sizeof(data_buffer));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToCDS returned SUCCESS")

    ret_code = CFE_PSP_ReadFromCDS(data_buffer_readback, 0, sizeof(data_buffer_readback));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadToCDS returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(data_buffer));
    FT_Assert_True(ret_code == 0, "ReadCDS returned the same data as the WriteCDS")


    ret_code = CFE_PSP_GetResetArea(&ptrResetAddr,&size_of_resetArea);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Reset Area")

    ret_code = CFE_PSP_GetUserReservedArea(&ptrUserAddr, &size_of_UserArea);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get User Reserved Memory")

    ret_code = CFE_PSP_GetVolatileDiskMem(&ptrVolAddr, &size_of_VolArea);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Volatile Disk Memory")

    ret_code = CFE_PSP_GetKernelTextSegmentInfo(&ptrKernelSeg, &size_of_KernelSeg);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Kernel Text Segment Info")
    
    ret_code = CFE_PSP_GetCFETextSegmentInfo(&ptrCFETextSeg, &size_of_CFETestSeg);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get CFE Text Segment Info")

    /*
    Following set of functions should only be used by PSP internally. The memory
    is already setup and initialized by the time the functional test runs.
    CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_PROCESSOR)
    CFE_PSP_InitProcessorReservedMemory(CFE_PSP_RST_TYPE_POWERON )
    CFE_PSP_SetupReservedMemoryMap
    CFE_PSP_DeleteProcessorReservedMemory
    */

    OS_printf("[MEMORY END]\n\n");
}

/*
Functional Tests:
- Get Reset Type (void)
- Get Process Reset Type (void)
- Get Last Software Reset Type (void)
- Get POST results (void)
- Change Task Priority with new priority and same priority
- Change Task Priority of wrong task name
*/
void ft_start(void)
{
/*     bool        test_case = false;
    uint32_t    last_reset_type;
    char        buffer[256]; */
    int32_t     ret_code;

    OS_printf("[START]\n");

    /* Check if it gets the right Reset type */
/*     last_reset_type = CFE_PSP_GetRestartType(NULL);
    test_case = ((last_reset_type == CFE_PSP_RST_TYPE_POWERON) ||
                (last_reset_type == CFE_PSP_RST_TYPE_PROCESSOR) ||
                (last_reset_type == CFE_PSP_RST_TYPE_MAX));
    FT_Assert_True(test_case, "CFE_PSP_GetRestartType returned valid reset type") */

    /* Get Active CFS Partition */
/*     memset(buffer,'\0', sizeof(buffer));
    CFE_PSP_GetActiveCFSPartition(buffer, sizeof(buffer));
    memset(buffer,(int32_t) NULL, sizeof(buffer));
    FT_Assert_True(memchr(buffer, (int32_t) NULL, sizeof(buffer)) != NULL,"CFE_PSP_GetActiveCFSPartition returned a string") */

    /* CFE_PSP_StartupTimer, CFE_PSP_StartupFailedRestartSP0_hook, CFE_PSP_StartupClear */
/*     CFE_PSP_StartupTimer();
    CFE_PSP_StartupClear(); */

    /* Print the POST results */
/*     CFE_PSP_ProcessPOSTResults();
    FT_Assert_True(true, "CFE_PSP_ProcessPOSTResults has no return value (void)") */

    /* Change task priority tests */
    ret_code = CFE_PSP_SetTaskPrio(CFE_1HZ_TASK_NAME, 26);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "CFE_PSP_SetTaskPrio return code for successful priority change")

    ret_code = CFE_PSP_SetTaskPrio(CFE_1HZ_TASK_NAME, 26);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "CFE_PSP_SetTaskPrio return code for successful priority change")

    ret_code = CFE_PSP_SetTaskPrio("SOMETHING_WRONG", 220);
    FT_Assert_True(ret_code == CFE_PSP_ERROR, "CFE_PSP_SetTaskPrio return code for wrong task name")

    OS_printf("[START END]\n\n");
}

void ft_ssr(void)
{
    int32_t ret_code;

    OS_printf("[SSR START]\n\n");

    /* Make sure InitSSR returns not implemented */
    ret_code = CFE_PSP_InitSSR(0,0,NULL);
    FT_Assert_True(ret_code == CFE_PSP_ERROR_NOT_IMPLEMENTED, "CFE_PSP_InitSSR returned not implemented")

    OS_printf("[SSR END]\n\n");
}

void ft_mem_scrub(void)
{
    // int32_t       ret_code = 0;
    // uint32_t      index_counter = 0;

    // CFE_PSP_MemScrubStatus_t  mem_scrub_stats1;
    // CFE_PSP_MemScrubStatus_t  mem_scrub_stats2;
    // CFE_PSP_MemScrubErrStats_t mem_scrub_ddr_stats;

    OS_printf("[MEM_SCRUB]\n");

    // /* Status should return all zeros */
    // CFE_PSP_MemScrubGet(&mem_scrub_stats1, false);
    // FT_Assert_True((mem_scrub_stats1.uiMemScrubEndAddr != 0), "Retrieved MEM Scrub configuration and status")

    // if (mem_scrub_stats1.RunMode != MEMSCRUB_MANUAL_MODE)
    // {
    //     /* Disable MEM Scrub task */
    //     CFE_PSP_MemScrubDisable();
    //     FT_Assert_True((CFE_PSP_MemScrubIsRunning() == false), "Mem Scrub task disabled and confirm is not running")
    // }    

    // mem_scrub_stats1.RunMode = MEMSCRUB_IDLE_MODE;
    // ret_code = CFE_PSP_MemScrubSet(&mem_scrub_stats1);
    // FT_Assert_True((ret_code == CFE_PSP_SUCCESS), "Mem Scrub Run Mode configuration applied")

    // /* Enable MEM Scrub task */
    // ret_code = CFE_PSP_MemScrubEnable();
    // FT_Assert_True((ret_code == CFE_PSP_SUCCESS), "Mem Scrub task enabled and confirm is running")

    // /* Get error statistics */
    // CFE_PSP_MemScrubErrStats(&mem_scrub_ddr_stats, false);
    // FT_Assert_True(true, "Mem Scrub DDR statistics")

    // /* Set different values */
    // mem_scrub_stats1.uiMemScrubStartAddr = 20 * MEMSCRUB_PAGE_SIZE;
    // mem_scrub_stats1.uiMemScrubEndAddr = 120 * MEMSCRUB_PAGE_SIZE;
    // mem_scrub_stats1.opMemScrubTaskPriority = 240;
    // ret_code = CFE_PSP_MemScrubSet(&mem_scrub_stats1);
    // CFE_PSP_MemScrubGet(&mem_scrub_stats2, false);
    // FT_Assert_True((mem_scrub_stats2.uiMemScrubStartAddr == mem_scrub_stats1.uiMemScrubStartAddr), "Mem Scrub Start Address successfully changed")
    // FT_Assert_True((mem_scrub_stats2.uiMemScrubEndAddr == mem_scrub_stats1.uiMemScrubEndAddr), "Mem Scrub End Address successfully changed")
    // FT_Assert_True((mem_scrub_stats2.opMemScrubTaskPriority == mem_scrub_stats1.opMemScrubTaskPriority), "Mem Scrub priority successfully changed")

    // /* Delete Task */
    // ret_code = CFE_PSP_MemScrubDelete();
    // FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Deleted task return code")
    // FT_Assert_True((CFE_PSP_MemScrubIsRunning() == false), "Deleted task confirmed is not running")
    
    // /* Re-Initialize Task */
    // ret_code = CFE_PSP_MemScrubInit();
    // FT_Assert_True((CFE_PSP_MemScrubIsRunning() == true), "Initialized task confirmed is running")
    // FT_Assert_True((ret_code == CFE_PSP_SUCCESS), "Mem Scrub task enabled and confirm is running")

    // /* Check if value of memory pages scrubbed changes after 10 seconds */
    // CFE_PSP_MemScrubGet(&mem_scrub_stats1, false);
    // for (index_counter = 0; index_counter < 10; index_counter++)
    // {
    //     CFE_PSP_MemScrubGet(&mem_scrub_stats2, false);
    //     if (mem_scrub_stats1.uiMemScrubTotalPages < mem_scrub_stats2.uiMemScrubTotalPages)
    //     {
    //         break;
    //     }
    //     else
    //     {
    //         OS_TaskDelay(1000);
    //     }
    // }
    // FT_Assert_True((mem_scrub_stats1.uiMemScrubTotalPages < mem_scrub_stats2.uiMemScrubTotalPages), 
    //     "MEM Scrub status returns progress after %u sec", index_counter
    //     )

    OS_printf("[MEM_SCRUB END]\n\n");
}

/*
Functional Tests:

*/
void ft_gr740_info(void)
{
    int32_t     ret_code = CFE_PSP_ERROR;
    uint32_t    uiTemp_value = 0;
    CFE_PSP_GR740StaticInfoTable_t     gr740_static_table;
    CFE_PSP_GR740DynamicInfoTable_t    gr740_dynamic_table;

    OS_printf("[GR740_INFO]\n");

    /*** Collect GR740 Info ***/
    /* Setup onboard temperature sensor */
    ret_code = CFE_PSP_TempSensorInit();
    /* Check result */
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Setting up onboard temperature sensor - returned success")

    ret_code = CFE_PSP_GetTemperatureAwait(&uiTemp_value);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Getting onboard temperature value - returned success")
    if (ret_code == CFE_PSP_SUCCESS)
    {
        OS_printf("GR740 Temp: %u\n", uiTemp_value);
        FT_Assert_True( 
            check_range_value(uiTemp_value, 55, 10), 
            "GR740 Temperature is %u [C]", uiTemp_value)
    }

    /*** Collect Static GR740 Info ***/
    /* Get the hardware/software information from target */
    ret_code = CFE_PSP_GR740CollectStaticInfo();
    /* Check result */
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Static GR740 info - returned success")

    /*** Collect Dynamic GR740 Info ***/
    /* Get the hardware/software information from target */
    ret_code = CFE_PSP_GR740CollectDynamicInfo();
    /* Check result */
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Dynamic GR740 info - returned success")

    /*** Get GR740 Info Table and check their values ***/
    /* Get the table that was filled up by the GetInfo function */
    ret_code = CFE_PSP_GR740GetStaticInfoTable(&gr740_static_table, sizeof(gr740_static_table), 0);
    OS_printf("Data gathered at %s UTC\n", ctime(&gr740_static_table.lastUpdatedUTC.tv_sec));
    FT_Assert_True(
        ret_code == CFE_PSP_SUCCESS,
        "CFE_PSP_GR740GetStaticInfoTable returns SUCCESS"
    )
    /* Get the table that was filled up by the GetInfo function */
    ret_code = CFE_PSP_GR740GetDynamicInfoTable(&gr740_dynamic_table, sizeof(gr740_dynamic_table), 0);
    OS_printf("Data gathered at %s UTC\n", ctime(&gr740_dynamic_table.lastUpdatedUTC.tv_sec));
    FT_Assert_True(
        ret_code == CFE_PSP_SUCCESS,
        "CFE_PSP_GR740GetDynamicInfoTable returns SUCCESS"
    )
    OS_printf("[GR740_INFO END]\n\n");
}

/*
Functional Tests:
- Test NTP client daemon APIs. Check status, Enable and Disable
- Check if time has been synchronized
*/
void ft_ntp(void)
{
    int32_t     ret_code = CFE_PSP_ERROR;
    char        strMsg[256] = "";
    CFE_TIME_SysTime_t cfe_time_utc;
    CFE_TIME_SysTime_t cfe_time;
    CFE_TIME_SysTime_t psp_time;

    OS_printf("[NTP_SYNC]\n");

    /* PSP OS_Application_Startup starts the NTP Sync Task */

    /* Make sure that NTPd client is running */
    ret_code = CFE_PSP_StartNTPDaemon();

    /* Test Turning ON/OFF NTP Synchronization */


    /* Get current CFE time */
    cfe_time_utc = CFE_TIME_GetUTC();
    cfe_time = CFE_TIME_GetMET();
    ret_code = CFE_PSP_GetOSTime(&psp_time);

    /* Print it out */
    CFE_TIME_Print(strMsg, cfe_time);
    OS_printf("Current CFE Time From MET %s - "
              "Sec: %u - "
              "SubSec: %u\n",
              strMsg, cfe_time.Seconds, cfe_time.Subseconds);
    
    CFE_TIME_Print(strMsg, cfe_time_utc);
    OS_printf("Current PSP Time From UTC %s - "
              "Sec: %u - "
              "SubSec: %u\n",
              strMsg, cfe_time_utc.Seconds, cfe_time_utc.Subseconds);

    /* Test NTPd client daemon APIs */
    ret_code = CFE_PSP_NTPDaemonIsRunning();
    if (ret_code == CFE_PSP_SUCCESS)
    {
        FT_Assert_True(true,"NTPd client daemon is running")
        ret_code = CFE_PSP_StopNTPDaemon();
        FT_Assert_True(ret_code == CFE_PSP_SUCCESS,"NTPd client daemon successfully stopped")
    }
    else
    {
        FT_Assert_True(true,"NTPd client daemon is not running")
        ret_code = CFE_PSP_StartNTPDaemon();
        FT_Assert_True(ret_code,"NTPd client daemon successfully started")
    }

    OS_printf("[NTP_SYNC END]\n\n");
}

void ft_timer(void)
{
    OS_time_t   uiFirstValue;
    OS_time_t   uiSecondValue;
    OS_time_t   uiDiff;
    int64       iDiffTicks = 0;
    int32       iTicksPerSeconds = 0;

    OS_printf("[TIMER START]\n\n");
    
    iTicksPerSeconds = CFE_PSP_GetTimerTicksPerSecond();
    OS_printf("iTicksPerSeconds = %u\n\n", iTicksPerSeconds);

    /*
    Check that the Task Delay and the calculated difference in
    values between GetTime is similar.
    TODO: Maybe it is not working as expected. Probably 
    CFE_PSP_GetTime uses the same timer as OS_TaskDelay.
    */
    CFE_PSP_GetTime(&uiFirstValue);
    OS_TaskDelay(1000);
    CFE_PSP_GetTime(&uiSecondValue);
    uiDiff = OS_TimeSubtract(uiSecondValue, uiFirstValue);
    iDiffTicks = OS_TimeGetTotalMilliseconds(uiDiff);
    OS_printf("vxTimeBaseGet: TD = 1 sec -> dT = %d Ticks\n", (int32)iDiffTicks);

    CFE_PSP_GetTime(&uiFirstValue);
    OS_TaskDelay(2000);
    CFE_PSP_GetTime(&uiSecondValue);
    uiDiff = OS_TimeSubtract(uiSecondValue, uiFirstValue);
    iDiffTicks = OS_TimeGetTotalMilliseconds(uiDiff);
    OS_printf("vxTimeBaseGet: TD = 2 sec -> dT = %d Ticks\n", (int32)iDiffTicks);

    OS_printf("[TIMER END]\n\n");
}

/**
 ** \brief Test functionality of Watchdog
 ** 
 ** \par Description:
 ** Test will use all the API functions available. Watchdog time is set to
 ** 1000 msec while the simulated work is set to 900 msec. A for loop simulate
 ** a working loop and it is set to run for about 20*0.9=18 seconds.
 */
void ft_watchdog(void)
{
    int32_t           status = 0;
    uint32_t          wd_time_ms = 1000;
    uint32_t          wd_time_ms_default;
    int32_t           i = 0;
    uint32_t          iter_max = 20;

    OS_printf("[WATCHDOG]\n");

    FT_Assert_True(CFE_PSP_WatchdogStatus() == false, "Watchdog currently disabled")

    /*
    Initialize Watchdog (WD) sets the WD timer to default CFE_PSP_WATCHDOG_DEFAULT_MSEC
    */
    CFE_PSP_WatchdogInit();
    wd_time_ms_default = CFE_PSP_WatchdogGet();
    FT_Assert_True(wd_time_ms_default == CFE_PSP_WATCHDOG_DEFAULT_MSEC, "Watchdog timeout set to default %u ms (CFE_PSP_WATCHDOG_DEFAULT_MSEC)\n", wd_time_ms_default);

    /* Change WD value to 1 sec */
    CFE_PSP_WatchdogSet(wd_time_ms);
    wd_time_ms = CFE_PSP_WatchdogGet();
    FT_Assert_True(wd_time_ms == 1000, "Watchdog timeout set to %u ms\n", wd_time_ms);

    /* Start Pulse Loop every 900 ms for 20 times */
    CFE_PSP_WatchdogEnable();
    FT_Assert_True(CFE_PSP_WatchdogStatus() == true, "Watchdog currently enabled")
    for (i = 0; i < iter_max; i++)
    {
        OS_printf(".");
        /* Pulse WD */
        CFE_PSP_WatchdogService();

        /* Simulate work for 900 ms */
        status = OS_TaskDelay(900);
        if (status == OS_ERROR)
        {
            /* Disable WD right away before target restarts */
            CFE_PSP_WatchdogDisable();
            OS_printf("Unknown OS_TaskDelay error, shutting down test\n");
        }
    }
    /* Disable WD */
    CFE_PSP_WatchdogDisable();
    FT_Assert_True(CFE_PSP_WatchdogStatus() == false, "Watchdog currently disabled")
    FT_Assert_True(i == iter_max, "Watchdog survived %u pulses of %u msec", iter_max, wd_time_ms)

    OS_printf("[WATCHDOG END]\n\n");
}