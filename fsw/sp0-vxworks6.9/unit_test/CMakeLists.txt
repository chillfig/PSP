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

enable_testing()

# include_directories(${MISSION_SOURCE_DIR}/osal/ut_assert/inc)
# include_directories(${MISSION_SOURCE_DIR}/osal/src/os/inc)
# include_directories(${MISSION_SOURCE_DIR}/osal/src/os/shared/inc)
# include_directories(${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/src/time)
# include_directories(${MISSION_SOURCE_DIR}/${UT_NAME}/fsw/${CFE_SYSTEM_PSPNAME}/ut_stubs)
# include_directories(${MISSION_SOURCE_DIR}/osal/src/os/shared)
# include_directories(${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/ut-stubs)
# include_directories(${MISSION_SOURCE_DIR}/osal/src/ut-stubs)

# The following are needed to leverage ut_support from the cFE unit tests
# include_directories(${cfe-core_MISSION_DIR}/unit-test)
# include_directories(${cfe-core_MISSION_DIR}/src/evs)
# include_directories(${cfe-core_MISSION_DIR}/src/es)
# include_directories(${cfe-core_MISSION_DIR}/src/time)

# Source for this app's UT executable
aux_source_directory(${MISSION_SOURCE_DIR}/${UT_NAME}/fsw/${CFE_SYSTEM_PSPNAME}/unit_test UT_SRCS)
aux_source_directory(${MISSION_SOURCE_DIR}/${UT_NAME}/fsw/${CFE_SYSTEM_PSPNAME}/ut_stubs UT_SRCS)

add_executable(${UT_NAME}_ut ${UT_SRCS})

# as UT assert defines OS_Application_Startup and OS_Application_Run, we need
# to change the ones in this module to a different name.
target_compile_definitions(${UT_NAME}_ut PRIVATE
    OS_Application_Startup=UT_OS_Application_Startup
    OS_Application_Run=UT_OS_Application_Run
)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    inc
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:time,INTERFACE_INCLUDE_DIRECTORIES>
)

add_test(${UT_NAME}_ut ${UT_NAME}_ut)

install(TARGETS ${UT_NAME}_ut DESTINATION ./)

target_link_libraries(${UT_NAME}_ut 
    ut_assert
    ut_osapi_stubs
    )
