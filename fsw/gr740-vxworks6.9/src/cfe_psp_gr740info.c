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
*   \file cfe_psp_gr740info.c
*
*   \brief API for collecting GR740 hardware and software information
*
*   \brief Functions here allow CFS to provide a method to probe various board hardware and software 
*   information from POST, Temperatures, Voltages, Active Boot EEPROM, etc. In addition, this module has
*   a function to save a dump_core text file before aborting CFS execution. 
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

/*
**  Include Files
*/

#include <stdio.h>
#include <ioLib.h>
#include <errno.h>
#include <vxWorks.h>
#include <float.h>
#include <stat.h>

/* GR740 Hardware Registers Definitions */
#include "cfe_psp_gr740.h"

/* Include PSP API */
#include "cfe_psp.h"
#include "cfe_psp_config.h"
#include "cfe_psp_gr740info.h"


int32 CFE_PSP_TempSensorInit(void)
{
    int32 iReturnCode = CFE_PSP_SUCCESS;

    /* Configure Sensor DIV, DCORRECT, CLKEN */
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_DIV, TEMPERATURE_CONTROL_DIV_DEFAULT);
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_DCORRECT, 0x03);
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_CLKEN, 1);

    /* Raise RSTN and PDN */
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_SRSTN, 1);
    SET_TEMPERATURE_CONTROL(TEMPERATURE_CONTROL_PDN, 1);
    /* Wait 1ms + 25 sensor clock cycles*/

    return iReturnCode;
}

int32 CFE_PSP_GetTemperatureAwait(uint32 *uiTemperature)
{
    int32 iReturnCode = CFE_PSP_ERROR;
    uint32 uiLocalTemp = 0;

    if (GET_TEMPERATURE_STATUS(TEMPERATURE_STATUS_UPD) == 1)
    {
        uiLocalTemp = (GET_TEMPERATURE_STATUS(TEMPERATURE_STATUS_DATA));
        memcpy(uiTemperature, &uiLocalTemp, sizeof(uint32));
        iReturnCode = CFE_PSP_SUCCESS;
    }

    return iReturnCode;
}

