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
#include <cfe_psp_gr740.h>
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
bool g_bTimerInitialized = false;


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

    if ((Tbu == NULL) || (Tbl == NULL))
    {
        OS_printf(TIMER_PRINT_SCOPE "Input variables are NULL\n");
        goto vxTimeBaseGet_Exit_Tag;
    }

    if (g_bTimerInitialized != true)
    {
        OS_printf(TIMER_PRINT_SCOPE "Timer is not initialized\n");
        goto vxTimeBaseGet_Exit_Tag;
    }

    /*
     ** We don't want an interrupt to occur when the time requested.
     **
     ** sysTimestampLock() automatically blocks interrupts during execution.
     **
     */
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

vxTimeBaseGet_Exit_Tag:
    return;
}