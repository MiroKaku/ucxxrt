/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      invalid_parameter.cpp
 * DATA:      2020/02/05
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// invalid_parameter.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The invalid parameter handlers and related functionality
//

#include <vcruntime_exception.h>

_STD_BEGIN

#if !_HAS_EXCEPTIONS
_CRTIMP2_PURE_IMPORT _Prhand _Raise_handler = nullptr; // pointer to raise handler
#endif

extern "C" void __cdecl _invalid_parameter_advanced(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved
)
{
    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

#ifdef _DEBUG
extern "C" void __cdecl _invalid_parameter(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved)
{
    return _invalid_parameter_advanced(expression, function_name, file_name, line_number, reserved);
}
#endif

extern "C" void __cdecl _invalid_parameter_noinfo()
{
    _invalid_parameter_advanced(nullptr, nullptr, nullptr, 0, 0);
}

// This is used by inline code in the C++ Standard Library and the SafeInt
// library.  Because it is __declspec(noreturn), the compiler can better
// optimize use of the invalid parameter handler for inline code.
extern "C" __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn()
{
#ifdef _DEBUG
    _invalid_parameter(nullptr, nullptr, nullptr, 0, 0);
#endif

    _invoke_watson(nullptr, nullptr, nullptr, 0, 0);
}

extern "C" __declspec(noreturn) void __cdecl _invoke_watson(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved
)
{
    UNREFERENCED_PARAMETER(expression);
    UNREFERENCED_PARAMETER(function_name);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);
    UNREFERENCED_PARAMETER(reserved);

    __fastfail(FAST_FAIL_INVALID_ARG);
}

_STD_END
