/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      dbgrpt.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/***
*dbgrpt.c - Debug CRT Reporting Functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#if defined(_DEBUG) and defined(_KERNEL_MODE)

_CRT_BEGIN_C_HEADER

/***
*int _CrtDbgReport() - primary reporting function
*
*Purpose:
*       Display a message window with the following format.
*
*       ================= Microsft Visual C++ Debug Library ================
*
*       {Warning! | Error! | Assertion Failed!}
*
*       Program: c:\test\mytest\foo.exe
*       [Module: c:\test\mytest\bar.dll]
*       [File: c:\test\mytest\bar.c]
*       [Line: 69]
*
*       {<warning or error message> | Expression: <expression>}
*
*       [For information on how your program can cause an assertion
*        failure, see the Visual C++ documentation on asserts]
*
*       (Press Retry to debug the application)
*
*       ===================================================================
*
*Entry:
*       int             nRptType    - report type
*       void *          returnAddress - return address of caller
*       const TCHAR *    szFile      - file name
*       int             nLine       - line number
*       const TCHAR *    szModule    - module name
*       const TCHAR *    szFormat    - format string
*       ...                         - var args
*
*Exit:
*       if (MessageBox)
*       {
*           Abort -> aborts
*           Retry -> return TRUE
*           Ignore-> return FALSE
*       }
*       else
*           return FALSE
*
*Exceptions:
*       If something goes wrong, we do not assert, but we return -1.
*
*******************************************************************************/
int __cdecl _CrtDbgReport(
    int         const /*report_type*/,
    char const* const /*file_name*/,
    int         const /*line_number*/,
    char const* const /*module_name*/,
    char const* const /*format*/,
    ...)
{
    // TODO
    return TRUE;
}

int __cdecl _CrtDbgReportW(
    int            const /*report_type*/,
    wchar_t const* const /*file_name*/,
    int            const /*line_number*/,
    wchar_t const* const /*module_name*/,
    wchar_t const* const /*format*/,
    ...)
{
    // TODO
    return TRUE;
}

_CRT_END_C_HEADER
#endif
