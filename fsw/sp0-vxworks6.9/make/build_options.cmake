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

# This indicates where to install target binaries created during the build
set(INSTALL_SUBDIR "cf")

message(STATUS "WIND_BASE IS $ENV{WIND_BASE}")

##
## Target Defines for the OS, Hardware Arch, etc..
##
add_definitions("-D_VXWORKS_OS_ -D_HAVE_STDINT_ -D_PPC_ -D__PPC__ -D_EMBED_ -DTOOL_FAMILY=gnu -DTOOL=e500v2gnu -D_WRS_KERNEL -DCPU=PPC85XX -DCPU_VARIANT=_e500v2 \"-D_VSB_CONFIG_FILE=<../lib/h/config/vsbConfig.h>\"")


## 
## Endian Defines
##
add_definitions("-D_EB -DENDIAN=_EB -DSOFTWARE_BIG_BIT_ORDER") 

##
## Compiler Architecture Switches
##
add_definitions("-mcpu=8548 -te500v2 -mstrict-align -fno-builtin -fvolatile -mhard-float -fno-implicit-fp -mfloat-gprs=double -mspe=yes -mabi=spe -mlongcall")

include_directories($ENV{WIND_BASE}/target/h/wrn/coreip)
include_directories($ENV{WIND_BASE}/target/3rdparty/aitech/sp0)
include_directories($ENV{WIND_BASE}/target/config/sp0)

set(CFE_PSP_EXPECTED_OSAL_BSPTYPE "generic-vxworks")
