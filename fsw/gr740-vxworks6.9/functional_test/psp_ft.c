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
/* #include "cfe_psp_support.h" */
#include "cfe_psp_memscrub.h"
#include "cfe_psp_gr740info.h"
#include "cfe_psp_timesync.h"
#include "cfe_psp_timer.h"
#include "psp_version.h"

#include "psp_ft.h"

#define PSP_FT_TEST_VXWORKS_TASK_NAME "TEST_PSP_FT_TASK"
#define PSP_FT_TEST_VXWORKS_TASK_PRIORITY 100
#define PSP_FT_TEST_VXWORKS_TASK_STACK_SIZE 4096

typedef struct
{
    bool bStarted;
} PSP_FT_TEST_TASK_CONTEXT;

/** \brief OSAL OS_BSPMain Entry Point */
extern int OS_BSPMain(void);

/* Global count of pass and fail */
uint16_t cnt_tests = 0;
uint16_t cnt_pass = 0;
uint16_t cnt_fail = 0;
PSP_FT_TEST_TASK_CONTEXT testContext;

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

    ft_ntp_sync();

    ft_ssr();

    ft_start();

    ft_support();

    ft_gr740_info();

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
    OS_printf("[SUPPORT START]\n");

    /* Nothing to do for GR740. */

    OS_printf("[SUPPORT END]\n\n");
}

