
#include_directories(${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/src/time)

# To support the starting and killing of NTP daemon process
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/include)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/include)

# Create the module
add_psp_module(ntp_clock_vxworks cfe_psp_ntp.c)
