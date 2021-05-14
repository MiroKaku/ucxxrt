/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ehhelpers.h
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

#include <ehdata4.h>

EXTERN_C_START

#if defined(_M_X64) || defined(_M_ARM_NT) || defined(_M_ARM64) || defined(_CHPE_X86_ARM64_EH_)

#define _pForeignExcept   (*((EHExceptionRecord **) &(RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))()->_pForeignException)))

#endif

#define pFrameInfoChain   (*((FRAMEINFO **)         &(RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))()->_pFrameInfoChain)))
#define __pSETranslator   (_se_translator_function) ((RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd))())->_translator)

// Pre-V4 managed exception code
#define MANAGED_EXCEPTION_CODE  0XE0434F4D

// V4 and later managed exception code
#define MANAGED_EXCEPTION_CODE_V4  0XE0434352

void __cdecl __except_validate_context_record(
    _In_ PCONTEXT ContextRecord
    );

_VCRTIMP void * __AdjustPointer(
    void *,
    const PMD&
);

_VCRTIMP void * __GetPlatformExceptionInfo(
    int *
);

_VCRTIMP int __cdecl __FrameUnwindFilter(
    EXCEPTION_POINTERS *
);

_VCRTIMP void __cdecl __DestructExceptionObject(
    EHExceptionRecord *,
    BOOLEAN
);

_VCRTIMP void __cdecl RENAME_EH_EXTERN(__BuildCatchObject)(
    EHExceptionRecord *,
    void *,
    HandlerType *,
    CatchableType *
);

_VCRTIMP int __cdecl RENAME_EH_EXTERN(__TypeMatch4)(
    FH4::HandlerType4 *,
    CatchableType *,
    ThrowInfo *
    );

_VCRTIMP int __cdecl RENAME_EH_EXTERN(__TypeMatch)(
    HandlerType *,
    CatchableType *,
    ThrowInfo *
);

EXTERN_C_END

//
// Prototype for the internal handler
//

template<class T>
EXCEPTION_DISPOSITION __InternalCxxFrameHandler(
    EHExceptionRecord    *pExcept,
    EHRegistrationNode   *pRN,
    CONTEXT              *pContext,
    DispatcherContext    *pDC,
    typename T::FuncInfo *pFuncInfo,
    int                  CatchDepth,
    EHRegistrationNode   *pMarkerRN,
    BOOLEAN              recursive);

#if _EH_RELATIVE_TYPEINFO

#undef THROW_COUNT
#define THROW_COUNT(ti)         THROW_COUNT_IB(ti, _GetThrowImageBase())
#undef THROW_CTLIST
#define THROW_CTLIST(ti)        THROW_CTLIST_IB(ti, _GetThrowImageBase())

#endif // _EH_RELATIVE_TYPEINFO

