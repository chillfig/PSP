/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**
**      Copyright (c) 2004-2011, United States Government as represented by 
**      Administrator for The National Aeronautics and Space Administration. 
**      All Rights Reserved.
**
**      This is governed by the NASA Open Source Agreement and may be used,
**      distributed and modified only pursuant to the terms of that agreement. 
**
**
**
**  Purpose:
**  Provide version identifiers for the cFE Platform Support Packages (PSP).
**
*/

/*! @file mcp750-vxworks/inc/psp_version.h
 *  @brief Purpose: 
 *  @details Provide version identifiers for the cFE Platform Support Packages (PSP).   
 *  See @ref cfsversions for version and build number and description
 */
#ifndef _psp_version_
#define _psp_version_

/*
 * Development Build Macro Definitions 
 */
#define CFE_PSP_IMPL_BUILD_NUMBER 24
#define CFE_PSP_IMPL_BUILD_BASELINE "v1.5.0-rc1"

/*
 * Version Macro Definitions
 */
#define CFE_PSP_IMPL_MAJOR_VERSION 1 /*!< @brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define CFE_PSP_IMPL_MINOR_VERSION 4 /*!< @brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define CFE_PSP_IMPL_REVISION      14 /*!< @brief ONLY APPLY for OFFICIAL releases. Revision number. */
#define CFE_PSP_IMPL_MISSION_REV   0 /*!< @brief ONLY USED by MISSION Implementations. Mission revision */

/*
 * Tools to construct version string
 */ 
#define CFE_PSP_IMPL_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer */ 
#define CFE_PSP_IMPL_STR(x)        CFE_PSP_IMPL_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer */

/*! @brief Development Build Version Number. 
 *  @details Baseline git tag + Number of commits since baseline. @n
 *  See @ref cfsversions for format differences between development and release versions.
 */
#define CFE_PSP_IMPL_VERSION CFE_PSP_IMPL_BUILD_BASELINE CFE_PSP_IMPL_STR(CFE_PSP_IMPL_BUILD_NUMBER)

/*! @brief Development Build Version String.
 *  @details Reports the current development build's baseline, number, and name. Also includes a note about the latest official version. @n
 *  See @ref cfsversions for format differences between development and release versions. 
 */     
#define CFE_PSP_IMPL_VERSION_STRING                                                                      \
    " PSP Development Build " CFE_PSP_IMPL_VERSION                /* Codename for current development */ \
    ", Last Official Release: psp v1.4.0"                        /* For full support please use this version */

#endif  /* _psp_version_ */
