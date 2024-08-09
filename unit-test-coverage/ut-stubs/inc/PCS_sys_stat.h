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

/* PSP coverage stub replacement for sys/stat.h */
#ifndef PCS_SYS_STAT_H
#define PCS_SYS_STAT_H

#include "PCS_basetypes.h"

/* ----------------------------------------- */
/* constants normally defined in sys/stat.h */
/* ----------------------------------------- */

#define PCS_O_CREAT  0x101A
#define PCS_O_RDONLY 0x101B
#define PCS_O_RDWR   0x101C
#define PCS_S_IRWXU  0x1010

/* ----------------------------------------- */
/* types normally defined in sys/stat.h */
/* ----------------------------------------- */
struct PCS_stat
{
    int x;
};

/* ----------------------------------------- */
/* prototypes normally declared in sys/stat.h */
/* ----------------------------------------- */

extern int PCS_stat(const char *path, struct PCS_stat *buf);
extern int PCS_open(const char *path, int oflag, ...);

#endif
