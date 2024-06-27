/*********************************************************************************************************/
/** \export_control EAR ECCN 9E515.a and/or 9E515.f (HALO)
*   Export Administration Regulations (EAR) Notice
*   This document contains information which falls under the purview of the Export Administration Regulations (EAR),
*   15 CFR §730-774 and is export controlled. It may be used only to fulfill responsibilities of the Parties of,
*   or a Cooperating Agency of a NASA Gateway Program Partner (CSA, ESA, JAXA, MBRSC) and their contractors in
*   furtherance of the Gateway MOUs with ESA, CSA, and Japan and IA with MBRSC. Any use, re-transfer,
*   or disclosure to any party for any purpose other than the designated use of fulfilling the responsibilities
*   of the Gateway MOUs and IA requires prior U.S. Government authorization.
*
*   \file ping_stubs.c
*
*   \brief This file contains PING stub functions.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

#include <string.h>
#include "utstubs.h"
#include "utgenstub.h"
#include <selectLib.h>
#include <arpa/inet.h>

void UT_DefaultHandler_inet_addr(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_DefaultHandler_recvfrom(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_DefaultHandler_inet_addr(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32     status;
    in_addr_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(inet_addr), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

void UT_DefaultHandler_recvfrom(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32   status;
    void * buffer = UT_Hook_GetArgValueByName(Context, "buffer", void *);

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(recvfrom), buffer, sizeof(*buffer));
    }

    return status;
}

in_addr_t inet_addr( const char *__cp)
{
    UT_GenStub_SetupReturnBuffer(inet_addr, in_addr_t);

    UT_GenStub_AddParam(inet_addr, const char*, __cp);

    UT_GenStub_Execute(inet_addr, Basic, UT_DefaultHandler_inet_addr);

    return UT_GenStub_GetReturnValue(inet_addr, in_addr_t);
}

int socket(int a, int b, int c)
{
    int status;

    status = UT_DEFAULT_IMPL_RC(socket, 1);

    return status;
}

ssize_t sendto(int socket, const void *message, size_t length,
              int flags, const struct sockaddr *dest_addr,
              socklen_t dest_len)
{
    ssize_t status;

    status = UT_DEFAULT_IMPL_RC(sendto, length);

    return status;
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout)
{
    UT_GenStub_SetupReturnBuffer(select, int);

    UT_GenStub_AddParam(select, int, nfds);
    UT_GenStub_AddParam(select, fd_set *, readfds);
    UT_GenStub_AddParam(select, fd_set *, writefds);
    UT_GenStub_AddParam(select, fd_set *, exceptfds);
    UT_GenStub_AddParam(select, struct timeval *, timeout);

    UT_GenStub_Execute(select, Basic, NULL);

    return UT_GenStub_GetReturnValue(select, int);
}

ssize_t recvfrom(int socket, void *buffer, size_t length,
                 int flags, struct sockaddr *address,
                 socklen_t *address_len)
{
    UT_GenStub_SetupReturnBuffer(recvfrom, ssize_t);

    UT_GenStub_AddParam(recvfrom, void *, buffer);

    UT_GenStub_Execute(recvfrom, Basic, UT_DefaultHandler_recvfrom);

    return UT_GenStub_GetReturnValue(recvfrom, ssize_t);
}
