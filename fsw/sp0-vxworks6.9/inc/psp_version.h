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
*   \file psp_version.h
*
*   \brief This header file contains the values of the various version identifiers.
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
** \addtogroup psp_public_api_sp0vx69 PSP Public APIs - SP0-VxWorks6.9 Platform
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
#define CFE_PSP_IMPL_MINOR_VERSION 10
/** \brief ONLY APPLY for OFFICIAL releases. Revision number. */
#define CFE_PSP_IMPL_REVISION      0

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
