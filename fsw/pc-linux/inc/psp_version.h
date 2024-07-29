/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/
 
/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/*! @file
 *  @brief Purpose:
 *  @details Provide version identifiers for the cFE Platform Support Packages (PSP).
 *  See @ref cfsversions for version and build number and description
 */
#ifndef PSP_VERSION_H
#define PSP_VERSION_H

/*
 * Development Build Macro Definitions
 */
#define CFE_PSP_IMPL_BUILD_NUMBER   0  /** \brief Build Number - reserved for end user   */
#define CFE_PSP_IMPL_BUILD_BASELINE "" /** \brief Build Baseline is not used for gateway */

/*
 * Version Macros, see \ref cfsversions for definitions.
 */
#define CFE_PSP_IMPL_MAJOR_VERSION 1  /*!< @brief Major version number */
#define CFE_PSP_IMPL_MINOR_VERSION 5  /*!< @brief Minor version number */
#define CFE_PSP_IMPL_REVISION      99 /*!< @brief Revision version number. Value of 99 indicates a development version.*/

/*!
 * @brief Mission revision.
 *
 * Reserved for mission use to denote patches/customizations as needed.
 */
#define CFE_PSP_IMPL_MISSION_REV 0x01

/*! 
 * @brief Release Bundle name.
 * @details The parent bundle this version was released with.
 */
#define CFE_PSP_IMPL_CODENAME "gw_draco_v2.2" 

/*
 * Tools to construct version string
 */
#define CFE_PSP_IMPL_STR_HELPER(x) #x /*!< @brief Helper function to concatenate strings from integer */
#define CFE_PSP_IMPL_STR(x) \
    CFE_PSP_IMPL_STR_HELPER(x) /*!< @brief Helper function to concatenate strings from integer */

/*! 
 * @brief Version Number.
 */
#define CFE_PSP_IMPL_VERSION "v"CFE_PSP_IMPL_STR(CFE_PSP_IMPL_MAJOR_VERSION)"." \
    CFE_PSP_IMPL_STR(CFE_PSP_IMPL_MINOR_VERSION)"." \
    CFE_PSP_IMPL_STR(CFE_PSP_IMPL_REVISION)"." \
    CFE_PSP_IMPL_STR(CFE_PSP_IMPL_MISSION_REV) \

/** @brief Version String.
 ** @details Reports the current version number and bundle name.
 */
#define CFE_PSP_IMPL_VERSION_STRING CFE_PSP_IMPL_VERSION " " CFE_PSP_IMPL_CODENAME


#endif
