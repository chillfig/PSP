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
COMPILER_PATH="${WIND_HOME}/gnu/4.3.3-vxworks-6.9/x86-linux2/bin/ccppc"

# Prepare all paths
SCRIPT_ROOT=$(readlink -f $(dirname $0))
CERT_TESTBED_ROOT=$(dirname $(dirname $(dirname $(dirname $SCRIPT_ROOT))))

SCRIPT_ROOT=$CERT_TESTBED_ROOT/psp/fsw/$CFE_SYSTEM_PSPNAME/unit_test
PSP_UT_IMPL_FOLDER=$CERT_TESTBED_ROOT/build/$TARGET_SYSTEM/default_$TARGET_NAME/psp/fsw/$CFE_SYSTEM_PSPNAME/unit_test
PSP_UT_BUILD_FILE=$PSP_UT_IMPL_FOLDER/CMakeFiles/psp_ut.dir/build.make

# Check that the WindRiver environment is enabled
if ! command -v vxprj &> /dev/null
then
    echo "Please enable WindRiver environment"
    exit
fi

SKIP_COVERAGE=0

while getopts 'sh' flag; do
  case "${flag}" in
    s) SKIP_COVERAGE=1
       echo 'Building without coverage' ;;
    h) printf '\nPSP Unit Test\n\nUsage: %s [-s] [-h]\n\n\t[-s] to skip adding coverage\n\t[-h] to show this help\n\n' $0
       exit 2 ;;
  esac
done

# Delete all intermediate coverage files
rm $SCRIPT_ROOT/db/*
rm $SCRIPT_ROOT/html/*
rm $SCRIPT_ROOT/runs/*

# Prepare PSP Unit Tests
cd $CERT_TESTBED_ROOT
make distclean
make prep TARGET=$TARGET_NAME ENABLE_UNIT_TESTS=1

# Add coverage if needed
if [ $SKIP_COVERAGE == 0 ]; then
    ## Change Build.make with a custom made to add `coverage` before ccppc for shared object
    echo "build.make Modified"
    # /^.*tmp_code_coverage.*/ 
    sed -i "/^.*tmp_code_coverage.*/ s:$COMPILER_PATH:${WIND_HOME}/workbench-3.3/analysis/host/bin/x86-linux2/coverage -project=${SCRIPT_ROOT}/vx_code_coverage.prj $COMPILER_PATH:g" $PSP_UT_BUILD_FILE
fi

# Build unit test
cd $PSP_UT_IMPL_FOLDER
make psp_ut

# TODO: Add method to detect if make psp_ut failed
if [ -f psp_ut.exe ]; then
    # Copy executable back
    if [ ! -d $SCRIPT_ROOT/payload ]; then
        mkdir $SCRIPT_ROOT/payload
    fi

    cp psp_ut.exe $SCRIPT_ROOT/payload
else 
    echo "Application was not found"
fi
