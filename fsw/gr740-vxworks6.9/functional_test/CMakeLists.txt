#*********************************************************************************************************
# Export Control Marking
#
# EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
# This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
# 15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government 
# and are authorized for export by NASA only to fulfill responsibilities of the parties or of a 
# Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using 
# License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and 
# Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
# or to any person other than the authorized ultimate consignee or end-user(s), either in their
# original form or after being incorporated into other items, without first obtaining approval from
# the U.S. government or as otherwise authorized by U.S. law and regulations.
#*********************************************************************************************************

######################################################################
#
# CMAKE build recipe for gr740-vxworks6.9 PSP white-box functional tests
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
    psp-${CFE_PSP_TARGETNAME}-impl
    psp-${CFE_PSP_TARGETNAME}-shared
    )
