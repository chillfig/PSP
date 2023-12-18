/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file psp_ping_testcases.h
*
*   \brief This header file contains function declarations for test cases for the cfe_psp_ping.c.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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
