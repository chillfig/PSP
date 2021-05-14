
######################################################################
#
# CMAKE build recipe for shared PSP component
#
######################################################################

# This contains supplemental code to support running CFE
# on a variety of targets.
#
# Note this shared PSP code is currently built against headers provided by the
# target implementation.  This makes it implementation-specific even though
# the same source code is used with multiple targets.

# To support the starting and killing of NTP daemon process
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/include)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/include)

add_compile_options(-Werror=conversion -Werror=sign-conversion -Werror=all)

# Build the sp0-vxworks implementation as a library
add_library(psp-${CFE_PSP_TARGETNAME}-impl OBJECT
    src/cfe_psp_exception.c
    src/cfe_psp_memory.c
    src/cfe_psp_start.c
    src/cfe_psp_support.c
    src/cfe_psp_watchdog.c)

target_compile_definitions(psp-${CFE_SYSTEM_PSPNAME}-impl PRIVATE
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_COMPILE_DEFINITIONS>
)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    inc
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:time,INTERFACE_INCLUDE_DIRECTORIES>
)


# Add unit test, if enabled
if (ENABLE_UNIT_TESTS)
    add_subdirectory(unit_test)
endif (ENABLE_UNIT_TESTS)
