/*********************************************************************************************************/
/** \export_control EAR ECCN 9D515.a, 9E515.a, License Exception GOV 740.11 (b)(2)(ii):
*   This document contains data within the purview of the U.S. Export Administration Regulations (EAR),
*   15 CFR 730-774, as is classified as ECCN 9E515.a. These items are controlled by the U.S. Government
*   and are authorized for export by NASA only to fulfill responsibilities of the parties or of a
*   Cooperating Agency of a NASA Gateway program partner (CSA, ESA, JAXA) and their contractors using
*   License Exception GOV 740.11 (b)(2)(ii) in furtherance of the ISS Intergovernmental Agreement and
*   Gateway MOUs. They may not be resold, transferred, or otherwise disposed of, to any other country
*   or to any person other than the authorized ultimate consignee or end-user(s), either in their
*   original form or after being incorporated into other items, without first obtaining approval from
*   the U.S. government or as otherwise authorized by U.S. law and regulations.
*
*   \file ut_psp_utils.h
*
*   \brief This file defines data types, internal macros, local constants, global variables and function
*   declarations used by os_stubs, kernel_stubs.
*
*   \copyright
*   Copyright © 2023-2024 United States Government as represented by the Administrator of the National
*   Aeronautics and Space Administration. All Other Rights Reserved.
*
*   \brief This file was created at the NASA Johnson Space Center.
**********************************************************************************************************/

/*=======================================================================================
** Includes
**=======================================================================================*/

/* #include "cfe.h" */
#include "utstubs.h"
#include "uttools.h"

#ifndef _UT_PSP_UTILS_H_
#define _UT_PSP_UTILS_H_

#define     URM_SIZE        10000

/*=======================================================================================
** Macro Definitions
**=======================================================================================*/

/****************** Helper macros for PSP asserts ******************/
#define     UtAssert_OS_print(cText, Description) \
                UtAssert(Ut_OS_printfHistoryWithText(cText), Description, __FILE__, __LINE__)
#define     UtAssert_NoOS_print(cText, Description) \
                UtAssert(Ut_OS_printfHistoryWithText(cText) == false, Description, __FILE__, __LINE__)
#define     UtAssert_ES_WriteToSysLog(cText, Description) \
                UtAssert(Ut_ES_WriteToSysLogWithText(cText), Description, __FILE__, __LINE__)
#define     UtAssert_NoES_WriteToSysLog(cText, Description) \
                UtAssert(Ut_ES_WriteToSysLogWithText(cText) == false, Description, __FILE__, __LINE__)
#define     UtAssert_logMsg(cText, Description) \
                UtAssert(Ut_logMsgHistoryWithText(cText), Description, __FILE__, __LINE__)
#define     UtAssert_NologMsg(cText, Description) \
                UtAssert(Ut_logMsgHistoryWithText(cText) == false, Description, __FILE__, __LINE__)

/*=======================================================================================
** Data Structures
**=======================================================================================*/

char nvram[2000];

/*=======================================================================================
** Function Declarations
**=======================================================================================*/
int32   Ut_OS_printf_Hook(void *UserObj, int32 StubRetcode,
                          uint32 CallCount, const UT_StubContext_t *Context, va_list va);
void    Ut_OS_printf_Setup(void);
bool    Ut_OS_printfHistoryWithText(const char *cText);
int32   Ut_ES_WriteToSysLog_Hook(void *UserObj, int32 StubRetcode,
                                 uint32 CallCount, const UT_StubContext_t *Context, va_list va);
void    Ut_ES_WriteToSysLog_Setup(void);
bool    Ut_ES_WriteToSysLogWithText(const char *cText);
int32   Ut_logMsg_hook(void *UserObj, int32 StubRetcode,
                       uint32 CallCount, const UT_StubContext_t *Context, va_list va);
void    Ut_logMsg_Setup(void);
bool    Ut_logMsgHistoryWithText(const char *cText);

/**
 ** \brief Prints the whole array of strings printed using OS_printf
 ** \par Description:
 ** DEBUG Helper function Print all OS_printf messages
 ** \param[in] quite - When True, it will only return the number of messages
 ** in the OS_printf array
 */
void    Ut_OS_printfPrint(void);

/**
 ** \brief Gives the number of messages printed
 ** \return uint8
 */
uint8   Ut_OS_printf_MsgCount(void);

int32 taskPriorityGet(int32 iTid, int32 *piCurPrio);
int32 taskPrioritySet(int32 iTid, int32 iNewPrio);
char * taskName(TASK_ID tid);
int aimonGetBITResults(uint64 *pulResult, int iValue);
int aimonGetBITExecuted(uint64 *pulRun, int iValue);
char *sysModel(void);
uint32 getCoreClockSpeed(void);
int returnSelectedBootFlash(void);

int PCS_snprintf(char *s, size_t maxlen, const char *format, ...);
int PCS_OS_BSPMain(void);
STATUS userNvRamSet (char *dat_ptr, int nbytes, int offset);
STATUS userNvRamGet (char *dat_ptr, int nbytes, int offset);
void userReservedGet( char **  pUserReservedAddr, size_t * pUserReservedSize );
void reboot(int iBootType);

void SystemMain(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath);

#endif  /* _UT_PSP_UTILS_H_ */

/*=======================================================================================
** End of file ut_psp_stubs.h
**=======================================================================================*/
