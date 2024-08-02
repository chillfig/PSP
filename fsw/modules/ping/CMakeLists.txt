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

# Pseudo-terminal interface module
add_psp_module(ping src/cfe_psp_ping.c)

target_include_directories(ping PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

include_directories(${io_lib_MISSION_DIR}/fsw/public_inc)

target_include_directories(ping PRIVATE 
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
)
