//
// _ctype.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the function equivalents of the character classification macros in
// <ctype.h>.  These function definitions make use of the macros.  The functions
// return zero if the character does not meet the requirements, and nonzero if
// the character does meet the requirements.
//
#include <corecrt_internal.h>
#include <ctype.h>
#include <locale.h>

// The ctype functions (isalnum(), isupper(), etc) are very small and quick.
// Optimizing for size has a measurable negative impact, so we optimize for speed here.
#pragma optimize("t", on)


extern "C" int (__cdecl _isalpha_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isalpha(c);
}

extern "C" int (__cdecl isalpha)(int const c)
{
    return
        (c >= 0x41 && c <= 0x5a) ||
        (c >= 0x61 && c <= 0x7a);
}


extern "C" int (__cdecl _isupper_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isupper(c);
}


extern "C" int (__cdecl _islower_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return islower(c);
}


extern "C" int (__cdecl _isdigit_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isdigit(c);
}


extern "C" int (__cdecl _isxdigit_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isxdigit(c);
}


extern "C" int (__cdecl _isspace_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isspace(c);
}


extern "C" int (__cdecl _ispunct_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return ispunct(c);
}

extern "C" int (__cdecl ispunct)(int const c)
{
    return
        (c >= 0x21 && c <= 0x2f) ||
        (c >= 0x3a && c <= 0x40) ||
        (c >= 0x5b && c <= 0x60) ||
        (c >= 0x7b && c <= 0x7e);
}


extern "C" int (__cdecl _isblank_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isblank(c);
}

extern "C" int (__cdecl isblank)(int const c)
{
    // \t is a blank character, but is not registered as _Blank on the table, because that will make it
    //printable. Also Windows (via GetStringType()) considered all _BLANK characters to also be _PRINT characters,
    //so does not have a way to specify blank, non-printable.
    if (c == '\t') {
        return _BLANK;
    }

    if (c == ' ') {
        return _SPACE;
    }

    return 0;
}


extern "C" int (__cdecl _isalnum_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isalnum(c);
}

extern "C" int (__cdecl isalnum)(int const c)
{
    return isdigit(c) || isalpha(c);
}


extern "C" int (__cdecl _isprint_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isprint(c);
}


extern "C" int (__cdecl _isgraph_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return isgraph(c);
}

extern "C" int (__cdecl isgraph)(int const c)
{
    return !iscntrl(c);
}


extern "C" int (__cdecl _iscntrl_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return iscntrl(c);
}

extern "C" int (__cdecl iscntrl)(int const c)
{
    return (c == 0x7f || (c >= 0x00 && c <= 0x1f));
}


extern "C" int (__cdecl __isascii)(int const c)
{
    return ((unsigned)(c) < 0x80);
}

extern "C" int (__cdecl __toascii)(int const c)
{
    return ((c) & 0x7f);
}


extern "C" int (__cdecl _iscsymf_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return __iscsymf(c);
}

extern "C" int (__cdecl __iscsymf)(int const c)
{
    return (isalpha(c) || ((c) == '_'));
}


extern "C" int (__cdecl _iscsym_l)(int const c, _locale_t const locale)
{
    UNREFERENCED_PARAMETER(locale);
    return __iscsym(c);
}

extern "C" int (__cdecl __iscsym)(int const c)
{
    return (isalnum(c) || ((c) == '_'));
}
