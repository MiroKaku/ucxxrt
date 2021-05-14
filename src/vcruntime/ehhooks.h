/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ehhooks.h
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 //
 // Copyright (c) Microsoft Corporation. All Rights reserved.
 //

#pragma once
#include <vcruntime_internal.h>
#include <trnsctrl.h>

#define EHTRACE_ENTER_FMT1(...)
#define EHTRACE_ENTER_FMT2(...)
#define EHTRACE_FMT1(...)
#define EHTRACE_FMT2(...)

#define EHTRACE_ENTER
#define EHTRACE_EXIT
#define EHTRACE_EXCEPT(x) x
#define EHTRACE_HANDLER_EXIT(x)
#define EHTRACE_SAVE_LEVEL
#define EHTRACE_RESTORE_LEVEL(x)

#define EHTRACE_RESET

#define DASSERT(x)
#define _VCRT_VERIFY(x)

#define _ValidateRead(ptr)    (ptr != NULL)
#define _ValidateWrite(ptr)   (ptr != NULL)
#define _ValidateExecute(ptr) (ptr != NULL)


#define RENAME_EH_EXTERN(x) x


enum : size_t
{
#if (defined(_M_X64) || defined(_M_ARM) || defined(_M_ARM64)) && !defined(_CHPE_X86_ARM64_EH_)
    STACK_MISALIGNMENT = ((1 << 3) - 1)
#else
    STACK_MISALIGNMENT = 3
#endif
};


#ifdef _KERNEL_MODE
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

EXTERN_C NTSYSAPI VOID NTAPI 
RtlUnwind(
    PVOID             TargetFrame,
    PVOID             TargetIp,
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID             ReturnValue
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

#if _EH_RELATIVE_FUNCINFO

EXTERN_C uintptr_t __cdecl _GetImageBase();

EXTERN_C void __cdecl _SetImageBase(uintptr_t ImageBaseToRestore);
#endif

#if _EH_RELATIVE_TYPEINFO
EXTERN_C uintptr_t __cdecl _GetThrowImageBase();

EXTERN_C void __cdecl _SetThrowImageBase(uintptr_t NewThrowImageBase);
#endif


EXTERN_C _VCRTIMP FRAMEINFO * __cdecl _CreateFrameInfo(
	FRAMEINFO * pFrameInfo,
	PVOID       pExceptionObject
);

EXTERN_C _VCRTIMP void __cdecl _FindAndUnlinkFrame(
	FRAMEINFO * pFrameInfo
);


EXTERN_C _VCRTIMP BOOL __cdecl _IsExceptionObjectToBeDestroyed(
	PVOID pExceptionObject
);

#ifdef _EH_RELATIVE_FUNCINFO
template <class T>
BOOL _CallSETranslator(
    EHExceptionRecord    *pExcept,    // The exception to be translated
    EHRegistrationNode   *pRN,        // Dynamic info of function with catch
    CONTEXT              *pContext,   // Context info
    DispatcherContext    *pDC,        // More dynamic info of function with catch (ignored)
    typename T::FuncInfo *pFuncInfo,  // Static info of function with catch
    ULONG                CatchDepth,  // How deeply nested in catch blocks are we?
    EHRegistrationNode   *pMarkerRN   // Marker for parent context
    );
#endif

#if defined(_M_X64)
EXTERN_C void* __cdecl _CallSettingFrame(
	void*               handler,
	EHRegistrationNode* pEstablisher,
	ULONG               NLG_CODE
    );

EXTERN_C void* __cdecl _CallSettingFrameEncoded(
	void*               handler,
	EHRegistrationNode  pEstablisher,
	void*               object,
	ULONG               NLG_CODE
    );

EXTERN_C void* __cdecl _CallSettingFrame_LookupContinuationIndex(
    void*               handler,
    EHRegistrationNode  *pEstablisher,
    ULONG               NLG_CODE
	);

EXTERN_C void* __cdecl _CallSettingFrame_NotifyContinuationAddr(
    void*               handler,
    EHRegistrationNode  *pEstablisher
	);

#elif defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)
EXTERN_C void* __cdecl _CallSettingFrame(
    void*               handler,
    EHRegistrationNode* pRN,
    PULONG              pNonVolatiles,
    ULONG               NLG_CODE
	);
#endif

#ifndef EXCEPTION_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION STATUS_NONCONTINUABLE_EXCEPTION
#endif

EXTERN_C [[noreturn]] void __CxxRaiseException(
    _In_ DWORD dwExceptionCode,
    _In_ DWORD dwExceptionFlags,
    _In_ DWORD nNumberOfArguments,
    _In_reads_opt_(nNumberOfArguments) CONST ULONG_PTR* lpArguments
);

EXTERN_C [[noreturn]] void __CxxDispatchException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord
);

EXTERN_C EXCEPTION_DISPOSITION __CxxExecuteHandlerForException(
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID EstablisherFrame,
    PCONTEXT Context,
    PVOID DispatcherContext,
    PEXCEPTION_ROUTINE ExceptionHandler
);
