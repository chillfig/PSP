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

/* PSP coverage stub replacement for ipcom_err.h */
#ifndef PCS_IPCOM_ERR_H
#define PCS_IPCOM_ERR_H

#include "PCS_basetypes.h"

/* ----------------------------------------- */
/* constants normally defined in ipcom_err.h */
/* ----------------------------------------- */
#define PCS_IPCOM_SUCCESS                     0
#define PCS_IPCOM_ERR_FAILED              -1000
#define PCS_IPCOM_ERR_NO_MEMORY           -1010
#define PCS_IPCOM_ERR_NOT_FOUND           -1011
#define PCS_IPCOM_ERR_NOT_SUPPORTED       -1012
#define PCS_IPCOM_ERR_PERMISSION_DENIED   -1013
#define PCS_IPCOM_ERR_ALREADY_OPEN        -1040
#define PCS_IPCOM_ERR_ALREADY_CLOSED      -1041
#define PCS_IPCOM_ERR_ALREADY_INSTALLED   -1042
#define PCS_IPCOM_ERR_ALREADY_STARTED     -1043
#define PCS_IPCOM_ERR_NOT_OPENED          -1050
#define PCS_IPCOM_ERR_NOT_CLOSED          -1051
#define PCS_IPCOM_ERR_NOT_INSTALLED       -1052
#define PCS_IPCOM_ERR_NOT_STARTED         -1053
#define PCS_IPCOM_ERR_INVALID_ARG         -1060
#define PCS_IPCOM_ERR_INVALID_CONFIG      -1061
#define PCS_IPCOM_ERR_INVALID_VERSION     -1062
#define PCS_IPCOM_ERR_INVALID_ALIGNMENT   -1063
#define PCS_IPCOM_ERR_INVALID_INTERFACE   -1064
#define PCS_IPCOM_ERR_NO_ROUTE            -1070
#define PCS_IPCOM_ERR_DEAD_ROUTE          -1071
#define PCS_IPCOM_ERR_INVALID_ROUTE       -1072
#define PCS_IPCOM_ERR_INVALID_MASK        -1073
#define PCS_IPCOM_ERR_INVALID_RTCLIENT    -1074
#define PCS_IPCOM_ERR_DUPLICATE           -1080
#define PCS_IPCOM_ERR_LOCKED              -1081
#define PCS_IPCOM_ERR_END                 -1082
#define PCS_IPCOM_ERR_READONLY            -1083
#define PCS_IPCOM_ERR_INVALID_TAG_VALUE   -1090
#define PCS_IPCOM_ERR_UNSUPPORTED_TAG     -1091
#define PCS_IPCOM_ERR_INVALID_PACKET      -1100
#define PCS_IPCOM_ERR_BAD_PACKET          -1101
#define PCS_IPCOM_ERR_IGNORED_PACKET      -1102
#define PCS_IPCOM_ERR_SOCKCALLFAILED      -1110
#define PCS_IPCOM_ERR_REACHED_MIN         -1111
#define PCS_IPCOM_ERR_REACHED_MAX         -1112
#define PCS_IPCOM_ERR_TIMEOUT             -1120
#define PCS_IPCOM_ERR_WOULDBLOCK          -1121
#define PCS_IPCOM_ERR_INTERRUPT           -1122
#define PCS_IPCOM_ERR_PENDING             -1123
#define PCS_IPCOM_ERR_AUTH_FAILED         -1130
#define PCS_IPCOM_ERR_AUTH_UNKNOWN        -1131

/* ----------------------------------------- */
/* types normally defined in ipcom_err.h */
/* ----------------------------------------- */
typedef int PCS_Ip_err;

/* ----------------------------------------- */
/* prototypes normally declared in ipcom_err.h */
/* ----------------------------------------- */

#endif
