/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file cfe_psp_memutils.c
*
*   \brief Implementation of PSP memory utility functions
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was cloned-n-owned at the NASA Johnson Space Center from the NASA cFE open source 
*   with license below.
**********************************************************************************************************/

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

/**********************************************************
 * 
 * Function: CFE_PSP_MemCpy
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemCpy(void *dest, const void *src, uint32 size)
{
    if ((dest != NULL) && (src != NULL))
    {
        memcpy(dest, src, size);
        return (CFE_PSP_SUCCESS);
    }
    else
    {
        return CFE_PSP_ERROR;
    }
}

/**********************************************************
 * 
 * Function: CFE_PSP_MemSet
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_MemSet(void *dest, uint8 value, uint32 size)
{
    int32 iReturnValue = CFE_PSP_ERROR;

    if (dest != NULL)
    {
        memset(dest, (int)value, (size_t)size);
        iReturnValue = CFE_PSP_SUCCESS;
    }

    return iReturnValue;
}
