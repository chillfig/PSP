######################################################################
#
# CMAKE build recipe for gr740-vxworks6.9 PSP white-box coverage tests
#
######################################################################

include_directories("${CFEPSP_SOURCE_DIR}/fsw/gr740-vxworks6.9/inc")
include_directories("${PSPCOVERAGE_SOURCE_DIR}/shared/inc")
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
    -DSOFTWARE_BIG_BIT_ORDER
)

# The target names will be the PSP name with a "ut" prefix
# this is to distinguish these test targets from the FSW targets.
set(CFE_PSP_TARGETNAME "ut-${SETNAME}")
add_subdirectory("${CFEPSP_SOURCE_DIR}/fsw/${SETNAME}" "${CFE_PSP_TARGETNAME}-impl")
add_subdirectory("${CFEPSP_SOURCE_DIR}/fsw/shared" "${CFE_PSP_TARGETNAME}-shared")
add_subdirectory(adaptors)

# as UT assert defines OS_Application_Startup and OS_Application_Run, we need
# to change the ones in this module to a different name.
target_compile_definitions(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    OS_Application_Startup=UT_OS_Application_Startup
    OS_Application_Run=UT_OS_Application_Run
    OS_BSPMain=PCS_OS_BSPMain
)

# only the actual FSW src file gets the coverage instrumentation
target_compile_options(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    ${UT_COVERAGE_COMPILE_FLAGS}
)
target_compile_options(psp-${CFE_PSP_TARGETNAME}-shared PRIVATE
    ${UT_COVERAGE_COMPILE_FLAGS}
)

# both the FSW src file and the adaptor file get compiled with override includes
target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl BEFORE PRIVATE
    ${PSPCOVERAGE_SOURCE_DIR}/ut-stubs/override_inc
)
target_include_directories(psp-${CFE_PSP_TARGETNAME}-shared BEFORE PRIVATE
    ${PSPCOVERAGE_SOURCE_DIR}/ut-stubs/override_inc
)

get_target_property(TARGET_DIRS psp-${CFE_PSP_TARGETNAME}-impl INCLUDE_DIRECTORIES)
foreach(dir ${TARGET_DIRS})
  message(STATUS "target impl include dir='${dir}'")
endforeach()
get_target_property(TARGET_DIRS psp-${CFE_PSP_TARGETNAME}-shared INCLUDE_DIRECTORIES)
foreach(dir ${TARGET_DIRS})
  message(STATUS "target shared include dir='${dir}'")
endforeach()

add_executable(coverage-${CFE_PSP_TARGETNAME}-testrunner
    src/coveragetest-psp-gr740-vxworks6.9.c
    src/coveragetest-cfe-psp-exception.c
    src/coveragetest-cfe-psp-gr740info.c
    src/coveragetest-cfe-psp-memory.c
    src/coveragetest-cfe-psp-memscrub.c
    src/coveragetest-cfe-psp-ntp.c
    src/coveragetest-cfe-psp-ssr.c
    src/coveragetest-cfe-psp-start.c
    src/coveragetest-cfe-psp-support.c
    src/coveragetest-cfe-psp-timer.c
    src/coveragetest-cfe-psp-watchdog.c
    ${PSPCOVERAGE_SOURCE_DIR}/shared/src/coveragetest-cfe-psp-exceptionstorage.c
    ${PSPCOVERAGE_SOURCE_DIR}/shared/src/coveragetest-psp-shared.c
    $<TARGET_OBJECTS:psp-${CFE_PSP_TARGETNAME}-shared>
    $<TARGET_OBJECTS:psp-${CFE_PSP_TARGETNAME}-impl>
)

target_link_libraries(coverage-${CFE_PSP_TARGETNAME}-testrunner
    ${UT_COVERAGE_LINK_FLAGS}
    ut-adaptor-${CFE_PSP_TARGETNAME}
    psp_module_api
    ut_psp_cfe_stubs
    ut_psp_libc_stubs
    ut_osapi_stubs
    ut_assert
)

target_include_directories(coverage-${CFE_PSP_TARGETNAME}-testrunner BEFORE PRIVATE
    ${PSPCOVERAGE_SOURCE_DIR}/ut-stubs/override_inc
)

get_target_property(TARGET_DIRS coverage-${CFE_PSP_TARGETNAME}-testrunner INCLUDE_DIRECTORIES)
foreach(dir ${TARGET_DIRS})
  message(STATUS "target main testrunner include dir='${dir}'")
endforeach()

add_test(coverage-${CFE_PSP_TARGETNAME} coverage-${CFE_PSP_TARGETNAME}-testrunner)

foreach(TGT ${INSTALL_TARGET_LIST})
    install(TARGETS coverage-${CFE_PSP_TARGETNAME}-testrunner DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
endforeach()