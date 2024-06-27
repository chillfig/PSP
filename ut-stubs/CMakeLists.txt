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

# Public cfe_psp.h stubs, shared stubs
aux_source_directory(shared/src SHARED_SRC)

# Sets the target name (not available in toolchain file when working with stubs)
set(STUB_TARGET_NAME "sp0-vxworks6.9")
#set(STUB_TARGET_NAME "gr740-vxworks6.9")

aux_source_directory(${STUB_TARGET_NAME}/src STUB_TARGET_NAME_SRC)

add_library(ut_psp-${CFE_PSP_TARGETNAME}_stubs ut_psp_stubs.c ${SHARED_SRC} ${STUB_TARGET_NAME_SRC} ${LINUX_SRC})

target_link_libraries(ut_psp-${CFE_PSP_TARGETNAME}_stubs PRIVATE 
    psp_module_api
    ut_assert
    ut_core_api_stubs 
)
target_include_directories(ut_psp-${CFE_PSP_TARGETNAME}_stubs PUBLIC
    #${psp_MISSION_DIR}/ut-stubs/shared/inc
    ${psp_MISSION_DIR}/fsw/shared/inc
    ${psp_MISSION_DIR}/ut-stubs/${STUB_TARGET_NAME}/inc
)

add_cfe_coverage_stubs("ut_psp-${CFE_PSP_TARGETNAME}_stubs"
    ${SHARED_SRC}
    ${STUB_TARGET_NAME_SRC}
)
