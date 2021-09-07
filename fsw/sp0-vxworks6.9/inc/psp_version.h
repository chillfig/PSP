/** 
 ** \file psp_version.h
 **
 ** \brief API header to obtain the values of the various version identifiers
 **
 ** \copyright
 ** Copyright 2016-2019 United States Government as represented by the 
 ** Administrator of the National Aeronautics and Space Administration. 
 ** All Other Rights Reserved.\n
 ** This software was created at NASA's Johnson Space Center.
 ** This software is governed by the NASA Open Source Agreement and may be 
 ** used, distributed and modified only pursuant to the terms of that agreement.
 **
 ** \par Description:
 ** Provide version identifiers for the cFE Platform Support Packages (PSP).
 ** See cfsversions for version and build number and description**  GSC-18128-1, "Core Flight Executive Version 6.7"
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */


#ifndef _PSP_VERSION_H_
#define _PSP_VERSION_

#ifdef __cplusplus
extern "C" {
#endif

/** \name Version Macro Definitions */
/** \{ */
/** \brief Development Build Macro Definitions - Build Number */
#define CFE_PSP_IMPL_BUILD_NUMBER   112
/** \brief Development Build Macro Definitions - Baseline */
#define CFE_PSP_IMPL_BUILD_BASELINE "v1.5.0-rc1"
/** \brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define CFE_PSP_IMPL_MAJOR_VERSION 1
/** \brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define CFE_PSP_IMPL_MINOR_VERSION 4 
/** \brief ONLY APPLY for OFFICIAL releases. Revision number. */
#define CFE_PSP_IMPL_REVISION      0 
/** \brief ONLY APPLY for OFFICIAL releases.
 ** Revision version number. A value of "99" indicates an unreleased development version.
 */
#define CFE_PSP_IMPL_MISSION_REV   99
/** \brief ONLY APPLY for OFFICIAL releases. Codename */
#define CFE_PSP_IMPL_CODENAME "Bootes"
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

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERSION_H */
