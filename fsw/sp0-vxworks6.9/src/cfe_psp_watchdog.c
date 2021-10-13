/**
 ** \file: cfe_psp_watchdog.c
 **
 ** \brief API to support Watchdog
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** API to enable/disable, and control FPGA watchdog
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** The FPGA watchdog timer has a counter with a tick precision of about 48 nano-seconds
 **
 */

/*
**  Include Files
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Aitech BSP Specific */
#include <sysApi.h>

#include "common_types.h"
#include "osapi.h"

#include "cfe_psp.h"
#include "cfe_psp_config.h"

/*
** Global Variables
*/
/** \brief Watchdog current millisecond value */
static uint32 g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_DEFAULT_MSEC;  /* Watchdog time in msecs */


/**
** \func Initialize the watchdog timer
**
** \par Description:
** This function configures and initializes the watchdog timer to its 
** default setting.
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
    g_uiCFE_PSP_WatchdogValue_ms = CFE_PSP_WATCHDOG_DEFAULT_MSEC;

    CFE_PSP_WatchdogSet(g_uiCFE_PSP_WatchdogValue_ms);  /* in msecs */
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
    int ret_code = OK;

    /*
    ** TRUE (allows slave SBC's WDT failure to reset all SBCs)
    ** FALSE (slave SBC's WDT failure resets slave SBC only)
    */
    ret_code = sysEnableFpgaWdt(true);
    if (ret_code != OK)
    {
        OS_printf("PSP Watchdog: Could not enable FPGA Watchdog\n");
    }
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
    int ret_code = OK;

    ret_code = sysDisableFpgaWdt();

    if (ret_code != OK)
    {
        OS_printf("PSP Watchdog: Could not disable FPGA Watchdog\n");
    }
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
    return g_uiCFE_PSP_WatchdogValue_ms;
}


/**
** \func Set the watchdog time
**
** \par Description:
** This function sets the current watchdog time, in milliseconds.
**
** \par Assumptions, External Events, and Notes:
** Although the WatchDog can be set to nano-seconds precision, the implementation
** only allows milliseconds precision.
**
** \param[in] watchDogValue_ms - watchdog time in milliseconds
**
** \return None
*/
void CFE_PSP_WatchdogSet(uint32 watchDogValue_ms)
{
    int   ret_code = OK;
    float fRate = 0.0f;

    g_uiCFE_PSP_WatchdogValue_ms = watchDogValue_ms;  /* input already in msecs */

    /*Rate is in seconds*/
    fRate = (((float)watchDogValue_ms) * 0.001f);

    ret_code = sysSetFpgaWdt(fRate);

    if (ret_code != OK)
    {
        OS_printf("PSP Watchdog: Could not set FPGA Watchdog rate\n");
    }
}


