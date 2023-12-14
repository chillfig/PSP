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

/*=======================================================================================
 * \file
 * \ingroup  modules
 *
 * Coverage test for Ping
**=======================================================================================*/

#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"
#include "ut_psp_utils.h"

#include <vxWorks.h>
#include <timers.h>
#include "cfe_time_extern_typedefs.h"
#include "cfe_mission_cfg.h"
#include "cfe_psp.h"
#include "cfe_psp_config.h"

#include "../../modules/ping/cfe_psp_ping.c"

/*=======================================================================================
** Defined Values
**=======================================================================================*/

#define MAX_OS_PRINTF_MESSAGES      100
#define IP_HEADER_LEN                20

/*=======================================================================================
** PSP Ping test cases
**=======================================================================================*/

void Ut_CFE_PSP_CalculateICMPChecksum(void)
{
    /* Initialize Variable */
    char cStrTestDescription[256] = {0};
    uint16 uiCRCValue = 0;
    uint16 uiExpectedCRCValue = 0;
    uint8  ucData[100] = {0};
    uint8  ucIdx = 0; 
    /* ----- Test case #1 - Invalid Argument - Buffer is NULL ----- */

    /* ----- Test case #1 - Nominal ----- */
    /* Setup additional inputs  */
    for (ucIdx = 0; ucIdx < 100; ucIdx++)
    {
        ucData[ucIdx] = ucIdx;
    }
    /* Test setups */
    UT_ResetState(0);
    uiExpectedCRCValue = 0x6432;
    /* Execute test */
    uiCRCValue = CFE_PSP_CalculateICMPChecksum(ucData, sizeof(ucData));
    /* Verify outputs */
    strncpy(cStrTestDescription, "_CalculateChecksum() - 1/4: Nominal", sizeof(cStrTestDescription));
    UtAssert_True((uiCRCValue == uiExpectedCRCValue), cStrTestDescription);

    /* ----- Test case #2 - Nominal with odd Bytes ----- */
    Ut_OS_printf_Setup();

    /* Test setups */
    UT_ResetState(0);
    uiExpectedCRCValue = 0x6495;
    /* Execute test */
    uiCRCValue = CFE_PSP_CalculateICMPChecksum(ucData, sizeof(ucData) - 1);
    /* Verify outputs */
    strncpy(cStrTestDescription, "_CalculateChecksum() - 2/4: Nominal", sizeof(cStrTestDescription));
    UtAssert_True((uiCRCValue == uiExpectedCRCValue), cStrTestDescription);

    /* ----- Test case #2 - NULL Argument ----- */

    /* Test setups */
    UT_ResetState(0);
    uiExpectedCRCValue = 0;
    /* Execute test */
    uiCRCValue = CFE_PSP_CalculateICMPChecksum(NULL, 0);
    /* Verify outputs */
    strncpy(cStrTestDescription, "_CalculateChecksum() - 3/4: NULL Argument", sizeof(cStrTestDescription));
    UtAssert_True((uiCRCValue == uiExpectedCRCValue), cStrTestDescription);
}

