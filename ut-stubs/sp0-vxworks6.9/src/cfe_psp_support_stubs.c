/**
 ** \file
 **
 ** \brief API header for supporting various SP0(s) features
 **
 ** \copyright
 ** Copyright (c) 2019-2021 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** Functions provides support to the PSP API for restarting the target and
 ** changing the boot parameters.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_support header
 */

#include "cfe_psp_support.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_GetBootStartupString()
 * ----------------------------------------------------
 */
int32 CFE_PSP_GetBootStartupString(char *startupBootString, uint32 bufferSize, uint32 talkative)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_GetBootStartupString, int32);

    UT_GenStub_AddParam(CFE_PSP_GetBootStartupString, char *, startupBootString);
    UT_GenStub_AddParam(CFE_PSP_GetBootStartupString, uint32, bufferSize);
    UT_GenStub_AddParam(CFE_PSP_GetBootStartupString, uint32, talkative);

    UT_GenStub_Execute(CFE_PSP_GetBootStartupString, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_GetBootStartupString, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_GetBootStructure()
 * ----------------------------------------------------
 */
int32 CFE_PSP_GetBootStructure(BOOT_PARAMS *target_boot_parameters, uint32 talkative)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_GetBootStructure, int32);

    UT_GenStub_AddParam(CFE_PSP_GetBootStructure, BOOT_PARAMS *, target_boot_parameters);
    UT_GenStub_AddParam(CFE_PSP_GetBootStructure, uint32, talkative);

    UT_GenStub_Execute(CFE_PSP_GetBootStructure, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_GetBootStructure, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_PrintBootParameters()
 * ----------------------------------------------------
 */
void CFE_PSP_PrintBootParameters(BOOT_PARAMS *target_boot_parameters)
{
    UT_GenStub_AddParam(CFE_PSP_PrintBootParameters, BOOT_PARAMS *, target_boot_parameters);

    UT_GenStub_Execute(CFE_PSP_PrintBootParameters, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SetBootStartupString()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SetBootStartupString(char *startupScriptPath, uint32 talkative)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SetBootStartupString, int32);

    UT_GenStub_AddParam(CFE_PSP_SetBootStartupString, char *, startupScriptPath);
    UT_GenStub_AddParam(CFE_PSP_SetBootStartupString, uint32, talkative);

    UT_GenStub_Execute(CFE_PSP_SetBootStartupString, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SetBootStartupString, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SetBootStructure()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SetBootStructure(BOOT_PARAMS new_boot_parameters, uint32 talkative)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SetBootStructure, int32);

    UT_GenStub_AddParam(CFE_PSP_SetBootStructure, BOOT_PARAMS, new_boot_parameters);
    UT_GenStub_AddParam(CFE_PSP_SetBootStructure, uint32, talkative);

    UT_GenStub_Execute(CFE_PSP_SetBootStructure, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SetBootStructure, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_ToggleCFSBootPartition()
 * ----------------------------------------------------
 */
void CFE_PSP_ToggleCFSBootPartition(void)
{

    UT_GenStub_Execute(CFE_PSP_ToggleCFSBootPartition, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_KernelGetCRC()
 * ----------------------------------------------------
 */
uint32 CFE_PSP_KernelGetCRC(char *pCatalogEntryName, bool bFirstCatalog)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_KernelGetCRC, int32);

    UT_GenStub_AddParam(CFE_PSP_KernelGetCRC, char *, pCatalogEntryName);
    UT_GenStub_AddParam(CFE_PSP_KernelGetCRC, bool, bFirstCatalog);

    UT_GenStub_Execute(CFE_PSP_KernelGetCRC, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_KernelGetCRC, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_KernelLoadNew()
 * ----------------------------------------------------
 */

int32 CFE_PSP_KernelLoadNew(char *pKernelPath, char *pKernelCatalogName)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_KernelLoadNew, int32);

    UT_GenStub_AddParam(CFE_PSP_KernelLoadNew, char *, pKernelPath);
    UT_GenStub_AddParam(CFE_PSP_KernelLoadNew, char *, pKernelCatalogName);

    UT_GenStub_Execute(CFE_PSP_KernelLoadNew, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_KernelLoadNew, int32);
}
