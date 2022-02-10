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
extern CFE_PSP_ModuleApi_t CFE_PSP_ntp_clock_vxworks_API;

CFE_StaticModuleLoadEntry_t CFE_PSP_BASE_MODULE_LIST[] =
{
    { .Name = "timebase_vxworks", .Api = &CFE_PSP_timebase_vxworks_API },
    { .Name = "ntp_clock_vxworks", .Api = &CFE_PSP_ntp_clock_vxworks_API },
    {NULL}
};

void ft_support(void);
void ft_exception(void);
void ft_cds_flash(void);
void ft_mem_scrub(void);
void ft_sp0_info(void);
void ft_memory(void);
void ft_start(void);
void ft_ntp_sync(void);
void ft_watchdog(void);
void PSP_FT_Setup(void);
void PSP_FT_Run(void);
void PSP_FT_Start(void);
