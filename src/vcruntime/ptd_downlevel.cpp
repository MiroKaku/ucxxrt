/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ptd_downlevel.cpp
 * DATA:      2020/02/08
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include "vcruntime/ptd_downlevel.h"
#include <intrin.h>

namespace ucxxrt
{
    static __ucxxrt_ptd ___ptd_downlevel_data[0x4000];

    extern"C" ULONG_PTR __GetCurrentThreadId()
    {
#if __has_include(<wdm.h>)
        return (ULONG_PTR)PsGetCurrentThreadId();
#else
        return (ULONG_PTR)GetCurrentThreadId();
#endif
    }

    extern"C" __ucxxrt_ptd * __cdecl __ucxxrt_getptd()
    {
        auto ptd =  &___ptd_downlevel_data[(__GetCurrentThreadId() >> 1) % _countof(___ptd_downlevel_data)];
        if (_InterlockedCompareExchange8((volatile char*)&ptd->_init, true, false) == false)
        {
#if defined _M_X64 || defined _M_ARM || defined _M_ARM64 || defined _M_HYBRID
            ptd->_CatchStateInParent = INVALID_CATCH_SPECIFIC_STATE;
#else
            ;
#endif
        }

        return ptd;
    }
}
