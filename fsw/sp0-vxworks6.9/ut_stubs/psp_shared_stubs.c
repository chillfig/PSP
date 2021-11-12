#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"
#include "cfe_psp_exceptionstorage_types.h"

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

void CFE_PSP_ModuleInit(void)
{   
    
}
