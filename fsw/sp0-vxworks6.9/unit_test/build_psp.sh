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
# NOTE: It seems that the compiler cannot build the instrumented code (code coverage)
# with the multithread gcc option (-j).
#

# From targets.cmake
TARGET_NAME="vx_es2"
TARGET_SYSTEM="powerpc-sp0-vxworks"
# From toolchain file
CFE_SYSTEM_PSPNAME="sp0-vxworks6.9"
COMPILER_PATH="${WIND_HOME}"/gnu/4.3.3-vxworks-6.9/x86-linux2/bin/ccppc

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

SKIP_COVERAGE=0

while getopts 'sh' flag; do
  case "${flag}" in
    s) SKIP_COVERAGE=1
       echo 'Building without coverage' ;;
    h) printf '\nPSP Unit Test\n\nUsage: %s [-s] [-h]\n\n\t[-s] to skip adding coverage\n\t[-h] to show this help\n\n' "$0"
       exit 2 ;;
    *) exit 1 ;;
  esac
done

# Delete all intermediate coverage files
rm "${SCRIPT_ROOT}"/db/*
rm "${SCRIPT_ROOT}"/html/*
rm "${SCRIPT_ROOT}"/runs/*

# Prepare PSP Unit Tests
cd "${CERT_TESTBED_ROOT}" || exit 1
make distclean
make prep TARGET="${TARGET_NAME}" ENABLE_UNIT_TESTS=1

# Add coverage if needed
if [ "${SKIP_COVERAGE}" = 0 ]; then
    ## Change Build.make with a custom made to add `coverage` before ccppc for shared object
    echo "build.make Modified"
    # /^.*tmp_code_coverage.*/ 
    sed -i "/^.*tmp_code_coverage.*/ s:${COMPILER_PATH}:${WIND_HOME}/workbench-3.3/analysis/host/bin/x86-linux2/coverage -project=${SCRIPT_ROOT}/vx_code_coverage.prj ${COMPILER_PATH}:g" ${PSP_UT_BUILD_FILE}
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
