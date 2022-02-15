#! /bin/bash
#
# author: claudio.olmi@nasa.gov
#

ERROR=`tput setaf 1`
SUCCESS=`tput setaf 2`
INFO=`tput setaf 4`
RESET=`tput sgr0`

RUN_PSP_HELP="To run PSP UT on Target\nSyntax: \$bash run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH]\n"

SCRIPT_ROOT=$(readlink -f $(dirname $0))
CERT_TESTBED_ROOT=$(dirname $(dirname $(dirname $(dirname $SCRIPT_ROOT))))

# Check that the WindRiver environment is enabled
if ! command -v tgtsvr &> /dev/null
then
    echo "Please enable WindRiver environment"
    exit 1
fi

# Check that the WindRiver communication tool daemon is active
WTXREGD=`ps ax | grep wtxregd | grep workbench`
if [ -z "$WTXREGD" ]
then
    echo "WTX Register Daemon not found."
    echo -e "Run: $ wtxregd start\n"
    exit 1
fi

# Check if user specified the flight computer
if [ -z $1 ]; then
    echo -e "Which flight computer to connect to? [TARGET_IP]\n"
    echo -e $RUN_PSP_HELP
    exit 1
fi
TARGET_IP=$1

# Check if user specified the flight computer
if [ -z $2 ]; then
    echo -e "Where is the Kernel file that is loaded on the target? [KERNEL_FILE_PATH]\n"
    echo -e $RUN_PSP_HELP
    exit 1
fi
if [ ! -f $2 ]; then
    echo -e "${ERROR}Provided Kernel file does not exists\n${RESET}"
    exit 1
fi
TARGET_KERNEL=$2

TARGET_NAME="PSP_FT"

WTXREGD=`ps ax | grep wtxregd | grep workbench`
if [ -z "$WTXREGD" ]
then
    echo "${ERROR}WTX Register Daemon not found${RESET}"
    echo "Run: $ wtxregd start"
    exit
fi

# Check that the zip application is available
DO_ZIP=1
if ! command -v zip &> /dev/null
then
    echo "ZIP application not available, HTML report will not be packged in a single file."
    DO_ZIP=0
fi

SCRIPT_ROOT=$CERT_TESTBED_ROOT/psp/fsw/sp0-vxworks6.9/functional_test

TCL_SCRIPT_ROOT=$SCRIPT_ROOT/tcl_scripts
RUNS_ROOT=$SCRIPT_ROOT/runs
HTML_ROOT=$SCRIPT_ROOT/html

cd $TCL_SCRIPT_ROOT

# Start the target servers
tgtsvr -V -n $TARGET_NAME -RW -Bt 3 -c $TARGET_KERNEL $TARGET_IP &

echo "${INFO}Starting tests on $TARGET_NAME${RESET}"
# Call tcl script that downloads the object modules from the
# target and runs the tests
wtxtcl run_functional_tests.tcl

echo "${INFO}Waiting to complete...${RESET}"
sleep 2

echo "${INFO}Kill tgtsvr...${RESET}"
# Shutdown the target servers
wtxtcl kill_target_service.tcl

sleep 2

cd $SCRIPT_ROOT
