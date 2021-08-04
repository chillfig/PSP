include_directories(${MISSION_SOURCE_DIR}/cfe/modules/core_api/fsw/inc)
include_directories(${MISSION_SOURCE_DIR}/psp/fsw/${CFE_SYSTEM_PSPNAME}/inc)

set_source_files_properties(cfe_psp_ntp.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)

# Create the module
add_psp_module(ntp_clock_linux cfe_psp_ntp.c)
