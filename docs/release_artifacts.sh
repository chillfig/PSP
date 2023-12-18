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

SW_DIR=$(git rev-parse --show-toplevel)

# -------------------------------------------------------------------------------------
# Main
# -------------------------------------------------------------------------------------

echo -e "\nProduct top-level directory: $SW_DIR\n"

echo -e "\n1. Go into the \"docs\" directory\n"
cd $SW_DIR/docs

echo -e "\n2. Make a \"release-artifacts\" directory\n"
mkdir -p psp-release-artifacts

echo -e "\n3. Build Section 5 of the SDD\n"
./build_dox.sh pdf sp0-vxworks6.9

echo -e "\n4. Generate tables from Doxygen comments\n"
./gen_tables.sh

echo -e "\n5. Move PDF files into \"release-artifacts\" directory\n"
mv *.pdf psp-release-artifacts

echo -e "\n6. Copy PDF version of Word documents from \"documentation\" branch to \"release-artifact\"\n"
git checkout origin/documentation masters
mv -f masters/*.pdf psp-release-artifacts

echo -e "\n7. Create a release-artifacts.tar file\n"
rm -f psp-release-artifacts.tar && \
tar -zcvf psp-release-artifacts.tgz psp-release-artifacts

echo -e "\n8. Remove unwanted files/dirs\n"
git reset HEAD masters
rm -rf psp-release-artifacts masters

echo -e "\nFinished!\n"

# -------------------------------------------------------------------------------------

