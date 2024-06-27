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

/*
** File   :	cfe_psp_eeprom_direct.c
**
** Author :	Ezra Yeheskeli
**
** Purpose:
**		   This file  contains some of the OS APIs abstraction layer.
**         It contains the processor architecture specific calls.
**
**  16-Nov-2003 Ezra Yeheskeli
**          - First Creation.
**
*/

/*
** Include section
*/
#include <stdio.h>

#include "cfe_psp.h"
#include "cfe_psp_module.h"

CFE_PSP_MODULE_DECLARE_SIMPLE(eeprom_direct);

void eeprom_direct_Init(uint32 PspModuleId)
{
    /* Inform the user that this module is in use */
    OS_printf("CFE_PSP: Using DIRECT memory mapped EEPROM implementation\n");
}

/*
** global memory
*/

/*
 **
 ** Purpose:
 **
 ** Assumptions and Notes:
 **
 ** Parameters:
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 ** Return Values:
 **	 CFE_PSP_SUCCESS
 **	 CFE_PSP_ERROR_ADDRESS_MISALIGNED The Address is not aligned to 16 bit addressing
 **   scheme.
 */
int32 CFE_PSP_EepromWrite32(cpuaddr MemoryAddress, uint32 uint32Value)
{
    uint32 ret_value = CFE_PSP_SUCCESS;

    /* check 32 bit alignment  */
    if (MemoryAddress & 0x00000003)
    {
        return CFE_PSP_ERROR_ADDRESS_MISALIGNED;
    }

    /* make the Write */
    *((uint32 *)MemoryAddress) = uint32Value;

    return ret_value;
}

/*
 **
 ** Purpose:
 **
 ** Assumptions and Notes:
 **
 ** Parameters:
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 ** Return Values:
 **   CFE_PSP_SUCCESS
 **   CFE_PSP_ERROR_ADDRESS_MISALIGNED The Address is not aligned to 16 bit addressing
 **   scheme.
 */
int32 CFE_PSP_EepromWrite16(cpuaddr MemoryAddress, uint16 uint16Value)
{
    uint32 write32;
    uint32 temp32;
    uint32 aligned_address;

    /*
    ** check 16 bit alignment  , check the 1st lsb
    */
    if (MemoryAddress & 0x00000001)
    {
        return CFE_PSP_ERROR_ADDRESS_MISALIGNED;
    }

    temp32 = uint16Value;

#ifdef SOFTWARE_LITTLE_BIT_ORDER
    /*
    ** Implementation for Little Endian architectures ( x86 )
    */

    /*
    ** check the 2nd lsb to see if it's the 1st or 2nd 16 bit word
    */
    if ((MemoryAddress & 0x00000002))
    {
        /*
        ** writing the 16 high bit order of 32 bit field
        */
        aligned_address = MemoryAddress - 2;
        CFE_PSP_MemRead32(aligned_address, &write32);
        write32 = (write32 & 0x0000FFFF) | (temp32 << 16);
    }
    else
    {
        /*
        ** writing the 16 low bit order of 32 bit field
        */
        aligned_address = MemoryAddress;
        CFE_PSP_MemRead32(aligned_address, &write32);
        write32 = (write32 & 0xFFFF0000) | (temp32);
    }

#else

    /*
    ** Implementation for Big Endian architectures (PPC, Coldfire )
    */
    /*
    ** check the 2nd lsb to see if it's the 1st or 2nd 16 bit word
    */
    if ((MemoryAddress & 0x00000002))
    {
        /*
        ** writing the 16 high bit order of 32 bit field
        */
        aligned_address = MemoryAddress - 2;
        CFE_PSP_MemRead32(aligned_address, &write32);
        write32 = (write32 & 0xFFFF0000) | (temp32);
    }
    else
    {
        /*
        ** writing the 16 low bit order of 32 bit field
        */
        aligned_address = MemoryAddress;
        CFE_PSP_MemRead32(aligned_address, &write32);
        write32 = (write32 & 0x0000FFFF) | (temp32 << 16);
    }
#endif

    return CFE_PSP_EepromWrite32(aligned_address, write32);
}

