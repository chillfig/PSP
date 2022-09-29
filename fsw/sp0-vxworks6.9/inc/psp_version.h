/** 
 ** \file
 **
 ** \brief API header to obtain the values of the various version identifiers
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
 ** Provide version identifiers for the cFE Platform Support Packages (PSP).
 ** See #cfsversions for version and build number and description**  GSC-18128-1, "Core Flight Executive Version 6.7"
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */


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
#define CFE_PSP_IMPL_BUILD_NUMBER  41
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
