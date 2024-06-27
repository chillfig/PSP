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

######################################################################
#
# CMAKE build recipe for shared PSP component
#
######################################################################

# Build the shared implementation as a library
add_library(psp-${CFE_PSP_TARGETNAME}-shared OBJECT
    src/cfe_psp_error.c
    src/cfe_psp_exceptionstorage.c
    src/cfe_psp_memrange.c
    src/cfe_psp_memutils.c
    src/cfe_psp_module.c
    src/cfe_psp_version.c
)

target_compile_definitions(psp-${CFE_SYSTEM_PSPNAME}-shared PRIVATE
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_COMPILE_DEFINITIONS>
)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-shared PRIVATE
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_INCLUDE_DIRECTORIES>
)
