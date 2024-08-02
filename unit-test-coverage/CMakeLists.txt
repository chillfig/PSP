######################################################################
#
# CMAKE build recipe for PSP white-box coverage tests
#
######################################################################

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

# The following cache variables are recognized:
#   PSPCOVERAGE_TARGETS  -> the intended PSP module(s) that run on the actual target
#
# Like OSAL coverage testing, the actual underlying OS calls are stubbed out, there
# is no dependency on the actual underlying OS.  All coverage tests can be built on
# all platforms regardless of the actual PSP in use for flight software.

project(PSPCOVERAGE C)

# Currently only mcp750-vxworks is implemented a demonstration of how this works.
set(PSPCOVERAGE_TARGETS "mcp750-vxworks" "sp0-vxworks6.9" "gr740-vxworks6.9" CACHE STRING "PSP target(s) to build coverage tests for (default=all)")

# Check that coverage has been implemented for this PSPTYPE
foreach(PSPTYPE ${PSPCOVERAGE_TARGETS})
    if (NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PSPTYPE})
      message(FATAL_ERROR "No coverage tests implemented for ${PSPTYPE}")
    endif (NOT IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${PSPTYPE})
endforeach(PSPTYPE ${PSPCOVERAGE_TARGETS})

message(STATUS "PSP Coverage Test Targets: ${PSPCOVERAGE_TARGETS}")

# Utilize the shared UT assert library, along with the standard OSAL includes
include_directories(${UT_ASSERT_SOURCE_DIR}/inc)
include_directories(${PSPCOVERAGE_SOURCE_DIR}/ut-stubs/inc)

add_subdirectory(ut-stubs)
add_subdirectory(modules)

# Build targets for each of the indicated PSPs
foreach(SETNAME ${PSPCOVERAGE_TARGETS})
    add_subdirectory(${SETNAME})
endforeach(SETNAME ${PSPCOVERAGE_TARGETS})

