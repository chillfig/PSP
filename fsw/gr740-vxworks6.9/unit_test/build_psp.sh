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

# Script for building PSP Unit Test
#
# author: claudio.olmi@nasa.gov
#

# From targets.cmake
TARGET_NAME="vx_gr740"
TARGET_SYSTEM="sparcv8-gr740-vxworks"
# From toolchain file
CFE_SYSTEM_PSPNAME="gr740-vxworks6.9"
COMPILER_PATH="/users/acssl/GR740-BSP/sparc-wrs-vxworks-4.9/ccsparc"

# Prepare all paths
SCRIPT_ROOT=$(readlink -f "$(dirname "$0")")
CERT_TESTBED_ROOT=$(dirname "$(dirname "$(dirname "$(dirname "${SCRIPT_ROOT}")")")")

SCRIPT_ROOT=${CERT_TESTBED_ROOT}/psp/fsw/${CFE_SYSTEM_PSPNAME}/unit_test
PSP_UT_IMPL_FOLDER=${CERT_TESTBED_ROOT}/build/${TARGET_SYSTEM}/default_${TARGET_NAME}/psp/fsw/${CFE_SYSTEM_PSPNAME}/unit_test
PSP_UT_BUILD_FILE=${PSP_UT_IMPL_FOLDER}/CMakeFiles/psp_ut.dir/build.make

# Check that the WindRiver environment is enabled
if ! command -v vxprj > /dev/null
then
    echo "Please enable WindRiver environment"
    exit
fi

SKIP_PREP=0

while getopts 'sh' flag; do
  case "${flag}" in
    s) SKIP_PREP=1 ;;
    h) printf '\nUsage: %s [-s] [-h]\n\n' "$0"
       exit 2 ;;
    *) exit 1 ;;
  esac
done

# Prepare PSP Unit Tests
cd "${CERT_TESTBED_ROOT}" || exit 1
if [ "${SKIP_PREP}" = 0 ]; then
    make distclean
    make prep TARGET="${TARGET_NAME}" ENABLE_UNIT_TESTS=1

    # TODO: Add method to detect if make prep failed

    ## Change Build.make with a custom made to add `coverage` before ccppc for shared object
    # TODO: The sed command will find all occurrences of the use of ccppc compiler. This is not 
    # a desired feature since the stubs will be instrumented too and counted against the total %.
    echo "build.make Modified"
    sed -i "s:$COMPILER_PATH:${WIND_HOME}/workbench-3.3/analysis/host/bin/x86-linux2/coverage -project=${SCRIPT_ROOT}/vx_code_coverage.prj $COMPILER_PATH:g" $PSP_UT_BUILD_FILE
fi

# Build unit test
cd "${PSP_UT_IMPL_FOLDER}" || exit 1
make psp_ut

# TODO: Add method to detect if make psp_ut failed
if [ -f psp_ut.exe ]; then
    # Copy executable back
    if [ ! -d "${SCRIPT_ROOT}/payload" ]; then
        mkdir "${SCRIPT_ROOT}/payload"
    fi

    cp psp_ut.exe "${SCRIPT_ROOT}/payload"
else 
    echo "Application was not found"
fi
