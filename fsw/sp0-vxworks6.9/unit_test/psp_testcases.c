/*=======================================================================================
**
** File:  psp_testcases.c
**
** Purpose:
**    This file contains test cases for the PSP.
**
** Modification History:
**    Date       | Author           | Description
**    ---------- | ---------------- | ----------------------------------------------------
**    2020-12-16 | Minh Luong       | Initial revision
**=======================================================================================*/

#include <unistd.h>
#include "uttest.h"
#include "utstubs.h"
#include "psp_testcases.h"

/*=======================================================================================
** Test Setup & Teardown functions
**=======================================================================================*/

void Ut_CFE_PSP_Setup(void)
{
    UT_ResetState(0);
}

void Ut_CFE_PSP_Teardown(void)
{
}

void UtTest_Setup(void)
{
    /* cfe_psp_start.c test cases area */
    UtTest_Add(Ut_CFE_PSP_ProcessPOSTResults,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ProcessPOSTResults");

    UtTest_Add(Ut_CFE_PSP_ProcessResetType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ProcessResetType");

    UtTest_Add(Ut_CFE_PSP_LogSoftwareResetType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_LogSoftwareResetType");

    UtTest_Add(Ut_OS_Application_Startup,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Startup"); 

    UtTest_Add(Ut_CFE_PSP_GetRestartType,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetRestartType"); 

    UtTest_Add(Ut_SetTaskPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_SetTaskPrio");

    UtTest_Add(Ut_SetSysTasksPrio,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_SetSysTasksPrio");

    UtTest_Add(Ut_PSP_1HzLocalCallback,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_PSP_1HzLocalCallback");

    UtTest_Add(Ut_OS_Application_Run,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_OS_Application_Run");

    UtTest_Add(Ut_vxFpscrGet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_vxFpscrGet");

    UtTest_Add(Ut_vxFpscrSet,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_vxFpscrSet");

    /* cfe_psp_memory.c test cases area */
    UtTest_Add(Ut_CFE_PSP_GetCDSSize,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetCDSSize");

    UtTest_Add(Ut_CFE_PSP_SetReadCDSMethod,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetReadCDSMethod");

    UtTest_Add(Ut_CFE_PSP_GetReadCDSMethod,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetReadCDSMethod");

    UtTest_Add(Ut_CFE_PSP_SetStaticCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_SetStaticCRC");

    UtTest_Add(Ut_CFE_PSP_GetStaticCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_GetStaticCRC");

    UtTest_Add(Ut_CFE_PSP_CalculateCRC,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_CalculateCRC");

    UtTest_Add(Ut_CFE_PSP_ReadCDSFromFlash,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_ReadCDSFromFlash");

    UtTest_Add(Ut_CFE_PSP_WriteCDSToFlash,
               Ut_CFE_PSP_Setup, Ut_CFE_PSP_Teardown,
               "Ut_CFE_PSP_WriteCDSToFlash");
} 

/*=======================================================================================
** End of file psp_testcases.c
**=======================================================================================*/

