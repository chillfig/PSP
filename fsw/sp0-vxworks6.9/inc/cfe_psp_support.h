/**
 ** \file
 **
 ** \brief API header for supporting various SP0(s) features
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
 ** Functions provides support to the PSP API for restarting the target and
 ** changing the boot parameters.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
*/

#ifndef PSP_SUPPORT_H
#define PSP_SUPPORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*
**  Include Files
*/
#include <bootLib.h>

/**
** \addtogroup psp_public_api PSP Public APIs - Common
** \{
*/

/**
 ** \func Toggle among CFS partitions
 **
 ** \par Description:
 ** Function change the boot startup string with the next available CFS partition
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param None
 **
 ** \return None
 */
void CFE_PSP_ToggleCFSBootPartition(void);

/**
 ** \func Gets current boot startup string
 **
 ** \par Description:
 ** Function gets the current target boot startup string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param[out] startupBootString - Pointer to string where to save the boot string
 ** \param[in]  bufferSize - size of startupBootString array
 ** \param[in]  talkative - If true, print out the boot parameter structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GetBootStartupString(char *startupBootString, uint32 bufferSize, uint32 talkative);

/**
 ** \func Sets new boot startup string
 **
 ** \par Description:
 ** Function sets a new target boot startup string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param[in] startupScriptPath
 ** \param[in] talkative - If true, print out the boot parameter structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetBootStartupString(char *startupScriptPath, uint32 talkative);

/**
 ** \func Prints the boot parameters to console
 **
 ** \par Description:
 ** Prints the boot parameters to console
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] target_boot_parameters 
 **
 ** \return None
 */
void CFE_PSP_PrintBootParameters(BOOT_PARAMS *target_boot_parameters);

/**
 ** \func Gets current boot parameter structure
 **
 ** \par Description:
 ** Function gets the target boot string and converts it to
 ** a BOOT_PARAM structure.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param[out] target_boot_parameters
 ** \param[in]  talkative - If true, print out debug messages
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GetBootStructure(BOOT_PARAMS *target_boot_parameters, uint32 talkative);

/**
 ** \func Sets new boot parameter structure
 **
 ** \par Description:
 ** Function gets the target boot string and converts it to
 ** a BOOT_PARAM structure.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 ** 
 ** \param[in] new_boot_parameters
 ** \param[in]  talkative - If true, print out debug messages
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetBootStructure(BOOT_PARAMS new_boot_parameters, uint32 talkative);

/**
 ** \func Get CRC of Kernel in Catalog
 **
 ** \par Description:
 ** Function gets the CRC of the catalog entry name.
 **
 ** \par Assumptions, External Events, and Notes:
 ** On a SP0 there are 2 catalogs. If there are no errors, the first catalog is 
 ** exactly the same as the second catalog.
 ** 
 ** \param[in] pCatalogEntryName - Name of the catalog entry
 ** \param[in] bFirstCatalog - Catalog number (TRUE == 1,  FALSE == 2)
 **
 ** \return 0 - When the Kernel is not found or bad inputs
 ** \return CRC
 */
uint32 CFE_PSP_KernelGetCRC(char *pCatalogEntryName, bool bFirstCatalog);

/**
 ** \func Load new kernel on SP0 target
 **
 ** \par Description:
 ** This function programs a new kernel on the SP0 target.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The kernel file is saved in a special location on the SP0 Flash memory. This
 ** function is a blocking function and thus, it will slow down cFS. It is also 
 ** recommended to restart the target immediately.
 ** The function will set the new kernel file as the default kernel.
 ** Validity of the kernel path and file structure is left to the end user.
 ** Name of the Kernel pKernelCatalogName must be no more than 8 characters.
 ** 
 ** \param[in] pKernelPath - Kernel file path
 ** \param[in] pKernelCatalogName - Name of the kernel
 **
 ** \return #CFE_PSP_SUCCESS - When the kernel is successfully loaded
 ** \return #CFE_PSP_ERROR - When the kernel could not be loaded
 */

int32 CFE_PSP_KernelLoadNew(char *pKernelPath, char *pKernelCatalogName);

/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_SUPPORT_H_ */
