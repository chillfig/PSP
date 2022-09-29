/*=======================================================================================
**
** File:  ut_psp_utils.h
**
** Purpose:
**    This file defines data types, internal macros, local constants, global variables,
**    and function declarations used by os_stubs, kernel_stubs.
**
** Modification History:
**    Date       | Author            | Description
**    ---------- | ----------------- | ----------------------------------------------------
**    2020-12-21 | Minh Luong        | Initial Revision
**=======================================================================================*/


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
uint32_t CFE_SUPPORT_GetProcessorIdx(void);
STATUS userNvRamSet (char *dat_ptr, int nbytes, int offset);
STATUS userNvRamGet (char *dat_ptr, int nbytes, int offset);
void userReservedGet( char **  pUserReservedAddr, size_t * pUserReservedSize );
void reboot(int iBootType);

void SystemMain(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath);

#endif  /* _UT_PSP_UTILS_H_ */

/*=======================================================================================
** End of file ut_psp_stubs.h
**=======================================================================================*/
