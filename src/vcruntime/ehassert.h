/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ehassert.h
 * DATA:      2020/02/08
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once
#include "shared/image.h"


namespace ucxxrt
{
#ifdef _M_AMD64
#pragma warning(push)
#pragma warning(disable: 4201)

    //
    // Define exception dispatch context structure.
    //

    typedef struct _DISPATCHER_CONTEXT {
        DWORD64 ControlPc;
        DWORD64 ImageBase;
        PRUNTIME_FUNCTION FunctionEntry;
        DWORD64 EstablisherFrame;
        DWORD64 TargetIp;
        PCONTEXT ContextRecord;
        PEXCEPTION_ROUTINE LanguageHandler;
        PVOID HandlerData;
        PUNWIND_HISTORY_TABLE HistoryTable;
        DWORD ScopeIndex;
        DWORD Fill0;
    } DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

    //
    // Nonvolatile context pointer record.
    //

    typedef struct _KNONVOLATILE_CONTEXT_POINTERS {
        union {
            PM128A FloatingContext[16];
            struct {
                PM128A Xmm0;
                PM128A Xmm1;
                PM128A Xmm2;
                PM128A Xmm3;
                PM128A Xmm4;
                PM128A Xmm5;
                PM128A Xmm6;
                PM128A Xmm7;
                PM128A Xmm8;
                PM128A Xmm9;
                PM128A Xmm10;
                PM128A Xmm11;
                PM128A Xmm12;
                PM128A Xmm13;
                PM128A Xmm14;
                PM128A Xmm15;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME;

        union {
            PDWORD64 IntegerContext[16];
            struct {
                PDWORD64 Rax;
                PDWORD64 Rcx;
                PDWORD64 Rdx;
                PDWORD64 Rbx;
                PDWORD64 Rsp;
                PDWORD64 Rbp;
                PDWORD64 Rsi;
                PDWORD64 Rdi;
                PDWORD64 R8;
                PDWORD64 R9;
                PDWORD64 R10;
                PDWORD64 R11;
                PDWORD64 R12;
                PDWORD64 R13;
                PDWORD64 R14;
                PDWORD64 R15;
            } DUMMYSTRUCTNAME;
        } DUMMYUNIONNAME2;

    } KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;

#pragma warning(pop)
#endif // _AMD64_

#ifdef _M_ARM_

    //
    // Define exception dispatch context structure.
    //

    typedef struct _DISPATCHER_CONTEXT {
        DWORD ControlPc;
        DWORD ImageBase;
        PRUNTIME_FUNCTION FunctionEntry;
        DWORD EstablisherFrame;
        DWORD TargetPc;
        PCONTEXT ContextRecord;
        PEXCEPTION_ROUTINE LanguageHandler;
        PVOID HandlerData;
        PUNWIND_HISTORY_TABLE HistoryTable;
        DWORD ScopeIndex;
        BOOLEAN ControlPcIsUnwound;
        PBYTE  NonVolatileRegisters;
        DWORD Reserved;
    } DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

    //
    // Nonvolatile context pointer record.
    //

    typedef struct _KNONVOLATILE_CONTEXT_POINTERS {

        PDWORD R4;
        PDWORD R5;
        PDWORD R6;
        PDWORD R7;
        PDWORD R8;
        PDWORD R9;
        PDWORD R10;
        PDWORD R11;
        PDWORD Lr;

        PULONGLONG D8;
        PULONGLONG D9;
        PULONGLONG D10;
        PULONGLONG D11;
        PULONGLONG D12;
        PULONGLONG D13;
        PULONGLONG D14;
        PULONGLONG D15;

    } KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;
#endif // _ARM_

#ifdef _M_ARM64_

    //
    // Define exception dispatch context structure.
    //

    typedef struct _DISPATCHER_CONTEXT_ARM64 {
        ULONG_PTR ControlPc;
        ULONG_PTR ImageBase;
        PARM64_RUNTIME_FUNCTION FunctionEntry;
        ULONG_PTR EstablisherFrame;
        ULONG_PTR TargetPc;
        PARM64_NT_CONTEXT ContextRecord;
        PEXCEPTION_ROUTINE LanguageHandler;
        PVOID HandlerData;
        PUNWIND_HISTORY_TABLE HistoryTable;
        DWORD ScopeIndex;
        BOOLEAN ControlPcIsUnwound;
        PBYTE  NonVolatileRegisters;
    } DISPATCHER_CONTEXT_ARM64, * PDISPATCHER_CONTEXT_ARM64;

    typedef DISPATCHER_CONTEXT_ARM64 DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

    //
    // Nonvolatile context pointer record.
    //

    typedef struct _KNONVOLATILE_CONTEXT_POINTERS_ARM64 {

        PDWORD64 X19;
        PDWORD64 X20;
        PDWORD64 X21;
        PDWORD64 X22;
        PDWORD64 X23;
        PDWORD64 X24;
        PDWORD64 X25;
        PDWORD64 X26;
        PDWORD64 X27;
        PDWORD64 X28;
        PDWORD64 Fp;
        PDWORD64 Lr;

        PDWORD64 D8;
        PDWORD64 D9;
        PDWORD64 D10;
        PDWORD64 D11;
        PDWORD64 D12;
        PDWORD64 D13;
        PDWORD64 D14;
        PDWORD64 D15;

    } KNONVOLATILE_CONTEXT_POINTERS_ARM64, * PKNONVOLATILE_CONTEXT_POINTERS_ARM64;

    typedef KNONVOLATILE_CONTEXT_POINTERS_ARM64 KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;
#endif // _ARM64_


#if !__has_include(<winnt.h>)
    EXTERN_C NTSYSAPI VOID NTAPI
        RtlRaiseException(
            _In_ PEXCEPTION_RECORD ExceptionRecord
        );

    EXTERN_C NTSYSAPI PVOID NTAPI
        RtlPcToFileHeader(
            _In_ PVOID PcValue,
            _Out_ PVOID* BaseOfImage
        );

    EXTERN_C NTSYSAPI VOID NTAPI
        RtlCaptureContext(
            _Out_ PCONTEXT ContextRecord
        );

#if defined(_M_AMD64) || defined(_M_ARM64) || defined(_M_ARM)
    EXTERN_C NTSYSAPI PRUNTIME_FUNCTION NTAPI
        RtlLookupFunctionEntry(
            _In_ ULONG_PTR ControlPc,
            _Out_ PULONG_PTR ImageBase,
            _Inout_opt_ PUNWIND_HISTORY_TABLE HistoryTable
        );

    EXTERN_C NTSYSAPI VOID NTAPI
        RtlUnwindEx(
            _In_opt_ PVOID TargetFrame,
            _In_opt_ PVOID TargetIp,
            _In_opt_ PEXCEPTION_RECORD ExceptionRecord,
            _In_ PVOID ReturnValue,
            _In_ PCONTEXT ContextRecord,
            _In_opt_ PUNWIND_HISTORY_TABLE HistoryTable
        );

    EXTERN_C NTSYSAPI
        PEXCEPTION_ROUTINE
        NTAPI
        RtlVirtualUnwind(
            _In_ DWORD HandlerType,
            _In_ ULONG_PTR ImageBase,
            _In_ ULONG_PTR ControlPc,
            _In_ PRUNTIME_FUNCTION FunctionEntry,
            _Inout_ PCONTEXT ContextRecord,
            _Out_ PVOID* HandlerData,
            _Out_ PULONG_PTR EstablisherFrame,
            _Inout_opt_ PKNONVOLATILE_CONTEXT_POINTERS ContextPointers
        );
#endif

#endif
}