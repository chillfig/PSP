#!/bin/bash

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

# -------------------------------------------------------------------------------
# NOTE: This shell script must run from the app's "docs" directory.
# -------------------------------------------------------------------------------


RED="\e[31m"
GREEN="\e[32m"
LBLUE="\e[94m"
ENDCOLOR="\e[0m"

# Show help if no arguments
if [ -z $1 ] || [ -z $2 ]; then
echo -e ${LBLUE}" "
cat << EOF

usage: build_dox.sh [ACTION] [PLATFORM]

required arguments:
  ACTION            One of [clean|html|pdf|all]
  PLATFORM          Name of the platform [sp0-vxworks6.9|gr740-vxworks6.9]

In case you are on ACSSL or SIMHOST(s) and don't have all .STY files, temporarely
include a path where the system search for the .STY files:
$ export TEXINPUTS=".:/users/acssl/texlive/2017/texmf-dist/tex/latex//:"

EOF
    echo -e ${ENDCOLOR}" "
    exit 1
fi

EXAMPLE_RUN="Usage: ./build_dox.sh all sp0-vxworks6.9\n"

# Convert platform name to upper and lower case
ucAPP=PSP_$(echo $2 | tr [:lower:] [:upper:])
lcAPP=psp_$(echo $2 | tr [:upper:] [:lower:])

# Prepares folder paths
SCRIPT_ROOT=$(readlink -f $(dirname $0))
PLATFORM_PATH=$(readlink --canonicalize $SCRIPT_ROOT/../fsw/$2)
DOCX_SRC_PATH=$SCRIPT_ROOT/dox_src_$lcAPP
DOXYFILE_PATH=$DOCX_SRC_PATH/$lcAPP\_Doxyfile

# If we are on simhost, add relevant ACSSL folder where the STY files are located
hn=$(hostname)
if [[ $hn == "simhost"* ]]; then
    echo "Adding TexLive Latex ACSSL folder"
    export TEXINPUTS=".:/users/acssl/texlive/2017/texmf-dist/tex/latex//:"
fi

# -------------------------------------------------------------------------------

function do_clean()
{
    rm -rf html latex
}

# -------------------------------------------------------------------------------

function do_html()
{
    do_clean && \
    mkdir -p latex && \
    cp $DOCX_SRC_PATH/customs/*.tex latex && \
    doxygen $DOXYFILE_PATH && \
    if [ -e *.pdf ]; then
        cp *.pdf html
    fi

    # Note that the last command is important to get the document links
    # to work properly for HTML.
}

# -------------------------------------------------------------------------------

function update_refman()
{
    HEAD='^\%--- Begin generated contents ---$'
    TAIL='^\\section{Data Structure Documentation}$'

    sed -e "/$HEAD/,/$TAIL/{ /$HEAD/{p; r dox_src/customs/refman_content_reorder.txt
           }; /$TAIL/p; d }" latex/refman.tex >& temp && \
    mv temp latex/refman.tex
}

# -------------------------------------------------------------------------------

function do_pdf()
{
    do_s5 
    ./gen_tables.sh 
}

# -------------------------------------------------------------------------------

function do_s5()
{
    do_html
    ./html_to_pdf.sh psp 
    rm -rf latex
}

# -------------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------------

# Verify that the platform folder exists
if [ ! -d $PLATFORM_PATH ]; then
   echo -e "\n${RED}I need the name of the platform: $PLATFORM_PATH not found${ENDCOLOR}"
   echo -e $EXAMPLE_RUN
   exit 0
fi

# Verify that the Doxyfile for the platform exists
if [ ! -f $DOXYFILE_PATH ]; then
   echo -e "\n${RED}Doxyfile for specific platform does not exists: $DOXYFILE_PATH not found${ENDCOLOR}"
   echo -e $EXAMPLE_RUN
   exit 0
fi

if [ "$1" == "clean" ]; then
    do_clean
elif [ "$1" == "html" ]; then
    do_html && rm -rf latex
elif [ "$1" == "pdf" ]; then
    do_pdf && rm -rf html
elif [ "$1" == "all" ]; then
    do_pdf
elif [ "$1" == "s5" ]; then 
    do_s5
else
    echo -e EXAMPLE_RUN
fi

echo -e "\nDocs built for $ucAPP\nwith Hash:" 
git rev-parse HEAD 

# -------------------------------------------------------------------------------