/*
Functional Tests:
- Get GR740 Info and check return code
- Get GR740 Table and check values
- Dump GR740 Info and check if file exists
- Check file system sizes and check return value
*/
void ft_gr740_info(void)
{
    char                local_ram_disk[] = "/ram0";
    int64_t             fs_size_ram = 0;
    CFE_PSP_GR740StaticInfoTable_t     gr740_static_table;
    CFE_PSP_GR740DynamicInfoTable_t    gr740_dynamic_table;
    int32_t             ret_code = CFE_PSP_ERROR;
    uint32_t            uiTemp_value = 0;

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

    /*** Check file system sizes ***/
    /* Prepare */

    /* Execute function */
    fs_size_ram = CFE_PSP_GR740GetDiskFreeSize(local_ram_disk);
    /* Check result */
    FT_Assert_True(fs_size_ram > 0, "Check file system size - /ram0 file system size = %lld bytes", fs_size_ram)

    OS_printf("[GR740_INFO END]\n\n");
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

    cpuaddr     ptrSectionAddr = 0;
    cpuaddr     ptrKernelSeg = 0;
    cpuaddr     ptrCFETextSeg = 0;

    uint32_t    size_of_section_area = 0;
    uint32_t    size_of_KernelSeg = 0;
    uint32_t    size_of_CFETestSeg = 0;

    int32_t     char_data = 0x11;
    const int   DATA_BUFFER_SIZE = 100;
    char        data_buffer[DATA_BUFFER_SIZE];
    char        data_buffer_readback[DATA_BUFFER_SIZE];
    uint32_t    sizeof_test_data = 0;

    memset(data_buffer, 0, sizeof(data_buffer));
    memset(data_buffer_readback, 0, sizeof(data_buffer_readback));

    OS_printf("[MEMORY START]\n");

    ret_code = CFE_PSP_GetKernelTextSegmentInfo(&ptrKernelSeg, &size_of_KernelSeg);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Kernel Text Segment Info")
    
    ret_code = CFE_PSP_GetCFETextSegmentInfo(&ptrCFETextSeg, &size_of_CFETestSeg);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get CFE Text Segment Info")

    /**********************************************************
     * 
     * RESET MEMORY SECTION HANDLING
     * 
     **********************************************************/
    size_of_section_area = 0;
    ret_code = CFE_PSP_GetRESETSize(&size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS && size_of_section_area == GLOBAL_CONFIGDATA.CfeConfig->ResetAreaSize, "Get RESET Size %d", size_of_section_area)
    ret_code = CFE_PSP_GetResetArea(&ptrSectionAddr, &size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get Reset Area %p %d", (void*)ptrSectionAddr, size_of_section_area)

    sizeof_test_data = size_of_section_area > DATA_BUFFER_SIZE ? DATA_BUFFER_SIZE : size_of_section_area;

    memset(data_buffer, char_data, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToRESET(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToRESET returned SUCCESS")
    ret_code = CFE_PSP_ReadFromRESET(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromRESET returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromRESET returned the same data as the WriteCDS")

    memset(data_buffer, char_data+1, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToRESET(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToRESET returned SUCCESS")
    ret_code = CFE_PSP_ReadFromRESET(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromRESET returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromRESET returned the same data as the WriteCDS")

    /**********************************************************
     * 
     * CDS MEMORY SECTION HANDLING
     * 
     **********************************************************/
    size_of_section_area = 0;
    ret_code = CFE_PSP_GetCDSSize(&size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS && size_of_section_area == GLOBAL_CONFIGDATA.CfeConfig->CdsSize, "Get CDS Size %d", size_of_section_area)
    ret_code = CFE_PSP_GetCDSArea(&ptrSectionAddr, &size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get CDS Area %p %d", (void*)ptrSectionAddr, size_of_section_area)

    sizeof_test_data = size_of_section_area > DATA_BUFFER_SIZE ? DATA_BUFFER_SIZE : size_of_section_area;

    memset(data_buffer, char_data, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToCDS(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToCDS returned SUCCESS")
    ret_code = CFE_PSP_ReadFromCDS(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromCDS returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromCDS returned the same data as the WriteToCDS")

    memset(data_buffer, char_data+1, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToCDS(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToCDS returned SUCCESS")
    ret_code = CFE_PSP_ReadFromCDS(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromCDS returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromCDS returned the same data as the WriteToCDS")

    /**********************************************************
     * 
     * VOLATILE DISK MEMORY SECTION HANDLING
     * 
     **********************************************************/
    size_of_section_area = 0;
    ret_code = CFE_PSP_GetVOLATILEDISKSize(&size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS && size_of_section_area == GLOBAL_CONFIGDATA.CfeConfig->RamDiskSectorSize * GLOBAL_CONFIGDATA.CfeConfig->RamDiskTotalSectors, "Get VOLATILEDISK Size %d", size_of_section_area)
    ret_code = CFE_PSP_GetVolatileDiskMem(&ptrSectionAddr, &size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get VOLATILEDISK Area %p %d", (void*)ptrSectionAddr, size_of_section_area)

    sizeof_test_data = size_of_section_area > DATA_BUFFER_SIZE ? DATA_BUFFER_SIZE : size_of_section_area;

    memset(data_buffer, char_data, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToVOLATILEDISK(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToVOLATILEDISK returned SUCCESS")
    ret_code = CFE_PSP_ReadFromVOLATILEDISK(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromVOLATILEDISK returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromVOLATILEDISK returned the same data as the WriteToVOLATILEDISK")

    memset(data_buffer, char_data+1, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToVOLATILEDISK(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToVOLATILEDISK returned SUCCESS")
    ret_code = CFE_PSP_ReadFromVOLATILEDISK(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromVOLATILEDISK returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromVOLATILEDISK returned the same data as the WriteToVOLATILEDISK")

    /**********************************************************
     * 
     * USER RESERVED MEMORY SECTION HANDLING
     * 
     **********************************************************/
    size_of_section_area = 0;
    ret_code = CFE_PSP_GetUSERRESERVEDSize(&size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS && size_of_section_area == GLOBAL_CONFIGDATA.CfeConfig->UserReservedSize, "Get USERRESERVED Size %d", size_of_section_area)
    ret_code = CFE_PSP_GetUserReservedArea(&ptrSectionAddr, &size_of_section_area);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "Get UserReserved Area %p %d", (void*)ptrSectionAddr, size_of_section_area)

    sizeof_test_data = size_of_section_area > DATA_BUFFER_SIZE ? DATA_BUFFER_SIZE : size_of_section_area;

    memset(data_buffer, char_data, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToUSERRESERVED(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToUSERRESERVED returned SUCCESS")
    ret_code = CFE_PSP_ReadFromUSERRESERVED(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromUSERRESERVED returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromUSERRESERVED returned the same data as the WriteToUSERRESERVED")

    memset(data_buffer, char_data+1, sizeof(sizeof_test_data));
    ret_code = CFE_PSP_WriteToUSERRESERVED(data_buffer, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "WriteToUSERRESERVED returned SUCCESS")
    ret_code = CFE_PSP_ReadFromUSERRESERVED(data_buffer_readback, 0, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "ReadFromUSERRESERVED returned SUCCESS")
    ret_code = memcmp(data_buffer, data_buffer_readback, sizeof(sizeof_test_data));
    FT_Assert_True(ret_code == 0, "ReadFromUSERRESERVED returned the same data as the WriteToUSERRESERVED")


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

static void ft_start_test_task(_Vx_usr_arg_t arg)
{
    PSP_FT_TEST_TASK_CONTEXT *pContext = (PSP_FT_TEST_TASK_CONTEXT*)arg;
    pContext->bStarted = true;
    while (1)
    {
        OS_TaskDelay(1000);
    }
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
    int32_t     ret_code;

    OS_printf("[START]\n");

    TASK_ID tid = taskCreate(
        (char*)PSP_FT_TEST_VXWORKS_TASK_NAME,
        PSP_FT_TEST_VXWORKS_TASK_PRIORITY,
        0,
        PSP_FT_TEST_VXWORKS_TASK_STACK_SIZE,
        (FUNCPTR)ft_start_test_task,
        (_Vx_usr_arg_t)&testContext,
        0, 0, 0, 0, 0, 0, 0, 0, 0);

    FT_Assert_True(tid != TASK_ID_NULL, "Test Task created")

    FT_Assert_True(taskActivate(tid) == OK, "Test Task activated")

    while (testContext.bStarted == false)
    {
        OS_printf("Waiting for test task to start...\n\n");
        OS_TaskDelay(2000);
    }

    /* Change task priority tests */
    ret_code = CFE_PSP_SetTaskPrio((char *)PSP_FT_TEST_VXWORKS_TASK_NAME, 26);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "CFE_PSP_SetTaskPrio return code for successful priority change")

    ret_code = CFE_PSP_SetTaskPrio((char *)PSP_FT_TEST_VXWORKS_TASK_NAME, 30);
    FT_Assert_True(ret_code == CFE_PSP_SUCCESS, "CFE_PSP_SetTaskPrio return code for successful priority change")

    ret_code = CFE_PSP_SetTaskPrio("SOMETHING_WRONG", 220);
    FT_Assert_True(ret_code == CFE_PSP_ERROR, "CFE_PSP_SetTaskPrio return code for wrong task name")

    taskDelete(tid);

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
    OS_printf("[MEM_SCRUB]\n");

    /* GR740 memory scrubbing is managed by the CPU, so nothing for us to test here */

    OS_printf("[MEM_SCRUB END]\n\n");
}

/*
Functional Tests:
- Test NTP client daemon APIs. Check status, Enable and Disable
- Check if time has been synchronized
*/
void ft_ntp_sync(void)
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