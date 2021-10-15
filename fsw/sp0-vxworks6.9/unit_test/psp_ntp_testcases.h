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
**    2021-01-05 | Minh Luong       | Initial revision
**    2021-09-13 | Matthew Rumbel   | Added function declaration
**=======================================================================================*/

#ifndef UT_PSP_NTP_TESTCASES_H
#define UT_PSP_NTP_TESTCASES_H

/*=======================================================================================
** Includes
**=======================================================================================*/

#include <errno.h>

#include "utassert.h"
#include "cfe_time_extern_typedefs.h"
#include "psp_time_sync.h"

/*=======================================================================================
** External Global Variable Declarations
**=======================================================================================*/

/*=======================================================================================
** Function Declarations
**=======================================================================================*/
void Ut_CFE_PSP_Sync_From_OS_Enable(void);
void Ut_CFE_PSP_Sync_From_OS_SetFreq(void);
void Ut_CFE_PSP_Sync_From_OS_GetFreq(void);
void Ut_CFE_PSP_Set_OS_Time(void);
void Ut_CFE_PSP_Get_OS_Time(void);
void Ut_CFE_PSP_NTP_Daemon_Get_Status(void);
void Ut_CFE_PSP_NTP_Daemon_Enable(void);
void Ut_CFE_PSP_TIME_Init(void);
void Ut_CFE_PSP_Update_OS_Time(void);
void Ut_CFE_PSP_StartNTPDaemon(void);
void Ut_CFE_PSP_StopNTPDaemon(void);
void Ut_CFE_PSP_TimeService_Ready(void);

void Ut_net_clock_vxworks_Destroy(void);
void Ut_ntp_clock_vxworks_Init(void);

#endif  /* UT_PSP_NTP_TESTCASES_H */

/*=======================================================================================
** End of file psp_ntp_testcases.h
**=======================================================================================*/