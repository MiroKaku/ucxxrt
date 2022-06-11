/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      unexpected.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// unexpected.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The unexpected handler
//
#include <eh.h>
#include <vcruntime_internal.h>



static unexpected_handler __cdecl get_unexpected_or_default(
    RENAME_BASE_PTD(__vcrt_ptd) const* const ptd
    ) noexcept
{
    return ptd->_unexpected ? ptd->_unexpected : &terminate;
}

EXTERN_C unexpected_handler __cdecl _get_unexpected() noexcept
{
    return get_unexpected_or_default(RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))());
}

EXTERN_C unexpected_handler __cdecl set_unexpected(
    unexpected_handler const new_handler
    ) noexcept
{
    RENAME_BASE_PTD(__vcrt_ptd)* const ptd = RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))();

    unexpected_handler const old_handler = get_unexpected_or_default(ptd);

    ptd->_unexpected = new_handler;

    return old_handler;
}

EXTERN_C void __cdecl unexpected() noexcept(false)
{
    unexpected_handler const handler = RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))()->_unexpected;
    if (handler)
    {
        handler();
    }

    terminate();
}
