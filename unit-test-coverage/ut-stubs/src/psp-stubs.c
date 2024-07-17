#include <stdint.h>

#include "utstubs.h"

int PCS_OS_BSPMain(void)
{
    int32_t iStatus;

    iStatus = UT_DEFAULT_IMPL(PCS_OS_BSPMain);

    return iStatus;
}
