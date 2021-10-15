# Code Coverage for PSP on Target

Folder contains scripts to build and run Code Coverage on SP0 Targets

[[_TOC_]]

___

## Building PSP Unit Test for running on Target

All commands can be run from anywhere. Following commands assume that they are run from the _cert_testbed_ repo root folder.

1. Build PSP Unit Test application

   > ```$ sh scripts/ut_on_target/build_psp.sh```

   Above command will build the PSP Unit Test application and save it in the following folder:

   ```cert_testbed/scripts/ut_on_target/payload/```

2. Run PSP Unit Test application on Target

   > ```$ sh scripts/ut_on_target/run_psp.sh FC1```

   Above command will connect to FC1, load the PSP Unit Test application, run it, collect the data, convert the collected data to HTML, and finally compress the HTML report folder to a single file.

   HTML folder: ```cert_testbed/scripts/ut_on_target/html/```

   ZIP file: ```cert_testbed/scripts/ut_on_target/psp_ut_html_report.zip```

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

```$ sh scripts/ut_on_target/run_psp.sh FC1```

### vx_code_coverage.prj

Project file specifying the Code Coverage options

> <span style="color:teal">**NOTE 1**</span>: Verbosity tag [0-5]

> <span style="color:teal">**NOTE 2**</span>: Inlined and Included tags should be off. Otherwise compiler things that comments are code

### tcl_scripts/killtgtsvrs-FC1.tcl

TCL script for killing communication with FC1. It is run after the unit test is complete 

### tcl_scripts/killtgtsvrs-FC2.tcl

TCL script for killing communication with FC2. It is run after the unit test is complete

### tcl_scripts/runcoveragetests-FC1.tcl

TCL script for starting the communication with FC1 and running the unit test

> <span style="color:teal">**NOTE 1**</span>: The msleep command really depends on how long the test it. I have not yet found a way to detect when the unit test is complete. Also I do not know how to perform while loop in TCL.

### tcl_scripts/runcoveragetests-FC1.tcl

TCL script for starting the communication with FC2 and running the unit test

