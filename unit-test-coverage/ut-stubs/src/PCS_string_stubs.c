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
 * Auto-Generated stub implementations for functions defined in PCS_string header
 */

#include "PCS_string.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_memcpy()
 * ----------------------------------------------------
 */
void *PCS_memcpy(void *dest, const void *src, size_t n)
{
    UT_GenStub_SetupReturnBuffer(PCS_memcpy, void *);

    UT_GenStub_AddParam(PCS_memcpy, void *, dest);
    UT_GenStub_AddParam(PCS_memcpy, const void *, src);
    UT_GenStub_AddParam(PCS_memcpy, size_t, n);

    UT_GenStub_Execute(PCS_memcpy, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_memcpy, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_memset()
 * ----------------------------------------------------
 */
void *PCS_memset(void *s, int c, size_t n)
{
    UT_GenStub_SetupReturnBuffer(PCS_memset, void *);

    UT_GenStub_AddParam(PCS_memset, void *, s);
    UT_GenStub_AddParam(PCS_memset, int, c);
    UT_GenStub_AddParam(PCS_memset, size_t, n);

    UT_GenStub_Execute(PCS_memset, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_memset, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strcat()
 * ----------------------------------------------------
 */
char *PCS_strcat(char *dest, const char *src)
{
    UT_GenStub_SetupReturnBuffer(PCS_strcat, char *);

    UT_GenStub_AddParam(PCS_strcat, char *, dest);
    UT_GenStub_AddParam(PCS_strcat, const char *, src);

    UT_GenStub_Execute(PCS_strcat, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strcat, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strchr()
 * ----------------------------------------------------
 */
char *PCS_strchr(const char *s, int c)
{
    UT_GenStub_SetupReturnBuffer(PCS_strchr, char *);

    UT_GenStub_AddParam(PCS_strchr, const char *, s);
    UT_GenStub_AddParam(PCS_strchr, int, c);

    UT_GenStub_Execute(PCS_strchr, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strchr, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strcmp()
 * ----------------------------------------------------
 */
int PCS_strcmp(const char *s1, const char *s2)
{
    UT_GenStub_SetupReturnBuffer(PCS_strcmp, int);

    UT_GenStub_AddParam(PCS_strcmp, const char *, s1);
    UT_GenStub_AddParam(PCS_strcmp, const char *, s2);

    UT_GenStub_Execute(PCS_strcmp, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strcmp, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strcpy()
 * ----------------------------------------------------
 */
char *PCS_strcpy(char *dest, const char *src)
{
    UT_GenStub_SetupReturnBuffer(PCS_strcpy, char *);

    UT_GenStub_AddParam(PCS_strcpy, char *, dest);
    UT_GenStub_AddParam(PCS_strcpy, const char *, src);

    UT_GenStub_Execute(PCS_strcpy, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strcpy, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strerror()
 * ----------------------------------------------------
 */
char *PCS_strerror(int errnum)
{
    UT_GenStub_SetupReturnBuffer(PCS_strerror, char *);

    UT_GenStub_AddParam(PCS_strerror, int, errnum);

    UT_GenStub_Execute(PCS_strerror, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strerror, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strlen()
 * ----------------------------------------------------
 */
size_t PCS_strlen(const char *s)
{
    UT_GenStub_SetupReturnBuffer(PCS_strlen, size_t);

    UT_GenStub_AddParam(PCS_strlen, const char *, s);

    UT_GenStub_Execute(PCS_strlen, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strlen, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strncat()
 * ----------------------------------------------------
 */
char *PCS_strncat(char *dest, const char *src, size_t n)
{
    UT_GenStub_SetupReturnBuffer(PCS_strncat, char *);

    UT_GenStub_AddParam(PCS_strncat, char *, dest);
    UT_GenStub_AddParam(PCS_strncat, const char *, src);
    UT_GenStub_AddParam(PCS_strncat, size_t, n);

    UT_GenStub_Execute(PCS_strncat, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strncat, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strncmp()
 * ----------------------------------------------------
 */
int PCS_strncmp(const char *s1, const char *s2, size_t n)
{
    UT_GenStub_SetupReturnBuffer(PCS_strncmp, int);

    UT_GenStub_AddParam(PCS_strncmp, const char *, s1);
    UT_GenStub_AddParam(PCS_strncmp, const char *, s2);
    UT_GenStub_AddParam(PCS_strncmp, size_t, n);

    UT_GenStub_Execute(PCS_strncmp, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strncmp, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strncpy()
 * ----------------------------------------------------
 */
char *PCS_strncpy(char *dest, const char *src, size_t n)
{
    UT_GenStub_SetupReturnBuffer(PCS_strncpy, char *);

    UT_GenStub_AddParam(PCS_strncpy, char *, dest);
    UT_GenStub_AddParam(PCS_strncpy, const char *, src);
    UT_GenStub_AddParam(PCS_strncpy, size_t, n);

    UT_GenStub_Execute(PCS_strncpy, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strncpy, char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for PCS_strrchr()
 * ----------------------------------------------------
 */
char *PCS_strrchr(const char *s, int c)
{
    UT_GenStub_SetupReturnBuffer(PCS_strrchr, char *);

    UT_GenStub_AddParam(PCS_strrchr, const char *, s);
    UT_GenStub_AddParam(PCS_strrchr, int, c);

    UT_GenStub_Execute(PCS_strrchr, Basic, NULL);

    return UT_GenStub_GetReturnValue(PCS_strrchr, char *);
}
