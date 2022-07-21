/**
 ** \file
 **
 ** \brief API header to control FLASH interaction
 **
 ** \copyright
 ** Copyright (c) 2019-2022 United States Government as represented by
 ** the Administrator of the National Aeronautics and Space Administration.
 ** All Rights Reserved.
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **
 ** \par Description:
 ** This file contains the function prototypes relating to FLASH interactions.
 **
 ** \par Limitations, Assumptions, External Events, and Notes:
 ** None
 */

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_psp_flash header
 */

#include "cfe_psp_flash.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_CheckFile()
 * ----------------------------------------------------
 */
bool CFE_PSP_CheckFile(char *fname)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_CheckFile, bool);

    UT_GenStub_AddParam(CFE_PSP_CheckFile, char *, fname);

    UT_GenStub_Execute(CFE_PSP_CheckFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_CheckFile, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_CreateDirectory()
 * ----------------------------------------------------
 */
int32 CFE_PSP_CreateDirectory(const char *p_dir)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_CreateDirectory, int32);

    UT_GenStub_AddParam(CFE_PSP_CreateDirectory, const char *, p_dir);

    UT_GenStub_Execute(CFE_PSP_CreateDirectory, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_CreateDirectory, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_CreateFile()
 * ----------------------------------------------------
 */
int32 CFE_PSP_CreateFile(char *fname)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_CreateFile, int32);

    UT_GenStub_AddParam(CFE_PSP_CreateFile, char *, fname);

    UT_GenStub_Execute(CFE_PSP_CreateFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_CreateFile, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_DeleteFile()
 * ----------------------------------------------------
 */
int32 CFE_PSP_DeleteFile(char *fname)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_DeleteFile, int32);

    UT_GenStub_AddParam(CFE_PSP_DeleteFile, char *, fname);

    UT_GenStub_Execute(CFE_PSP_DeleteFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_DeleteFile, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_ReadFromFlash()
 * ----------------------------------------------------
 */
int32 CFE_PSP_ReadFromFlash(uint32 *p_dest, size_t size, char *fname)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_ReadFromFlash, int32);

    UT_GenStub_AddParam(CFE_PSP_ReadFromFlash, uint32 *, p_dest);
    UT_GenStub_AddParam(CFE_PSP_ReadFromFlash, size_t, size);
    UT_GenStub_AddParam(CFE_PSP_ReadFromFlash, char *, fname);

    UT_GenStub_Execute(CFE_PSP_ReadFromFlash, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_ReadFromFlash, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_PSP_WriteToFlash()
 * ----------------------------------------------------
 */
int32 CFE_PSP_WriteToFlash(uint32 *p_src, size_t size, char *fname)
{
    UT_GenStub_SetupReturnBuffer(CFE_PSP_WriteToFlash, int32);

    UT_GenStub_AddParam(CFE_PSP_WriteToFlash, uint32 *, p_src);
    UT_GenStub_AddParam(CFE_PSP_WriteToFlash, size_t, size);
    UT_GenStub_AddParam(CFE_PSP_WriteToFlash, char *, fname);

    UT_GenStub_Execute(CFE_PSP_WriteToFlash, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_PSP_WriteToFlash, int32);
}