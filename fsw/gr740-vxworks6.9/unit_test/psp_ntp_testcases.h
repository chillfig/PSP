/*=======================================================================================
**
** File:  psp_ntp_testcases.h
**
** Purpose:
**    This file contains function declarations of test cases for the cfe_psp_ntp.c.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**
**=======================================================================================*/

#ifndef UT_PSP_NTP_TESTCASES_H
#define UT_PSP_NTP_TESTCASES_H

/*=======================================================================================
** Includes
**=======================================================================================*/

#include <errno.h>

#include "utassert.h"
#include "cfe_time_extern_typedefs.h"
#include "cfe_psp_timesync.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function Declarations
**=======================================================================================*/
void Ut_CFE_PSP_SetOSTime(void);
void Ut_CFE_PSP_GetOSTime(void);
void Ut_CFE_PSP_NTPDaemonIsRunning(void);
void Ut_CFE_PSP_StartNTPDaemon(void);
void Ut_CFE_PSP_StopNTPDaemon(void);

#endif  /* UT_PSP_NTP_TESTCASES_H */

/*=======================================================================================
** End of file psp_ntp_testcases.h
**=======================================================================================*/