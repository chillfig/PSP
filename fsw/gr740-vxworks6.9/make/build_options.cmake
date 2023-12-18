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

##########################################################################
#
# Build options for "gr740-vxworks" PSP
# This file specifies any global-scope compiler options when using this PSP
#
##########################################################################

# This indicates where to install target binaries created during the build
# Note - this should be phased out in favor of the staging dir from OSAL BSP
set(INSTALL_SUBDIR "cf")

# Some upper-level code may be gated on _VXWORKS_OS_ being defined
# This is for compatibility with older build scripts which defined this symbol,
# but no CFE/OSAL framework code depends on this symbol.
add_definitions("-D_VXWORKS_OS_")   # This is for io_lib/fsw/public_inc/network_includes.h

# Additional definitions and options
add_compile_options(-fno-builtin -Wcast-align -m32 -mcpu=leon3 -mhard-float -gdwarf-2 ) # note gdwarf-2 due to vxworks toolchain not supporting newer
add_definitions(
    -DCPU=_VX_SPARCV8
    -DTOOL_FAMILY=gnu
    -DTOOL=gnu
    -DCPU_VARIANT=_sparcleon
    -D_WRS_KERNEL
    -D_WRS_VX_SMP
    -D_WRS_CONFIG_SMP \"-D_VSB_CONFIG_FILE=<../lib/h/config/vsbConfig.h>\"
    -DIP_PORT_VXWORKS=69
    -D_EMBED_
    -D_EB
    -DENDIAN=_EB
    -DSOFTWARE_BIG_BIT_ORDER
    ) 


# Use the gr740-specific VxWorks BSP include directory
# This needs to be globally used, not just private to the PSP, because
# some VxWorks headers reference files contained here.

# Platform includes
include_directories(${MISSION_SOURCE_DIR}/prj_cfe/gateway_msg/fsw/inc)
include_directories(${MISSION_SOURCE_DIR}/ccdd/include/include32)
include_directories(${WIND_BASE}/target/h/wrn/coreip)
include_directories(${WIND_BASE}/target/h)

# Only for adding NFS
include_directories(${WIND_BASE}/target/h/nfs)

# BSP
include_directories(${WIND_BASE}/target/config/gr_cpci_gr740)

# NOTE: the __SPARC__  macro is referenced in some system headers.
# therefore all code compiled for this platform should always define these symbols.
#add_definitions("-D__SPARC__")

set(CFE_PSP_EXPECTED_OSAL_BSPTYPE "generic-vxworks")

