######################################################################
#
# CMAKE build recipe for white-box coverage tests of Gateway ping module
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
include_directories("${CFEPSP_SOURCE_DIR}/fsw/modules/ping/inc")
include_directories("${CFEPSP_SOURCE_DIR}/unit-test-coverage/shared/inc")
include_directories("${CFEPSP_SOURCE_DIR}/unit-test-coverage/shared/adaptors/inc")

set(UTTEST_SRCS
    src/coveragetest-ping.c
    ${CFEPSP_SOURCE_DIR}/unit-test-coverage/shared/src/coveragetest-psp-shared.c
    ${CFEPSP_SOURCE_DIR}/ut-stubs/shared/src/cfe_psp_timesync_stubs.c
)

set(UT_SRCS
    ${CFEPSP_SOURCE_DIR}/fsw/modules/ping/src/cfe_psp_ping.c
)

add_psp_covtest(ping 
    "${UTTEST_SRCS}"
    "${UT_SRCS}"
)
