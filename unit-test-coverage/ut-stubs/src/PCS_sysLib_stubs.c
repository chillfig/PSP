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
 * @file
 *
 * Auto-Generated stub implementations for functions defined in PCS_sysLib header
 */

#include "PCS_sysLib.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_GetWrsKernelTextEnd()
 * ----------------------------------------------------
 */
unsigned int PCS_GetWrsKernelTextEnd(void)
{
    UT_GenStub_SetupReturnBuffer(PCS_GetWrsKernelTextEnd, unsigned int);

    UT_GenStub_Execute(PCS_GetWrsKernelTextEnd, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_GetWrsKernelTextEnd, unsigned int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_GetWrsKernelTextStart()
 * ----------------------------------------------------
 */
unsigned int PCS_GetWrsKernelTextStart(void)
{
    UT_GenStub_SetupReturnBuffer(PCS_GetWrsKernelTextStart, unsigned int);

    UT_GenStub_Execute(PCS_GetWrsKernelTextStart, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_GetWrsKernelTextStart, unsigned int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_PciOutByte()
 * ----------------------------------------------------
 */
void PCS_PciOutByte(uint32_t address, uint8_t data)
{
    UT_GenStub_AddParam(PCS_PciOutByte, uint32_t, address);
    UT_GenStub_AddParam(PCS_PciOutByte, uint8_t, data);

    UT_GenStub_Execute(PCS_PciOutByte, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_PciOutLong()
 * ----------------------------------------------------
 */
void PCS_PciOutLong(uint32_t address, uint32_t data)
{
    UT_GenStub_AddParam(PCS_PciOutLong, uint32_t, address);
    UT_GenStub_AddParam(PCS_PciOutLong, uint32_t, data);

    UT_GenStub_Execute(PCS_PciOutLong, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_sysClkRateGet()
 * ----------------------------------------------------
 */
int PCS_sysClkRateGet(void)
{
    UT_GenStub_SetupReturnBuffer(PCS_sysClkRateGet, int);

    UT_GenStub_Execute(PCS_sysClkRateGet, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_sysClkRateGet, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_sysMemTop()
 * ----------------------------------------------------
 */
char *PCS_sysMemTop(void)
{
    UT_GenStub_SetupReturnBuffer(PCS_sysMemTop, char *);

    UT_GenStub_Execute(PCS_sysMemTop, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_sysMemTop, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_sysPciRead32()
 * ----------------------------------------------------
 */
void PCS_sysPciRead32(uint32_t address, uint32_t *data)
{
    UT_GenStub_AddParam(PCS_sysPciRead32, uint32_t, address);
    UT_GenStub_AddParam(PCS_sysPciRead32, uint32_t *, data);

    UT_GenStub_Execute(PCS_sysPciRead32, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_sysPciWrite32()
 * ----------------------------------------------------
 */
void PCS_sysPciWrite32(uint32_t address, uint32_t data)
{
    UT_GenStub_AddParam(PCS_sysPciWrite32, uint32_t, address);
    UT_GenStub_AddParam(PCS_sysPciWrite32, uint32_t, data);

    UT_GenStub_Execute(PCS_sysPciWrite32, Basic, NULL);
}
