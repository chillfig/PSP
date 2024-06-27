/* UndCC_Begin(*) Maintained by GSFC */
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

/**
 * \file
 *
 * A PSP module to satisfy the "PORT" API on systems which
 * can access I/O ports directly via memory mapped addresses.
 */

#include "cfe_psp.h"
#include "cfe_psp_module.h"

CFE_PSP_MODULE_DECLARE_SIMPLE(port_direct);

/**
 ** \func Initialize port direct module
 **
 ** \par Description:
 ** Prepare port direct data for use in other functions
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[in] PspModuleId - Not used
 **
 ** \return None
 */
void port_direct_Init(uint32 PspModuleId)
{
    /* Inform the user that this module is in use */
    OS_printf("CFE_PSP: Using DIRECT memory mapped PORT implementation\n");
}

/*
** global memory
*/

/*
** Name:
**  CFE_PSP_PortRead8
**
** Purpose:
**  Read one byte of memory.
**
** Parameters:
**	PortAddress : Address to be read
**  ByteValue   : The address content will be copied to the location pointed by
**            this argument
**
** Global Inputs:
**  None
**
** Global Outputs:
*   None
**
** Return Values:
**  CFE_PSP_SUCCESS
**  CFE_PSP_INVALID_POINTER
**  CFE_PSP_INVALID_MEM_ADDR
*/
int32 CFE_PSP_PortRead8(cpuaddr PortAddress, uint8 *ByteValue)
{
    int32 iReturnValue = CFE_PSP_SUCCESS;

    /* Null check */
    if (ByteValue == NULL)
    {
        iReturnValue = CFE_PSP_INVALID_POINTER;
    }
    /* Port Address check */
    else if (PortAddress == 0x00000000)
    {
        iReturnValue = CFE_PSP_INVALID_MEM_ADDR;
    }
    else
    {
        (*ByteValue) = (uint8) * ((uint8 *)PortAddress);
        iReturnValue = CFE_PSP_SUCCESS;
    }

    return iReturnValue;
}

/*
** Name:
**  CFE_PSP_PortWrite8
**
** Purpose:
**  Write one byte of memory.
**
** Parameters:
**	PortAddress : Address to be written to
**  ByteValue   : The content pointed by this argument will be copied to the
**            address
**
** Global Inputs:
**  None
**
** Global Outputs:
**  None
**
** Return Values:
**  CFE_PSP_SUCCESS
**  CFE_PSP_INVALID_MEM_ADDR
*/
int32 CFE_PSP_PortWrite8(cpuaddr PortAddress, uint8 ByteValue)
{
    int32 iReturnValue = CFE_PSP_INVALID_MEM_ADDR;

    /* Address check */
    if (PortAddress != 0x00000000)
    {
        *((uint8 *)PortAddress) = ByteValue;
        iReturnValue = CFE_PSP_SUCCESS;
    }

    return iReturnValue;
}

/*
** Name:
**  CFE_PSP_PortRead16
**
** Purpose:
**  Read 2 bytes of memory.
**
**
** Parameters:
**	PortAddress : Address to be read
**  uint16Value : The address content will be copied to the location pointed by
**            this argument
**
** Global Inputs:
**  None
**
** Global Outputs:
**  None
**
** Return Values:
**  CFE_PSP_SUCCESS
**  CFE_PSP_INVALID_POINTER
**	CFE_PSP_ERROR_ADDRESS_MISALIGNED The Address is not aligned to 16 bit
**  addressing scheme.
**  CFE_PSP_INVALID_MEM_ADDR
*/
int32 CFE_PSP_PortRead16(cpuaddr PortAddress, uint16 *uint16Value)
{
    int32 iReturnValue = CFE_PSP_SUCCESS;

    /* NULL check */
    if (uint16Value == NULL)
    {
        iReturnValue = CFE_PSP_INVALID_POINTER;
    }
    /* Port Address check */
    else if (PortAddress == 0x00000000)
    {
        iReturnValue = CFE_PSP_INVALID_MEM_ADDR;
    }
    /* check 16 bit alignment  , check the 1st lsb */
    else if (PortAddress & 0x00000001)
    {
        iReturnValue = CFE_PSP_ERROR_ADDRESS_MISALIGNED;
    }
    else
    {
        (*uint16Value) = *((uint16 *)PortAddress);
    }

    return iReturnValue;
}