void Ut_CFE_PSP_GenerateICMPPkt(void)
{
    /* Initialize Variable */
    char cStrTestDescription[256] = {0};
    char cStrOSprintfOutput[MAX_OS_PRINTF_MESSAGES] = {0};
    uint8 outIcmpPkt[ICMP_PACKET_SIZE] = {0};
    struct timespec getTimeReturnStruct = {{0}};
    PSP_Ping_Result_t retVal = PSP_PING_SUCCESS;
    CFE_TIME_SysTime_t myTime = {0};
    struct icmphdr icmpHdr = {0};
    uint16 usID = 0;

    /* ----- Test case #1 - Invalid Argument - Buffer is NULL ----- */

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* Execute test */
    retVal = CFE_PSP_GenerateICMPPkt(NULL, &usID);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_GenerateICMPPkt() - 1/6: Null Argument - Buffer is NULL", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_GenerateICMPPkt(): Invalid Argument.\n", MAX_OS_PRINTF_MESSAGES);
    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_CRT_PKT_ERR, cStrTestDescription);

    /* ----- Test case #2 - Invalid Argument - ID value is NULL ----- */

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();

    /* Execute test */
    retVal = CFE_PSP_GenerateICMPPkt(outIcmpPkt, NULL);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_GenerateICMPPkt() - 2/6: Null Argument - ID value is NULL", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_GenerateICMPPkt(): Invalid Argument.\n", MAX_OS_PRINTF_MESSAGES);
    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_CRT_PKT_ERR, cStrTestDescription);


    /* ----- Test case #2 - Failed Mutex Take ----- */

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    memset(outIcmpPkt, 0, sizeof(outIcmpPkt));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);

    /* Execute test */
    retVal = CFE_PSP_GenerateICMPPkt(outIcmpPkt, &usID);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_GenerateICMPPkt() - 3/6: Failed to MutSemTake()", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_GenerateICMPPkt(): Failed to take semaphore.\n", MAX_OS_PRINTF_MESSAGES);
    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_CRT_PKT_ERR, cStrTestDescription);

    /* ----- Test case #3 - Failed Mutex Give ----- */

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    memset(outIcmpPkt, 0, sizeof(outIcmpPkt));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_ERROR);

    /* Execute test */
    retVal = CFE_PSP_GenerateICMPPkt(outIcmpPkt, &usID);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_GenerateICMPPkt() - 4/6: Failed to MutSemGive()", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_GenerateICMPPkt(): Failed to give semaphore.\n", MAX_OS_PRINTF_MESSAGES);
    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_CRT_PKT_ERR, cStrTestDescription);

    /* ----- Test case #4 - GetOSTime Fails ----- */

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    memset(outIcmpPkt, 0, sizeof(outIcmpPkt));
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);

    /* Execute test */
    retVal = CFE_PSP_GenerateICMPPkt(outIcmpPkt, &usID);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_GenerateICMPPkt() - 5/6: Failed to get time", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_GenerateICMPPkt(): Failed to get time.\n", MAX_OS_PRINTF_MESSAGES);
    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_CRT_PKT_ERR, cStrTestDescription);

    /* ----- Test case #5 - Nominal ----- */

    /* Test setups */
    UT_ResetState(0);
    memset(outIcmpPkt, 0, sizeof(outIcmpPkt));
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    g_usPingIdentNum = 10;
    
    /* Execute test */
    retVal = CFE_PSP_GenerateICMPPkt(outIcmpPkt, &usID);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_GenerateICMPPkt() - 6/6: Nominal", sizeof(cStrTestDescription));
    memcpy(&icmpHdr, outIcmpPkt, sizeof(struct icmphdr));
    UtAssert_True(retVal == CFE_PSP_SUCCESS, cStrTestDescription);
    UtAssert_True(strcmp(CFE_PSP_GetPacketTypeName(icmpHdr.ucType), "Echo") == 0, cStrTestDescription);
    UtAssert_True(icmpHdr.ucCode == 0, cStrTestDescription);
    UtAssert_True(icmpHdr.ucType == ICMP_ECHO, cStrTestDescription);
    UtAssert_True(icmpHdr.un.echo.usID == 10, cStrTestDescription);
    UtAssert_True(icmpHdr.un.echo.usSequence == 0, cStrTestDescription);
    UtAssert_True(icmpHdr.usChecksum == 28311, cStrTestDescription);

    /* Move the PacketHdr to point at the Payload */
    memcpy(&myTime, &outIcmpPkt[ICMP_HEADER_SIZE], sizeof(CFE_TIME_SysTime_t));
    UtAssert_True(myTime.Seconds == 1, cStrTestDescription);
    UtAssert_True(myTime.Subseconds == 1000, cStrTestDescription);
    UtAssert_True(g_usPingIdentNum == 11, cStrTestDescription);
    UtAssert_True(usID == 10, cStrTestDescription);    
}

