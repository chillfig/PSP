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

/* PSP coverage stub replacement for gr740.h */
#ifndef PCS_GR740_H
#define PCS_GR740_H

#include "PCS_basetypes.h"

/* ----------------------------------------- */
/* constants normally defined in gr740.h */
/* ----------------------------------------- */
#ifndef __IO
/* Both read and write */
#define __IO volatile
#endif
#ifndef __I
/* Read-only */
#define __I  volatile const
#endif
#ifndef __O
/* Write-only */
#define __O  volatile
#endif
#ifndef __R
/* Reserved */
#define __R  volatile const
#endif

/* ----------------------------------------- */
/* types normally defined in gr740.h */
/* ----------------------------------------- */
typedef struct gprb_reg_tag
{
    __IO uint32_t ftmctrl;
    __IO uint32_t alt_func;
    __IO uint32_t lvds_mclk;
    __IO uint32_t pll_newcfg;
    __IO uint32_t pll_recfg;
    __IO uint32_t pll_curcfg;
    __IO uint32_t drv_str1;
    __IO uint32_t drv_str2;
    __IO uint32_t lockdown;
} GPRB_REG_T;

typedef struct timer_reg_tag
{
    __IO uint32_t counter;
    __IO uint32_t reload;
    __IO uint32_t control;
    __I uint32_t latch;
} TIMER_REG;

typedef struct sparc_leon4_timer_reg_tag
{
    __IO uint32_t scaler;
    __IO uint32_t scalerReload;
    __IO uint32_t config;
    __IO uint32_t latchConfig;
    TIMER_REG       timer[5];
} SPARC_LEON4_TIMER_REG_T;

typedef struct l4stat_reg_tag
{
    __IO uint32_t counter_value_register[16];
    __R uint32_t reserved_1[60];
    __IO uint32_t counter_control_register[16];
    __R uint32_t reserved_2[60];
    __IO uint32_t counter_max_latch_register[16];
} L4STAT_REG_T;

typedef struct sparc_leon4_memory_scrubber_reg_tag
{
    __IO uint32_t ahb_status;
    __I uint32_t ahb_failing_address;
    __IO uint32_t ahb_error_configuration;
    __R uint32_t reserved;
    __IO uint32_t status;
    /* Memory Scrubber Configuration */
    __IO uint32_t configuration;
    __IO uint32_t range_low_addr;     /* [4:0] bit are always 0 */
    __IO uint32_t range_high_addr;    /* [4:0] bit are always 1 */
    /* Scrubber position while active, otherwise all zeros */
    __IO uint32_t position;           /* [4:0] bit are always 0 */
    /* Configure thresholds to raise interrupts when number of block or run errors pass thresholds */
    __IO uint32_t error_threshold;
    /* TODO: Learn more. Data to use during the scrubber initialization */
    __IO uint32_t initialization_data;
    /* Currently second range is not used in CFS PSP */
    __IO uint32_t second_range_low_addr;
    __IO uint32_t second_range_high_addr;
} SPARC_LEON4_MEMORY_SCRUBBER_REG_T;

typedef struct temperature_reg_tag
{
    __IO uint32_t control_reg;
    __IO uint32_t status_reg;
    __IO uint32_t threshold_reg;
} TEMPERATURE_REG_T;

/* ----------------------------------------- */
/* prototypes normally declared in gr740.h */
/* ----------------------------------------- */

extern uint32_t *PCS_APBUART0_REG_ADDR;
extern uint32_t *PCS_APBUART1_REG_ADDR;
extern uint32_t *PCS_GRGPIO0_REG_ADDR;
extern uint32_t *PCS_FTMCTRL_REG_ADDR;
extern uint32_t *PCS_IRQAMP_REG_ADDR;
extern SPARC_LEON4_TIMER_REG_T *PCS_GPTIMER0_REG_ADDR;
extern SPARC_LEON4_TIMER_REG_T *PCS_GPTIMER1_REG_ADDR;
extern SPARC_LEON4_TIMER_REG_T *PCS_GPTIMER2_REG_ADDR;
extern SPARC_LEON4_TIMER_REG_T *PCS_GPTIMER3_REG_ADDR;
extern SPARC_LEON4_TIMER_REG_T *PCS_GPTIMER4_REG_ADDR;
extern uint32_t *PCS_GRSPWROUTER0_REG_ADDR;
extern uint32_t *PCS_GRSPWROUTER1_REG_ADDR;
extern uint32_t *PCS_GRSPWROUTER2_REG_ADDR;
extern uint32_t *PCS_GRSPWROUTER3_REG_ADDR;
extern uint32_t *PCS_GRETH_GBIT0_REG_ADDR;
extern uint32_t *PCS_GRETH_GBIT1_REG_ADDR;
extern uint32_t *PCS_APBBRIDGE0_REG_ADDR;
extern uint32_t *PCS_APBBRIDGE1_REG_ADDR;
extern SPARC_LEON4_MEMORY_SCRUBBER_REG_T *PCS_MEMSCRUB_REG_ADDR;
extern TEMPERATURE_REG_T *PCS_GR740THSEND_REG_ADDR;
extern GPRB_REG_T *PCS_GRGPRBANK_REG_ADDR;
extern L4STAT_REG_T *PCS_L4STAT_REG_ADDR;

#endif
