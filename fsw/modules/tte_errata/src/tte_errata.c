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
*   \file tte_errata.c
*
*   \brief API for resolving TTE erratas
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

// UndCC_NextLine(SSET_007, SSET_056) - This macro is part of cFS and cannot be changed
CFE_PSP_MODULE_DECLARE_SIMPLE(tte_errata);

/**
 ** \brief Initialize TTE Errata API
 **
 ** \par Description:
 ** Initialize the TTE Errata module. Function will look for the TTE device
 ** specified by #TTE_ERRATA_VENDOR_ID and #TTE_ERRATA_DEVICE_ID. When it finds
 ** it, it reads the Detected Parity Error bit from the PCI Bus Status.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] uiPspModuleId - Not used
 **
 ** \return None
 */
// UndCC_NextLine(SSET_106) Func. name part of PSP API, cannot change
void tte_errata_Init(uint32 uiPspModuleId)
{
    int iVendorId = TTE_ERRATA_VENDOR_ID;
    int iDevId = TTE_ERRATA_DEVICE_ID;
    uint32 uiDpeBit = 0;

    /*
    Search for device in active PCI.
    The active PCI is set by the kernel in usrAppInit.c
    */
    if (pciFindDevice(iVendorId, iDevId, 0, &g_iBusNum, &g_iDevNum, &g_iFuncNum ) == OK)
    {
        /* If the TTE device is found, get PCI Detected Parity Error bit value */
        if (CFE_PSP_GetPCIDpeBit(&uiDpeBit) == CFE_PSP_ERROR)
        {
            OS_printf("TTE Errata: Device found, but unknown error getting DPE bit\n");
        }
        else
        {
            OS_printf("TTE Errata: Initialized Successfully Bus #: %d Dev #: %d Func #: %d\n", 
                      g_iBusNum, g_iDevNum, g_iFuncNum);
        }
    }
    else
    {
        OS_printf("TTE Errata: Device not found\n");
    }
    //UndCC_Line(SSET_102)
}
