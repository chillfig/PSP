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

# Script for building PSP Functional Test
#
# author: claudio.olmi@nasa.gov
#

# From targets.cmake
TARGET_NAME="vx_gr740"
TARGET_SYSTEM="sparcv8-gr740-vxworks"
# From toolchain file
CFE_SYSTEM_PSPNAME="gr740-vxworks6.9"

# Prepare all paths
SCRIPT_ROOT=$(readlink -f "$(dirname "$0")")
CERT_TESTBED_ROOT=$(dirname "$(dirname "$(dirname "$(dirname "${SCRIPT_ROOT}")")")")

SCRIPT_ROOT=${CERT_TESTBED_ROOT}/psp/fsw/${CFE_SYSTEM_PSPNAME}/functional_test
PSP_FT_IMPL_FOLDER=${CERT_TESTBED_ROOT}/build/${TARGET_SYSTEM}/default_${TARGET_NAME}/psp/fsw/${CFE_SYSTEM_PSPNAME}/functional_test

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
    # Create empty files so they get picked up by CMAKE
    install -D /dev/null build/src/cfe_build_env_table.c
    install -D /dev/null build/src/cfe_module_version_table.c
    make prep TARGET="${TARGET_NAME}" ENABLE_FUNCTIONAL_TESTS=1
fi

cd "${CERT_TESTBED_ROOT}/build" || exit 1
make mission-version

# Build unit test
cd "${PSP_FT_IMPL_FOLDER}" || exit 1
make psp_ft

# TODO: Add method to detect if make psp_ft failed
if [ -f psp_ft.exe ]; then
    # Copy executable back
    if [ ! -d "${SCRIPT_ROOT}/payload" ]; then
        mkdir "${SCRIPT_ROOT}/payload"
    fi

    cp psp_ft.exe "${SCRIPT_ROOT}/payload"
else 
    echo "Application was not found"
fi
