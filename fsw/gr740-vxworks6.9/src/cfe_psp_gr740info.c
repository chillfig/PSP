/**
 ** \file
 **
 ** \brief API for collecting GR740 hardware and software information
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
 ** Functions here allow CFS to provide a method to probe various board
 ** hardware and software information from POST, Temperatures, Voltages, Active 
 ** Boot EEPROM, etc. In addition, this module has a function to save a 
 ** dump_core text file before aborting CFS execution. 
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

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

