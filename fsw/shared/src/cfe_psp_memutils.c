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
** \file   :	cfe_psp_memutils.c
**
** Author :	Ezra Yeheskeli
**
** Purpose:
**		   This file  contains some of the cFE Platform Support Layer.
**        It contains the processor architecture specific calls.
**
**
*/

/*
** Include section
*/

#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/*
** User defined include files
*/

#include "cfe_psp.h"
/*
** global memory
*/

/**
 ** \func Copy from one memory block to another memory block
 **
 ** \par Description:
 ** Copies 'size' byte from memory address pointed by 'src' to memory
 ** address pointed by ' dst' For now we are using the standard c library
 ** call 'memcpy' but if we find we need to make it more efficient then
 ** we'll implement it in assembly.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[inout] dest - Pointer to an address to copy to
 ** \param[inout] src - Pointer address to copy from
 ** \param[in] size - Number of bytes to copy
 **
 ** \return #CFE_PSP_SUCCESS 
 */
int32 CFE_PSP_MemCpy(void *dest, const void *src, uint32 size)
{
    memcpy(dest, src, size);
    return (CFE_PSP_SUCCESS);
}

/**
 ** \func Initialize the specified memory block with the specified value
 **
 ** \par Description:
 ** Copies 'size' number of byte of value 'value' to memory address pointed
 ** by 'dst' .For now we are using the standard c library call 'memset'
 ** but if we find we need to make it more efficient then we'll implement
 ** it in assembly.
 **
 ** \par Assumptions, External Events, and Notes:
 ** None
 **
 ** \param[inout] dest - Pointer to destination address
 ** \param[in] value - An 8-bit value to fill in the memory
 ** \param[in] size - The number of values to write
 **
 ** \return #CFE_PSP_SUCCESS
 */
int32 CFE_PSP_MemSet(void *dest, uint8 value, uint32 size)
{
    memset(dest, (int)value, (size_t)size);
    return (CFE_PSP_SUCCESS);
}