/*
 **
 ** Purpose:
 **
 ** Assumptions and Notes:
 **
 ** Parameters:
 **
 ** Global Inputs: None
 **
 ** Global Outputs: None
 **
 **
 ** Return Values:
 ** CFE_PSP_SUCCESS
 ** CFE_PSP_ERROR_AND_MISALIGNED
 */

int32 CFE_PSP_EepromWrite8(cpuaddr MemoryAddress, uint8 ByteValue)
{
    uint32 aligned_address;
    uint16 write16, temp16;

    temp16 = ByteValue;

#ifdef SOFTWARE_LITTLE_BIT_ORDER
    /*
    ** Implementation for Little Endian architectures ( x86 )
    */
    /*
    ** check the 1st lsb
    */
    if (MemoryAddress & 0x00000001)
    {
        /*
        ** writing the 8 high bit order of 16 bit field
        */
        aligned_address = MemoryAddress - 1;
        CFE_PSP_MemRead16(aligned_address, &write16);
        write16 = (write16 & 0x00FF) | (temp16 << 8);
    }
    else
    {
        /*
        ** writing the 8 low bit order of 16 bit field
        */
        aligned_address = MemoryAddress;
        CFE_PSP_MemRead16(aligned_address, &write16);
        write16 = (temp16) | (write16 & 0xFF00);
    }
#else

    /*
    ** Implementation for Big Endian architectures (PPC, Coldfire )
    */

    /*
    ** check the 1st lsb
    */
    if (MemoryAddress & 0x00000001)
    {
        /*
        ** writing the 8 high bit order of 16 bit field
        */
        aligned_address = MemoryAddress - 1;
        CFE_PSP_MemRead16(aligned_address, &write16);
        write16 = (write16 & 0xFF00) | (temp16);
    }
    else
    {
        /*
        ** writing the 8 low bit order of 16 bit field
        */
        aligned_address = MemoryAddress;
        CFE_PSP_MemRead16(aligned_address, &write16);
        write16 = (temp16 << 8) | (write16 & 0x00FF);
    }

#endif

    return CFE_PSP_EepromWrite16(aligned_address, write16);
}

/*
**
** Purpose:
**		Enable the eeprom for write operation
**
** Assumptions and Notes:
**
** Parameters:
**   Bank: Which bank of EEPROM
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values:
**   CFE_PSP_SUCCESS
*/
int32 CFE_PSP_EepromWriteEnable(uint32 Bank)
{
    return CFE_PSP_SUCCESS;
}

/*
**
** Purpose:
**		Disable  the eeprom from write operation
**
** Assumptions and Notes:
**
** Parameters:
**   Bank: Which bank of EEPROM
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values:
**   CFE_PSP_SUCCESS
*/
int32 CFE_PSP_EepromWriteDisable(uint32 Bank)
{
    return CFE_PSP_SUCCESS;
}

/*
**
** Purpose:
**		Power up the eeprom
** Assumptions and Notes:
**
** Parameters:
**   Bank: Which bank of EEPROM
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values:
**   CFE_PSP_SUCCESS
*/
int32 CFE_PSP_EepromPowerUp(uint32 Bank)
{
    return CFE_PSP_SUCCESS;
}

/*
**
** Purpose:
**		Power down the eeprom
** Assumptions and Notes:
**
** Parameters:
**   Bank: Which bank of EEPROM
**
** Global Inputs: None
**
** Global Outputs: None
**
**
** Return Values:
**   CFE_PSP_SUCCESS
*/
int32 CFE_PSP_EepromPowerDown(uint32 Bank)
{
    return CFE_PSP_SUCCESS;
}
/* UndCC_End(*) */
