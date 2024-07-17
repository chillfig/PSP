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

/* PSP coverage stub replacement for edrLib.h */
#ifndef PCS_EDRLIB_H
#define PCS_EDRLIB_H

#include "PCS_basetypes.h"
#include "PCS_vxWorks.h"
#include "PCS_taskLib.h"

/* ----------------------------------------- */
/* constants normally defined in edrLib.h */
/* ----------------------------------------- */

/* ----------------------------------------- */
/* types normally defined in edrLib.h */
/* ----------------------------------------- */
typedef struct
{
    PCS_TASK_ID         taskId;         /* task's id                        */
    PCS_Vx_exit_code_t  status;         /* exit status to use when deleting */
    int                 vector;         /* exception vector number          */
    char *              pEsf;           /* exception frame pointer          */

    PCS_BOOL            isException;    /* are we handling an exception?    */
    const char *        msg;            /* ED&R record optional message     */
} PCS_EDR_TASK_INFO;

/* ----------------------------------------- */
/* prototypes normally declared in edrLib.h */
/* ----------------------------------------- */
void * PCS_edrPolicyHandlerHookGet(void);
int PCS_edrPolicyHandlerHookAdd(void *pPtrFunc);
int PCS_edrErrorPolicyHookRemove(void);

#endif
