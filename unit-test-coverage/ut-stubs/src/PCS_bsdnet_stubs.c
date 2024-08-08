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
 * Auto-Generated stub implementations for functions defined in PCS_bsdnet header
 */

#include "PCS_bsdnet.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_rtems_bsdnet_do_dhcp_failsafe()
 * ----------------------------------------------------
 */
void PCS_rtems_bsdnet_do_dhcp_failsafe(void)
{

    UT_GenStub_Execute(PCS_rtems_bsdnet_do_dhcp_failsafe, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_rtems_bsdnet_initialize_network()
 * ----------------------------------------------------
 */
int PCS_rtems_bsdnet_initialize_network(void)
{
    UT_GenStub_SetupReturnBuffer(PCS_rtems_bsdnet_initialize_network, int);

    UT_GenStub_Execute(PCS_rtems_bsdnet_initialize_network, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_rtems_bsdnet_initialize_network, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_rtems_fxp_attach()
 * ----------------------------------------------------
 */
int PCS_rtems_fxp_attach(struct PCS_rtems_bsdnet_ifconfig *, int)
{
    UT_GenStub_SetupReturnBuffer(PCS_rtems_fxp_attach, int);

    UT_GenStub_AddParam(PCS_rtems_fxp_attach, , int);

    UT_GenStub_Execute(PCS_rtems_fxp_attach, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_rtems_fxp_attach, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_rtems_status_text()
 * ----------------------------------------------------
 */
const char *PCS_rtems_status_text(PCS_rtems_status_code)
{
    UT_GenStub_SetupReturnBuffer(PCS_rtems_status_text, const char *);

    UT_GenStub_AddParam(PCS_rtems_status_text, , PCS_rtems_status_code);

    UT_GenStub_Execute(PCS_rtems_status_text, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_rtems_status_text, const char *);
}
