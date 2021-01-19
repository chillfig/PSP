#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "utstubs.h"
#include "cfe_psp_exceptionstorage_types.h"

CFE_PSP_Exception_LogData_t * CFE_PSP_Exception_GetNextContextBuffer(void)
{
    CFE_PSP_Exception_LogData_t retVal;
    CFE_PSP_Exception_LogData_t *pRetVal = NULL;
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(CFE_PSP_Exception_GetNextContextBuffer);

    if(iStatus == 0)
        pRetVal = &retVal;
    
    return pRetVal;
}

void CFE_PSP_Exception_WriteComplete(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(CFE_PSP_Exception_WriteComplete);
}

int currentedrPolicyHandlerHook1(int type, void *pInfo_param, BOOL debug)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(currentedrPolicyHandlerHook1);
    
    return iStatus;
}

void * edrPolicyHandlerHookGet(void)
{
    int32 iStatus;
    void *pRetVal = NULL;

    iStatus = UT_DEFAULT_IMPL(edrPolicyHandlerHookGet);
    
    if(iStatus == 0)
        pRetVal = &currentedrPolicyHandlerHook1;

    return pRetVal;
}

int edrErrorPolicyHookRemove(void)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(edrErrorPolicyHookRemove);

    return iStatus;
}

int edrPolicyHandlerHookAdd(void * pPtrFunc)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(edrPolicyHandlerHookAdd);

    return iStatus;
}

