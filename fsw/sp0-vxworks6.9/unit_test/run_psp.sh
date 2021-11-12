#! /bin/bash
#
# author: claudio.olmi@nasa.gov
#

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
TARGET_KERNEL=$2

TARGET_NAME="PSP_UT_FC"

WTXREGD=`ps ax | grep wtxregd | grep workbench`
if [ -z "$WTXREGD" ]
then
    echo "WTX Register Daemon not found."
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

SCRIPT_ROOT=$CERT_TESTBED_ROOT/psp/fsw/sp0-vxworks6.9/unit_test

TCL_SCRIPT_ROOT=$SCRIPT_ROOT/tcl_scripts
RUNS_ROOT=$SCRIPT_ROOT/runs
HTML_ROOT=$SCRIPT_ROOT/html

cd $TCL_SCRIPT_ROOT

# Start the target servers
tgtsvr -V -n $TARGET_NAME -RW -Bt 3 -c $TARGET_KERNEL $TARGET_IP &

echo "Starting tests on $TARGET_NAME"
# Call tcl script that downloads the object modules from the
# target and runs the tests
wtxtcl runcoveragetests.tcl

# Remove coverage data and reports from the last run
rm -f $SCRIPT_ROOT/psp_ut*.zip
rm -f $RUNS_ROOT/*
rm -f $SCRIPT_ROOT/html/*

echo "Waiting to complete..."
sleep 2

# Use coverageupload to upload the coverage data from the targets
coverageupload -configuration ~/pne_vxworks_69_Config -t $TARGET_NAME -f $RUNS_ROOT/psp_ut_coverage.run -p $SCRIPT_ROOT/vx_code_coverage.prj -d 3 -data ~

# Shutdown the target servers
wtxtcl killtgtsvrs.tcl

sleep 2

# Merge the coverage results into one run
coverageconvert -configuration ~/pne_vxworks_69_Config -f $RUNS_ROOT/psp_ut_coverage.run -b -c -n -m $RUNS_ROOT/psp_ut_report.run -l $HTML_ROOT

cd $SCRIPT_ROOT

# Pack all html files into a single zip file
if [ $DO_ZIP == 1 ]; then
    echo "Packing HTML folder to psp_ut_html_report.zip"
    zip -r -q psp_ut_html_report.zip html
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
