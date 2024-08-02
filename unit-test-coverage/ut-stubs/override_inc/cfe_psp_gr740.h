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

/* PSP coverage stub replacement for cfe_psp_gr740.h */
#ifndef OVERRIDE_CFE_PSP_GR740_H
#define OVERRIDE_CFE_PSP_GR740_H

#include "PCS_cfe_psp_gr740.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_gr740vx69_platcfg PSP Platform Configurations - GR740-VxWorks6.9
** \{
*/

/* APB Bridge 0 */
#define APBUART0_REG_ADDR       (PCS_APBUART0_REG_ADDR)  /* UART 0 registers Processor */
#define APBUART1_REG_ADDR       (PCS_APBUART1_REG_ADDR)  /* UART 1 registers Processor */
#define GRGPIO0_REG_ADDR        (PCS_GRGPIO0_REG_ADDR)  /* General purpose I/O port registers Processor */
#define FTMCTRL_REG_ADDR        (PCS_FTMCTRL_REG_ADDR)  /* PROM/IO controller registers Processor */
#define IRQAMP_REG_ADDR         (PCS_IRQAMP_REG_ADDR)  /* Interrupt controller registers Processor */
#define GPTIMER0_REG_ADDR       (PCS_GPTIMER0_REG_ADDR)  /* Timer unit 0 registers */
#define GPTIMER1_REG_ADDR       (PCS_GPTIMER1_REG_ADDR)  /* Timer unit 1 registers */
#define GPTIMER2_REG_ADDR       (PCS_GPTIMER2_REG_ADDR)  /* Timer unit 2 registers */
#define GPTIMER3_REG_ADDR       (PCS_GPTIMER3_REG_ADDR)  /* Timer unit 3 registers */
#define GPTIMER4_REG_ADDR       (PCS_GPTIMER4_REG_ADDR)  /* Timer unit 4 registers */
#define GRSPWROUTER0_REG_ADDR   (PCS_GRSPWROUTER0_REG_ADDR)  /* SpaceWire router AMBA interface 0 Processor */
#define GRSPWROUTER1_REG_ADDR   (PCS_GRSPWROUTER1_REG_ADDR)  /* SpaceWire router AMBA interface 1 Processor */
#define GRSPWROUTER2_REG_ADDR   (PCS_GRSPWROUTER2_REG_ADDR)  /* SpaceWire router AMBA interface 2 Processor */
#define GRSPWROUTER3_REG_ADDR   (PCS_GRSPWROUTER3_REG_ADDR)  /* SpaceWire router AMBA interface 3 Processor */
#define GRETH_GBIT0_REG_ADDR    (PCS_GRETH_GBIT0_REG_ADDR)  /* Gigabit Ethernet MAC 0 registers Processor */
#define GRETH_GBIT1_REG_ADDR    (PCS_GRETH_GBIT1_REG_ADDR)  /* Gigabit Ethernet MAC 1 registers Processor */
#define APBBRIDGE0_REG_ADDR     (PCS_APBBRIDGE0_REG_ADDR)  /* APB bus 0 plug&play area Processor */


/* APB Bridge 1 */
#define APBBRIDGE1_REG_ADDR     (PCS_APBBRIDGE1_REG_ADDR)  /* APB bridge 1 Processor */

#define MEMSCRUB_REG_ADDR       (PCS_MEMSCRUB_REG_ADDR)  /* Memory Scrubber Registers */
#define GR740THSEND_REG_ADDR    (PCS_GR740THSEND_REG_ADDR)  /* Temperature Sensor */
#define GRGPRBANK_REG_ADDR      (PCS_GRGPRBANK_REG_ADDR)  /* General Purpose Register Bank */
#define L4STAT_REG_ADDR         (PCS_L4STAT_REG_ADDR)  /* Leon 4 Statistics Unit */

#define GR740_SYSTEM_CLOCK      (250000000)

/*************** General Purpose Register Bank ***************/
#define GPRB_REG                      ((GPRB_REG_T *)GRGPRBANK_REG_ADDR)

/*************** STATISTICS UNIT ***************/

#define L4STAT_CONTROL_NCPU_MASK            (0xF0000000)
#define L4STAT_CONTROL_NCPU_POS             28U
#define L4STAT_CONTROL_NCNT_MASK            (0x0F800000)
#define L4STAT_CONTROL_NCNT_POS             23U
#define L4STAT_CONTROL_MC_MASK              (0x00400000)
#define L4STAT_CONTROL_MC_POS               22U
#define L4STAT_CONTROL_IA_MASK              (0x00200000)
#define L4STAT_CONTROL_IA_POS               21U
#define L4STAT_CONTROL_DS_MASK              (0x00100000)
#define L4STAT_CONTROL_DS_POS               20U
#define L4STAT_CONTROL_EE_MASK              (0x00080000)
#define L4STAT_CONTROL_EE_POS               19U
#define L4STAT_CONTROL_AE_MASK              (0x00040000)
#define L4STAT_CONTROL_AE_POS               18U
#define L4STAT_CONTROL_EL_MASK              (0x00020000)
#define L4STAT_CONTROL_EL_POS               17U
#define L4STAT_CONTROL_CD_MASK              (0x00010000)
#define L4STAT_CONTROL_CD_POS               16U
#define L4STAT_CONTROL_SU_MASK              (0x0000C000)
#define L4STAT_CONTROL_SU_POS               14U
#define L4STAT_CONTROL_CL_MASK              (0x00002000)
#define L4STAT_CONTROL_CL_POS               13U
#define L4STAT_CONTROL_EN_MASK              (0x00001000)
#define L4STAT_CONTROL_EN_POS               12U
#define L4STAT_CONTROL_EVENT_ID_MASK        (0x00000FF0)
#define L4STAT_CONTROL_EVENT_ID_POS         4U
#define L4STAT_CONTROL_CPU_AHBM_MASK        (0x0000000F)
#define L4STAT_CONTROL_CPU_AHBM_POS         0U

#define L4STAT_REG                      ((L4STAT_REG_T *)L4STAT_REG_ADDR)

/* MACRO */
#define SET_L4STAT_CONTROL(reg,value)   ((L4STAT_REG->counter_control_register)  |= (reg##_MASK & (value << (reg##_POS))))
#define GET_L4STAT_CONTROL(reg)         (((L4STAT_REG->counter_control_register) &= reg##_MASK) >> (reg##_POS))

/*************** TEMPERATURE SENSOR ***************/

#define TEMPERATURE_CONTROL_DIV_MASK        (0x03FF0000)
#define TEMPERATURE_CONTROL_DIV_POS         16U
#define TEMPERATURE_CONTROL_ALEN_MASK       (0x00000100)
#define TEMPERATURE_CONTROL_ALEN_POS        8U
#define TEMPERATURE_CONTROL_PDN_MASK        (0x00000080)
#define TEMPERATURE_CONTROL_PDN_POS         7U
#define TEMPERATURE_CONTROL_DCORRECT_MASK   (0x0000006C)
#define TEMPERATURE_CONTROL_DCORRECT_POS    2U
#define TEMPERATURE_CONTROL_SRSTN_MASK      (0x00000002)
#define TEMPERATURE_CONTROL_SRSTN_POS       1U
#define TEMPERATURE_CONTROL_CLKEN_MASK      (0x00000001)
#define TEMPERATURE_CONTROL_CLKEN_POS       0U

#define TEMPERATURE_STATUS_MAX_MASK         (0x7F000000)
#define TEMPERATURE_STATUS_MAX_POS          24U
#define TEMPERATURE_STATUS_MIN_MASK         (0x007F0000)
#define TEMPERATURE_STATUS_MIN_POS          16U
#define TEMPERATURE_STATUS_SCLK_MASK        (0x00008000)
#define TEMPERATURE_STATUS_SCLK_POS         15U
#define TEMPERATURE_STATUS_WE_MASK          (0x00000400)
#define TEMPERATURE_STATUS_WE_POS           10U
/* This bit is set to ’1’ whenever a new DATA value is read from the sensor. The bit
is reset to ’0’ when software reads this status register. */
#define TEMPERATURE_STATUS_UPD_MASK         (0x00000200)
#define TEMPERATURE_STATUS_UPD_POS          9U
#define TEMPERATURE_STATUS_ALACT_MASK       (0x00000100)
#define TEMPERATURE_STATUS_ALACT_POS        8U
#define TEMPERATURE_STATUS_DATA_MASK        (0x0000007F)
#define TEMPERATURE_STATUS_DATA_POS         0U

#define TEMPERATURE_THRESHOLD_THRS_MASK     (0x0000007F)
#define TEMPERATURE_THRESHOLD_THRS_POS      0U

#define TEMPERATURE_CONTROL_DIV_MIN     (GR740_SYSTEM_CLOCK/250000)
#define TEMPERATURE_CONTROL_DIV_DEFAULT (GR740_SYSTEM_CLOCK/400000)
#define TEMPERATURE_CONTROL_DIV_MAX     (GR740_SYSTEM_CLOCK/500000)

#define TEMPERATURE_REG                 ((TEMPERATURE_REG_T *)GR740THSEND_REG_ADDR)

/* MACRO */
#define SET_TEMPERATURE_CONTROL(reg,value)   ((TEMPERATURE_REG->control_reg) |= (reg##_MASK & (value << (reg##_POS))))
#define GET_TEMPERATURE_STATUS(reg)          (((TEMPERATURE_REG->status_reg) & reg##_MASK) >>  (reg##_POS))

/*************** TIMERS ***************/
/*
vxWorks kernel includes gptimer driver to automatically set the following:
- System Clock to Timer 2 Number 0
- Auxilary Clock to Timer 2 Number 1
- TimeStamp  to Timer 2 Number 2
with a scalar reload of 1000 --> 1MHz.
vxWorks kernel is configured to not use Timer 0 and 1. So below we only have control
on Timer 0 and 1.
*/

#define TIMER_SCALER_MASK       (0x0000FFFF)
#define TIMER_SCALER_POS        0U
#define TIMER_SRELOAD_MASK      (0x0000FFFF)
#define TIMER_SRELOAD_POS       0U

#define TIMER_CONTROL_CH    (0x00000020U) /* Enable Chaining */
#define TIMER_CONTROL_IP    (0x00000010U) /* Interrupt pending */
#define TIMER_CONTROL_IE    (0x00000008U) /* Interrupt Enable */
#define TIMER_CONTROL_LD    (0x00000004U) /* Load  */
#define TIMER_CONTROL_RS    (0x00000002U) /* Restart  */
#define TIMER_CONTROL_EN    (0x00000001U) /* Enable */

#define TIMER0_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER0_REG_ADDR)
#define TIMER1_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER1_REG_ADDR)
/*
#define TIMER2_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER2_REG_ADDR)
#define TIMER3_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER3_REG_ADDR)
#define TIMER4_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER4_REG_ADDR)
*/

/** \brief Timer Scaler to set timer to 1 Million ticks per second */
#define CFE_PSP_TIMER_1MHZ_SCALER           (uint32_t) GR740_SYSTEM_CLOCK/1000000
/** \brief Timer Scaler to set timer to 25 Million ticks per second  */
#define CFE_PSP_TIMER_25MHZ_SCALER          (uint32_t) GR740_SYSTEM_CLOCK/25000000

/*************** MEMORY SCRUBBER ***************/

/* AHB Status Register */
/*  */
#define MEMSCRUB_AHB_STATUS_CECNT_MASK   (0xFFC00000U) /* Delay in cycles (rw) */
#define MEMSCRUB_AHB_STATUS_CECNT_POS    22U
/*  */
#define MEMSCRUB_AHB_STATUS_UECNT_MASK   (0x002FC000U) /* Delay in cycles (rw) */
#define MEMSCRUB_AHB_STATUS_UECNT_POS    14U


/* Configuration */
#define MEMSCRUB_CONFIG_DELAY_BETWEEN_BLOCKS_MASK   (0x0000FF00U) /* Delay in cycles (rw) */
#define MEMSCRUB_CONFIG_DELAY_BETWEEN_BLOCKS_POS    8U
/* Interrupt when DONE */
#define MEMSCRUB_CONFIG_INTERRUPT_DONE_MASK         (0x00000080U) /* Interrupt when done (IRQD) (rw) */
#define MEMSCRUB_CONFIG_INTERRUPT_DONE_POS          7U
/* Secondary Memory Range */
#define MEMSCRUB_CONFIG_SECONDARY_RANGE_MASK        (0x00000020U) /* Secondary Memory Range (rw) */
#define MEMSCRUB_CONFIG_SECONDARY_RANGE_POS         5U
/* Restart scrubber when run finishes */
#define MEMSCRUB_CONFIG_LOOP_MODE_MASK              (0x00000010U) /* Loop Mode       (rw) */
#define MEMSCRUB_CONFIG_LOOP_MODE_POS               4U
/* Operation Mode (MODE) - 00=Scrub, 01=Regenerate, 10=Initialize, 11=Undefined */
#define MEMSCRUB_CONFIG_OPERATION_MODE_MASK         (0x0000000CU) /* Operation Mode  (rw) */
#define MEMSCRUB_CONFIG_OPERATION_MODE_POS          2U
#define MEMSCRUB_CONFIG_SCRUBBER_ENABLE_MASK        (0x00000001U) /* Enable Scrubber (rw) */
#define MEMSCRUB_CONFIG_SCRUBBER_ENABLE_POS         0U

/* Status */
#define MEMSCRUB_STATUS_CURRENT_STATE_MASK          (0x00000001U) /* Current State 0-idle (r) */
#define MEMSCRUB_STATUS_CURRENT_STATE_POS           0U
#define MEMSCRUB_STATUS_TASK_COMPLETE_MASK          (0x00002000U) /* Task Complete 0 to clear (wr) */
#define MEMSCRUB_STATUS_TASK_COMPLETE_POS           13U
#define MEMSCRUB_STATUS_BLOCK_ERRORS_MASK           (0x003F0000U) /* Block errors    (r) */
#define MEMSCRUB_STATUS_BLOCK_ERRORS_POS            14U
#define MEMSCRUB_STATUS_RUN_ERRORS_MASK             (0xFFC00000U) /* Run errors      (r) */
#define MEMSCRUB_STATUS_RUN_ERRORS_POS              22U

#define MEMSCRUB_REG                 ((SPARC_LEON4_MEMORY_SCRUBBER_REG_T *)MEMSCRUB_REG_ADDR)

/* MACRO */
#define SET_MEMSCRUB_CONFIG(reg,value)       ((MEMSCRUB_REG->configuration) |= (reg##_MASK & (value << (reg##_POS))))
#define GET_MEMSCRUB_CONFIG(reg)             (((MEMSCRUB_REG->configuration) &= reg##_MASK) >>  (reg##_POS))
#define GET_MEMSCRUB_STATUS(reg)             (((MEMSCRUB_REG->status) &= reg##_MASK) >>  (reg##_POS))
#define GET_MEMSCRUB_AHB_STATUS(reg)          (((MEMSCRUB_REG->ahb_status) &= reg##_MASK) >>  (reg##_POS))

/***************  ***************/

/**
** \} <!-- End of group "psp_gr740vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif
