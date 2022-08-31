# Script for building PSP Unit Test
#
# author: claudio.olmi@nasa.gov
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

SKIP_PREP=0

while getopts 'sh' flag; do
  case "${flag}" in
    s) SKIP_PREP=1 ;;
    h) printf '\nUsage: %s [-s] [-h]\n\n' $0
       exit 2 ;;
  esac
done

# Prepare PSP Unit Tests
cd $CERT_TESTBED_ROOT
if [ $SKIP_PREP == 0 ]; then
    make distclean
    make prep TARGET=$TARGET_NAME ENABLE_UNIT_TESTS=1

    # TODO: Add method to detect if make prep failed

    ## Change Build.make with a custom made to add `coverage` before ccppc for shared object
    # TODO: The sed command will find all occurrences of the use of ccppc compiler. This is not 
    # a desired feature since the stubs will be instrumented too and counted against the total %.
    echo "build.make Modified"
    sed -i "s:$COMPILER_PATH:${WIND_HOME}/workbench-3.3/analysis/host/bin/x86-linux2/coverage -project=${SCRIPT_ROOT}/vx_code_coverage.prj $COMPILER_PATH:g" $PSP_UT_BUILD_FILE
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