void Ut_ping_Init(void)
{
    /* Initialize Variable */
    char cStrTestDescription[256] = {0};
    uint32 uiPspModuleId = 0;
    char cStrOSprintfOutput[MAX_OS_PRINTF_MESSAGES] = {0};

    /* ----- Test case #1 - Nominal ----- */

    /* Test setups */
    UT_ResetState(0);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, CFE_PSP_SUCCESS);

    /* Execute test */
    strncpy(cStrTestDescription, "_ping_Init() - 1/3: Nominal", sizeof(cStrTestDescription));
    UtAssert_True(g_bInitMutex == false, cStrTestDescription);
    ping_Init(uiPspModuleId);
    UtAssert_True(g_bInitMutex == true, cStrTestDescription);

    /* ----- Test case #2 - Nominal - Mutex Already Defined ----- */

    /* Test setups */
    UT_ResetState(0);

    /* Execute test */
    strncpy(cStrTestDescription, "_ping_Init() - 2/3: Nominal - Mutex Already Defined", sizeof(cStrTestDescription));
    UtAssert_True(g_bInitMutex == true, cStrTestDescription);
    ping_Init(uiPspModuleId);
    UtAssert_True(g_bInitMutex == true, cStrTestDescription);

    /* ----- Test case #3 - MutSemCreate Fails ----- */

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    g_bInitMutex = false;
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERROR);

    /* Execute test */
    strncpy(cStrTestDescription, "_ping_Init() - 3/3: MutSemCreate Fails", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "ping_Init() - OS_MutSemCreate() failed: 0xFFFFFFFF\n", MAX_OS_PRINTF_MESSAGES);
    UtAssert_True(g_bInitMutex == false, cStrTestDescription);
    ping_Init(uiPspModuleId);
    UtAssert_True(g_bInitMutex == false, cStrTestDescription);
    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
}
void Ut_CFE_PSP_Process_Packet(void)
{
     /* Initialize Variable */
    char cStrTestDescription[256] = {0};
    char cStrOSprintfOutput[MAX_OS_PRINTF_MESSAGES] = {0};
    uint8 inIcmpPkt[ICMP_RTN_PACKET_SIZE] = {0};
    PSP_Ping_Result_t retVal = 0;
    uint16 usIdent = 20;
    uint8 ucBytesProcessing = ICMP_PACKET_SIZE + IP_HEADER_LEN;
    struct sockaddr_in from = {0};
    PSP_Ping_Stats_t StatPacket = {0};

    struct icmphdr *pIcmpHdr    = {0};
    struct ip      *pIpReplyHdr = {0};

    struct timespec getTimeReturnStruct;
    getTimeReturnStruct.tv_nsec = 0;
    CFE_TIME_SysTime_t *pMyTime = {0};

    /* ----- Test case #1 - Invalid Packet Type ----- */

    /* Test setups */
    UT_ResetState(0);
    memset(inIcmpPkt, 0, ICMP_RTN_PACKET_SIZE);

    /* Pointers to modify the ip header length */
    pIpReplyHdr = &inIcmpPkt;
    pIpReplyHdr->ip_hl = IP_HEADER_LEN >> 2;

    /* Pointers to modify the icmp packet info */
    pIcmpHdr = (char *)inIcmpPkt + IP_HEADER_LEN;
    pIcmpHdr->ucType = ICMP_DEST_UNREACH;

    /* Execute test */
    retVal = CFE_PSP_Process_Packet(&inIcmpPkt, ucBytesProcessing, &from, usIdent, &StatPacket);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_CFE_PSP_Process_Packet() - 1/4: Invalid Packet Type", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_INVALID_PKT_TYPE_ERR, cStrTestDescription);

    /* ----- Test case #2 - Invalid ID ----- */

    /* Test setups */
    UT_ResetState(0);
    memset(inIcmpPkt,0,ICMP_RTN_PACKET_SIZE);

    /* Pointers to modify the ip header length */
    pIpReplyHdr = &inIcmpPkt;
    pIpReplyHdr->ip_hl = IP_HEADER_LEN >> 2;

    /* Pointers to modify the icmp packet info */
    pIcmpHdr = (char *)inIcmpPkt + IP_HEADER_LEN;
    pIcmpHdr->ucType = ICMP_ECHOREPLY;
    pIcmpHdr->un.echo.usID = 99;

    /* Execute test */
    retVal = CFE_PSP_Process_Packet(&inIcmpPkt, ucBytesProcessing, &from, usIdent, &StatPacket);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_CFE_PSP_Process_Packet() - 2/4: Invalid ID", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_IGNORE, cStrTestDescription);

/* ----- Test case #3 - GetOSTime Fails ----- */

    /* Test setups */
    UT_ResetState(0);
    memset(inIcmpPkt, 0, ICMP_RTN_PACKET_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), ERROR);

    /* Pointers to modify the ip header length */
    pIpReplyHdr = &inIcmpPkt;
    pIpReplyHdr->ip_hl = IP_HEADER_LEN >> 2;

    /* Pointers to modify the icmp packet info */
    pIcmpHdr = (char *)inIcmpPkt + IP_HEADER_LEN;
    pIcmpHdr->ucType = ICMP_ECHOREPLY;
    pIcmpHdr->un.echo.usID = 20;

    /* Execute test */
    retVal = CFE_PSP_Process_Packet(&inIcmpPkt, ucBytesProcessing, &from, usIdent, &StatPacket);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_CFE_PSP_Process_Packet() - 3/4: GetOSTime Fails", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_IGNORE, cStrTestDescription);

    /* ----- Test case #4 - Invalid Packet Length ----- */

    /* Test setups */
    UT_ResetState(0);
    memset(inIcmpPkt, 0, ICMP_RTN_PACKET_SIZE);

    /* Pointers to modify the ip header length */
    pIpReplyHdr = &inIcmpPkt;
    pIpReplyHdr->ip_hl = IP_HEADER_LEN >> 2;

    /* Pointers to modify the icmp packet info */
    pIcmpHdr = (char *)inIcmpPkt + IP_HEADER_LEN;
    pIcmpHdr->ucType = ICMP_DEST_UNREACH;

    /* Set the Bytes Processing to be a invalid value */
    ucBytesProcessing = 100;

    /* Execute test */
    retVal = CFE_PSP_Process_Packet(&inIcmpPkt, ucBytesProcessing, &from, usIdent, &StatPacket);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_CFE_PSP_Process_Packet() - 1/4: Invalid Packet Length", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_IGNORE, cStrTestDescription);


