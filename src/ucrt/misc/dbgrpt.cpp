/***
*dbgrpt.c - Debug CRT Reporting Functions
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*
*******************************************************************************/

#if defined(_DEBUG)

#pragma warning(push)
#pragma warning(disable: 4996)

#include <stdio.h>
#include <stdarg.h>
#include <crtdbg.h>

#include <ntstrsafe.h>

_CRT_BEGIN_C_HEADER

int _CrtDbgMode[_CRT_ERRCNT] = {
        _CRTDBG_MODE_DEBUG,
        _CRTDBG_MODE_DEBUG,
        _CRTDBG_MODE_DEBUG
};

_HFILE _CrtDbgFile[_CRT_ERRCNT] = { _CRTDBG_INVALID_HFILE,
                                    _CRTDBG_INVALID_HFILE,
                                    _CRTDBG_INVALID_HFILE
};

static const char* _CrtDbgModeMsg[_CRT_ERRCNT] = { "Warning", "Error", "Assertion Failed" };

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
    int         const report_type,
    char const* const file_name,
    int         const line_number,
    char const* const module_name,
    char const* const format,
    ...)
{
    va_list args;
    va_start(args, format);
    {
        int count = _vscprintf(format, args);
        if (count <= 0)
        {
            return FALSE;
        }

        count += sizeof("");

        char* buffer = (char*)malloc(count);
        if (buffer == nullptr)
        {
            return FALSE;
        }

        _vsnprintf(buffer, count, format, args);

        DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL,
            "================= Microsft Visual C++ Debug Library ================\n"
            "\n"
            "%s"
            "\n"
            "Module: %s\n"
            "File  : %s\n"
            "Line  : %d\n"
            "\n"
            "[%s]\n",
            _CrtDbgModeMsg[report_type],
            module_name,
            file_name,
            line_number,
            buffer);

        free(buffer);
    }
    va_end(args);
    
    return FALSE;
}

int __cdecl _CrtDbgReportW(
    int            const report_type,
    wchar_t const* const file_name,
    int            const line_number,
    wchar_t const* const module_name,
    wchar_t const* const format,
    ...)
{
    va_list args;
    va_start(args, format);
    {
        int count = _vscwprintf(format, args);
        if (count <= 0)
        {
            return FALSE;
        }

        count += sizeof("");

        wchar_t* buffer = (wchar_t*)malloc(count * sizeof(L""));
        if (buffer == nullptr)
        {
            return FALSE;
        }

        _vsnwprintf(buffer, count, format, args);

        DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL,
            "================= Microsft Visual C++ Debug Library ================\n"
            "\n"
            "%ls"
            "\n"
            "Module: %ls\n"
            "File  : %ls\n"
            "Line  : %d\n"
            "\n"
            "[%ls]\n",
            _CrtDbgModeMsg[report_type],
            module_name,
            file_name,
            line_number,
            buffer);

        free(buffer);
    }
    va_end(args);

    return FALSE;
}

#pragma warning(pop)

_CRT_END_C_HEADER

#endif // _DEBUG
