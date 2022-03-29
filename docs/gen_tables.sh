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

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/psp_exceptions.h "SP0 Exceptions" && \
    mv PSP_exceptions.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_exceptions.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/psp_flash.h "FLASH" && \
    mv PSP_flash.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_flash.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/psp_mem_scrub.h "Memory Scrub" && \
    mv PSP_mem_scrub.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_memoryscrub.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/psp_sp0_info.h "SP0 Data" && \
    mv PSP_sp0_info.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_sp0.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/psp_start.h "Startup" && \
    mv PSP_start.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_startup.pdf

    python3 gen_api_tbls.py psp $SW_DIR/fsw/sp0-vxworks6.9/inc/psp_time_sync.h "Time Sync" && \
    mv PSP_time_sync.pdf $SW_DIR/docs/PSP_sp0-vxworks6.9_timesync.pdf

    echo -e "\n2. Remove un-needed files\n"
    rm -f *.docx
}

# -------------------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------------------

echo -e "\nProduct top-level directory: $SW_DIR\n"

gen_tables_from_doxygen_comments

echo -e "\nFinished!\n"

# -------------------------------------------------------------------------------------

