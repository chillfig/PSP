#! /bin/bash
#
# author: claudio.olmi@nasa.gov
#
# Script will run the Functional Tests for gr740 on target.
# + Software Requirements:
#     - WindRiver environment
#     - IP network communication to the target (to load and run the tests)
#     - Serial communication to the target (to save the console logs)
#     - GNU screen (to establish the serial communication)
#
# + Revision:
#     - Initial
#
# + Known Issues:
#     - If the serial port is already being used, the script will continue but
#       no reports will be saved.
#     - If the target has not completed loading before starting this script,
#       the command to start loading the functional test will not work.


REPORT_FILENAME="psp_gr740-vxworks6.9_ft_results.log"

ERROR=`tput setaf 1`
SUCCESS=`tput setaf 2`
INFO=`tput setaf 4`
RESET=`tput sgr0`

RUN_PSP_HELP="To run PSP UT on Target\nSyntax: \$bash run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH] [TARGET_SERIAL] [TARGET_SERIAL_BAUD]\n"

SCRIPT_ROOT=$(readlink -f $(dirname $0))
CERT_TESTBED_ROOT=$(dirname $(dirname $(dirname $(dirname $SCRIPT_ROOT))))

# Check that the WindRiver environment is enabled
if ! command -v tgtsvr &> /dev/null
then
    echo "${ERROR}Please enable WindRiver environment${RESET}"
    exit 1
fi
# Check that the screen command is available
if ! command -v screen &> /dev/null
then
    echo "${ERROR}Please install the application `screen`${RESET}"
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

if [ -z $3 ]; then
    echo -e "Please enter target serial port? [TARGET_SERIAL]\n"
    echo -e $RUN_PSP_HELP
    exit 1
fi
TARGET_SERIAL=$3

if [ -z $4 ]; then
    echo -e "Using default 115200 for [TARGET_SERIAL_BAUD]\n"
    TARGET_SERIAL_BAUD=115200
elif
    TARGET_SERIAL_BAUD=$4
fi

TARGET_NAME="PSP_FT"

# Check that the zip application is available
DO_ZIP=1
if ! command -v zip &> /dev/null
then
    echo "ZIP application not available, HTML report will not be packged in a single file."
    DO_ZIP=0
fi

SCRIPT_ROOT=$CERT_TESTBED_ROOT/psp/fsw/gr740-vxworks6.9/functional_test

TCL_SCRIPT_ROOT=$SCRIPT_ROOT/tcl_scripts

cd $TCL_SCRIPT_ROOT

# Start the target servers
tgtsvr -V -n $TARGET_NAME -RW -Bt 3 -c $TARGET_KERNEL $TARGET_IP &

# Build log file full path
logfilename=$SCRIPT_ROOT"/"$REPORT_FILENAME
# Remove log file in case it is already there
rm $logfilename

# Prepare a screen configuration file
cat << EOT >> startup_screenrc
logfile $logfilename
EOT

# Start a screen session and save the output to file
screen  -L -c startup_screenrc -S PSP_FT_SCREEN -dm $TARGET_SERIAL $TARGET_SERIAL_BAUD

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

# Quit the screen session
screen -XS PSP_FT_SCREEN quit
# Delete screen configuration file
rm "startup_screenrc"

cd $SCRIPT_ROOT

# Extract summary of results
tail -2 $REPORT_FILENAME |  grep -oP ".*Passed: (\d+).*Failed: (\d+)"
