/**
 ** \file: cfe_psp_watchdog.c
 **
 ** \brief API to support Watchdog
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved. \n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that 
 ** agreement.
 **
 ** \par Description:
 ** API to enable/disable, and control watchdog
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "common_types.h"
#include "osapi.h"
#include "cfe_psp.h"
#include "sysApi.h"


/*
** Global Variables
*/
/** \brief Watchdog current millisecond value */
uint32 CFE_PSP_WatchdogValue = 20000;  /* Watchdog time in msecs */


/**
** \func Initialize the watchdog timer
**
** \par Description:
** This function configures and intializes the watchdog timer.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void CFE_PSP_WatchdogInit(void)
{
	CFE_PSP_WatchdogSet(CFE_PSP_WatchdogValue);  /* in msecs */
}


/**
** \func Enable the watchdog timer
**
** \par Description:
** This function enables the watchdog timer.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void CFE_PSP_WatchdogEnable(void)
{
	sysEnableFpgaWdt(1);
}


/**
** \func Disable the watchdog timer
**
** \par Description:
** This function disables the watchdog timer.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void CFE_PSP_WatchdogDisable(void)
{
	sysDisableFpgaWdt();
}


/**
** \func Service the watchdog timer
**
** \par Description:
** This function services the watchdog timer according to the value set in CFE_PSP_WatchdogSet().
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return None
*/
void CFE_PSP_WatchdogService(void)
{
	/*
     * This function toggles the watchdog timer and it must be called at least
     * once every watchdog timer period if the watchdog timer is enabled.
     * Otherwise, the board will perform a reset once the timer expires.
     */
	sysPulseFpgaWdt();
}


/**
** \func Get the watchdog time
**
** \par Description:
** This function fetches the watchdog time, in milliseconds.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param None
**
** \return The watchdog time in milliseconds
*/
uint32 CFE_PSP_WatchdogGet(void)
{
   return((uint32)CFE_PSP_WatchdogValue);
}


/**
** \func Set the watchdog time
**
** \par Description:
** This function sets the current watchdog time, in milliseconds.
**
** \par Assumptions, External Events, and Notes:
** None
**
** \param[in] watchDogValue - watchdog time in milliseconds
**
** \return None
*/
void CFE_PSP_WatchdogSet(uint32 watchDogValue)
{
    float rate = 0.0;

    CFE_PSP_WatchdogValue = watchDogValue / 1000U;  /* in msecs */

    /*Rate is in seconds*/
    rate = (((float)watchDogValue) * 0.001F);
    sysSetFpgaWdt(rate);
}


