/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      handlers.cpp
 * DATA:      2020/02/10
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include "vcruntime/ehdata.h"
#include "vcruntime/ehdata4.h"


namespace ucxxrt
{

#if defined _M_X64 || defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_

    EXTERN_C PVOID __cdecl RENAME_EH_EXTERN(_CallSettingFrame)(
        void* handler,
        EHRegistrationNode* pEstablisher,
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
        PULONG pNonVolatiles,
#endif
        ULONG NLG_CODE
        )
    {
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
#error Not yet implemented.
#else
        return static_cast<void* (__cdecl*)(void*, EHRegistrationNode, ULONG)>(handler)(handler, *pEstablisher, NLG_CODE);
#endif
    }

    EXTERN_C PVOID __cdecl RENAME_EH_EXTERN(_CallSettingFrame_LookupContinuationIndex)(
        void* handler,
        EHRegistrationNode* pEstablisher,
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
        PULONG pNonVolatiles,
#endif
        ULONG NLG_CODE
        )
    {
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
#error Not yet implemented.
#else
        return static_cast<void* (__cdecl*)(void*, EHRegistrationNode, ULONG)>(handler)(handler, *pEstablisher,NLG_CODE);
#endif
    }

    EXTERN_C PVOID __cdecl RENAME_EH_EXTERN(_CallSettingFrameEncoded)(
        void* handler,
        EHRegistrationNode Establisher,
        void* object,
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
        PULONG pNonVolatiles,
#endif
        ULONG NLG_CODE
        )
    {
#if defined _M_ARM_NT || defined _M_ARM64 || defined _CHPE_X86_ARM64_EH_
#error Not yet implemented.
#else
        return static_cast<void* (__cdecl*)(void*, EHRegistrationNode, ULONG)>(handler)(object, Establisher, NLG_CODE);
#endif
    }

#endif

}
