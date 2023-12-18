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
*   \file cfe_psp_timer.c
*
*   \brief Contains glue routines between the cFE and the OS Board Support Package (BSP)
*
*   \brief The functions here allow the cFE to interface functions that are board and OS specific and 
*   usually do not fit well in the OS abstraction layer.
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
#include <stdlib.h>
#include <unistd.h>
#include <vxWorks.h>
#include "sysLib.h"

/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

#include <target_config.h>
#include "cfe_psp_gr740.h"
#include "cfe_psp_timer.h"

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"

/******************* Macro Definitions ***********************/

#define TS_TIMER_LOW_ID     (0)
#define TS_TIMER_HIGH_ID    (1)

/*
 ** local global variables for PSP Timer
 */
/* static void CFE_PSP_AuxClkHandler(int32 arg); */
static bool g_bTimerInitialized = false;


void CFE_PSP_InitLocalTime(void)
{
    if (g_bTimerInitialized == false)
    {
        TIMER1_REG->scaler                          = CFE_PSP_TIMER_25MHZ_SCALER;
        TIMER1_REG->scalerReload                    = CFE_PSP_TIMER_25MHZ_SCALER;
        TIMER1_REG->timer[TS_TIMER_LOW_ID].control  = 0x0;
        TIMER1_REG->timer[TS_TIMER_HIGH_ID].control = 0x0;
        TIMER1_REG->timer[TS_TIMER_LOW_ID].reload   = 0xFFFFFFFF;
        TIMER1_REG->timer[TS_TIMER_HIGH_ID].reload  = 0xFFFFFFFF;
        TIMER1_REG->timer[TS_TIMER_LOW_ID].control  = TIMER_CONTROL_LD | TIMER_CONTROL_RS | TIMER_CONTROL_EN;
        TIMER1_REG->timer[TS_TIMER_HIGH_ID].control =
            TIMER_CONTROL_CH | TIMER_CONTROL_LD | TIMER_CONTROL_RS | TIMER_CONTROL_EN;

        g_bTimerInitialized = true;
    }
    else
    {
        OS_printf(TIMER_PRINT_SCOPE "Decrementer timer already initialized\n");
    }

}/* end CFE_PSP_InitLocalTime */


void vxTimeBaseGet(uint32 *Tbu, uint32 *Tbl) //UndCC_Line(SSET106) Func. name part of the VxWorks API, cannot change
{
    uint32_t lower = 0;
    uint32_t upper0 = 0;
    uint32_t upper1 = 0;
    uint64_t time = 0;

    uint32_t uiRet_Code = CFE_PSP_SUCCESS;

    /* If any of the pointers provided are NULL, return immediately */
    if ((Tbu == NULL) || (Tbl == NULL))
    {
        uiRet_Code = CFE_PSP_ERROR;
        OS_printf(TIMER_PRINT_SCOPE "Input variables are NULL\n");
    }

    /* Check that the timer is initialized */
    if (g_bTimerInitialized != true)
    {
        uiRet_Code = CFE_PSP_ERROR;
        OS_printf(TIMER_PRINT_SCOPE "Timer is not initialized\n");
    }

    /*
     ** We don't want an interrupt to occur when the time requested.
     **
     ** sysTimestampLock() automatically blocks interrupts during execution.
     **
     */
    if (uiRet_Code == CFE_PSP_SUCCESS)
    {
        /*
         ** The resolution of our timer is given by:
         **   one timer tick = 1 / sysTimestampFreq() in seconds
         */

        upper0 = TIMER1_REG->timer[TS_TIMER_HIGH_ID].counter;
        lower  = TIMER1_REG->timer[TS_TIMER_LOW_ID].counter;
        upper1 = TIMER1_REG->timer[TS_TIMER_HIGH_ID].counter;

        if (upper0 != upper1)
        {
            lower = 0xFFFFFFFF; // lower rolled over before reading upper1
        }
        time = upper1;
        time = (time << 32) | lower;
        time = 0xFFFFFFFFFFFFFFFFULL - time;

        *Tbu = (time >> 32) & 0xFFFFFFFFU;
        *Tbl = time & 0xFFFFFFFFU;
    }
}

// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
// This function is for debug only.
// Remove this function after the CFE_PSP_Get_Timebase() function has been fully debugged.
// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
int DEBUG_GetTime(void)
{
    uint32 Tbu, Tbl;
    OS_time_t localTime;

    CFE_PSP_Get_Timebase(&Tbu, &Tbl);
    CFE_PSP_GetTime(&localTime);

    OS_printf("CFE_PSP_Get_Timebase(0x%08x, 0x%08x)\n", Tbu, Tbl);
    OS_printf("CFE_PSP_GetTime(%lld)\n", localTime.ticks);

    int64_t subsecondDiv = 10000000ULL;
    uint32_t seconds = localTime.ticks / subsecondDiv;
    uint32_t subsecs = localTime.ticks % subsecondDiv;

    OS_printf("%d sec, %d us\n", seconds, subsecs / 10U);

    return 0;
}
