*********************************************************************************************************
# Export Control Marking
 
All files under this top-level directory, including all files in its sub-directories, are subject to
NASA Export Control restrictions, as stated below:
 
EAR ECCN 9E515.a and/or 9E515.f (HALO)
 
    Export Administration Regulations (EAR) Notice
    This document contains information which falls under the purview of the Export Administration Regulations (EAR),
    15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
    or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
    furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
    or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
    of the Gateway MOUs and IA requires prior U.S. Government authorization.
**********************************************************************************************************
# GR740 implementation of PSP

Based on UT700 BSP

## VxWorks Kernel Requirements

Timers should be statically assigned.

gptimer0.0->sysclk
gptimer0.1->auxclk
gptimer0.2->timestamp

### Functions
### Components
