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

/* PSP coverage stub replacement for cfe_psp_sp0.h */
#include <string.h>
#include <stdlib.h>
#include "utstubs.h"

#include "PCS_cfe_psp_sp0.h"

static uint32_t PCS_SP0_BOOT_ROM_STATUS_ADDR_VALUE;

uint32_t *PCS_SP0_BOOT_ROM_STATUS_ADDR = &PCS_SP0_BOOT_ROM_STATUS_ADDR_VALUE;

/* default values */
uint32_t PCS_SP0_ROM1_CODE_LOCK = 0x000000A1;
uint32_t PCS_SP0_ROM1_CODE_UNLOCK = 0x000000A3;
uint32_t PCS_SP0_ROM2_CODE_LOCK = 0x000000B1;
uint32_t PCS_SP0_ROM2_CODE_UNLOCK = 0x000000B3;
uint32_t PCS_SP0_ROM1_MASK = 0x00000100;
uint32_t PCS_SP0_ROM2_MASK = 0x00000200;
uint32_t PCS_SP0_ROM1_STATUS_SHIFT = 8;
uint32_t PCS_SP0_ROM2_STATUS_SHIFT = 9;
