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

/* PSP coverage stub replacement for cpusetCommon.h */
#ifndef OVERRIDE_CPUSETCOMMON_H
#define OVERRIDE_CPUSETCOMMON_H

#include <base/b_cpuset_t.h>
#include "PCS_cpusetCommon.h"

/* ----------------------------------------- */
/* mappings for declarations in cpusetCommon.h */
/* ----------------------------------------- */
#define CPUSET_SET PCS_CPUSET_SET
#define CPUSET_ISSET PCS_CPUSET_ISSET
#define CPUSET_CLR PCS_CPUSET_CLR
#define CPUSET_ZERO PCS_CPUSET_ZERO
#define CPUSET_ISZERO PCS_CPUSET_ISZERO

#endif
