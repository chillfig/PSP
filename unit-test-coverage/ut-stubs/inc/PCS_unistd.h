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

/* PSP coverage stub replacement for unistd.h */
#ifndef PCS_UNISTD_H
#define PCS_UNISTD_H

#include "PCS_basetypes.h"
#include "PCS_sys_types.h"

/* ----------------------------------------- */
/* constants normally defined in unistd.h */
/* ----------------------------------------- */

#define PCS_SEEK_SET      0x1C01
#define PCS_SEEK_CUR      0x1C02
#define PCS_SEEK_END      0x1C03
#define PCS_STDIN_FILENO  0x1C04
#define PCS_STDOUT_FILENO 0x1C05
#define PCS_STDERR_FILENO 0x1C06

/* ----------------------------------------- */
/* types normally defined in unistd.h */
/* ----------------------------------------- */

/* ----------------------------------------- */
/* prototypes normally declared in unistd.h */
/* ----------------------------------------- */

extern int         PCS_close(int fd);
extern PCS_gid_t   PCS_getegid(void);
extern PCS_uid_t   PCS_geteuid(void);
extern long int    PCS_gethostid(void);
extern int         PCS_gethostname(char *name, size_t len);
extern PCS_pid_t   PCS_getpid(void);
extern PCS_off_t   PCS_lseek(int fd, PCS_off_t offset, int whence);
extern PCS_ssize_t PCS_read(int fd, void *buf, size_t nbytes);
extern int         PCS_rmdir(const char *path);
extern long int    PCS_sysconf(int name);
extern PCS_ssize_t PCS_write(int fd, const void *buf, size_t n);

#endif
