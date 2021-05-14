/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      throw_bad_alloc.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// throw_bad_alloc.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines functions that throw std::bad_alloc and std::bad_array_new_length.
// We cannot include the <vcruntime_exception.h> header in the source files
// that define the various operator new functions because that header defines
// inline functions that may be referenced by user code.  The operators
// themselves are replaceable by the user.
//
#include <exception>
#include <vcruntime_exception.h>
#include <internal_shared.h>


__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_alloc()
{
    throw std::bad_alloc{};
}

__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_array_new_length()
{
    throw std::bad_array_new_length{};
}