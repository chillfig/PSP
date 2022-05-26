######################################################################
#
# CMAKE build recipe for sp0-vxworks6.9 PSP white-box functional tests
#
######################################################################

# CMake snippet for PSP functional tests
#

set (FT_NAME "psp")

# Source for this app's UT executable
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ FT_SRCS)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/vx_es2/cfe_psp_module_list.c)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/vx_es2/cfe_static_module_list.c)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/vx_es2/cfe_static_symbol_list.c)

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
    config
    es
    evs
    fs
    gateway_msg
    osal
    resourceid
    sb
    sbr
    tbl
    time
    psp_module_api
    soft_timebase
    timebase_vxworks
    psp-${CFE_PSP_TARGETNAME}-impl
    psp-${CFE_PSP_TARGETNAME}-shared
    )
