##########################################################################################################
#  Note that this file has been cloned-n-owned from its open-source github
#  repository for Gateway as of September 2023. Therefore, it is subject to
#  NASA Export Control restrictions, as stated below.
##########################################################################################################
 
##########################################################################################################
# Export Control Marking
#
# EAR ECCN 9E515.a and/or 9E515.f (HALO)
#
# Export Administration Regulations (EAR) Notice
# This document contains information which falls under the purview of the Export Administration Regulations (EAR),
# 15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
# or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
# furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
# or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
# of the Gateway MOUs and IA requires prior U.S. Government authorization.
##########################################################################################################

# Script generates the stubs for a PSP Target and adds them to the PSP ut-stubs
# folder.

GENERATES_STUB_HELP="\nScript generates PSP stubs for a new target\nSyntax: \$sh generates_stubs.sh [TARGET_NAME]\n"

if [ -z $1]; then
    echo -e $GENERATES_STUB_HELP
    exit 1
fi

PSP_TARGET=$1

# Prepare all paths
SCRIPT_ROOT=$(readlink -f $(dirname $0))
CERT_TESTBED_ROOT=$(dirname $(dirname $SCRIPT_ROOT))

PERL_TOOL_PATH="$CERT_TESTBED_ROOT/osal/ut_assert/scripts/generate_stubs.pl"
NEW_STUB_PATH="$CERT_TESTBED_ROOT/psp/ut-stubs/$PSP_TARGET"
PSP_HEADER_FILES_PATH="$CERT_TESTBED_ROOT/psp/fsw/$PSP_TARGET/inc"

# Check that PERL tool is accessible
if [ ! -f $PERL_TOOL_PATH ]; then
    echo -e "generate_stubs.pl could not be found at $PERL_TOOL_PATH\n"
    echo -e $GENERATES_STUB_HELP
    exit 1
fi

# Check that new PSP target is accessible
if [ ! -d $PSP_HEADER_FILES_PATH ]; then
    echo -e "PSP Target could not be found at $PSP_HEADER_FILES_PATH\n"
    echo -e $GENERATES_STUB_HELP
    exit 1
fi

# Check the perl missing command?
if ! command -v perl &> /dev/null
then
    echo "Perl is not installed in your environment"
    exit 1
fi

# Check the clang-format missing command?
if ! command -v clang-format &> /dev/null
then
    echo "clang-format is not installed in your environment"
    echo -e $GENERATES_STUB_HELP
    exit 1
fi

perl $PERL_TOOL_PATH $NEW_STUB_PATH $PSP_HEADER_FILES_PATH/*
