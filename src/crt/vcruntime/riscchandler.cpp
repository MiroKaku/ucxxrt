/*++

Copyright (c) Microsoft Corporation

Module Name:

    riscchandler.cpp

Abstract:

    This module implements the C specific exception handler that provides
    structured condition handling for the C language on RISC and AMD64
    platforms.

--*/

#include <vcruntime_internal.h>

#ifndef DISABLE_NLG

extern "C" VOID _cdecl
_NLG_Notify (
    IN PVOID uoffDestination,
    IN PVOID uoffFramePointer,
    IN ULONG dwCode
    );

extern "C" VOID _cdecl
__NLG_Return2 (
    VOID
    );

#else

#define _NLG_Notify(...)
#define __NLG_Return2(...)

#endif

#define EH_EXCEPTION_NUMBER ('msc' | 0xE0000000)

//
// If frame.obj is pulled in then _pDestructExceptionObject points to
// __DestructExceptionObject, otherwise, it's NULL.
//

extern "C" void
(__cdecl *_pDestructExceptionObject) (
    void *,
    int
    );

#if defined(_M_ARM64EC)

extern "C" void
__except_validate_context_record (
    _In_ PCONTEXT ContextRecord
    );

//
// The following two wrapper functions are used to call the exception filter
// and termination handlers. They preserve all non-volatile registers, and set
// up the frame pointer as appropriate when invoking the filters and termination
// handlers.
//

extern "C" LONG
__C_ExecuteExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ PVOID EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PUCHAR NonVolatileRegisters
    );

extern "C" VOID
__C_ExecuteTerminationHandler(
    _In_ BOOLEAN AbnormalTermination,
    _In_ PVOID EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _In_ PUCHAR NonVolatileRegisters
    );

#define DC_TARGETPC(DispatcherContext) ((DispatcherContext)->TargetIp)

__forceinline
LONG 
EXECUTE_EXCEPTION_FILTER(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ PVOID EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PDISPATCHER_CONTEXT DispatcherContext
    )
{
    LONG Disposition;
    PDISPATCHER_CONTEXT_ARM64EC DispatcherContextEc;

    if (RtlIsEcCode((DispatcherContext->ControlPc))) {
        DispatcherContextEc = (PDISPATCHER_CONTEXT_ARM64EC)DispatcherContext;
        Disposition =
            __C_ExecuteExceptionFilter(ExceptionPointers,
                                    EstablisherFrame,
                                    ExceptionFilter,
                                    DispatcherContextEc->NonVolatileRegisters);

    } else {
        Disposition = (ExceptionFilter)(ExceptionPointers, EstablisherFrame);
    }

    return Disposition;
}

__forceinline
VOID
EXECUTE_TERMINATION_HANDLER(
    _In_ BOOLEAN AbnormalTermination,
    _In_ PVOID EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _Inout_ PDISPATCHER_CONTEXT DispatcherContext
    )
{
    PDISPATCHER_CONTEXT_ARM64EC DispatcherContextEc;

    if (RtlIsEcCode((DispatcherContext->ControlPc))) {
        DispatcherContextEc = (PDISPATCHER_CONTEXT_ARM64EC)DispatcherContext;
        __C_ExecuteTerminationHandler(AbnormalTermination,
                                    EstablisherFrame,
                                    TerminationHandler,
                                    DispatcherContextEc->NonVolatileRegisters);

    } else {
        (TerminationHandler)(AbnormalTermination, EstablisherFrame);
    }

    return;
}

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

//
// The following two wrapper functions are used to call the exception filter
// and termination handlers. They preserve all non-volatile registers, and set
// up the frame pointer as appropriate when invoking the filters and termination
// handlers.
//

extern "C" LONG
__C_ExecuteExceptionFilter(
    _In_ PEXCEPTION_POINTERS ExceptionPointers,
    _In_ PVOID EstablisherFrame,
    _In_ PEXCEPTION_FILTER ExceptionFilter,
    _In_ PUCHAR NonVolatileRegisters
    );

extern "C" VOID
__C_ExecuteTerminationHandler(
    _In_ BOOLEAN AbnormalTermination,
    _In_ PVOID EstablisherFrame,
    _In_ PTERMINATION_HANDLER TerminationHandler,
    _In_ PUCHAR NonVolatileRegisters
    );

#define DC_TARGETPC(DispatcherContext) ((DispatcherContext)->TargetPc)

#define EXECUTE_EXCEPTION_FILTER(ExceptionPointers, \
                                 EstablisherFrame, \
                                 ExceptionFilter, \
                                 DispatcherContext) \
    __C_ExecuteExceptionFilter(ExceptionPointers, \
                               EstablisherFrame, \
                               ExceptionFilter, \
                               DispatcherContext->NonVolatileRegisters)

#define EXECUTE_TERMINATION_HANDLER(AbnormalTermination, \
                                    EstablisherFrame, \
                                    TerminationHandler, \
                                    DispatcherContext) \
    __C_ExecuteTerminationHandler(AbnormalTermination, \
                                  EstablisherFrame, \
                                  TerminationHandler, \
                                  DispatcherContext->NonVolatileRegisters)

#elif defined(_M_X64) || defined(_M_ARM64EC)

extern "C" void
__except_validate_context_record (
    _In_ PCONTEXT ContextRecord
    );

#define EXECUTE_EXCEPTION_FILTER(ExceptionPointers, \
                                 EstablisherFrame, \
                                 ExceptionFilter, \
                                 DispatcherContext) \
    (ExceptionFilter)(ExceptionPointers, EstablisherFrame)

#define EXECUTE_TERMINATION_HANDLER(AbnormalTermination, \
                                    EstablisherFrame, \
                                    TerminationHandler, \
                                    DispatcherContext) \
    (TerminationHandler)(AbnormalTermination, EstablisherFrame)

#define DC_TARGETPC(DispatcherContext) ((DispatcherContext)->TargetIp)

#elif defined(_M_IX86)

#else

#error Unknown processor architecture.

#endif
