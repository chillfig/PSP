/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2023 United States Government as represented by the
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

/**
 * \file
 *
 * Internal header for vxworks_sysmon.c
 */

#ifndef VXWORKS_SYSMON_H_
#define VXWORKS_SYSMON_H_

/********************************************************************
 * Local Defines
 ********************************************************************/
#ifdef OS_MAXIMUM_PROCESSORS
    #define VXWORKS_SYSMON_MAX_CPUS  OS_MAXIMUM_PROCESSORS
#else
    #define VXWORKS_SYSMON_MAX_CPUS  1
#endif

#define VXWORKS_SYSMON_AGGREGATE_SUBSYS   0
#define VXWORKS_SYSMON_CPULOAD_SUBSYS     1
#define VXWORKS_SYSMON_AGGR_CPULOAD_SUBCH 0
#define VXWORKS_SYSMON_SAMPLE_DELAY       1000
#define VXWORKS_SYSMON_TASK_PRIORITY      100
#define VXWORKS_SYSMON_STACK_SIZE         4096
#define VXWORKS_AUX_CLOCK_INTERRUPT_FREQ  100  /* Frequency to collect data (interrupts per second) */
#define VXWORKS_SYSMON_MAX_SCALE          100
#define VXWORKS_SYSMON_TASK_NAME          "VXWORKS SYSMON"
#define VXWORKS_SYSMON_MAX_SPY_TASKS      100 /* Max number of tasks to spy on */

#ifdef DEBUG_BUILD
#define VXWORKS_SYSMON_DEBUG(...) OS_printf(__VA_ARGS__)
#else
#define VXWORKS_SYSMON_DEBUG(...)
#endif

/********************************************************************
 * Local Type Definitions
 ********************************************************************/
typedef struct vxworks_sysmon_va_arg
{
    char *name;
    char *placeholder; /* empty */
    int  total_idle_percent;
    int  total_idle_ticks;
    int  idle_percent_since_last_report;
    int  idle_ticks_since_last_report;

} vxworks_sysmon_va_arg_t;

typedef struct vxworks_sysmon_cpuload_core
{
    CFE_PSP_IODriver_AdcCode_t avg_load;

    vxworks_sysmon_va_arg_t idle_state;
} vxworks_sysmon_cpuload_core_t;

typedef struct vxworks_sysmon_cpuload_state
{
    volatile bool is_running;
    volatile bool should_run;

    osal_id_t   task_id;

    uint8_t    num_cpus;

    vxworks_sysmon_cpuload_core_t per_core[VXWORKS_SYSMON_MAX_CPUS];

} vxworks_sysmon_cpuload_state_t;

typedef struct vxworks_sysmon_state
{
    uint32_t                     local_module_id;
    vxworks_sysmon_cpuload_state_t cpu_load;
} vxworks_sysmon_state_t;

/********************************************************************
 * Local Function Prototypes
 ********************************************************************/
int vxworks_sysmon_update_stat(const char *fmt, ...);
void vxworks_sysmon_Task(void);

int32_t vxworks_sysmon_Start(vxworks_sysmon_cpuload_state_t *state);
int32_t vxworks_sysmon_Stop(vxworks_sysmon_cpuload_state_t *state);

int32_t vxworks_sysmon_aggregate_dispatch(uint32_t CommandCode, uint16_t Subchannel, CFE_PSP_IODriver_Arg_t Arg);
int32_t vxworks_sysmon_calc_aggregate_cpu(vxworks_sysmon_cpuload_state_t *state, CFE_PSP_IODriver_AdcCode_t *Val);

/* Function that starts up vxworks_sysmon driver. */
int32_t vxworks_sysmon_DevCmd(uint32_t CommandCode, uint16_t SubsystemId, uint16_t SubchannelId,
                                   CFE_PSP_IODriver_Arg_t Arg);

#endif /* VXWORKS_SYSMON_H_ */
