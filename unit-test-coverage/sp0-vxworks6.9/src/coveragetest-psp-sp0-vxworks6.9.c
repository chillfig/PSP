/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
 *
 *    Copyright (c) 2020, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

/**
 * \file
 * \ingroup  vxworks
 * \author   
 *
 */

#include "coveragetest-psp-sp0-vxworks6.9.h"

/* Psp_Test_Setup
 *
 * Purpose:
 *   Called by the unit test tool to set up the app prior to each test
 */
void Psp_Test_Setup(void)
{
    UT_ResetState(0);
}

/*
 * Test_Teardown
 *
 * Purpose:
 *   Called by the unit test tool to tear down the app after each test
 */
void Psp_Test_Teardown(void) {}

/* UtTest_Setup
 *
 * Purpose:
 *   Registers the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    ADD_TEST(CFE_PSP_Main);
    ADD_TEST(CFE_PSP_ProcessPOSTResults);
    ADD_TEST(CFE_PSP_ProcessResetType);
    ADD_TEST(CFE_PSP_LogSoftwareResetType);
    ADD_TEST(CFE_PSP_StartupFailed);
    ADD_TEST(CFE_PSP_StartupClear);
    ADD_TEST(CFE_PSP_GetActiveCFSPartition);
    ADD_TEST(CFE_PSP_StartupTimer);
    ADD_TEST(OS_Application_Startup);
    ADD_TEST(CFE_PSP_GetRestartType);
    ADD_TEST(CFE_PSP_SetTaskPrio);
    ADD_TEST(OS_Application_Run);
    ADD_TEST(CFE_PSP_SuspendConsoleShellTask);
    ADD_TEST(CFE_PSP_SetFileSysAddFixedMap);
    ADD_TEST(CFE_PSP_InitSSR);

    ADD_TEST(CFE_PSP_Restart);
    ADD_TEST(CFE_PSP_Panic);
    ADD_TEST(CFE_PSP_FlushCaches);
    ADD_TEST(CFE_PSP_GetProcessorId);
    ADD_TEST(CFE_PSP_GetSpacecraftId);
    ADD_TEST(CFE_PSP_GetProcessorName);
    ADD_TEST(CFE_PSP_ToggleCFSBootPartition);
    ADD_TEST(CFE_PSP_ValidatePath);
    ADD_TEST(CFE_PSP_GetBootStartupString);
    ADD_TEST(CFE_PSP_SetBootStartupString);
    ADD_TEST(CFE_PSP_GetBootStructure);
    ADD_TEST(CFE_PSP_SetBootStructure);
    ADD_TEST(CFE_PSP_PrintBootParameters);
    ADD_TEST(CFE_PSP_KernelGetCRC);
    ADD_TEST(CFE_PSP_KernelLoadNew);

    ADD_TEST(CFE_PSP_ReadFromFlash);
    ADD_TEST(CFE_PSP_WriteToFlash);
    ADD_TEST(CFE_PSP_DeleteFile);
    ADD_TEST(CFE_PSP_CreateFile);
    ADD_TEST(CFE_PSP_CheckFile);
    ADD_TEST(CFE_PSP_CreateDirectory);

    ADD_TEST(CFE_PSP_edrPolicyHandlerHook);
    ADD_TEST(CFE_PSP_AttachExceptions);
    ADD_TEST(CFE_PSP_SetDefaultExceptionEnvironment);
    ADD_TEST(CFE_PSP_ExceptionGetSummary_Impl);
    
    ADD_TEST(CFE_PSP_SP0CollectStaticInfo);
    ADD_TEST(CFE_PSP_SP0CollectDynamicInfo);
    ADD_TEST(CFE_PSP_SP0PrintToBuffer);
    ADD_TEST(CFE_PSP_SP0GetStaticInfoTable);
    ADD_TEST(CFE_PSP_SP0GetDynamicInfoTable);
    ADD_TEST(CFE_PSP_SP0DumpData);
    ADD_TEST(CFE_PSP_SP0GetDiskFreeSize);
    ADD_TEST(CFE_PSP_SP0ROM1Lock);
    ADD_TEST(CFE_PSP_SP0ROM1Unlock);
    ADD_TEST(CFE_PSP_SP0ROM2Lock);
    ADD_TEST(CFE_PSP_SP0ROM2Unlock);
    ADD_TEST(CFE_PSP_SP0ROMXCmd);
    ADD_TEST(CFE_PSP_SP0ROM1Status);
    ADD_TEST(CFE_PSP_SP0ROM2Status);

    ADD_TEST(CFE_PSP_MemScrubInit);
    ADD_TEST(CFE_PSP_MemScrubTask);
    ADD_TEST(CFE_PSP_MemScrubIsRunning);
    ADD_TEST(CFE_PSP_MemScrubDisable);
    ADD_TEST(CFE_PSP_MemScrubEnable);
    ADD_TEST(CFE_PSP_MemScrubValidate);
    ADD_TEST(CFE_PSP_MemScrubSet);
    ADD_TEST(CFE_PSP_MemScrubGet);
    ADD_TEST(CFE_PSP_MemScrubErrStats);
    ADD_TEST(CFE_PSP_MemScrubDelete);
    ADD_TEST(CFE_PSP_MemScrubTrigger);

    ADD_TEST(CFE_PSP_NTPDaemonIsRunning);
    ADD_TEST(CFE_PSP_SetOSTime);
    ADD_TEST(CFE_PSP_GetOSTime);
    ADD_TEST(CFE_PSP_StartNTPDaemon);
    ADD_TEST(CFE_PSP_StopNTPDaemon);

    ADD_TEST(CFE_PSP_GetCDSSize);
    ADD_TEST(CFE_PSP_WriteToCDS);
    ADD_TEST(CFE_PSP_ReadFromCDS);
    ADD_TEST(CFE_PSP_GetResetArea);
    ADD_TEST(CFE_PSP_GetUserReservedArea);
    ADD_TEST(CFE_PSP_GetVolatileDiskMem);
    ADD_TEST(CFE_PSP_InitProcessorReservedMemory);
    ADD_TEST(CFE_PSP_SetupReservedMemoryMap);
    ADD_TEST(CFE_PSP_DeleteProcessorReservedMemory);
    ADD_TEST(CFE_PSP_GetKernelTextSegmentInfo);
    ADD_TEST(CFE_PSP_GetCFETextSegmentInfo);
    ADD_TEST(CFE_PSP_CalculateCRC);
    ADD_TEST(CFE_PSP_GetRESETSize);
    ADD_TEST(CFE_PSP_GetVOLATILEDISKSize);
    ADD_TEST(CFE_PSP_GetUSERRESERVEDSize);
    ADD_TEST(CFE_PSP_GetMemSize);
    ADD_TEST(CFE_PSP_GetMemArea);
    ADD_TEST(CFE_PSP_GetCDSArea);
    ADD_TEST(CFE_PSP_WriteToRESET);
    ADD_TEST(CFE_PSP_ReadFromRESET);
    ADD_TEST(CFE_PSP_WriteToVOLATILEDISK);
    ADD_TEST(CFE_PSP_ReadFromVOLATILEDISK);
    ADD_TEST(CFE_PSP_WriteToUSERRESERVED);
    ADD_TEST(CFE_PSP_ReadFromUSERRESERVED);
    ADD_TEST(CFE_PSP_RestoreReset);
    ADD_TEST(CFE_PSP_RestoreCDS);
    ADD_TEST(CFE_PSP_RestoreVolatileDisk);
    ADD_TEST(CFE_PSP_RestoreUserReserved);
    ADD_TEST(CFE_PSP_WriteToRAM);
    ADD_TEST(CFE_PSP_ReadFromRAM);
    ADD_TEST(CFE_PSP_RestoreData);
    ADD_TEST(CFE_PSP_MemSyncInit);
    ADD_TEST(CFE_PSP_MemSyncDestroy);
    ADD_TEST(CFE_PSP_MemSyncStart);
    ADD_TEST(CFE_PSP_MemSyncStop);
    ADD_TEST(CFE_PSP_MemSyncIsRunning);
    ADD_TEST(CFE_PSP_MemSyncSetPriority);
    ADD_TEST(CFE_PSP_MemSyncGetPriority);
    ADD_TEST(CFE_PSP_MemSyncSetFrequency);
    ADD_TEST(CFE_PSP_MemSyncGetFrequency);
    ADD_TEST(CFE_PSP_MemSyncgetStatus);
    ADD_TEST(CFE_PSP_MemSyncTask);
    ADD_TEST(CFE_PSP_CDSFilepath);
    ADD_TEST(CFE_PSP_ResetFilepath);
    ADD_TEST(CFE_PSP_VolatileDiskFilepath);
    ADD_TEST(CFE_PSP_UserReservedFilepath);
    ADD_TEST(CFE_PSP_BootRecordFilepath);
    ADD_TEST(CFE_PSP_ExceptionStorageFilepath);
    ADD_TEST(CFE_PSP_GenerateFilepath);
    ADD_TEST(CFE_PSP_CheckURMFilesExists);
    ADD_TEST(CFE_PSP_FlushToFLASH);


    ADD_TEST(CFE_PSP_WatchdogInit);
    ADD_TEST(CFE_PSP_WatchdogEnable);
    ADD_TEST(CFE_PSP_WatchdogDisable);
    ADD_TEST(CFE_PSP_WatchdogService);
    ADD_TEST(CFE_PSP_WatchdogGet);
    ADD_TEST(CFE_PSP_WatchdogSet);
    ADD_TEST(CFE_PSP_WatchdogStatus);

}
