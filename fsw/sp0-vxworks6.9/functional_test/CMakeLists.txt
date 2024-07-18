#*****************************************************************************************************************
# Export Control Marking
#
# EAR ECCN 9E515.a and/or 9E515.f (HALO)
#
# Export Administration Regulations (EAR) Notice
# This document contains information which falls under the purview of the Export Administration Regulations (EAR),
# 15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
# or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
# furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
# or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
# of the Gateway MOUs and IA requires prior U.S. Government authorization.
#*****************************************************************************************************************

######################################################################
#
# CMAKE build recipe for sp0-vxworks6.9 PSP white-box functional tests
#
######################################################################

# CMake snippet for PSP functional tests
#

set(FT_NAME "psp")

# Source for this app's UT executable
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/ FT_SRCS)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/${TGT_NAME}/cfe_psp_module_list.c)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/${TGT_NAME}/cfe_static_module_list.c)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/${TGT_NAME}/cfe_static_symbol_list.c)

file(TOUCH ${CMAKE_BINARY_DIR}/../../src/cfe_build_env_table.c)
file(TOUCH ${CMAKE_BINARY_DIR}/../../src/cfe_module_version_table.c)

list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/../../src/cfe_build_env_table.c)
list(APPEND FT_SRCS ${CMAKE_BINARY_DIR}/../../src/cfe_module_version_table.c)

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
    gateway_sb
    osal
    resourceid
    sbr
    tbl
    time
    psp_module_api
    soft_timebase
    timebase_vxworks
    iodriver
    vxworks_sysmon
    ping
    tte_errata
    psp-${CFE_PSP_TARGETNAME}-impl
    psp-${CFE_PSP_TARGETNAME}-shared
    )
