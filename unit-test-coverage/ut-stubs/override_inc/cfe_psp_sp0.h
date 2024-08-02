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

/* PSP coverage stub replacement for sp0.h */
#ifndef OVERRIDE_CFE_PSP_SP0_H
#define OVERRIDE_CFE_PSP_SP0_H

#include "PCS_cfe_psp_sp0.h"

/* ---------------------------------------------------- */
/* constants that need to be overwritten for unit tests */
/* ---------------------------------------------------- */
#define SP0_BOOT_ROM_STATUS_ADDR PCS_SP0_BOOT_ROM_STATUS_ADDR
#define SP0_ROM1_MASK PCS_SP0_ROM1_MASK
#define SP0_ROM2_MASK PCS_SP0_ROM2_MASK
#define SP0_ROM1_STATUS_SHIFT PCS_SP0_ROM1_STATUS_SHIFT
#define SP0_ROM2_STATUS_SHIFT PCS_SP0_ROM2_STATUS_SHIFT

/* ---------------------------------------------------- */
/* constants that need specific values for unit tests   */
/* ---------------------------------------------------- */
/** \brief ROM1 LOCK Code */
#define SP0_ROM1_CODE_LOCK                  0x000001A1
/** \brief ROM1 UNLOCK Code */
#define SP0_ROM1_CODE_UNLOCK                0x000000A3
/** \brief ROM2 LOCK Code */
#define SP0_ROM2_CODE_LOCK                  0x000002B1
/** \brief ROM2 UNLOCK Code */
#define SP0_ROM2_CODE_UNLOCK                0x000000B3

/* ---------------------------------------------------- */
/* constants that need do not need to be overwritten    */
/* ---------------------------------------------------- */
/** \brief SP0s Maximum Number of Voltage Sensors */
#define SP0_UPGRADE_MAX_VOLT_SENSORS    6
/** \brief SP0 Maximum Number of Voltage Sensors */
#define SP0_ORIGINAL_MAX_VOLT_SENSORS   0
/** \brief SP0s Maximum Number of Temperature Sensors */
#define SP0_UPGRADE_MAX_TEMP_SENSORS    4
/** \brief SP0 Maximum Number of Temperature Sensors */
#define SP0_ORIGINAL_MAX_TEMP_SENSORS   3

#endif
