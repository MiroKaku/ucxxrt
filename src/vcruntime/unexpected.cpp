/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      unexpected.cpp
 * DATA:      2020/02/28
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */


#include "vcruntime/ehdata_values.h"
#include "vcruntime/ehhooks.h"
#include "vcruntime/ptd_downlevel.h"


#if __has_include(<wdm.h>)

_CRT_BEGIN_C_HEADER

#if !_HAS_FLOATPOINT
_ACRTIMP __declspec(noreturn) void __cdecl abort(void)
{
    KeBugCheckEx(
        KMODE_EXCEPTION_NOT_HANDLED,
        EH_EXCEPTION_NUMBER,
        (ULONG_PTR)_ReturnAddress(),
        EH_MAGIC_NUMBER1,
        0);
}
#endif

_ACRTIMP __declspec(noreturn) void __cdecl terminate(void) throw()
{
    KeBugCheckEx(
        KMODE_EXCEPTION_NOT_HANDLED,
        EH_EXCEPTION_NUMBER,
        (ULONG_PTR)_ReturnAddress(),
        EH_MAGIC_NUMBER1,
        1);
}

static unexpected_handler __cdecl get_unexpected_or_default(
    RENAME_BASE_PTD(ucxxrt::__ucxxrt_ptd) const* const ptd
) noexcept
{
    return ptd->_unexpected ? ptd->_unexpected : &terminate;
}

_CRTIMP unexpected_handler __cdecl _get_unexpected(void) noexcept
{
    return get_unexpected_or_default(RENAME_BASE_PTD(ucxxrt::__ucxxrt_getptd)());
}

_CRTIMP unexpected_handler __cdecl set_unexpected(
    _In_opt_ unexpected_handler const new_handler
) noexcept
{
    RENAME_BASE_PTD(ucxxrt::__ucxxrt_ptd)* const ptd = RENAME_BASE_PTD(ucxxrt::__ucxxrt_getptd)();

    unexpected_handler const old_handler = get_unexpected_or_default(ptd);

    ptd->_unexpected = new_handler;

    return old_handler;
}

_VCRTIMP __declspec(noreturn) void __cdecl unexpected(void) noexcept(false)
{
    unexpected_handler const handler = RENAME_BASE_PTD(ucxxrt::__ucxxrt_getptd)()->_unexpected;
    if (handler)
    {
        handler();
    }

    terminate();
}
_CRT_END_C_HEADER


namespace std
{
    using ::abort;
    using ::terminate;
}
#endif
