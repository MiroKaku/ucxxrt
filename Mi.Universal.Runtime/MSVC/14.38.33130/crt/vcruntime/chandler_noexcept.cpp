//
// Ref: https://github.com/Chuyu-Team/VC-LTL5/blob/master/Sources/crt/vcruntime/chandler_noexcept.cpp
// Git: https://github.com/Chuyu-Team/VC-LTL5
// 
// author: mingkuang-Chuyu
//
// License: Eclipse Public License 2.0
//

#include <vcruntime_internal.h>

#define EH_EXCEPTION_NUMBER ('msc' | 0xE0000000)

#if defined _M_AMD64 || defined _M_ARM || defined _M_ARM64

extern "C" EXCEPTION_DISPOSITION __cdecl __C_specific_handler_noexcept(
    _In_    struct _EXCEPTION_RECORD* ExceptionRecord,
    _In_    void* EstablisherFrame,
    _Inout_ struct _CONTEXT* ContextRecord,
    _Inout_ struct _DISPATCHER_CONTEXT* DispatcherContext
)
{
    auto Excption = __C_specific_handler(ExceptionRecord, EstablisherFrame, ContextRecord, DispatcherContext);

    if (IS_DISPATCHING(ExceptionRecord->ExceptionFlags)
        && ExceptionRecord->ExceptionCode == EH_EXCEPTION_NUMBER
        && Excption == ExceptionContinueSearch)
    {
        terminate();
    }

    return Excption;
}

#elif defined _M_IX86

extern "C" EXCEPTION_DISPOSITION __cdecl _except_handler4(
    IN struct _EXCEPTION_RECORD* ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT struct _CONTEXT* ContextRecord,
    IN OUT PVOID DispatcherContext
);

extern "C" DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl _except_handler4_noexcept(
    _In_ PEXCEPTION_RECORD                ExceptionRecord,
    _In_ PEXCEPTION_REGISTRATION_RECORD   EstablisherFrame,
    _Inout_ PCONTEXT                     ContextRecord,
    _Inout_ PVOID                        DispatcherContext
)
{
    auto Excption = _except_handler4(ExceptionRecord, EstablisherFrame, ContextRecord, DispatcherContext);

    if (IS_DISPATCHING(ExceptionRecord->ExceptionFlags)
        && ExceptionRecord->ExceptionCode == EH_EXCEPTION_NUMBER
        && Excption == ExceptionContinueSearch)
    {
        terminate();
    }

    return Excption;
}

#endif
