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
#include "cfe_psp_version.h"

/**********************************************************
 * 
 * Function: CFE_PSP_GetVersionString
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
const char *CFE_PSP_GetVersionString(void)
{
    return CFE_PSP_IMPL_VERSION;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetVersionCodeName
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
const char *CFE_PSP_GetVersionCodeName(void)
{
    return CFE_PSP_IMPL_CODENAME;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetVersionNumber
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_GetVersionNumber(uint8 VersionNumbers[4])
{
    VersionNumbers[0] = CFE_PSP_IMPL_MAJOR_VERSION;
    VersionNumbers[1] = CFE_PSP_IMPL_MINOR_VERSION;
    VersionNumbers[2] = CFE_PSP_IMPL_REVISION;
    VersionNumbers[3] = CFE_PSP_IMPL_MISSION_REV;
}

/**********************************************************
 * 
 * Function: CFE_PSP_GetBuildNumber
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
uint32 CFE_PSP_GetBuildNumber(void)
{
    return CFE_PSP_IMPL_BUILD_NUMBER;
}
