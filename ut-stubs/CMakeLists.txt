# Public cfe_psp.h stubs, shared stubs
aux_source_directory(shared/src SHARED_SRC)
aux_source_directory(sp0-vxworks6.9/src SP0VXWORKS_SRC)
add_library(ut_psp-${CFE_PSP_TARGETNAME}_stubs ut_psp_stubs.c ${SHARED_SRC} ${SP0VXWORKS_SRC} ${LINUX_SRC})
target_link_libraries(ut_psp-${CFE_PSP_TARGETNAME}_stubs PRIVATE 
    psp_module_api
    ut_assert
    ut_core_api_stubs 
)
target_include_directories(ut_psp-${CFE_PSP_TARGETNAME}_stubs PUBLIC
    #${psp_MISSION_DIR}/ut-stubs/shared/inc
    ${psp_MISSION_DIR}/fsw/shared/inc
    ${psp_MISSION_DIR}/ut-stubs/sp0-vxworks6.9/inc
)

add_cfe_coverage_stubs("ut_psp-${CFE_PSP_TARGETNAME}_stubs"
    ${SHARED_SRC}
    ${SP0VXWORKS_SRC}
)