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
*   \file cfe_psp_version.h
*
*   \brief API header to obtain the values of the various version identifiers
*
*   \copyright
*   Copyright (c) 2019-2021 United States Government as represented by
*   the Administrator of the National Aeronautics and Space Administration.
*   All Rights Reserved.
*   Unless required by applicable law or agreed to in writing, software
*   distributed under the License is distributed on an "AS IS" BASIS,
*   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*   See the License for the specific language governing permissions and
*   limitations under the License.
*
*   \par Description:
*   Provide version identifiers for the cFE Platform Support Packages (PSP).
*   See cfsversions for version and build number and description**  GSC-18128-1, "Core Flight Executive Version 6.7"
*
*   \par Limitations, Assumptions, External Events, and Notes:
*   None
*
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
/** \brief Build Number - reserved for end user */
#define CFE_PSP_IMPL_BUILD_NUMBER   0
/** \brief Build Baseline is not used for gateway */
#define CFE_PSP_IMPL_BUILD_BASELINE ""
/** \brief ONLY APPLY for OFFICIAL releases. Major version number. */
#define CFE_PSP_IMPL_MAJOR_VERSION 1
/** \brief ONLY APPLY for OFFICIAL releases. Minor version number. */
#define CFE_PSP_IMPL_MINOR_VERSION 6 
/** \brief ONLY APPLY for OFFICIAL releases. Revision number. */
#define CFE_PSP_IMPL_REVISION      0 

/**
 ** \brief Mission revision.
 **
 ** Reserved for mission use to denote patches/customizations as needed.
 */
#define CFE_PSP_IMPL_MISSION_REV   0

/**
 * \brief Release bundle codename 
 * \details The parent bundle this version was released with.
 */
#define CFE_PSP_IMPL_CODENAME "gw_draco_v2.2"
/** \} */

/** \name Tools to construct version string */
/** \{ */
/** \brief Helper function to concatenate strings from integer */
#define CFE_PSP_IMPL_STR_HELPER(x) #x 
/** \brief Helper function to concatenate strings from integer */
#define CFE_PSP_IMPL_STR(x) CFE_PSP_IMPL_STR_HELPER(x) 

/** 
 ** \brief Version Number.
 */
#define CFE_PSP_IMPL_VERSION "v"CFE_PSP_IMPL_STR(CFE_PSP_IMPL_MAJOR_VERSION)"." \
    CFE_PSP_IMPL_STR(CFE_PSP_IMPL_MINOR_VERSION)"." \
    CFE_PSP_IMPL_STR(CFE_PSP_IMPL_REVISION)"." \
    CFE_PSP_IMPL_STR(CFE_PSP_IMPL_MISSION_REV) \

/** \brief Version String.
 ** \details Reports the current development version number and bundle name.
 */
#define CFE_PSP_IMPL_VERSION_STRING CFE_PSP_IMPL_VERSION " " CFE_PSP_IMPL_CODENAME
/** \} */


/**
** \} <!-- End of group "psp_public_api" -->
*/

#ifdef __cplusplus
}
#endif

#endif /* _PSP_VERSION_H */
