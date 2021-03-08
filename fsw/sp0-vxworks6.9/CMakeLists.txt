
######################################################################
#
# CMAKE build recipe for shared PSP component
#
######################################################################

# This contains supplemental code to support running CFE
# on a variety of targets.
#
# Note this shared PSP code is currently built against headers provided by the
# target implementation.  This makes it implementation-specific even though
# the same source code is used with multiple targets.

include_directories(inc)
include_directories(${MISSION_SOURCE_DIR}/cfe/fsw/cfe-core/src/time)

# Build the sp0-vxworks implementation as a library
add_library(psp-${CFE_PSP_TARGETNAME}-impl OBJECT
    src/cfe_psp_exception.c
    src/cfe_psp_memory.c
    src/cfe_psp_memtab.c
    src/cfe_psp_start.c
    src/cfe_psp_support.c
    src/cfe_psp_timer.c
    src/cfe_psp_watchdog.c)

# Add unit test, if enabled
if (ENABLE_UNIT_TESTS)
    add_subdirectory(unit_test)
endif (ENABLE_UNIT_TESTS)
