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

#ifndef COVERAGETEST_PSP_SP0_VXWORKS_H
#define COVERAGETEST_PSP_SP0_VXWORKS_H

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

#include "coveragetest-psp-shared.h"

#define ADD_TEST(test) UtTest_Add((Test_##test), Psp_Test_Setup, Psp_Test_Teardown, #test)

/* Psp_Test_Setup
 *
 * Purpose:
 *   Called by the unit test tool to set up the app prior to each test
 */
void Psp_Test_Setup(void);
void Psp_Test_Teardown(void);

/*
 * Test routine dedicated to coverage of each of the PSP implementation functions
 */

void Test_CFE_PSP_Main(void);
void Test_CFE_PSP_ProcessPOSTResults(void);
void Test_CFE_PSP_ProcessResetType(void);
void Test_CFE_PSP_LogSoftwareResetType(void);
void Test_CFE_PSP_StartupFailed(void);
void Test_CFE_PSP_StartupClear(void);
void Test_CFE_PSP_GetActiveCFSPartition(void);
void Test_CFE_PSP_StartupTimer(void);
void Test_OS_Application_Startup(void);
void Test_CFE_PSP_GetRestartType(void);
void Test_CFE_PSP_SetTaskPrio(void);
void Test_OS_Application_Run(void);
void Test_CFE_PSP_SuspendConsoleShellTask(void);
void Test_CFE_PSP_SetFileSysAddFixedMap(void);
void Test_CFE_PSP_InitSSR(void);

void Test_CFE_PSP_Restart(void);
void Test_CFE_PSP_Panic(void);
void Test_CFE_PSP_FlushCaches(void);
void Test_CFE_PSP_GetProcessorId(void);
void Test_CFE_PSP_GetSpacecraftId(void);
void Test_CFE_PSP_GetProcessorName(void);
void Test_CFE_PSP_ToggleCFSBootPartition(void);
void Test_CFE_PSP_ValidatePath(void);
void Test_CFE_PSP_GetBootStartupString(void);
void Test_CFE_PSP_SetBootStartupString(void);
void Test_CFE_PSP_GetBootStructure(void);
void Test_CFE_PSP_SetBootStructure(void);
void Test_CFE_PSP_PrintBootParameters(void);
void Test_CFE_PSP_KernelGetCRC(void);
void Test_CFE_PSP_KernelLoadNew(void);

void Test_CFE_PSP_ReadFromFlash(void);
void Test_CFE_PSP_WriteToFlash(void);
void Test_CFE_PSP_DeleteFile(void);
void Test_CFE_PSP_CreateFile(void);
void Test_CFE_PSP_CheckFile(void);
void Test_CFE_PSP_CreateDirectory(void);

void Test_CFE_PSP_edrPolicyHandlerHook(void);
void Test_CFE_PSP_AttachExceptions(void);
void Test_CFE_PSP_SetDefaultExceptionEnvironment(void);
void Test_CFE_PSP_ExceptionGetSummary_Impl(void);

void Test_CFE_PSP_SP0CollectStaticInfo(void);
void Test_CFE_PSP_SP0CollectDynamicInfo(void);
void Test_CFE_PSP_SP0PrintToBuffer(void);
void Test_CFE_PSP_SP0GetStaticInfoTable(void);
void Test_CFE_PSP_SP0GetDynamicInfoTable(void);
void Test_CFE_PSP_SP0DumpData(void);
void Test_CFE_PSP_SP0GetDiskFreeSize(void);
void Test_CFE_PSP_SP0ROM1Lock(void);
void Test_CFE_PSP_SP0ROM1Unlock(void);
void Test_CFE_PSP_SP0ROM2Lock(void);
void Test_CFE_PSP_SP0ROM2Unlock(void);
void Test_CFE_PSP_SP0ROMXCmd(void);
void Test_CFE_PSP_SP0ROM1Status(void);
void Test_CFE_PSP_SP0ROM2Status(void);

void Test_CFE_PSP_MemScrubInit(void);
void Test_CFE_PSP_MemScrubTask(void);
void Test_CFE_PSP_MemScrubIsRunning(void);
void Test_CFE_PSP_MemScrubDisable(void);
void Test_CFE_PSP_MemScrubEnable(void);
void Test_CFE_PSP_MemScrubValidate(void);
void Test_CFE_PSP_MemScrubSet(void);
void Test_CFE_PSP_MemScrubGet(void);
void Test_CFE_PSP_MemScrubErrStats(void);
void Test_CFE_PSP_MemScrubDelete(void);
void Test_CFE_PSP_MemScrubTrigger(void);

