/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/
 
/************************************************************************
 * Note that this file has been cloned-n-owned from its open-source github
 * repository for Gateway as of September 2023. Therefore, it is subject to 
 * NASA Export Control restrictions, as stated below.
 ************************************************************************/

/************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
 *
 * Export Administration Regulations (EAR) Notice
 *
 * This document contains information which falls under the purview of the 
 * Export Administration Regulations (EAR), 15 CFR §730-774 and is export 
 * controlled. It may be used only to fulfill responsibilities of the Parties
 * of, or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, 
 * JAXA, MBRSC) and their contractors in furtherance of the Gateway MOUs 
 * with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer, or
 * disclosure to any party for any purpose other than the designated use of 
 * fulfilling the responsibilities of the Gateway MOUs and IA requires prior
 * U.S. Government authorization.
 *************************************************************************/

/* PSP coverage stub replacement for stdio.h */
#ifndef PCS_STDIO_H
#define PCS_STDIO_H

#include "PCS_basetypes.h"
#include "PCS_stdarg.h"

/* Include this for va_list */
#include <stdarg.h>

/* ----------------------------------------- */
/* constants normally defined in stdio.h */
/* ----------------------------------------- */

/* ----------------------------------------- */
/* types normally defined in stdio.h */
/* ----------------------------------------- */
typedef struct PCS_FILE PCS_FILE;

/* ----------------------------------------- */
/* prototypes normally declared in stdio.h */
/* ----------------------------------------- */

extern int       PCS_fclose(PCS_FILE *stream);
extern char *    PCS_fgets(char *s, int n, PCS_FILE *stream);
extern PCS_FILE *PCS_fopen(const char *filename, const char *modes);
extern int       PCS_fputs(const char *s, PCS_FILE *stream);
extern int       PCS_remove(const char *filename);
extern int       PCS_rename(const char *old, const char *nw);
extern int       PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
extern int       PCS_vsnprintf(char *s, size_t maxlen, const char *format, va_list arg);
extern int       PCS_printf(const char *format, ...);
extern int       PCS_putchar(int c);
extern void      PCS_perror(const char *str);

extern PCS_FILE *PCS_stdin;
extern PCS_FILE *PCS_stdout;
extern PCS_FILE *PCS_stderr;

#endif
