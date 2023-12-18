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
*   \file cfe_psp_support.h
*
*   \brief API header for supporting various SP0(s) features
*
*   \brief Functions provides support to the PSP API for restarting the target and changing the boot parameters.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

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
 ** \param[out] pStartupBootString - Pointer to string where to save the boot string
 ** \param[in]  uiBufferSize - size of pStartupBootString array
 ** \param[in]  uiTalkative - If true, print out the boot parameter structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GetBootStartupString(char *pStartupBootString, uint32 uiBufferSize, uint32 uiTalkative);

/**
 ** \func Sets new boot startup string
 **
 ** \par Description:
 ** Function sets a new target boot startup string.
 **
 ** \par Assumptions, External Events, and Notes:
 ** \warning This function cannot be verified using bootChange() command on the shell.
 ** bootChange() command will remove the changes applied by the PSP API.
 ** 
 ** \param[in] pStartupBootString -Pointer to the filename with absolute path of the startup string
 ** \param[in] uiTalkative - If true, print out the boot parameter structure
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetBootStartupString(char *pStartupBootString, uint32 uiTalkative);

/**
 ** \func Prints the boot parameters to console
 **
 ** \par Description:
 ** Prints the boot parameters to console
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] pTargetBootParameters - Pointer to a BOOT_PARAMS structure 
 **
 ** \return None
 */
void CFE_PSP_PrintBootParameters(BOOT_PARAMS *pTargetBootParameters);

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
 ** \param[out] pTargetBootParameters - Pointer to a BOOT_PARAMS structure 
 ** \param[in]  uiTalkative - If true, print out debug messages
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_GetBootStructure(BOOT_PARAMS *pTargetBootParameters, uint32 uiTalkative);

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
 ** \param[in] NewBootParameters - New parameters
 ** \param[in] uiTalkative - If true, print out debug messages
 **
 ** \return #CFE_PSP_SUCCESS
 ** \return #CFE_PSP_ERROR
 */
int32 CFE_PSP_SetBootStructure(BOOT_PARAMS NewBootParameters, uint32 uiTalkative);

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
 ** \param[in] pCatalogEntryName - Name pointer of the catalog entry
 ** \param[in] bFirstCatalog - Get from first Catalog (TRUE == 1,  FALSE == 0)
 **
 ** \return 0 - When the Kernel is not found or bad inputs
 ** \return CRC - Checksum for catalog entry name 
 */
uint32 CFE_PSP_KernelGetCRC(char *pCatalogEntryName, bool bFirstCatalog);

/**
 ** \func Load new kernel on SP0 target
 **
 ** \par Description:
 ** This function programs a new kernel on the SP0 target.
 **
 ** \par Assumptions, External Events, and Notes:
 ** The kernel file is saved in a special location on the SP0 Flash memory.
 ** The function will set the new kernel file as the default kernel. 
 ** Validity of the kernel path and file structure is left to the end user.
 ** Name of the Kernel pKernelCatalogName must be no more than 8 characters.
 **
 ** \warning This function is a blocking function. All interrupts will be blocked
 ** for the duration of the function call; possibly for more than 20 seconds. 
 ** It is recommended to restart the target immediately after function completes.
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
