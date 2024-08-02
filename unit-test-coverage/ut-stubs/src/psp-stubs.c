#include <stdint.h>

#include "utstubs.h"

int PCS_OS_BSPMain(void)
{
    int32_t iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_OS_BSPMain);

    return iStatus;
}

int32 PCS_CFE_PSP_MemRangeSet(uint32 RangeNum, uint32 MemoryType, cpuaddr StartAddr, 
                              size_t Size, size_t WordSize, uint32 Attributes)
{
    int32 iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_CFE_PSP_MemRangeSet);
    
    return iStatus;
}
