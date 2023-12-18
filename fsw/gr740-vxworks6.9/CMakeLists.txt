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
string(FIND "$ENV{WIND_COMPONENTS_LIBNAMES}" "ip_net2-6.9" VXWORKS_COMPONENT_IPNET2_69)
if (NOT ${VXWORKS_COMPONENT_IPNET2_69} EQUAL -1)
    set(WIND_COMPONENT_STRING $ENV{WIND_COMPONENTS})
    string(REPLACE ":" ";" WIND_COMPONENT_LIST ${WIND_COMPONENT_STRING})
    list(LENGTH WIND_COMPONENT_LIST LEN)
    if (${LEN} GREATER 1)
        message(WARNING "Environment variable WIND_COMPONENTS includes more than one path. 
                The last one is choosen automatically.")
        list(GET $WIND_COMPONENT_LIST 1 WIND_COMPONENT_PATH)
    else()
        set(WIND_COMPONENT_PATH ${WIND_COMPONENT_STRING})
    endif()
    message("WIND_COMPONENT_PATH = `" ${WIND_COMPONENT_PATH} "`")
    include_directories(${WIND_COMPONENT_PATH}/ip_net2-6.9/ipcom/include)
    include_directories(${WIND_COMPONENT_PATH}/ip_net2-6.9/ipcom/config)
    include_directories(${WIND_COMPONENT_PATH}/ip_net2-6.9/ipcom/port/vxworks/config)
    include_directories(${WIND_COMPONENT_PATH}/ip_net2-6.9/ipcom/port/vxworks/include)
else()
    message("VxWorks Installed components are " $ENV{WIND_COMPONENTS_LIBNAMES})    
    message(FATAL_ERROR "VxWorks IP NET2 6.9 component is required to build PSP")
endif()

# Build the target implementation as a library
add_library(psp-${CFE_PSP_TARGETNAME}-impl OBJECT
    src/cfe_psp_exception.c
    src/cfe_psp_gr740info.c
    src/cfe_psp_memory.c
    src/cfe_psp_memscrub.c
    src/cfe_psp_ntp.c
    src/cfe_psp_ssr.c
    src/cfe_psp_start.c
    src/cfe_psp_support.c
    src/cfe_psp_timer.c
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