void Test_CFE_PSP_SetOSTime(void);
void Test_CFE_PSP_GetOSTime(void);
void Test_CFE_PSP_NTPDaemonIsRunning(void);
void Test_CFE_PSP_StartNTPDaemon(void);
void Test_CFE_PSP_StopNTPDaemon(void);

void Test_CFE_PSP_GetCDSSize(void);
void Test_CFE_PSP_WriteToCDS(void);
void Test_CFE_PSP_ReadFromCDS(void);
void Test_CFE_PSP_GetResetArea(void);
void Test_CFE_PSP_GetUserReservedArea(void);
void Test_CFE_PSP_GetVolatileDiskMem(void);
void Test_CFE_PSP_InitProcessorReservedMemory(void);
void Test_CFE_PSP_SetupReservedMemoryMap(void);
void Test_CFE_PSP_DeleteProcessorReservedMemory(void);
void Test_CFE_PSP_GetKernelTextSegmentInfo(void);
void Test_CFE_PSP_GetCFETextSegmentInfo(void);

void Test_CFE_PSP_CalculateCRC(void);
void Test_CFE_PSP_GetRESETSize(void);
void Test_CFE_PSP_GetVOLATILEDISKSize(void);
void Test_CFE_PSP_GetUSERRESERVEDSize(void);
void Test_CFE_PSP_GetMemSize(void);

void Test_CFE_PSP_GetMemArea(void);
void Test_CFE_PSP_GetCDSArea(void);
void Test_CFE_PSP_WriteToRESET(void);
void Test_CFE_PSP_ReadFromRESET(void);
void Test_CFE_PSP_WriteToVOLATILEDISK(void);
void Test_CFE_PSP_ReadFromVOLATILEDISK(void);
void Test_CFE_PSP_WriteToUSERRESERVED(void);
void Test_CFE_PSP_ReadFromUSERRESERVED(void);
void Test_CFE_PSP_RestoreReset(void);
void Test_CFE_PSP_RestoreCDS(void);
void Test_CFE_PSP_RestoreVolatileDisk(void);
void Test_CFE_PSP_RestoreUserReserved(void);
void Test_CFE_PSP_WriteToRAM(void);
void Test_CFE_PSP_ReadFromRAM(void);
void Test_CFE_PSP_RestoreData(void);
void Test_CFE_PSP_MemSyncInit(void);
void Test_CFE_PSP_MemSyncDestroy(void);
void Test_CFE_PSP_MemSyncStart(void);
void Test_CFE_PSP_MemSyncStop(void);
void Test_CFE_PSP_MemSyncIsRunning(void);
void Test_CFE_PSP_MemSyncSetPriority(void);
void Test_CFE_PSP_MemSyncGetPriority(void);
void Test_CFE_PSP_MemSyncSetFrequency(void);
void Test_CFE_PSP_MemSyncGetFrequency(void);
void Test_CFE_PSP_MemSyncgetStatus(void);
void Test_CFE_PSP_MemSyncTask(void);
void Test_CFE_PSP_CDSFilepath(void);
void Test_CFE_PSP_ResetFilepath(void);
void Test_CFE_PSP_VolatileDiskFilepath(void);
void Test_CFE_PSP_UserReservedFilepath(void);
void Test_CFE_PSP_BootRecordFilepath(void);
void Test_CFE_PSP_ExceptionStorageFilepath(void);
void Test_CFE_PSP_GenerateFilepath(void);
void Test_CFE_PSP_CheckURMFilesExists(void);
void Test_CFE_PSP_FlushToFLASH(void);

void Test_CFE_PSP_WatchdogInit(void);
void Test_CFE_PSP_WatchdogEnable(void);
void Test_CFE_PSP_WatchdogDisable(void);
void Test_CFE_PSP_WatchdogService(void);
void Test_CFE_PSP_WatchdogGet(void);
void Test_CFE_PSP_WatchdogSet(void);
void Test_CFE_PSP_WatchdogStatus(void);

#endif
