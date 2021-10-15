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

#ifndef _UT_PSP_UTILS_H_
#define _UT_PSP_UTILS_H_

/*=======================================================================================
** Includes
**=======================================================================================*/

/* #include "cfe.h" */
#include "utstubs.h"
/* #include "ut_support.h" when away for the below ??? */
#include "uttools.h"

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

#endif  /* _UT_PSP_UTILS_H_ */

/*=======================================================================================
** End of file ut_vsm_stubs.h
**=======================================================================================*/