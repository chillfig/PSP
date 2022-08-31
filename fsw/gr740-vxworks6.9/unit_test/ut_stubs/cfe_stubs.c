#include <string.h>
#include <ioLib.h>
#include <stdio.h>
#include "utstubs.h"

typedef struct CFE_TIME_SysTime
{
    uint32 Seconds;    /**< \brief Number of seconds since epoch */
    uint32 Subseconds; /**< \brief Number of subseconds since epoch (LSB = 2^(-32) seconds) */
} CFE_TIME_SysTime_t;

/*****************************************************************************/
/**
** \brief CFE_ES_WriteToSysLog stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_WriteToSysLog.  It always returns CFE_SUCCESS when called.
**        The log message is compared to known CFE function responses and if
**        a match is found the value WriteSysLogRtn.value is set to a
**        specified constant value.  WriteSysLogRtn.count is incremented to
**        keep track of the number of messages logged.  The unit test code
**        compares the WriteSysLogRtn value and count variables against
**        expected totals to ensure the proper response of other functions
**        being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \return
**        Returns CFE_SUCCESS.
**
******************************************************************************/

uint32 CFE_TIME_Micro2SubSecs(uint32 MicroSeconds)
{
    int32 iVal;

    iVal = UT_DEFAULT_IMPL(CFE_TIME_Micro2SubSecs);
    
    return iVal;
}
