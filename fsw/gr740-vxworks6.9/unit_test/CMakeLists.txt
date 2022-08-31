######################################################################
#
# CMAKE build recipe for gr740-vxworks6.9 PSP white-box coverage tests
#
######################################################################

#
# Unit Test for running on VxWorks GR740 target
# To test static functions, the testcases are combined with the original source 
# code under a new subfolder called tmp_code_coverage.
#

set(UT_NAME "psp")

set(IMPL_DIR ${PROJECT_SOURCE_DIR}/fsw/${CFE_SYSTEM_PSPNAME})

enable_testing()

if (CODE_COVERAGE)
    ### ### ###
    ### Preprocess TESTCASE source files to embed functions under test into the testcase files
    set(TMP_FOLDER_CODE_COVERAGE tmp_code_coverage)
    # Remove previous tmp_code_coverage
    file(REMOVE_RECURSE ${TMP_FOLDER_CODE_COVERAGE})
    message(${PSP_UT_PRE} "Generating Files for Code Coverage at ${TMP_FOLDER_CODE_COVERAGE}")
    
    # Function that performs the file merge
    # Gets content of IN_FILE1, find and replace SEARCH_STRING with content of IN_FILE2, save result in OUT_FILE
    function(mergeFilesTo IN_FILE1 IN_FILE2 SEARCH_STRING OUT_FILE)
        file(READ ${IN_FILE1} TEST_CONTENT)
        file(READ ${IN_FILE2} IMPL_CONTENT)
        string(REPLACE "${SEARCH_STRING}" "${IMPL_CONTENT}" COMBINED_CONTENT "${TEST_CONTENT}")
        file(APPEND ${OUT_FILE} "${COMBINED_CONTENT}")
    endfunction()

    # Create subfolder and all concatenated files
    file(MAKE_DIRECTORY ${TMP_FOLDER_CODE_COVERAGE})
    mergeFilesTo( psp_exception_testcases.c ${IMPL_DIR}/src/cfe_psp_exception.c "#include \"../src/cfe_psp_exception.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_expection_testcases.c)
    mergeFilesTo( psp_gr740info_testcases.c ${IMPL_DIR}/src/cfe_psp_gr740info.c "#include \"../src/cfe_psp_gr740info.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_gr740info_testcases.c)
    mergeFilesTo( psp_memory_testcases.c ${IMPL_DIR}/src/cfe_psp_memory.c "#include \"../src/cfe_psp_memory.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_memory_testcases.c)
    mergeFilesTo( psp_memscrub_testcases.c ${IMPL_DIR}/src/cfe_psp_memscrub.c "#include \"../src/cfe_psp_memscrub.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_memscrub_testcases.c)
    mergeFilesTo( psp_ntp_testcases.c ${IMPL_DIR}/src/cfe_psp_ntp.c "#include \"../src/cfe_psp_ntp.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_ntp_testcases.c)
    mergeFilesTo( psp_ssr_testcases.c ${IMPL_DIR}/src/cfe_psp_ssr.c "#include \"../src/cfe_psp_ssr.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_ssr_testcases.c)
    mergeFilesTo( psp_start_testcases.c ${IMPL_DIR}/src/cfe_psp_start.c "#include \"../src/cfe_psp_start.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_start_testcases.c)
    mergeFilesTo( psp_support_testcases.c ${IMPL_DIR}/src/cfe_psp_support.c "#include \"../src/cfe_psp_support.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_support_testcases.c)
    mergeFilesTo( psp_timer_testcases.c ${IMPL_DIR}/src/cfe_psp_timer.c "#include \"../src/cfe_psp_timer.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_timer_testcases.c)
    mergeFilesTo( psp_watchdog_testcases.c ${IMPL_DIR}/src/cfe_psp_watchdog.c "#include \"../src/cfe_psp_watchdog.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_watchdog_testcases.c)
    file(COPY psp_testcases.c DESTINATION ${IMPL_DIR}/unit_test/${TMP_FOLDER_CODE_COVERAGE})

    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FOLDER_CODE_COVERAGE} UT_SRCS)
else()
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} UT_SRCS)
endif()

### ### ###

# Remove stop on error flag so that compiler can skip over the Coverage intermediate files
add_compile_options(-w)

# Source for this app's UT executable
aux_source_directory(${IMPL_DIR}/unit_test/ut_stubs UT_SRCS)

add_executable(${UT_NAME}_ut ${UT_SRCS})

# as UT assert defines OS_Application_Startup and OS_Application_Run, we need
# to change the ones in this module to a different name.
target_compile_definitions(${UT_NAME}_ut PRIVATE
    OS_Application_Startup=UT_OS_Application_Startup
    OS_Application_Run=UT_OS_Application_Run
    OS_BSPMain=PCS_OS_BSPMain
    snprintf=PCS_snprintf
    bootStringToStruct=PCS_bootStringToStruct
)

# To support the starting and killing of NTP daemon process (NTP Sync Module)
string(FIND "$ENV{WIND_COMPONENTS_LIBNAMES}" "ip_net2-6.9" VXWORKS_COMPONENT_IPNET2_69)
if (NOT ${VXWORKS_COMPONENT_IPNET2_69} EQUAL -1)
    include_directories($ENV{WIND_COMPONENTS}/ip_net2-6.9/ipcom/include)
    include_directories($ENV{WIND_COMPONENTS}/ip_net2-6.9/ipcom/config)
    include_directories($ENV{WIND_COMPONENTS}/ip_net2-6.9/ipcom/port/vxworks/config)
    include_directories($ENV{WIND_COMPONENTS}/ip_net2-6.9/ipcom/port/vxworks/include)
else()
    message("VxWorks Installed components are " $ENV{WIND_COMPONENTS_LIBNAMES})    
    message(FATAL_ERROR "VxWorks IP NET2 6.9 component is required to build PSP")
endif()


include_directories(${IMPL_DIR}/unit_test/ut_stubs)
include_directories(${IMPL_DIR}/unit_test)
include_directories(${MISSION_SOURCE_DIR}/cfe/modules/core_api/fsw/inc/)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:time,INTERFACE_INCLUDE_DIRECTORIES>
)

add_test(${UT_NAME}_ut ${UT_NAME}_ut)


target_link_libraries(${UT_NAME}_ut 
    ut_assert
    ut_osapi_stubs
    psp_module_api
    )
