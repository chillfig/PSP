/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file tte_errata_stubs.c
*
*   \brief This file contains TTE Errata stub functions.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

#include <vxWorks.h>
#include "utstubs.h"
#include "utgenstub.h"

int pciConfigInWord(int busNo, int deviceNo, int funcNo, int offset, uint16*pData);
int pciFindDevice(int vendorId, int deviceId, int index, int*pBusNo, int*pDevNo, int*pFuncNo);


int pciConfigInWord(int busNo, int deviceNo, int funcNo, int offset, uint16*pData)
{
    int iRetCode = OK;

    iRetCode = UT_DEFAULT_IMPL(pciConfigInWord);

    UT_Stub_CopyToLocal(UT_KEY(pciConfigInWord), (uint16 *)pData, sizeof(*pData));

    return iRetCode;
}

int pciFindDevice(int vendorId, int deviceId, int index, int*pBusNo, int*pDevNo, int*pFuncNo)
{
    int iRetCode = OK;

    iRetCode = UT_DEFAULT_IMPL(pciFindDevice);

    return iRetCode;
}
