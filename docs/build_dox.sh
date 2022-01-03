#!/bin/bash

# NOTE: This shell script must run from the app's "docs" directory.

ucAPP=PSP_SP0-VXWORKS6.9
lcAPP=psp_sp0-vxworks6.9

function do_clean()
{
    rm -rf html latex
}

function do_html()
{
    do_clean && \
    mkdir -p latex && \
    cp dox_src/customs/*.tex latex && \
    doxygen dox_src/$lcAPP\_Doxyfile && \
    cp *.pdf html

    # Note that the last command is important to get the document links
    # to work properly for HTML.
}

function update_refman()
{
    HEAD='^\%--- Begin generated contents ---$'
    TAIL='^\\section{Data Structure Documentation}$'

    sed -e "/$HEAD/,/$TAIL/{ /$HEAD/{p; r dox_src/customs/refman_content_reorder.txt
           }; /$TAIL/p; d }" latex/refman.tex >& temp && \
    mv temp latex/refman.tex
}

function do_pdf()
{
    do_html && \
    update_refman && \
    cd latex && \
    make && \
    mv refman.pdf ../$ucAPP\_sdd_s5.pdf && \
    cd ..
}

if [ "$1" == "clean" ]; then
    do_clean
elif [ "$1" == "html" ]; then
    do_html && rm -rf latex
elif [ "$1" == "pdf" ]; then
    do_pdf && rm -rf html
elif [ "$1" == "all" ]; then
    do_pdf
else
    echo ""
    echo "Usage: ./build_dox.sh [clean|html|pdf|all]"
    echo ""
fi

