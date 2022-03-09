######################################################################
#
# CMAKE build recipe for sp0-vxworks6.9 PSP white-box coverage tests
#
######################################################################

# CMake snippet for PSP unit tests
#
# Unit Test here is using a mini unit test header for running on VxWorks SP0 target
#
#

set (UT_NAME "psp")

# PROJECT_SOURCE_DIR is $HOME/cert_testbed/psp
# CMAKE_CURRENT_SOURCE_DIR is $HOME/cert_testbed/psp/fsw/sp0-vxworks6.9/unit_test
set(IMPL_DIR ${MISSION_SOURCE_DIR}/${UT_NAME}/fsw/${CFE_SYSTEM_PSPNAME})

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
    mergeFilesTo(
        psp_exception_testcases.c ${IMPL_DIR}/src/cfe_psp_exception.c "#include \"../src/cfe_psp_exception.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_expection_testcases.c)
    mergeFilesTo(
        psp_memory_testcases.c ${IMPL_DIR}/src/cfe_psp_memory.c "#include \"../src/cfe_psp_memory.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_memory_testcases.c)
    mergeFilesTo(
        psp_flash_testcases.c ${IMPL_DIR}/src/cfe_psp_flash.c "#include \"../src/cfe_psp_flash.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_flash_testcases.c)
    mergeFilesTo(
        psp_mem_scrub_testcases.c ${IMPL_DIR}/src/cfe_psp_mem_scrub.c "#include \"../src/cfe_psp_mem_scrub.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_mem_scrub_testcases.c)
    mergeFilesTo(
        psp_start_testcases.c ${IMPL_DIR}/src/cfe_psp_start.c "#include \"../src/cfe_psp_start.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_start_testcases.c)
    mergeFilesTo(
        psp_support_testcases.c ${IMPL_DIR}/src/cfe_psp_support.c "#include \"../src/cfe_psp_support.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_support_testcases.c)
    mergeFilesTo(
        psp_watchdog_testcases.c ${IMPL_DIR}/src/cfe_psp_watchdog.c "#include \"../src/cfe_psp_watchdog.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_watchdog_testcases.c)
    mergeFilesTo(
        psp_sp0_info_testcases.c ${IMPL_DIR}/src/cfe_psp_sp0_info.c "#include \"../src/cfe_psp_sp0_info.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_sp0_info_testcases.c)
    mergeFilesTo(
        psp_ntp_testcases.c ${IMPL_DIR}/../modules/ntp_clock_vxworks/cfe_psp_ntp.c "#include \"../../modules/ntp_clock_wxworks/cfe_psp_ntp.c\"" ${TMP_FOLDER_CODE_COVERAGE}/psp_ntp_testcases.c)
    file(COPY psp_testcases.c DESTINATION ${IMPL_DIR}/unit_test/${TMP_FOLDER_CODE_COVERAGE})

    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/${TMP_FOLDER_CODE_COVERAGE} UT_SRCS)
else()
    aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} UT_SRCS)
endif()

### ### ###

# Source for this app's UT executable
aux_source_directory(${IMPL_DIR}/ut_stubs UT_SRCS)

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
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/include)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/config)
include_directories(${WIND_HOME}/components/ip_net2-6.9/ipcom/port/vxworks/include)

include_directories(${IMPL_DIR}/ut_stubs)
include_directories(${IMPL_DIR}/unit_test)
include_directories(${MISSION_SOURCE_DIR}/cfe/modules/core_api/fsw/inc/)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    ${WIND_HOME}/vxworks-6.9/target/3rdparty/aitech/sp0
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
