#!/bin/bash

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

