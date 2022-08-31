/**
 ** \file
 **
 ** \brief Registers and Component definitions for GR740
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
 ** The GR740 BSP for VxWorks 6.9 lacks a common header file with all the 
 ** definitions of components and registers. This header attempts to add what is
 ** missing.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** This is not a complete set.
 **
 */


#ifndef CFE_PSP_GR740_H
#define CFE_PSP_GR740_H

#ifdef __cplusplus
extern "C" {
#endif

/**
** \addtogroup psp_gr740vx69_platcfg PSP Platform Configurations - GR740-VxWorks6.9
** \{
*/

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

/* APB Bridge 0 */
#define APBUART0_REG_ADDR       (0xFF900000)  /* UART 0 registers Processor */
#define APBUART1_REG_ADDR       (0xFF901000)  /* UART 1 registers Processor */
#define GRGPIO0_REG_ADDR        (0xFF902000)  /* General purpose I/O port registers Processor */
#define FTMCTRL_REG_ADDR        (0xFF903000)  /* PROM/IO controller registers Processor */
#define IRQAMP_REG_ADDR         (0xFF904000)  /* Interrupt controller registers Processor */
#define GPTIMER0_REG_ADDR       (0xFF908000)  /* Timer unit 0 registers */
#define GPTIMER1_REG_ADDR       (0xFF909000)  /* Timer unit 1 registers */
#define GPTIMER2_REG_ADDR       (0xFF90A000)  /* Timer unit 2 registers */
#define GPTIMER3_REG_ADDR       (0xFF90B000)  /* Timer unit 3 registers */
#define GPTIMER4_REG_ADDR       (0xFF90C000)  /* Timer unit 4 registers */
#define GRSPWROUTER0_REG_ADDR   (0xFF90D000)  /* SpaceWire router AMBA interface 0 Processor */
#define GRSPWROUTER1_REG_ADDR   (0xFF90E000)  /* SpaceWire router AMBA interface 1 Processor */
#define GRSPWROUTER2_REG_ADDR   (0xFF90F000)  /* SpaceWire router AMBA interface 2 Processor */
#define GRSPWROUTER3_REG_ADDR   (0xFF910000)  /* SpaceWire router AMBA interface 3 Processor */
#define GRETH_GBIT0_REG_ADDR    (0xFF940000)  /* Gigabit Ethernet MAC 0 registers Processor */
#define GRETH_GBIT1_REG_ADDR    (0xFF980000)  /* Gigabit Ethernet MAC 1 registers Processor */
#define APBBRIDGE0_REG_ADDR     (0xFF9FF000)  /* APB bus 0 plug&play area Processor */


/* APB Bridge 1 */
#define APBBRIDGE1_REG_ADDR     (0xFFA00000)  /* APB bridge 1 Processor */

#define MEMSCRUB_REG_ADDR       (0xFFE01000)  /* Memory Scrubber Registers */
#define GR740THSEND_REG_ADDR    (0xFFA0A000)  /* Temperature Sensor */
#define GRGPRBANK_REG_ADDR      (0xFFA0B000)  /* General Purpose Register Bank */
#define L4STAT_REG_ADDR         (0xFFA0D000)  /* Leon 4 Statistics Unit */

#define GR740_SYSTEM_CLOCK      (250000000)

/*************** General Purpose Register Bank ***************/

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

typedef struct l4stat_reg_tag
{
    __IO uint32_t counter_value_register[16];
    __R uint32_t reserved_1[60];
    __IO uint32_t counter_control_register[16];
    __R uint32_t reserved_2[60];
    __IO uint32_t counter_max_latch_register[16];
} L4STAT_REG_T;

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

typedef struct temperature_reg_tag
{
    __IO uint32_t control_reg;
    __IO uint32_t status_reg;
    __IO uint32_t threshold_reg;
} TEMPERATURE_REG_T;

#define TEMPERATURE_CONTROL_DIV_MIN     (GR740_SYSTEM_CLOCK/250000)
#define TEMPERATURE_CONTROL_DIV_DEFAULT (GR740_SYSTEM_CLOCK/400000)
#define TEMPERATURE_CONTROL_DIV_MAX     (GR740_SYSTEM_CLOCK/500000)

#define TEMPERATURE_REG                 ((TEMPERATURE_REG_T *)GR740THSEND_REG_ADDR)

/* MACRO */
#define SET_TEMPERATURE_CONTROL(reg,value)   ((TEMPERATURE_REG->control_reg) |= (reg##_MASK & (value << (reg##_POS))))
#define GET_TEMPERATURE_STATUS(reg)          (((TEMPERATURE_REG->status_reg) &= reg##_MASK) >>  (reg##_POS))

/*************** TIMERS ***************/

// We're directly poking timer registers rather than going through the vxWorks
// gptimer drivers, since the drivers don't support features like chaining or
// the watchdog. Care must be taken so that the timer allocation by the drivers don't conflict w/ the PSP

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

#define TIMER0_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER0_REG_ADDR)
#define TIMER1_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER1_REG_ADDR)
#define TIMER2_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER2_REG_ADDR)
#define TIMER3_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER3_REG_ADDR)
#define TIMER4_REG                   ((SPARC_LEON4_TIMER_REG_T *)GPTIMER4_REG_ADDR)

/*************** MEMORY SCRUBBER ***************/

/* Configuration */
#define MEMSCRUB_CONFIG_DELAY_BETWEEN_BLOCKS_MASK   (0x0000FF00U) /* Delay in cycles (rw) */
#define MEMSCRUB_CONFIG_DELAY_BETWEEN_BLOCKS_POS    8U
/* Restart scrubber when run finishes */
#define MEMSCRUB_CONFIG_LOOP_MODE_MASK              (0x00000010U) /* Loop Mode       (rw) */
#define MEMSCRUB_CONFIG_LOOP_MODE_POS               4U
/* Operation Mode (MODE) - 00=Scrub, 01=Regenerate, 10=Initialize, 11=Undefined */
#define MEMSCRUB_CONFIG_OPERATION_MODE_MASK         (0x0000000CU) /* Operation Mode  (rw) */
#define MEMSCRUB_CONFIG_OPERATION_MODE_POS          2U
#define MEMSCRUB_CONFIG_SCRUBBER_ENABLE_MASK        (0x00000001U) /* Enable Scrubber (rw) */
#define MEMSCRUB_CONFIG_SCRUBBER_ENABLE_POS         0U

#define MEMSCRUB_STATUS_CURRENT_STATE_MASK          (0x00000001U) /* Current State 0-idle (r) */
#define MEMSCRUB_STATUS_CURRENT_STATE_POS           0U
#define MEMSCRUB_STATUS_TASK_COMPLETE_MASK          (0x00002000U) /* Task Complete 0 to clear (wr) */
#define MEMSCRUB_STATUS_TASK_COMPLETE_POS           13U
#define MEMSCRUB_STATUS_BLOCK_ERRORS_MASK           (0x003F0000U) /* Block errors    (r) */
#define MEMSCRUB_STATUS_BLOCK_ERRORS_POS            14U
#define MEMSCRUB_STATUS_RUN_ERRORS_MASK             (0xFFC00000U) /* Run errors      (r) */
#define MEMSCRUB_STATUS_RUN_ERRORS_POS              22U


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

#define MEMSCRUB_REG                 ((SPARC_LEON4_MEMORY_SCRUBBER_REG_T *)MEMSCRUB_REG_ADDR)

/* MACRO */
#define SET_MEMSCRUB_CONFIG(reg,value)       ((MEMSCRUB_REG->configuration) |= (reg##_MASK & (value << (reg##_POS))))

/***************  ***************/

/**
** \} <!-- End of group "psp_gr740vx69_platcfg" -->
*/

#ifdef __cplusplus
}
#endif

#endif  /* _CFE_PSP_CONFIG_ */