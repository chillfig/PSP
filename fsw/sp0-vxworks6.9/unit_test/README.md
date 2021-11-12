# Code Coverage for PSP on Target

Folder contains scripts to build and run Code Coverage on SP0 Targets

[[_TOC_]]

___

## Building PSP Unit Test for running on Target

All commands can be run from anywhere. Following commands assume that they are run from the cert_testbed/psp/fsw/sp0-vxworks6.9/unit_test repo folder.

1. Build PSP Unit Test application

   > ```$ sh build_psp.sh```

   Above command will build the PSP Unit Test application and save it in the following folder:

   ```cert_testbed/psp/fsw/sp0-vxworks6.9/unit_test/payload/```

2. Run PSP Unit Test application on Target

   > ```$ sh run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH]```

   Above command will connect to *TARGET_IP*, load the PSP Unit Test application, run it, collect the data, convert the collected data to HTML, and finally compress the HTML report folder to a single ZIP file.

   HTML folder: ```cert_testbed/psp/fsw/sp0-vxworks6.9/unit_test/html/```

   ZIP file: ```cert_testbed/psp/fsw/sp0-vxworks6.9/unit_test/psp_ut_html_report.zip```

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

```$ sh run_psp.sh [TARGET_IP] [KERNEL_FILE_PATH]```

### vx_code_coverage.prj

Project file specifying the Code Coverage options

> <span style="color:teal">**NOTE 1**</span>: Verbosity tag [0-5]

> <span style="color:teal">**NOTE 2**</span>: Inlined and Included tags should be off. Otherwise compiler things that comments are code

### tcl_scripts/killtgtsvrs.tcl

TCL script for killing communication with the target. It is run after the unit test is complete

### tcl_scripts/runcoveragetests.tcl

TCL script for starting the communication with the target and running the unit test

> <span style="color:teal">**NOTE 1**</span>: The msleep command really depends on how long the test it. I have not yet found a way to detect when the unit test is complete. Also I do not know how to perform while loop in TCL.


## Results

### Percentages

- Function 99.5% (199/200)
- Block 98.2% (544/554)
- Decision 93.3% (332/356)
- Condition 91.5% (355/388)
