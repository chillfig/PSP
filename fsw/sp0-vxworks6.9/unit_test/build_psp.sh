# Script for building PSP Unit Test
#
# author: claudio.olmi@nasa.gov
#

BUILD_PSP_HELP="To build PSP UT for running on Target\nSyntax: \$bash build_psp.sh [CERT_TESTBED_ROOT_PATH]\n"

# Check if the user provided the cert_testbed root path
if [ -z $1 ]; then
    echo -e "Missing the cert_testbed root path\n"
    echo -e $BUILD_PSP_HELP
    exit 1
fi
CERT_TESTBED_ROOT=$1

SCRIPT_ROOT=$CERT_TESTBED_ROOT/psp/fsw/sp0-vxworks6.9/unit_test
PSP_UT_IMPL_FOLDER=$CERT_TESTBED_ROOT/build/powerpc-sp0-vxworks/default_vx_es2/psp/fsw/sp0-vxworks6.9/unit_test
PSP_UT_BUILD_FOLDER=$PSP_UT_IMPL_FOLDER/CMakeFiles/psp_ut.dir/build.make

# Check that the WindRiver environment is enabled
if ! command -v vxprj &> /dev/null
then
    echo "Please enable WindRiver environment"
    exit
fi

# Prepare PSP Unit Tests
cd $CERT_TESTBED_ROOT
make distclean
make prep TARGET=vx_es2 ENABLE_UNIT_TESTS=1

# TODO: Add method to detect if make prep failed

## Change Build.make with a custom made to add `coverage` before ccppc for shared object
# TODO: The sed command will find all occurrences of the use of ccppc compiler. This is not 
# a desired feature since the stubs will be instrumented too and counted against the total %.
echo "build.make Modified"
sed -i "s:$WIND_HOME/gnu/4.3.3-vxworks-6.9/x86-linux2/bin/ccppc:$WIND_HOME/workbench-3.3/analysis/host/bin/x86-linux2/coverage -project=$SCRIPT_ROOT/vx_code_coverage.prj $WIND_HOME/gnu/4.3.3-vxworks-6.9/x86-linux2/bin/ccppc:g" $PSP_UT_BUILD_FOLDER

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