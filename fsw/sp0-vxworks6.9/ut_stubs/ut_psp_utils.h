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

#include "cfe.h"
#include "utstubs.h"
#include "ut_support.h"

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



#endif  /* _UT_PSP_UTILS_H_ */

/*=======================================================================================
** End of file ut_vsm_stubs.h
**=======================================================================================*/