/*
** Name:
**  CFE_PSP_PortWrite16
**
** Purpose:
**  Write 2 byte of memory.
**
** Parameters:
**	PortAddress : Address to be written to
**  uint16Value : The content pointed by this argument will be copied to the
**            address
**
** Global Inputs:
** None
**
** Global Outputs:
** None
**
** Return Values:
**  CFE_PSP_SUCCESS
**  CFE_PSP_INVALID_MEM_ADDR
**	CFE_PSP_ERROR_ADDRESS_MISALIGNED The Address is not aligned to 16 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortWrite16(cpuaddr PortAddress, uint16 uint16Value)
{
    int32 iReturnValue = CFE_PSP_SUCCESS;

    /* Address check */
    if (PortAddress != 0x00000000)
    {
        iReturnValue = CFE_PSP_INVALID_MEM_ADDR;
    }
    /* check 16 bit alignment  , check the 1st lsb */
    else if (PortAddress & 0x00000001)
    {
        iReturnValue = CFE_PSP_ERROR_ADDRESS_MISALIGNED;
    }
    else
    {
        *((uint16 *)PortAddress) = uint16Value;
    }

    return iReturnValue;
}

/*
** Name:
**  CFE_PSP_PortRead32
**
** Purpose:
**  Read 4 bytes of memory.
**
** Parameters:
**	PortAddress : Address to be read
**  uint32Value : The address content will be copied to the location pointed by
**            this argument
**
** Global Inputs:
**  None
**
** Global Outputs:
**  None
**
** Return Values:
**  CFE_PSP_SUCCESS
**  CFE_PSP_INVALID_POINTER
**	CFE_PSP_ERROR_ADDRESS_MISALIGNED The Address is not aligned to 32 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortRead32(cpuaddr PortAddress, uint32 *uint32Value)
{
    int32 iReturnValue = CFE_PSP_SUCCESS;

    /* NULL check */
    if (uint32Value == NULL)
    {
        iReturnValue = CFE_PSP_INVALID_POINTER;
    }
    /* check 32 bit alignment  */
    
    else if (PortAddress & 0x00000003)
    {
        iReturnValue = CFE_PSP_ERROR_ADDRESS_MISALIGNED;
    }
    else
    {
        (*uint32Value) = *((uint32 *)PortAddress);
    }

    return iReturnValue;
}

/*
** Name:
**  CFE_PSP_PortWrite32
**
** Purpose:
**  Write 4 byte of memory.
**
** Parameters:
**	PortAddress : Address to be written to
**  uint32Value : The content pointed by this argument will be copied to the
**            address
**
** Global Inputs:
**  None
**
** Global Outputs:
**  None
**
** Return Values:
**  CFE_PSP_SUCCESS
**  CFE_PSP_INVALID_MEM_ADDR
**  CFE_PSP_ERROR_ADDRESS_MISALIGNED The Address is not aligned to 32 bit
**      addressing scheme.
*/
int32 CFE_PSP_PortWrite32(cpuaddr PortAddress, uint32 uint32Value)
{   
    int32 iReturnValue = CFE_PSP_SUCCESS;

    /* Address check */
    if (PortAddress != 0x00000000)
    {
        iReturnValue = CFE_PSP_INVALID_MEM_ADDR;
    }
    /* check 32 bit alignment  */
    else if (PortAddress & 0x00000003)
    {
        iReturnValue = CFE_PSP_ERROR_ADDRESS_MISALIGNED;
    }
    else
    {
        *((uint32 *)PortAddress) = uint32Value;
    }

    return iReturnValue;
}
/* UndCC_End(*) */
