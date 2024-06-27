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
