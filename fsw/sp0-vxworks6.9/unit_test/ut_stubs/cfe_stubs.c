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
*   \file cfe_stubs.c
*
*   \brief This file contains cFE stub functions.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include "utstubs.h"
#include "utgenstub.h"

typedef struct CFE_TIME_SysTime
{
    uint32 Seconds;    /**< \brief Number of seconds since epoch */
    uint32 Subseconds; /**< \brief Number of subseconds since epoch (LSB = 2^(-32) seconds) */
} CFE_TIME_SysTime_t;

/*****************************************************************************/
/**
** \brief CFE_ES_WriteToSysLog stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_WriteToSysLog.  It always returns CFE_SUCCESS when called.
**        The log message is compared to known CFE function responses and if
**        a match is found the value WriteSysLogRtn.value is set to a
**        specified constant value.  WriteSysLogRtn.count is incremented to
**        keep track of the number of messages logged.  The unit test code
**        compares the WriteSysLogRtn value and count variables against
**        expected totals to ensure the proper response of other functions
**        being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \return
**        Returns CFE_SUCCESS.
**
******************************************************************************/

void UT_DefaultHandler_CFE_TIME_Subtract(void *, UT_EntryKey_t, const UT_StubContext_t *);

uint32 CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    int32 iVal;

    iVal = UT_DEFAULT_IMPL(CFE_TIME_Micro2SubSecs);
    
    return iVal;
}

CFE_TIME_SysTime_t CFE_TIME_Subtract(CFE_TIME_SysTime_t Time1, CFE_TIME_SysTime_t Time2)
{
    UT_GenStub_SetupReturnBuffer(CFE_TIME_Subtract, CFE_TIME_SysTime_t);

    UT_GenStub_AddParam(CFE_TIME_Subtract, CFE_TIME_SysTime_t, Time1);
    UT_GenStub_AddParam(CFE_TIME_Subtract, CFE_TIME_SysTime_t, Time2);

    UT_GenStub_Execute(CFE_TIME_Subtract, Basic, UT_DefaultHandler_CFE_TIME_Subtract);

    return UT_GenStub_GetReturnValue(CFE_TIME_Subtract, CFE_TIME_SysTime_t);
}

void UT_DefaultHandler_CFE_TIME_Subtract(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_Subtract), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}