/* ----- Test case #5 - Nominal ----- */

    /* Test setups */
    UT_ResetState(0);
    CFE_TIME_SysTime_t ExpecTime = {0};
    ExpecTime.Seconds = 1;
    ExpecTime.Subseconds = 0; 
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;
    ucBytesProcessing = ICMP_PACKET_SIZE + IP_HEADER_LEN;

    memset(inIcmpPkt, 0, ICMP_RTN_PACKET_SIZE);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDataBuffer(UT_KEY(CFE_TIME_Subtract), &ExpecTime, sizeof(ExpecTime), false);

    /* Pointers to modify the ip header length */
    pIpReplyHdr = &inIcmpPkt;
    pIpReplyHdr->ip_hl = IP_HEADER_LEN >> 2;

    /* Pointers to modify the icmp packet info */
    pIcmpHdr = (char *)inIcmpPkt + IP_HEADER_LEN;
    pIcmpHdr->ucType = ICMP_ECHOREPLY;
    pIcmpHdr->un.echo.usID = 20;

    /* Pointers to modify the payload */
    pMyTime = (char *)inIcmpPkt + IP_HEADER_LEN + ICMP_HEADER_SIZE;
    pMyTime->Seconds = 0;
    pMyTime->Subseconds = 0;
    inet_pton(AF_INET, "192.168.22.129", &from.sin_addr);

    /* Execute test */
    retVal = CFE_PSP_Process_Packet(&inIcmpPkt, ucBytesProcessing, &from, usIdent, &StatPacket);

    /* Verify outputs */
    strncpy(cStrTestDescription, "_CFE_PSP_Process_Packet() - 4/4: Nominal", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_SUCCESS, cStrTestDescription);
    UtAssert_True(strcmp(StatPacket.IPAddrName, "192.168.22.129") == 0, cStrTestDescription);
    UtAssert_True(StatPacket.roundTripTime.Seconds == 1, cStrTestDescription);
    UtAssert_True(StatPacket.usIdent == usIdent, cStrTestDescription);
    UtAssert_True(StatPacket.usType ==  ICMP_ECHOREPLY, cStrTestDescription);
}

void Ut_CFE_PSP_GetPacketTypeName(void)
{
    /* Initialize Variable */
    char cStrTestDescription[256] = {0};
    char* pBuffer[20] = {0};
    uint8 ucIcmpType = 0;

    /* ----- Test case #1 - Nominal ----- */
    
    /* Test setups */
    UT_ResetState(0);
    ucIcmpType = 0;

    /* Execute Test */
    strncpy(pBuffer, CFE_PSP_GetPacketTypeName(ucIcmpType), 20);

    /* Verify Output */
    strncpy(cStrTestDescription, "_GetPacketTypeName() - 1/3: Nominal", sizeof(cStrTestDescription));
    UtAssert_True(strcmp(pBuffer, "Echo Reply") == 0, cStrTestDescription);
    
    /* ----- Test case #2 - Invalid ucIcmpType ----- */
    
    /* Test setups */
    UT_ResetState(0);
    ucIcmpType = 20;
    
    /* Execute Test */
    strncpy(pBuffer, CFE_PSP_GetPacketTypeName(ucIcmpType), 20);

    /* Verify Output */
    strncpy(cStrTestDescription, "_GetPacketTypeName() - 3/3: Invalid ucIcmpType", sizeof(cStrTestDescription));
    UtAssert_True(strcmp(pBuffer, "OUT-OF-RANGE") == 0, cStrTestDescription);
}

