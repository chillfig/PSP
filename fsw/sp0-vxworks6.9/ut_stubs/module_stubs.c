#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"

/* ----------------------------------------- */
/* types normally defined in moduleLib.h */
/* ----------------------------------------- */
typedef struct MODULE { int m; }    MODULE;
typedef MODULE*                     MODULE_ID;

typedef struct MODULE_INFO
{
    struct
    {
        unsigned long textAddr;
        unsigned long textSize;
        unsigned long dataAddr;
        unsigned long dataSize;
        unsigned long bssAddr;
        unsigned long bssSize;
    } segInfo;

} MODULE_INFO;

MODULE_ID moduleFindByName(const char *moduleName)
{
    MODULE_ID retval;
    int32 iStatus;

    retval = NULL;
    iStatus = UT_DEFAULT_IMPL(moduleFindByName);
    if (iStatus == 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(moduleFindByName), &retval, sizeof(retval));
    }

    return (MODULE_ID)iStatus;
}

int moduleInfoGet(MODULE_ID moduleId, MODULE_INFO *pModuleInfo)
{
    int32 iStatus;

    UT_Stub_RegisterContext(UT_KEY(moduleInfoGet), pModuleInfo);

    iStatus = UT_DEFAULT_IMPL(moduleInfoGet);

    if (iStatus >= 0)       
    {
        UT_Stub_CopyToLocal(UT_KEY(moduleInfoGet), (MODULE_INFO *)pModuleInfo, sizeof(*pModuleInfo));
    }

    return iStatus;
}
