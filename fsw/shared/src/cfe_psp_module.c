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
 * \file
 *
 *  Created on: Jul 25, 2014
 *      Author: jphickey
 */

#include <stdio.h>
#include <string.h>
#include "osapi.h"

#include "cfe_psp_module.h"

/**
 ** \brief CFE PSP Module Base and Index
 **
 ** \par Description:
 ** When using an OSAL that also supports "opaque object ids", choose values here
 ** that will fit in with the OSAL object ID values and not overlap anything.
 */
#ifdef OS_OBJECT_TYPE_USER
 /** \brief CFE_PSP_MODULE_BASE */
 #define CFE_PSP_MODULE_BASE       ((OS_OBJECT_TYPE_USER + 0x100) << OS_OBJECT_TYPE_SHIFT)
 /** \brief CFE_PSP_MODULE_INDEX_MASK */
 #define CFE_PSP_MODULE_INDEX_MASK OS_OBJECT_INDEX_MASK
#else
 /** \brief CFE_PSP_MODULE_BASE */
 #define CFE_PSP_MODULE_BASE       0x01100000
 /** \brief CFE_PSP_MODULE_INDEX_MASK */
 #define CFE_PSP_MODULE_INDEX_MASK 0xFFFF
#endif

/**
 ** \brief Total number of fixed and user selected PSP modules.
 **
 ** \par Warning:
 ** This is actually incremented twice, once per module list (Fixed and User selected lists).
 ** There is an issue identified with this variable: https://github.com/nasa/PSP/issues/319
 */
static uint32 CFE_PSP_ModuleCount = 0;

/**
 ** \func Initialize a list of Modules
 ** 
 ** \par Description:
 ** Helper function to initialize a list of modules (not externally called)
 **
 ** \par Assumptions, External Events, and Notes:
 ** The module list pointed by ListPtr is generated by cmake during build time 
 ** with an added NULL at the end to guarantee that the while loop ends.
 **
 ** \param[out] ListPtr - Pointer to the list of modules
 ** 
 ** \return None
 */
void CFE_PSP_ModuleInitList(CFE_StaticModuleLoadEntry_t *ListPtr)
{
    CFE_StaticModuleLoadEntry_t *Entry;
    CFE_PSP_ModuleApi_t *        ApiPtr;

    /*
     * Call the init function for all statically linked modules
     */
    Entry = ListPtr;
    if (Entry != NULL)
    {
        while (Entry->Name != NULL)
        {
            ApiPtr = (CFE_PSP_ModuleApi_t *)Entry->Api;
            if ((uint32)ApiPtr->ModuleType != CFE_PSP_MODULE_TYPE_INVALID && ApiPtr->Init != NULL)
            {
                (*ApiPtr->Init)(CFE_PSP_MODULE_BASE | CFE_PSP_ModuleCount);
            }
            ++Entry;
            ++CFE_PSP_ModuleCount;
        }
    }
}

/**********************************************************
 * 
 * Function: CFE_PSP_ModuleInit
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
void CFE_PSP_ModuleInit(void)
{
    /* First initialize the fixed set of modules for this PSP */
    CFE_PSP_ModuleInitList(CFE_PSP_BASE_MODULE_LIST);

    /*
    ** Then initialize any user-selected extension modules
    ** 
    ** Set CFE_PSP_ModuleCount to 0 because each function
    ** that uses CFE_PSP_ModuleCount only iterates
    ** through GLOBAL_CONFIGDATA.PspModuleList with no consideration
    ** for other module lists (i.e., no consideration for
    ** CFE_PSP_BASE_MODULE_LIST)
    **
    ** See https://github.com/nasa/PSP/issues/319
    ** "A complete module list should be used for APIs that include both
    ** the built in and added"
    */
    CFE_PSP_ModuleCount = 0;
    CFE_PSP_ModuleInitList(GLOBAL_CONFIGDATA.PspModuleList);
}

/**********************************************************
 * 
 * Function: CFE_PSP_Module_GetAPIEntry
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_Module_GetAPIEntry(uint32 PspModuleId, CFE_PSP_ModuleApi_t **API)
{
    int32  Result;
    uint32 LocalId;

    Result = CFE_PSP_INVALID_MODULE_ID;
    if ((PspModuleId & ~CFE_PSP_MODULE_INDEX_MASK) == CFE_PSP_MODULE_BASE)
    {
        LocalId = PspModuleId & CFE_PSP_MODULE_INDEX_MASK;
        if (LocalId < CFE_PSP_ModuleCount)
        {
            *API   = (CFE_PSP_ModuleApi_t *)GLOBAL_CONFIGDATA.PspModuleList[LocalId].Api;
            Result = CFE_PSP_SUCCESS;
        }
    }

    return Result;
}

/**********************************************************
 * 
 * Function: CFE_PSP_Module_FindByName
 * 
 * Description: See function declaration for info
 *
 *********************************************************/
int32 CFE_PSP_Module_FindByName(const char *ModuleName, uint32 *PspModuleId)
{
    uint32                       i;
    int32                        Result;
    CFE_StaticModuleLoadEntry_t *Entry;

    Result = CFE_PSP_INVALID_MODULE_NAME;

    if ((ModuleName != NULL) && (PspModuleId != NULL))
    {
        Entry  = GLOBAL_CONFIGDATA.PspModuleList;
        
        if (Entry != NULL)
        {
            i = 0;
            while (i < CFE_PSP_ModuleCount)
            {
                if (strncmp(Entry->Name, ModuleName, CFE_PSP_MODULE_NAME_MAX_LENGTH) == 0)
                {
                    *PspModuleId = CFE_PSP_MODULE_BASE | (i & CFE_PSP_MODULE_INDEX_MASK);
                    Result       = CFE_PSP_SUCCESS;
                    break;
                }
                ++Entry;
                ++i;
            }
        }
    }

    return Result;
}
