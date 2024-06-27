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
*   \file psp_ft.h
*
*   \brief Header file for psp_ft.c
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at NASA's Johnson Space Center.
**********************************************************************************************************/

#include "cfe_psp_module.h"

/* Helper MACROS */
#define FT_Assert_True(cond, msg, ...)   cnt_tests++; if (cond) {OS_printf("\tPassed - " msg "\n", ##__VA_ARGS__); cnt_pass++;} else {OS_printf("\tFailed - " msg "\n", ##__VA_ARGS__);cnt_fail++;}

/* Type of Values */
typedef enum
{
    /* Used when the entered value is a percent */
    PSP_FT_VALUE_PERCENT,
    /* Used when the entered value is a raw, no other calculations needed */
    PSP_FT_VALUE_RAW
} PSP_FT_VALUE_TYPE_t;

bool check_file_exists(char *filename);
int32_t get_file_content(char *filename, char *buffer, uint16_t buffer_size);
bool check_range_value(float current_value, float right_value, PSP_FT_VALUE_TYPE_t vt, float positive_value, float negative_value);

extern CFE_PSP_ModuleApi_t CFE_PSP_timebase_vxworks_API;
extern CFE_PSP_ModuleApi_t CFE_PSP_iodriver_API;
extern CFE_PSP_ModuleApi_t CFE_PSP_vxworks_sysmon_API;
extern CFE_PSP_ModuleApi_t CFE_PSP_ping_API;

CFE_StaticModuleLoadEntry_t CFE_PSP_BASE_MODULE_LIST[] =
{
    { .Name = "timebase_vxworks", .Api = &CFE_PSP_timebase_vxworks_API },
    { .Name = "iodriver", .Api = &CFE_PSP_iodriver_API },
    { .Name = "vxworks_sysmon", .Api = &CFE_PSP_vxworks_sysmon_API },
    { .Name = "ping", .Api = &CFE_PSP_ping_API },
    {NULL}
};

void ft_support(void);
void ft_exception(void);
void ft_cds_flash(void);
void ft_mem_scrub(void);
void ft_sp0_info(void);
void ft_memory(void);
void ft_memory_sync(void);
void ft_start(void);
void ft_ntp_sync(void);
void ft_watchdog(void);
void PSP_FT_Setup(void);
void PSP_FT_Run(void);
void PSP_FT_Start(void);
void PSP_FT_SendEndTestEvent(void);
void ft_sysmon(void);
void ft_ping(void);
