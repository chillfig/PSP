/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: 
 * Draco
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 **********************************************************************/

/*
 *
 *    Copyright (c) 2023, United States government as represented by the
 *    administrator of the National Aeronautics Space Administration.
 *    All rights reserved. This software was created at NASA Goddard
 *    Space Flight Center pursuant to government contracts.
 *
 *    This is governed by the NASA Open Source Agreement and may be used,
 *    distributed and modified only according to the terms of that agreement.
 *
 */

#ifndef _PSP_PING_TESTCASES_H_
#define _PSP_PING_TESTCASES_H_

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

void Ut_CFE_PSP_CalculateICMPChecksum(void);
void Ut_CFE_PSP_GenerateICMPPkt(void);
void Ut_ping_Init(void);
void Ut_CFE_PSP_Process_Packet(void);
void Ut_CFE_PSP_GetPacketTypeName(void);
void Ut_CFE_PSP_SinglePing(void);

#endif /* _PSP_PING_TESTCASES_H_ */