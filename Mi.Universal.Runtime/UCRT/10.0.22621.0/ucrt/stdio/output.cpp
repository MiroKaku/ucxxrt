//
// output.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The standard _output functions, which perform formatted output to a stream.
//

#pragma warning(push)
#pragma warning(disable: 4996)

#include <stdio.h>
#include <wchar.h>
#include <stdarg.h>

_CRT_BEGIN_C_HEADER

_Check_return_
_CRT_STDIO_INLINE int __CRTDECL _vscprintf(
    _In_z_ _Printf_format_string_ char const* const _Format,
    va_list           _ArgList
)
{
    return _vsnprintf(0, 0, _Format, _ArgList);
}

_Success_(return >= 0)
_Check_return_
_CRT_STDIO_INLINE int __CRTDECL _vscwprintf(
    _In_z_ _Printf_format_string_ wchar_t const* const _Format,
    va_list              _ArgList
)
{
    return _vsnwprintf(0, 0, _Format, _ArgList);
}

_CRT_END_C_HEADER

#pragma warning(pop)
