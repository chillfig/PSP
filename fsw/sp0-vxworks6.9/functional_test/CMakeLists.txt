######################################################################
#
# CMAKE build recipe for sp0-vxworks6.9 PSP white-box functional tests
#
######################################################################

# CMake snippet for PSP functional tests
#

set (FT_NAME "psp")

# PROJECT_SOURCE_DIR is $HOME/cert_testbed/psp
# CMAKE_CURRENT_SOURCE_DIR is $HOME/cert_testbed/psp/fsw/sp0-vxworks6.9/funtional_test
set(IMPL_DIR ${MISSION_SOURCE_DIR}/${FT_NAME}/fsw/${CFE_SYSTEM_PSPNAME})

# Source for this app's UT executable
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ FT_SRCS)

add_executable(${FT_NAME}_ft ${FT_SRCS})

target_include_directories(${FT_NAME}_ft PRIVATE
    inc
    shared/inc
    $<TARGET_PROPERTY:time,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:osal,INTERFACE_INCLUDE_DIRECTORIES>
)

target_link_libraries(${FT_NAME}_ft
    core_api
    core_private
    es
    evs
    fs
    resourceid
    sb
    sbr
    tbl
    time
    osal
    gateway_msg
    psp_module_api
    timebase_vxworks
    ntp_clock_vxworks
    psp-${CFE_PSP_TARGETNAME}-impl
    psp-${CFE_PSP_TARGETNAME}-shared
    )
