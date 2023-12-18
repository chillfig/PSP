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

# Pseudo-terminal interface module
add_psp_module(ping cfe_psp_ping.c)

target_include_directories(ping PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

target_include_directories(ping PRIVATE 
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
)
