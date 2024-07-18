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
*   \file tte_errata.h
*
*   \brief Function declarations for tte_errata.c
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/**
 * \file
 *
 * Header for tte_errata.c
 */

#ifndef TTE_ERRATA_H_
#define TTE_ERRATA_H_


/********************************************************************
 * System Includes
 ********************************************************************/

#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_module.h"

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************
 * Global Variable
 ********************************************************************/

/*
Static variables are used to hold the bus, device, function numbers identifying
the specific TTE End System.
*/
static int g_iBusNum = 0;
static int g_iDevNum = 0;
static int g_iFuncNum = 0;

/********************************************************************
 * Local Function Prototypes
 ********************************************************************/

/* Get function to retrieve the Detected Parity Error */
int32 CFE_PSP_GetPCIDpeBit(uint32 *pDpeBit);

#ifdef __cplusplus
}
#endif

#endif /* TTE_ERRATA_H_ */
