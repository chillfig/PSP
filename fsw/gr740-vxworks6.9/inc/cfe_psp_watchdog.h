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
*   \file cfe_psp_watchdog.h
*
*   \brief psp watchdog headers for GR740
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef CFE_PSP_WATCHDOG_H
#define CFE_PSP_WATCHDOG_H

#include "cfe_psp_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WD_TIMER_ID         (4)
#define CFE_PSP_WATCHDOG_MIN_MS (CFE_PSP_WATCHDOG_MIN / CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC)
#define CFE_PSP_WATCHDOG_MAX_MS (CFE_PSP_WATCHDOG_MAX / CFE_PSP_WATCHDOG_CTR_TICKS_PER_MILLISEC)

void CFE_PSP_WatchdogInit(void);
void CFE_PSP_WatchdogEnable(void);
void CFE_PSP_WatchdogDisable(void);
void CFE_PSP_WatchdogService(void);
uint32 CFE_PSP_WatchdogGet(void);
void CFE_PSP_WatchdogSet(uint32 watchDogValue_ms);
bool CFE_PSP_WatchdogStatus(void);

#ifdef __cplusplus
}
#endif

#endif
