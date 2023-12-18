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

# Code Coverage for PSP on Target

Folder contains scripts to build and run Code Coverage on gr740 Targets

[[_TOC_]]

___

## Building PSP Unit Test for running on Target

All commands can be run from anywhere. Following commands assume that they are run from the cert_testbed/psp/fsw/gr740-vxworks6.9/unit_test repo folder.

1. Build PSP Unit Test application

   > ```$ sh build_psp.sh```

   Above command will build the PSP Unit Test application and save it in the following folder:

   ```cert_testbed/psp/fsw/gr740-vxworks6.9/unit_test/payload/```

2. Run PSP Unit Test application on Target

   > ```$ sh run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH] [TARGET_SERIAL]```

   Above command will connect to _TARGET_IP_, load the PSP Unit Test application, connect via serial port to capture the output, run the unit test, collect the data, convert the collected data to HTML, and finally compress the HTML report folder to a single ZIP file.

   HTML folder: ```cert_testbed/psp/fsw/gr740-vxworks6.9/unit_test/html/```

   ZIP file: ```cert_testbed/psp/fsw/gr740-vxworks6.9/unit_test/psp_ut_html_report.zip```

   > <span style="color:yellow">**WARNING**</span>: If the Unit Test crash, the script will still attempt to collected the data and run all the steps.

## Output Files Description

### runs/*

Binary data collected from target

### html/*

HTML report

### payload/*

Unit Test application

## Files Description

### build_psp.sh

Shell script to build the PSP unit test. Final application is save in _payload_ subfolder

### run_psp.sh

Shell script to run unit test on target. It requires target name as argument

```$ sh run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH] [TARGET_SERIAL]```

### vx_code_coverage.prj

Project file specifying the Code Coverage options

> <span style="color:teal">**NOTE 1**</span>: Verbosity tag [0-5]

> <span style="color:teal">**NOTE 2**</span>: Inlined and Included tags should be off. Otherwise compiler things that comments are code

### tcl_scripts/killtgtsvrs.tcl

TCL script for killing communication with the target. It is run after the unit test is complete

### tcl_scripts/runcoveragetests.tcl

TCL script for starting the communication with the target and running the unit test

## Results

### Sample Percentages

- Function 99.5% (199/200)
- Block 98.2% (544/554)
- Decision 93.3% (332/356)
- Condition 91.5% (355/388)
