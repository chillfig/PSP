
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
