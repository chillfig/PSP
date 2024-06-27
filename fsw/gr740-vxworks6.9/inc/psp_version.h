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
*   \file psp_version.h
*
*   \brief API header to obtain the values of the various version identifiers
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#ifndef PSP_VERSION_H
#define PSP_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_public_api_gr740vx69 PSP Public APIs - GR740-VxWorks6.9 Platform
** \{
*/

/** \name Version Macro Definitions */
/** \{ */
/** \brief Development Build Macro Definitions - Build Number */
#define CFE_PSP_IMPL_BUILD_NUMBER  47
/** \brief Development Build Macro Definitions - Baseline */
#define CFE_PSP_IMPL_BUILD_BASELINE "v1.6.0-rc4"
/** \brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define CFE_PSP_IMPL_MAJOR_VERSION 1
/** \brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define CFE_PSP_IMPL_MINOR_VERSION 6 
/** \brief ONLY APPLY for OFFICIAL releases. Revision number. */
#define CFE_PSP_IMPL_REVISION      2

/**
 ** \brief Mission revision.
 **
 ** Reserved for mission use to denote patches/customizations as needed.
 ** Values 1-254 are reserved for mission use to denote patches/customizations as needed. NOTE: Reserving 0 and 0xFF for
 ** cFS open-source development use (pending resolution of nasa/cFS#440)
 */
#define CFE_PSP_IMPL_MISSION_REV   0x01
/** \brief ONLY APPLY for OFFICIAL releases. Codename */
#define CFE_PSP_IMPL_CODENAME "Draco"
/** \} */

/** \name Tools to construct version string */
/** \{ */
/** \brief Helper function to concatenate strings from integer */
#define CFE_PSP_IMPL_STR_HELPER(x) #x 
/** \brief Helper function to concatenate strings from integer */
#define CFE_PSP_IMPL_STR(x) CFE_PSP_IMPL_STR_HELPER(x) 

/** \brief DEVELOPMENT Build Version Number.
 ** \details Baseline git tag + Number of commits since baseline. @n
 ** See @ref cfsversions for format differences between development and release versions.
 */
#define CFE_PSP_IMPL_VERSION CFE_PSP_IMPL_BUILD_BASELINE "+dev" CFE_PSP_IMPL_STR(CFE_PSP_IMPL_BUILD_NUMBER)

/** \brief DEVELOPMENT Build Version String.
 **  \details Reports the current development build's baseline, number, and name. Also includes a note about the latest
 ** official version. @n See @ref cfsversions for format differences between development and release versions.
 */
#define CFE_PSP_IMPL_VERSION_STRING                                                       \
    " PSP Development Build " CFE_PSP_IMPL_VERSION /* Codename for current development */ \
    ", Last Official Release: psp v1.4.0"          /* For full support please use this version */
/** \} */


/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERSION_H */
