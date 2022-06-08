/**
 ** \file
 **
 ** \brief API header for collecting SP0(s) hardware and software information
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
 ** Functions here allow PSP to load and save exceptions from EEPROM.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

#ifndef PSP_EXCEPTIONS_H
#define PSP_EXCEPTIONS_H

#include "common_types.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
** \{
*/

/**
 ** \func Load data from NVRAM
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will first verify that the EEPROM user area contains a valid,
 ** signature, then recover it.
 ** Data is saved to NVRAM (EEPROM) using #CFE_PSP_SaveToNVRAM.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the onboard EEPROM user area with maximum usable
 ** space of 20 kB. Read and Write access is slow.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_LoadFromNVRAM(void);

/**
 ** \func Save data to NVRAM
 ** 
 ** \par Description:
 ** During CFS startup, the ED&R and Memory Boot data in RAM is cleared.
 ** This function will save the ED&R and Memory Boot data to the NVRAM user area.
 ** It will also save a simple signature to allow the #CFE_PSP_LoadFromNVRAM to
 ** validate the presence of data.
 **
 ** \par Assumptions, External Events, and Notes:
 ** This function makes use of the onboard EEPROM user area with maximum usable
 ** space of 20 kB. Read and Write access is slow.
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_SaveToNVRAM(void);

/**
 ** \func Clear NVRAM
 ** 
 ** \par Description:
 ** Function reset to \0 the signature in NVRAM.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param None
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32   CFE_PSP_ClearNVRAM(void);

/**
** \} <!-- End of group "psp_public_api_sp0vx69" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_EXCEPTIONS_H_ */

