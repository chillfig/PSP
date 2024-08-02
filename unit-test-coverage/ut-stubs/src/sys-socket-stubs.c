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
 * \brief    Stubs for sys/sockets.h
 * \ingroup  ut-stubs
 */
#include "utstubs.h"
#include "PCS_sys_socket.h"

int PCS_accept(int fd, struct PCS_sockaddr *addr, PCS_socklen_t *addr_len)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_accept), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_accept), addr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_accept), addr_len);

    return UT_DEFAULT_IMPL(PCS_accept);
}

int PCS_bind(int fd, const struct PCS_sockaddr *addr, PCS_socklen_t len)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_bind), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_bind), addr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_bind), len);

    return UT_DEFAULT_IMPL(PCS_bind);
}

int PCS_connect(int fd, const struct PCS_sockaddr *addr, PCS_socklen_t len)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_connect), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_connect), addr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_connect), len);

    return UT_DEFAULT_IMPL(PCS_connect);
}

int PCS_getsockopt(int fd, int level, int optname, void *optval, PCS_socklen_t *optlen)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_getsockopt), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_getsockopt), level);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_getsockopt), optname);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_getsockopt), optval);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_getsockopt), optlen);

    return UT_DEFAULT_IMPL(PCS_getsockopt);
}

int PCS_listen(int fd, int n)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_listen), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_listen), n);

    return UT_DEFAULT_IMPL(PCS_listen);
}

PCS_ssize_t PCS_recvfrom(int fd, void *buf, size_t n, int flags, struct PCS_sockaddr *addr, PCS_socklen_t *addr_len)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_recvfrom);

    if (iStatus >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(PCS_recvfrom), buf, 256);
    }

    return iStatus;
}

PCS_ssize_t PCS_sendto(int fd, const void *buf, size_t n, int flags, const struct PCS_sockaddr *addr,
                       PCS_socklen_t addr_len)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_sendto), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_sendto), buf);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_sendto), n);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_sendto), flags);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_sendto), addr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_sendto), addr_len);

    return UT_DEFAULT_IMPL(PCS_sendto);
}

int PCS_setsockopt(int fd, int level, int optname, const void *optval, PCS_socklen_t optlen)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_setsockopt), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_setsockopt), level);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_setsockopt), optname);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_setsockopt), optval);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_setsockopt), optlen);

    return UT_DEFAULT_IMPL(PCS_setsockopt);
}

int PCS_shutdown(int fd, int how)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_shutdown), fd);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_shutdown), how);

    return UT_DEFAULT_IMPL(PCS_shutdown);
}

int PCS_socket(int domain, int type, int protocol)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_socket), domain);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_socket), type);
    UT_Stub_RegisterContextGenericArg(UT_KEY(PCS_socket), protocol);

    return UT_DEFAULT_IMPL(PCS_socket);
}
