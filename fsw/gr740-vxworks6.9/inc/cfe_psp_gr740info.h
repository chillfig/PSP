/**
 ** \file
 **
 ** \brief API header for collecting GR740 hardware and software information
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
 **
*/

#ifndef PSP_GR740_INFO_H
#define PSP_GR740_INFO_H

#ifdef __cplusplus
extern "C" {
#endif


/**
** \addtogroup psp_public_api_gr740vx69 PSP Public APIs - GR740-VxWorks6.9 Platform
** \{
*/

int32 CFE_PSP_TempSensorInit(void);
int32 CFE_PSP_GetTemperatureAwait(uint32 *uiTemperature);

/**
** \} <!-- End of group "psp_public_api_gr740vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* PSP_GR740_INFO_H */

