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
 ** Functions here allow CFS to provide a method to probe
 ** SP0 hardware for information from POST, Temperatures, Voltages, Active
 ** Boot EEPROM, etc. In addition, this module has a function to save a
 ** dump_core text file before aborting CFS execution.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 **
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_sp0info header
 */

/*
** USER ADDED CODE START
*/

/*
** USER ADDED CODE END
*/
#include "cfe_psp_sp0info.h"
// #include "cfe_psp_sp0info_stub.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0DumpData()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0DumpData(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0DumpData, int32);

    UT_GenStub_Execute(CFE_PSP_SP0DumpData, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0DumpData, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0GetDiskFreeSize()
 * ----------------------------------------------------
 */
int64_t CFE_PSP_SP0GetDiskFreeSize(char *ram_disk_root_path)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0GetDiskFreeSize, int64_t);

    UT_GenStub_AddParam(CFE_PSP_SP0GetDiskFreeSize, char *, ram_disk_root_path);

    UT_GenStub_Execute(CFE_PSP_SP0GetDiskFreeSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0GetDiskFreeSize, int64_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0GetInfo()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0GetInfo(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0GetInfo, int32);

    UT_GenStub_Execute(CFE_PSP_SP0GetInfo, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0GetInfo, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0GetInfoTable()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0GetInfoTable(CFE_PSP_SP0InfoTable_t *sp0_info, uint8_t print_to_console)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0GetInfoTable, int32);

    UT_GenStub_AddParam(CFE_PSP_SP0GetInfoTable, CFE_PSP_SP0InfoTable_t *, sp0_info);
    UT_GenStub_AddParam(CFE_PSP_SP0GetInfoTable, uint8_t, print_to_console);

    UT_GenStub_Execute(CFE_PSP_SP0GetInfoTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0GetInfoTable, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0ROM1Lock()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0ROM1Lock(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0ROM1Lock, int32);

    UT_GenStub_Execute(CFE_PSP_SP0ROM1Lock, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0ROM1Lock, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0ROM1Status()
 * ----------------------------------------------------
 */
bool CFE_PSP_SP0ROM1Status(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0ROM1Status, bool);

    UT_GenStub_Execute(CFE_PSP_SP0ROM1Status, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0ROM1Status, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0ROM1Unlock()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0ROM1Unlock(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0ROM1Unlock, int32);

    UT_GenStub_Execute(CFE_PSP_SP0ROM1Unlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0ROM1Unlock, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0ROM2Lock()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0ROM2Lock(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0ROM2Lock, int32);

    UT_GenStub_Execute(CFE_PSP_SP0ROM2Lock, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0ROM2Lock, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0ROM2Status()
 * ----------------------------------------------------
 */
bool CFE_PSP_SP0ROM2Status(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0ROM2Status, bool);

    UT_GenStub_Execute(CFE_PSP_SP0ROM2Status, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0ROM2Status, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_SP0ROM2Unlock()
 * ----------------------------------------------------
 */
int32 CFE_PSP_SP0ROM2Unlock(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_SP0ROM2Unlock, int32);

    UT_GenStub_Execute(CFE_PSP_SP0ROM2Unlock, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_SP0ROM2Unlock, int32);
}
