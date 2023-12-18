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
# CMAKE build recipe for CFE Platform Support Package (PSP)
#
######################################################################

project(CFEPSP C)

if (NOT CFE_SYSTEM_PSPNAME)
  message(FATAL_ERROR "CFE_SYSTEM_PSPNAME is not defined - do not know which to build")
endif()

set(CFE_PSP_TARGETNAME "${CFE_SYSTEM_PSPNAME}")
add_definitions(-D_CFE_PSP_)       # macro to indicate PSP scope

# The "psp_module_api" defines the interface between internal PSP components
add_library(psp_module_api INTERFACE)
target_compile_definitions(psp_module_api INTERFACE
    $<TARGET_PROPERTY:osal,INTERFACE_COMPILE_DEFINITIONS> # use defs from OSAL
)
target_include_directories(psp_module_api INTERFACE
    fsw/inc         # public API
    fsw/shared/inc  # all PSP shared headers
    fsw/${CFE_PSP_TARGETNAME}/inc # all impl headers
    ${CFE_SOURCE_DIR}/cmake/target/inc    # for sysconfig
    $<TARGET_PROPERTY:osal,INTERFACE_INCLUDE_DIRECTORIES>  # use headers from OSAL
)

# Translate the CFE_PSP_TARGETNAME to a set of additional modules to build
file(STRINGS "${CMAKE_CURRENT_LIST_DIR}/fsw/${CFE_PSP_TARGETNAME}/psp_module_list.cmake" PSP_TARGET_MODULE_LIST REGEX "^[a-zA-Z]")

# The PSP is currently built in modular parts, consisting of a platform-specific
# module(s) combined with a shared component which is built for multiple targets.
# The "shared" component is compiled using headers from the platform-specific module
# so it is still ultimately a platform-specific binary, and it all gets wrapped into
# a single PSP static library target.
add_subdirectory(fsw/${CFE_PSP_TARGETNAME} ${CFE_PSP_TARGETNAME}-impl)
add_subdirectory(fsw/shared ${CFE_PSP_TARGETNAME}-shared)

# Generate a list of PSP modules along with a pointer to its API structure/entry point
set(GENERATED_EXTERNS)
set(GENERATED_KEYVALS)
foreach(PSPMOD ${PSP_TARGET_MODULE_LIST})
    add_subdirectory(fsw/modules/${PSPMOD} ${PSPMOD}-${CFE_PSP_TARGETNAME}-impl)
    list(APPEND GENERATED_EXTERNS "extern CFE_PSP_ModuleApi_t CFE_PSP_${PSPMOD}_API\;\n")
    list(APPEND GENERATED_KEYVALS "{ .Name = \"${PSPMOD}\", .Api = &CFE_PSP_${PSPMOD}_API },\n")
endforeach()

string(CONCAT GENERATED_EXTERNS ${GENERATED_EXTERNS})
string(CONCAT GENERATED_KEYVALS ${GENERATED_KEYVALS})

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/module_list.c.in ${CMAKE_CURRENT_BINARY_DIR}/${CFE_PSP_TARGETNAME}_module_list.c @ONLY)

add_library(psp-${CFE_PSP_TARGETNAME} STATIC
    ${CMAKE_CURRENT_BINARY_DIR}/${CFE_PSP_TARGETNAME}_module_list.c
    $<TARGET_OBJECTS:psp-${CFE_PSP_TARGETNAME}-shared>
    $<TARGET_OBJECTS:psp-${CFE_PSP_TARGETNAME}-impl>
)
target_link_libraries(psp-${CFE_PSP_TARGETNAME} PUBLIC
    ${PSP_TARGET_MODULE_LIST}
)
target_link_libraries(psp-${CFE_PSP_TARGETNAME} PRIVATE
    psp_module_api
)

target_include_directories(psp-${CFE_PSP_TARGETNAME} INTERFACE
    fsw/inc
)

if (ENABLE_UNIT_TESTS)
    set(PSP_UT_PRE "PSP UT: ")

    # Figure out if the compiler is for a real target
    string(TOLOWER "${CFE_SYSTEM_PSPNAME}" SYSTEM_NAME)

    if("${SYSTEM_NAME}" STREQUAL "pc-linux")
        # ---- Run Unit Tests on Linux -----

        # Prepare for Unit Test on Linux (This is maintained by Github upstream PSP)
        message(${PSP_UT_PRE} "Preparing for Unit Test on Linux")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/unit-test-coverage)

    else()
        # ---- Run Unit Tests on Target -----

        # Check that the UT implementation for the specific Platform exists
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fsw/${CFE_PSP_TARGETNAME}/unit_test/CMakeLists.txt)
            # Prepare for Unit Test on Target
            message(${PSP_UT_PRE} "Preparing for Unit Test on ${CFE_SYSTEM_PSPNAME} with Code Coverage")
            set(CODE_COVERAGE TRUE)
            add_subdirectory(fsw/${CFE_PSP_TARGETNAME}/unit_test)
        else()
            message(FATAL_ERROR ${PSP_UT_PRE} "Enable Unit Test for a PSP system name that is not found")
        endif()

    endif()

    # Add stubs
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ut-stubs)

endif (ENABLE_UNIT_TESTS)

# Get Functional Test environment variable
set(ENABLE_FUNCTIONAL_TESTS $ENV{ENABLE_FUNCTIONAL_TESTS} CACHE BOOL "Enable build of functional tests")

if (ENABLE_FUNCTIONAL_TESTS)
    set(PSP_FT_PRE "PSP FT: ")

    # Figure out if the compiler is for a real target
    string(TOLOWER "${CFE_SYSTEM_PSPNAME}" SYSTEM_NAME)

    if("${SYSTEM_NAME}" STREQUAL "pc-linux")
        # ---- Run Functional Tests on Linux -----

        # Prepare for Functional Test on Linux
        message(${PSP_FT_PRE} "Functional Test on Linux is not implemented!")

    else()
        # ---- Run Functional Tests on Target -----

        # Prepare for Functional Test on Target
        # Check that the UT implementation for the specific Platform exists
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/fsw/${CFE_PSP_TARGETNAME}/functional_test/CMakeLists.txt)
            message(${PSP_FT_PRE} "Preparing for PSP Functional Test")
            add_subdirectory(fsw/${CFE_PSP_TARGETNAME}/functional_test)
        else()
            message(FATAL_ERROR ${PSP_FT_PRE} "Enable Unit Test for a PSP system name that is not found")
        endif()

    endif()
endif (ENABLE_FUNCTIONAL_TESTS)
