//
// per_thread_data.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Per-Thread Data (PTD) used by the VCRuntime.
//
#include <vcruntime_internal.h>
#include <stdint.h>


EXTERN_C_START

struct __vcrt_ptd_ext : public RENAME_BASE_PTD(__vcrt_ptd)
{
    volatile bool _init;
};

static __vcrt_ptd_ext __vcrt_ptd_data[0x4000];

extern"C" ULONG_PTR __GetCurrentThreadId()
{
#ifdef _KERNEL_MODE
    return (ULONG_PTR)PsGetCurrentThreadId();
#else
    return (ULONG_PTR)GetCurrentThreadId();
#endif
}

RENAME_BASE_PTD(__vcrt_ptd)* __cdecl RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))()
{
    auto ptd = &__vcrt_ptd_data[(__GetCurrentThreadId() >> 1) % _countof(__vcrt_ptd_data)];
    if (_InterlockedCompareExchange8((volatile char*)&ptd->_init, true, false) == false)
    {
#if defined _M_X64 || defined _M_ARM || defined _M_ARM64 || defined _M_HYBRID
        ptd->_CatchStateInParent = INVALID_CATCH_SPECIFIC_STATE;
#else
        __nop();
#endif
    }

    return ptd;
}

RENAME_BASE_PTD(__vcrt_ptd)* __cdecl RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd_noexit))()
{
    return RENAME_UCXXRT(__vcrt_getptd)();
}

RENAME_BASE_PTD(__vcrt_ptd)* __cdecl RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd_noinit))()
{
    return RENAME_UCXXRT(__vcrt_getptd)();
}

EXTERN_C_END