void Ut_CFE_PSP_SinglePing(void)
{
     /* Initialize Variable */
    char cStrTestDescription[256] = {0};
    char cStrOSprintfOutput[MAX_OS_PRINTF_MESSAGES] = {0};
    uint8 inIcmpPkt[ICMP_RTN_PACKET_SIZE] = {0};
    PSP_Ping_Result_t retVal = 0;
    uint16 usIdent = 20;
    in_addr_t destaddr;
    PSP_Ping_Stats_t StatPacket = {0};
    struct timespec getTimeReturnStruct;
    getTimeReturnStruct.tv_nsec = 0;
    CFE_TIME_SysTime_t CheckTime = {0};
    struct ip *pIpReplyHdr = {0};

    /* ----- Test case #1 - Invalid Argument - HostIP NULL ----- */

    /* Test setups */
    UT_ResetState(0);

    /* Execute test */
    strncpy(cStrTestDescription, "_SinglePing() - 1/13: Invalid Argument - HostIP NULL", sizeof(cStrTestDescription));
    retVal = CFE_PSP_SinglePing(NULL, 5000, &StatPacket);

    /* Verify Output */
    UtAssert_True(retVal == PSP_PING_INVALID_ARG_ERR, cStrTestDescription);

    /* ----- Test case #2 - Invalid Argument - StatPacket NULL ----- */
    
    /* Test setups */
    UT_ResetState(0);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.169", 5, NULL);

    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 2/13: Invalid Argument - StatPacket NULL", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_INVALID_ARG_ERR, cStrTestDescription);

    /* ----- Test case #3 - Invalid Argument - Both NULL ----- */
    
    /* Test setups */
    UT_ResetState(0);

    /* Execute test */
    retVal = CFE_PSP_SinglePing(NULL, 5000, NULL);

    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 3/13: Invalid Argument - Both NULL", sizeof(cStrTestDescription));
    UtAssert_True(retVal == PSP_PING_INVALID_ARG_ERR, cStrTestDescription);

    /* ----- Test case #4 - Unknown Host ----- */
    

    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = (in_addr_t)(-1);

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.000", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 4/13: Unknown Host", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_SinglePing() - Unknown host 192.168.22.000\n", MAX_OS_PRINTF_MESSAGES);

    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_UNKNOWN_HOST_ERR, cStrTestDescription);

    /* ----- Test case #5 - Failed to create socket ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, -1);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 5/13: Failed to Create Socket", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_SinglePing() - Error creating socket\n", MAX_OS_PRINTF_MESSAGES);

    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_SOCKET_ERR, cStrTestDescription);


    /* ----- Test case #6 - Failed to Generate ICMP Packet ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 6/13: Failed to Generate ICMP Packet", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_SinglePing() - Failed to create the packet\n", MAX_OS_PRINTF_MESSAGES);

    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_CRT_PKT_ERR, cStrTestDescription);

    /* ----- Test case #7 - Failed to Send ICMP Packet - Error at sendto ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(sendto), 1, -1);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 7/13: Failed to Send ICMP Packet - Error at sendto", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_SinglePing() - Unable to send ICMP packet to socket. Return Value of -1.\n", MAX_OS_PRINTF_MESSAGES);

    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_SND_PKT_ERR, cStrTestDescription);

    /* ----- Test case #8 - Failed to Send ICMP Packet - Different packet size sent out ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(sendto), 1, 85);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 8/13: Failed to Send ICMP Packet - Different packet size sent out", sizeof(cStrTestDescription));
    strncpy(cStrOSprintfOutput, "CFE_PSP_SinglePing() - Something is wrong, sent 85 bytes instead 64 bytes\n", MAX_OS_PRINTF_MESSAGES);

    UtAssert_OS_print(cStrOSprintfOutput, cStrTestDescription);
    UtAssert_True(retVal == PSP_PING_SND_PKT_ERR, cStrTestDescription);

    /* ----- Test case #9 - Select Call Fails ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(sendto), 1, 64);
    UT_SetDeferredRetcode(UT_KEY(select), 1, -1);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 9/13: Select call failed", MAX_OS_PRINTF_MESSAGES);
    UtAssert_True(retVal == PSP_PING_SELECT_ERR, cStrTestDescription);

    /* ----- Test case #10 - Empty Packets ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(sendto), 1, 64);
    UT_SetDeferredRetcode(UT_KEY(select), 5000, 0);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);
    
    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 10/13: Empty Packets", MAX_OS_PRINTF_MESSAGES);
    UtAssert_True(retVal == PSP_PING_TIMEOUT_ERR, cStrTestDescription);

    /* ----- Test case #11 - RecvFrom Fails its call ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(sendto), 1, 64);
    UT_SetDeferredRetcode(UT_KEY(select), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(recvfrom), 1, -1);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);

    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 11/13: RecvFrom Fails its call", MAX_OS_PRINTF_MESSAGES);
    UtAssert_True(retVal == PSP_PING_RCV_PKT_ERR, cStrTestDescription);

    /* ----- Test case #12 - Packet was ignored and timed out ----- */
    
    /* Test setups */
    UT_ResetState(0);
    Ut_OS_printf_Setup();
    destaddr = 0;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(clock_gettime), 1, OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(sendto), 1, 64);
    UT_SetDeferredRetcode(UT_KEY(select), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(recvfrom), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(select), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(recvfrom), 1, 100);
    UT_SetDeferredRetcode(UT_KEY(clock_gettime), 1, ERROR);
    UT_SetDeferredRetcode(UT_KEY(select), 5000, 0);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);

    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 12/13: Packet was ignored and timed out", MAX_OS_PRINTF_MESSAGES);
    UtAssert_True(retVal == PSP_PING_TIMEOUT_ERR, cStrTestDescription);

    /* ----- Test case #13 - Nominal ----- */
    
    /* Test setups */
    UT_ResetState(0);
    destaddr = 0;
    g_usPingIdentNum = 0;
    CheckTime.Seconds = 3;
    CheckTime.Subseconds = 1;
    getTimeReturnStruct.tv_sec = CFE_MISSION_TIME_EPOCH_UNIX_DIFF + 1;
    pIpReplyHdr = &inIcmpPkt;
    pIpReplyHdr->ip_hl = IP_HEADER_LEN >> 2;

    UT_SetDataBuffer(UT_KEY(inet_addr), &destaddr, sizeof(destaddr), false);
    UT_SetDeferredRetcode(UT_KEY(socket), 1, 1);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TIME_Micro2SubSecs), 1000);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(sendto), 1, 64);
    UT_SetDeferredRetcode(UT_KEY(select), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(recvfrom), 1, 84);
    UT_SetDataBuffer(UT_KEY(recvfrom), (void *)&inIcmpPkt, sizeof(inIcmpPkt), false);
    UT_SetDefaultReturnValue(UT_KEY(clock_gettime), OK);
    UT_SetDataBuffer(UT_KEY(clock_gettime), &getTimeReturnStruct, sizeof(getTimeReturnStruct), false);
    UT_SetDataBuffer(UT_KEY(CFE_TIME_Subtract), &CheckTime, sizeof(CheckTime), false);

    /* Execute test */
    retVal = CFE_PSP_SinglePing("192.168.22.129", 5000, &StatPacket);

    /* Verify Output */
    strncpy(cStrTestDescription, "_SinglePing() - 13/13: Nominal", MAX_OS_PRINTF_MESSAGES);
    UtAssert_True(retVal == PSP_PING_SUCCESS, cStrTestDescription);
    UtAssert_True(StatPacket.roundTripTime.Seconds == CheckTime.Seconds, cStrTestDescription);
    UtAssert_True(StatPacket.roundTripTime.Subseconds == CheckTime.Subseconds, cStrTestDescription);
    UtAssert_True(StatPacket.usType == ICMP_ECHOREPLY, cStrTestDescription);
    UtAssert_True(StatPacket.usType == 0, cStrTestDescription);
}