##########################################################################
#
# Build options for "pc-linux" PSP
# This file specifies any global-scope compiler options when using this PSP
#
##########################################################################

#*****************************************************************************************************************
#  Note that this file has been cloned-n-owned from its open-source github
#  repository for Gateway as of September 2023. Therefore, it is subject to
#  NASA Export Control restrictions, as stated below.
#*****************************************************************************************************************
 
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
# Note - this should be phased out in favor of the staging dir from OSAL BSP
set(INSTALL_SUBDIR "cf")

# Some upper-level code may be gated on _LINUX_OS_ being defined
# This is for compatibility with older build scripts which defined this symbol,
# but no CFE/OSAL framework code depends on this symbol.
add_definitions("-D_LINUX_OS_")

set(CFE_PSP_EXPECTED_OSAL_BSPTYPE "generic-linux")

