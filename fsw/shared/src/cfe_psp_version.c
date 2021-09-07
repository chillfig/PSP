/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/**
 ** \file cfe_psp_version.c
 **
 ** \brief API to obtain the values of the various version identifiers
 **
 ** \par Description:
 **  GSC-18128-1, "Core Flight Executive Version 6.7"
 ** \n
 **  Copyright (c) 2006-2019 United States Government as represented by
 **  the Administrator of the National Aeronautics and Space Administration.
 **  All Rights Reserved.
 ** \n
 **  Licensed under the Apache License, Version 2.0 (the "License");
 **  you may not use this file except in compliance with the License.
 **  You may obtain a copy of the License at
 ** \n
 **    http://www.apache.org/licenses/LICENSE-2.0
 ** \n
 **  Unless required by applicable law or agreed to in writing, software
 **  distributed under the License is distributed on an "AS IS" BASIS,
 **  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 **  See the License for the specific language governing permissions and
 **  limitations under the License.
 */

#include "cfe_psp.h"
#include "psp_version.h"

/**
 ** \func Obtain the PSP version/baseline identifier string
 **
 ** \par Description:
 ** This retrieves the PSP version identifier string without extra info.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \returns Version string. This is a fixed string and cannot be NULL.
 */
const char *CFE_PSP_GetVersionString(void)
{
    return CFE_PSP_IMPL_VERSION;
}

/**
 ** \func Obtain the version code name
 **
 ** \par Description:
 ** This retrieves the PSP code name.\n This is a compatibility indicator for the
 ** overall NASA CFS ecosystem.\n All modular components which are intended to
 ** interoperate should report the same code name.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \returns Code name.  This is a fixed string and cannot be NULL.
 */
const char *CFE_PSP_GetVersionCodeName(void)
{
    return CFE_PSP_IMPL_CODENAME;
}

/**
 ** \func Obtain the PSP numeric version numbers as uint8 values
 **
 ** \par Description:
 ** This retrieves the numeric PSP version identifier as an array of 4 uint8 values.
 ** \n
 ** The array of numeric values is in order of precedence:
 ** [0] = Major Number
 ** [1] = Minor Number
 ** [2] = Revision Number
 ** [3] = Mission Revision
 ** \n
 ** The "Mission Revision" (last output) also indicates whether this is an
 ** official release, a patched release, or a development version.
 ** 0 indicates an official release
 ** 1-254 local patch level (reserved for mission use)
 ** 255 indicates a development build
 ** 
 ** 
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[out] VersionNumbers  A fixed-size array to be filled with the version numbers
 **
 ** \return None
 */
void CFE_PSP_GetVersionNumber(uint8 VersionNumbers[4])
{
    VersionNumbers[0] = CFE_PSP_IMPL_MAJOR_VERSION;
    VersionNumbers[1] = CFE_PSP_IMPL_MINOR_VERSION;
    VersionNumbers[2] = CFE_PSP_IMPL_REVISION;
    VersionNumbers[3] = CFE_PSP_IMPL_MISSION_REV;
}

/**
 ** \func Obtain the PSP library numeric build number
 **
 ** \par Description:
 ** The build number is a monotonically increasing number that (coarsely)
 ** reflects the number of commits/changes that have been merged since the
 ** epoch release. During development cycles this number should increase
 ** after each subsequent merge/modification.
 ** \n
 ** Like other version information, this is a fixed number assigned at compile time.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \returns The OSAL library build number
 */
uint32 CFE_PSP_GetBuildNumber(void)
{
    return CFE_PSP_IMPL_BUILD_NUMBER;
}
