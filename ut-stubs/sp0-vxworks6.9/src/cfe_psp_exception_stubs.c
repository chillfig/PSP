/**
 ** \file
 **
 ** \brief API header for collecting SP0(s) hardware and software information
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
 ** Functions here allow PSP to load and save exceptions from EEPROM.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_exception header
 */

#include "cfe_psp_exception.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_ClearNVRAM()
 * ----------------------------------------------------
 */
int32 CFE_PSP_ClearNVRAM(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_ClearNVRAM, int32);

    UT_GenStub_Execute(CFE_PSP_ClearNVRAM, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_ClearNVRAM, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_LoadFromNVRAM()
 * ----------------------------------------------------
 */
int32 CFE_PSP_LoadFromNVRAM(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_LoadFromNVRAM, int32);

    UT_GenStub_Execute(CFE_PSP_LoadFromNVRAM, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_LoadFromNVRAM, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SaveToNVRAM()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SaveToNVRAM(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SaveToNVRAM, int32);

    UT_GenStub_Execute(CFE_PSP_SaveToNVRAM, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SaveToNVRAM, int32);
}
