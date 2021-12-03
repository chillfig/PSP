/**
 ** \file  psp_cds_flash.h
 **
 ** \brief API header to save and restore CDS in FLASH memory
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
 ** This file contains the function prototypes relating to CDS flash memory.
 ** API header to save and restore CDS in FLASH memory.
 ** 
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

#ifndef _PSP_CDS_FLASH_H_
#define _PSP_CDS_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api PSP Public APIs
** \{
*/

/**
 ** \func Enable CDS sync to FLASH
 **
 ** \par Description:
 ** This function will enable CDS syncing to FLASH
 **
 ** \par Assumptions, External Events, and Notes:
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_MEMORY_SYNC_Enable(void);

/**
 ** \func Disable CDS sync to FLASH
 **
 ** \par Description:
 ** This function will disable CDS syncing to FLASH
 **
 ** \par Assumptions, External Events, and Notes:
 ** This will not cancel/stop any in-progress syncing
 **
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_MEMORY_SYNC_Disable(void);

/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_CDS_FLASH_H_ */
