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

#ifndef COVERAGETEST_PSP_GR740_VXWORKS_H
#define COVERAGETEST_PSP_GR740_VXWORKS_H

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
void Test_OS_Application_Startup(void);
void Test_CFE_PSP_SetTaskPrio(void);
void Test_CFE_PSP_SetSysTasksPrio(void);
void Test_OS_Application_Run(void);
void Test_CFE_PSP_ResetSysTasksPrio(void);
void Test_CFE_PSP_SuspendConsoleShellTask(void);
void Test_CFE_PSP_SetFileSysAddFixedMap(void);
void Test_CFE_PSP_FindProcessor(void);
void Test_CFE_PSP_OS_EventHandler(void);

void Test_CFE_PSP_Restart(void);
void Test_CFE_PSP_Panic(void);
void Test_CFE_PSP_FlushCaches(void);
void Test_CFE_PSP_GetProcessorId(void);
void Test_CFE_PSP_GetSpacecraftId(void);
void Test_CFE_PSP_GetProcessorName(void);

void Test_OS_Application_Startup(void);
void Test_OS_Application_Run(void);

void Test_CFE_PSP_edrPolicyHandlerHook(void);
void Test_CFE_PSP_AttachExceptions(void);
void Test_CFE_PSP_SetDefaultExceptionEnvironment(void);
void Test_CFE_PSP_ExceptionGetSummary_Impl(void);


void Test_CFE_PSP_TempSensorInit(void);
void Test_CFE_PSP_GetTemperatureAwait(void);
void Test_CFE_PSP_GR740CollectStaticInfo(void);
void Test_CFE_PSP_GR740CollectDynamicInfo(void);
void Test_CFE_PSP_GR740GetStaticInfoTable(void);
void Test_CFE_PSP_GR740GetDynamicInfoTable(void);

void Test_CFE_PSP_MemScrubInit(void);
void Test_CFE_PSP_MemScrubIsRunning(void);
void Test_CFE_PSP_MemScrubDisable(void);
void Test_CFE_PSP_MemScrubEnable(void);
void Test_CFE_PSP_MemScrubValidate(void);
void Test_CFE_PSP_MemScrubSet(void);
void Test_CFE_PSP_MemScrubGet(void);
void Test_CFE_PSP_MemScrubErrStats(void);
void Test_CFE_PSP_MemScrubDelete(void);

void Test_CFE_PSP_SetOSTime(void);
void Test_CFE_PSP_GetOSTime(void);
void Test_CFE_PSP_NTPDaemonIsRunning(void);
void Test_CFE_PSP_StartNTPDaemon(void);
void Test_CFE_PSP_StopNTPDaemon(void);

void Test_CFE_PSP_InitSSR(void);

void Test_CFE_PSP_InitLocalTime(void);
void Test_vxTimeBaseGet(void);

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

void Test_CFE_PSP_WatchdogInit(void);
void Test_CFE_PSP_WatchdogEnable(void);
void Test_CFE_PSP_WatchdogDisable(void);
void Test_CFE_PSP_WatchdogService(void);
void Test_CFE_PSP_WatchdogGet(void);
void Test_CFE_PSP_WatchdogSet(void);
void Test_CFE_PSP_WatchdogStatus(void);

#endif
