######################################################################
#
# CMAKE build recipe for white-box coverage tests of VxWorks timebase module
#

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

add_definitions(-D_CFE_PSP_MODULE_)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/inc")
include_directories("${CFEPSP_SOURCE_DIR}/fsw/modules/iodriver/inc")
include_directories("${CFEPSP_SOURCE_DIR}/fsw/modules/vxworks_sysmon")

add_psp_covtest(vxworks_sysmon src/coveragetest-vxworks_sysmon.c
    ${CFEPSP_SOURCE_DIR}/fsw/modules/vxworks_sysmon/vxworks_sysmon.c
)
