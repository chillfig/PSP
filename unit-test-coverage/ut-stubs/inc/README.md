*********************************************************************************************************
# Export Control Marking
 
The code base for this product has been cloned-n-owned from its open-source github repository for Gateway
as of September 2023. Therefore, all files under this top-level directory, including all files in its
sub-directories, are subject to NASA Export Control restrictions, as stated below:
 
EAR ECCN 9E515.a and/or 9E515.f (HALO)
  
    Export Administration Regulations (EAR) Notice
    This document contains information which falls under the purview of the Export Administration Regulations (EAR),
    15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
    or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
    furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
    or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
    of the Gateway MOUs and IA requires prior U.S. Government authorization.
**********************************************************************************************************

ABOUT THE SYSTEM HEADER OVERRIDES
=================================

The "overrides" directory contains replacement versions of many system-provided 
header files.  All replacement functions and types are identified using an 
`OCS_` prefix.

The header file that a particular source will use depends on the 
way that the compiler include path is set up.  This is intentional.

1. For test support code:

This refers to all code _other_ than the unit actually being tested, including
stub function implementations and test configuration.
 
All coverage test recipes should include `ut-stubs/inc` in the INCLUDE_DIRECTORIES.
These prototypes can be explicitly obtained by putting an `overrides/` prefix
on the #include statement, for instance:
    
    #include <stdio.h>              <-- Gets the regular system version as usual
    #include <overrides/stdio.h>    <-- Gets this replacement OCS version
    
Note that the two shouldn't conflict, so it is possible to include both where needed.
  
2. For code units under test:

When compiling the actual unit under test with coverage instrumentation,
the CMake recipe should add "ut-stubs/inc/overrides" into the INCLUDE_DIRECTORIES.
This way, all source files compiled will find these replacements instead of
the regular system header file.  So a statement like:

    #include <stdio.h>              <-- Gets this replacement OCS version
    
This way it is possible to simply recompile the original source files and all the
included system headers will be transparently replaced with these override versions.

When using the file in this way, one must also provide a companion macro to
divert the actual usage to the OCS namespace as well.  For instance:

    #define fputs   OCS_fputs
    
Will cause all references to `fputs` in the subsequent code to refer to `OCS_fputs`
instead.  

Important Notes
---------------

Some notable exclusions exist to this general pattern.  These are cases where
attempting to provide a reasonable alternate for the system header file would 
be very difficult to do in a portable manner, so we must allow the system
header to be used.  These cases are:

    stdint.h  : Fixed-width types need to be correct for the host CPU.
    limits.h  : The MIN/MAX macros need to be correct
    stddef.h  : Macros like offsetof() and size_t/ptrdiff_t must be correct for the CPU
    stdbool.h : Preserves correct boolean semantics

 
Note that header files from all supported platforms (VxWorks, Rtems, Posix) may
all be placed here.  Since the files are empty placeholders, they simply need to
exist, and there is no need to duplicate this entire tree for every OS since they
will overlap considerably.
  