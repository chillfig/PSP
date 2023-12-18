#! /bin/bash
#
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

# author: claudio.olmi@nasa.gov
#
# Script will run the Unit Tests for gr740 on target.
# + software Requirements:
#     - WindRiver environment
#     - IP network communication to the target (to load and run the tests)
#     - Serial communication to the target (to save the console logs)
#     - GNU screen (to establish the serial communication)
#     - xmllint (to extract data from results)
#
# + Revision:
#     - Initial
#
# + Known Issues:
#     - If the serial port is already being used, the script will continue but
#       no reports will be saved.
#     - If the target has not completed loading before starting this script,
#       the command to start loading the functional test will not work.

REPORT_FILENAME="psp_gr740-vxworks6.9_ut_results.log"
ZIP_COVERAGE_FILENAME="psp_gr740-vxworks6.9_ut_coverage.zip"

ERROR=`tput setaf 1`
SUCCESS=`tput setaf 2`
INFO=`tput setaf 4`
RESET=`tput sgr0`

RUN_PSP_HELP="To run PSP UT on Target\nSyntax: \$bash run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH] [TARGET_SERIAL]\n"

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
# Check that the xmllint command is available
if ! command -v xmllint &> /dev/null
then
    echo "${ERROR}Please install the application `xmllint`${RESET}"
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

TARGET_NAME="PSP_UT"

# Check that the zip application is available
DO_ZIP=1
if ! command -v zip &> /dev/null
then
    echo "ZIP application not available, HTML report will not be packged in a single file."
    DO_ZIP=0
fi

SCRIPT_ROOT=$CERT_TESTBED_ROOT/psp/fsw/gr740-vxworks6.9/unit_test

TCL_SCRIPT_ROOT=$SCRIPT_ROOT/tcl_scripts
RUNS_ROOT=$SCRIPT_ROOT/runs
HTML_ROOT=$SCRIPT_ROOT/html

cd $TCL_SCRIPT_ROOT

# Start the target servers
tgtsvr -V -n $TARGET_NAME -RW -Bt 3 -c $TARGET_KERNEL $TARGET_IP &

# Build log file full path
logfilename=$SCRIPT_ROOT"/"$REPORT_FILENAME
# Remove log and zip files in case they are already there
if [ -e $logfilename ]; then
    rm $logfilename
fi
if [ -e $SCRIPT_ROOT"/"$ZIP_COVERAGE_FILENAME ]; then
    rm $SCRIPT_ROOT"/"$ZIP_COVERAGE_FILENAME
fi

# Prepare a screen configuration file
cat << EOT >> startup_screenrc
logfile $logfilename
EOT

# Start a screen session and save the output to file
screen -L -c startup_screenrc -S PSP_UT_SCREEN -dm $TARGET_SERIAL

echo "${INFO}Starting tests on $TARGET_NAME${RESET}"
# Call tcl script that downloads the object modules from the
# target and runs the tests
wtxtcl run_coverage_tests.tcl

# Remove coverage data and reports from the last run
rm -f $RUNS_ROOT/*
rm -f $SCRIPT_ROOT/html/*

echo "${INFO}Waiting to complete...${RESET}"
sleep 2


echo "${INFO}Download from Target Coverage Data...${RESET}"
# Use coverageupload to upload the coverage data from the targets
coverageupload -configuration ~/pne_vxworks_69_Config -t $TARGET_NAME -f $RUNS_ROOT/psp_ut_coverage.run -p $SCRIPT_ROOT/vx_code_coverage.prj -d 3 -data ~

sleep 2

echo "${INFO}Preparing HTML output...${RESET}"

# Merge the coverage results into one run
coverageconvert -configuration ~/pne_vxworks_69_Config -f $RUNS_ROOT/psp_ut_coverage.run -b -c -n -m $RUNS_ROOT/psp_ut_report.run -l $HTML_ROOT

echo "${INFO}Preparing ZIP of HTML output...${RESET}"

# Quit the screen session
screen -XS PSP_UT_SCREEN quit
# Delete screen configuration file
rm "startup_screenrc"

sleep 1

echo "${INFO}Kill tgtsvr...${RESET}"

# Shutdown the target servers
wtxtcl kill_target_service.tcl

cd $SCRIPT_ROOT

# Pack all html files into a single zip file
if [ $DO_ZIP == 1 ]; then
    echo "${SUCCESS}Packing HTML folder to $ZIP_COVERAGE_FILENAME${RESET}"
    zip -r -q $ZIP_COVERAGE_FILENAME html
fi

# Extract Code Coverage Results from HTML
RESULT_ALL=`xmllint --xpath '//html/body/table[last()]/tr' --html $HTML_ROOT/main.html | sed -e 's/<[^>]*>//g'`

RESULT_FUNCTION=`egrep -o '<TR><TH ALIGN=left>Function.*</TD></TR>' $HTML_ROOT/main.html |  sed -e 's/<[^>]*>//g' | awk -F':| ' '{print $1, $2, $3}'`
RESULT_BLOCK=`egrep -o '<TR><TH ALIGN=left>Block.*</TD></TR>' $HTML_ROOT/main.html |  sed -e 's/<[^>]*>//g' | awk -F':| ' '{print $1, $2, $3}'`
RESULT_DECISION=`egrep -o '<TR><TH ALIGN=left>Decision.*</TD></TR>' $HTML_ROOT/main.html |  sed -e 's/<[^>]*>//g' | awk -F':| ' '{print $1, $2, $3}'`
RESULT_CONDITION=`egrep -o '<TR><TH ALIGN=left>Condition.*</TD></TR>' $HTML_ROOT/main.html |  sed -e 's/<[^>]*>//g'  | awk -F':| ' '{print $1, $2, $3}'`

echo $RESULT_FUNCTION
echo $RESULT_BLOCK
echo $RESULT_DECISION
echo $RESULT_CONDITION
