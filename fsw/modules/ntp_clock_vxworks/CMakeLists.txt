include_directories(${MISSION_SOURCE_DIR}/cfe/modules/core_api/fsw/inc)
include_directories(${MISSION_SOURCE_DIR}/psp/fsw/${CFE_SYSTEM_PSPNAME}/inc)

# To support the starting and killing of NTP daemon process
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/include)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/include)

set_source_files_properties(cfe_psp_ntp.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)

# Create the module
add_psp_module(ntp_clock_vxworks cfe_psp_ntp.c)
