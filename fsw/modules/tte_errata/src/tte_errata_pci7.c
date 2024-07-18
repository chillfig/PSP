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
*   \file tte_errata_pci7.c
*
*   \brief API for resolving TTE errata PCI7
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/************************************************************************
 * Includes
 ************************************************************************/

#include "tte_errata.h"

#include <vxWorks.h>
/* Defines pciFindDevice, pciConfigInWord, and PCI_CFG_STATUS */
#include <drv/pci/pciConfigLib.h>
#include <sys950Lib.h>

/*
If we are building for functional tests, add the VxWorks events
*/
#ifdef ENABLE_FUNCTIONAL_TESTS
#define WV_EVENT_BASE_NUM 110
#define WV_EVENT_START    WV_EVENT_BASE_NUM + 1
#define WV_EVENT_END      WV_EVENT_BASE_NUM + 2
#include "wvLib.h"
#endif

/* TTE End System Vendor ID */
#define TTE_ERRATA_VENDOR_ID 0x1c7e
/* TTE End System Device ID */
#define TTE_ERRATA_DEVICE_ID 0x012e

/********************************************************************
 * Local Function Prototypes
 ********************************************************************/


/********************************************************************
 * Global Variable
 ********************************************************************/

/*
Static variables are used to hold the bus, device, function numbers identifying
the specific TTE End System.
*/
extern int g_iBusNum;
extern int g_iDevNum;
extern int g_iFuncNum;

/***********************************************************************
 * Global Functions
 ********************************************************************/

/**
 ** \brief Get TTE End System PCI Detected Parity Error Bit
 **
 ** \par Description:
 ** Get the PCI Detected Parity Error bit off the TTE End System identified by
 ** busNum, devNum, funcNum.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in,out] pDpeBit - Pointer to Detected Parity Error variable
 **
 ** \return CFE_PSP_SUCCESS
 ** \return CFE_PSP_ERROR
 */
int32 CFE_PSP_GetPCIDpeBit(uint32 *pDpeBit)
{
    uint16 uiData = 0;
    int32 iRetCode = CFE_PSP_ERROR;

#ifdef ENABLE_FUNCTIONAL_TESTS
    wvEvent( WV_EVENT_START, NULL, 0 );
#endif

    /* Get PCI Detected Parity Error bit value */
    if (pciConfigInWord(g_iBusNum, g_iDevNum, g_iFuncNum, PCI_CFG_STATUS, &uiData) == OK)
    {
        /* DPE bit is number 15 */
        *pDpeBit = (uiData >> 15);

        iRetCode = CFE_PSP_SUCCESS;
    }

#ifdef ENABLE_FUNCTIONAL_TESTS
    wvEvent( WV_EVENT_END, NULL, 0 );
#endif

    return iRetCode;
}
