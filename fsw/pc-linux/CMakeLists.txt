######################################################################
#
# CMAKE build recipe for pc-linux PSP component
#
######################################################################

set_source_files_properties(cfe_psp_exception.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_memory.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_memtab.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_ssr.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_start.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_support.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_watchdog.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)
set_source_files_properties(cfe_psp_ntp.c PROPERTIES COMPILE_FLAGS -Werror=conversion -Werror=sign-conversion -Werror=all)

# This contains the fully platform-specific code to
# run CFE on this target.

# Build the pc-linux implementation as a library
add_library(psp-${CFE_PSP_TARGETNAME}-impl OBJECT
    src/cfe_psp_exception.c
    src/cfe_psp_memory.c
    src/cfe_psp_ssr.c
    src/cfe_psp_start.c
    src/cfe_psp_support.c
    src/cfe_psp_watchdog.c
    src/cfe_psp_ntp.c
)

# The _GNU_SOURCE directive is required to call non-posix APIs
# that are specific to the Linux/glibc environment.
# Code outside the pc-linux PSP should _not_ depend on this.
target_compile_definitions(psp-${CFE_SYSTEM_PSPNAME}-impl PRIVATE
    _GNU_SOURCE
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_COMPILE_DEFINITIONS>
)

target_include_directories(psp-${CFE_PSP_TARGETNAME}-impl PRIVATE
    inc
    $<TARGET_PROPERTY:psp_module_api,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:core_api,INTERFACE_INCLUDE_DIRECTORIES>
)

