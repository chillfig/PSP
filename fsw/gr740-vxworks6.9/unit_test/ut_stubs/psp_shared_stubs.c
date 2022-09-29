#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include "utstubs.h"
#include "ut_psp_utils.h"
#include "cfe_psp_exceptionstorage_types.h"

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

/* From PSP Shared */
CFE_PSP_Exception_LogData_t * CFE_PSP_Exception_GetNextContextBuffer(void)
{
    static CFE_PSP_Exception_LogData_t retVal;
    CFE_PSP_Exception_LogData_t *pRetVal = NULL;
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(CFE_PSP_Exception_GetNextContextBuffer);

    if(iStatus == 0)
        pRetVal = &retVal;
    
    return pRetVal;
}

/* From PSP Shared */
uint32 CFE_PSP_Exception_GetCount(void)
{
    int32 ret_value;

    ret_value = UT_DEFAULT_IMPL(CFE_PSP_Exception_GetCount);

    return ret_value;
}

/* From PSP Shared */
void CFE_PSP_Exception_WriteComplete(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(CFE_PSP_Exception_WriteComplete);
}

/* From PSP Shared */
int32 CFE_PSP_MemRangeSet(uint32 RangeNum, 
                          uint32 MemoryType, 
                          cpuaddr StartAddr, 
                          size_t Size, 
                          size_t WordSize,
                          uint32 Attributes)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(CFE_PSP_MemRangeSet);
    
    return iStatus;
}

int currentedrPolicyHandlerHook1(int type, void *pInfo_param, BOOL debug)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(currentedrPolicyHandlerHook1);
    
    return iStatus;
}

/* From Aitech */
void * edrPolicyHandlerHookGet(void)
{
    int32 iStatus;
    void *pRetVal = NULL;

    iStatus = UT_DEFAULT_IMPL(edrPolicyHandlerHookGet);
    
    if(iStatus == 0)
        pRetVal = &currentedrPolicyHandlerHook1;

    return pRetVal;
}

/* From Aitech */
int edrErrorPolicyHookRemove(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(edrErrorPolicyHookRemove);

    return iStatus;
}

/* From Aitech */
int edrPolicyHandlerHookAdd(void * pPtrFunc)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(edrPolicyHandlerHookAdd);

    return iStatus;
}

MODULE_ID moduleFindByName(const char *moduleName)
{
    MODULE_ID retval = NULL;
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(moduleFindByName);
    if (iStatus == 0)
    {
        retval = 0x12345678;
    }
    return retval;
}

int moduleInfoGet(MODULE_ID moduleId, MODULE_INFO *pModuleInfo)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(moduleInfoGet);

    if (iStatus == 0 &&
        UT_Stub_CopyToLocal(UT_KEY(moduleInfoGet), (MODULE_INFO *)pModuleInfo, sizeof(*pModuleInfo)) < sizeof(*pModuleInfo))
    {
        memset(pModuleInfo, 0, sizeof(*pModuleInfo));
    }

    return iStatus;
}

void CFE_PSP_ModuleInit(void)
{

}

uint32_t CFE_SUPPORT_GetProcessorIdx(void)
{
    int32 iStatus;
    uint32 uiRet_Value = 4;

    iStatus = UT_DEFAULT_IMPL(CFE_SUPPORT_GetProcessorIdx);

    if (iStatus == 0)
    {
        uiRet_Value = 0;
    }

    return uiRet_Value;
}

int PCS_OS_BSPMain(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_OS_BSPMain);

    return iStatus;
}
