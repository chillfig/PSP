#!/bin/bash

SW_DIR=$(git rev-parse --show-toplevel)

# -------------------------------------------------------------------------------------

function gen_tables_from_doxygen_comments()
{
    cd $SW_DIR/docs/python-docx

    echo -e "\n1. Generate Public API tables\n"

    python3 gen_api_tbls.py psp $SW_DIR/fsw/inc/cfe_psp.h "Common" && \
    mv cfe_PSP.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_common.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/shared/inc/cfe_psp_module.h "Module" && \
    mv cfe_PSP_module.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_module.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/shared/inc/cfe_psp_exceptionstorage_api.h "Exception Storage" && \
    mv cfe_PSP_exceptionstorage_api.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_exceptionstorage.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/shared/inc/cfe_psp_memory.h "Memory" && \
    mv cfe_PSP_memory.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_memory.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/cfe_psp_exception.h "SP0 Exceptions" && \
    mv cfe_PSP_exception.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_exceptions.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/cfe_psp_flash.h "FLASH" && \
    mv cfe_PSP_flash.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_flash.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/cfe_psp_memscrub.h "Memory Scrub" && \
    mv cfe_PSP_memscrub.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_memoryscrub.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/cfe_psp_sp0info.h "SP0 Data" && \
    mv cfe_PSP_sp0info.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_sp0.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/cfe_psp_start.h "Startup" && \
    mv cfe_PSP_start.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_startup.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/cfe_psp_support.h "Startup" && \
    mv cfe_PSP_support.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_support.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/shared/inc/cfe_psp_timesync.h "Time Sync" && \
    mv cfe_PSP_timesync.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_timesync.pdf

    
    mv -f *.docx $SW_DIR/docs
}

# -------------------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------------------

echo -e "\nProduct top-level directory: $SW_DIR\n"

gen_tables_from_doxygen_comments

echo -e "\nFinished!\n"

# -------------------------------------------------------------------------------------

