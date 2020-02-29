/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      vcruntime.h
 * DATA:      2020/02/08
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once
#include <eh.h>


namespace ucxxrt
{

    _CRT_BEGIN_C_HEADER;


    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //
    // Optimization macros
    //
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // Note:  These macros must match their definitions in the Windows build.
#ifndef BEGIN_PRAGMA_OPTIMIZE_DISABLE
#define BEGIN_PRAGMA_OPTIMIZE_DISABLE(flags, bug, reason) \
        __pragma(optimize(flags, off))

#define BEGIN_PRAGMA_OPTIMIZE_ENABLE(flags, bug, reason) \
        __pragma(optimize(flags, on))

#define END_PRAGMA_OPTIMIZE() \
        __pragma(optimize("", on))
#endif

// When building the satellite, define __ucxxrt_ptd to be the satellite-specific implementation, rename the base implementation to __vcrt_ptd_base
#if defined _VCRT_SAT_1
#define RENAME_BASE_PTD(x) x##base
#else
#define RENAME_BASE_PTD(x) x
#endif

    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //
    // Per-thread data
    //
    //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // WARNING WARNING WARNING
    // This entire structure is inferred by the satellite DLL from the address of _curexception and
    // __ProcessingThrow. Any changes to this structure must continue to allow this to happen
    // WARNING WARNING WARNING
    typedef struct RENAME_BASE_PTD(__ucxxrt_ptd)
    {
        volatile bool   _init;

        // C++ Exception Handling (EH) state
        unsigned long      _NLG_dwCode;      // Required by NLG routines
        unexpected_handler _unexpected;      // unexpected() routine
        void* _translator;      // S.E. translator
        void* _purecall;        // called when pure virtual happens
        void* _curexception;    // current exception
        void* _curcontext;      // current exception context
        int   _ProcessingThrow; // for uncaught_exception
        void* _curexcspec;      // for handling exceptions thrown from std::unexpected
        int   _cxxReThrow;      // true if it's a rethrown C++ exception

#if defined _M_X64 || defined _M_ARM || defined _M_ARM64 || defined _M_HYBRID
        void* _pExitContext;
        void* _pUnwindContext;
        void* _pFrameInfoChain;
        uintptr_t _ImageBase;
        uintptr_t _ThrowImageBase;
        void* _pForeignException;
        int _CatchStateInParent;        // Used to link together the catch funclet with the parent. During dispatch contains state associated
                                        // with catch in the parent. During unwind represents the current unwind state that is resumed to
                                        // during collided unwind and used to look for handlers of the throwing dtor.
#elif defined _M_IX86
        void* _pFrameInfoChain;
#endif

    } RENAME_BASE_PTD(__ucxxrt_ptd);

// Represents uninitialized value of _CatchStateInParent
#define INVALID_CATCH_SPECIFIC_STATE -2

// Utilize windows scope index to resolve collided unwind, since the uninitialized value is 0 we subtract the value by 2
// so that a scope index value of 1 maps to a EH state of -1 etc.
#define SCOPE_INDEX_SHIFT_VALUE 2

#if defined _VCRT_SAT_1
    typedef struct __ucxxrt_ptd
    {
        int _CatchStateInParent;    // Used to link together the catch funclet with the parent. During dispatch contains state associated
                                    // with catch in the parent. During unwind represents the current unwind state that is resumed to
                                    // during collided unwind and used to look for handlers of the throwing dtor.
    } __ucxxrt_ptd;

    RENAME_BASE_PTD(__ucxxrt_ptd)* __cdecl RENAME_BASE_PTD(__ucxxrt_getptd)(void);
#endif

    // These functions are defined differently for satellite DLL compilation to get the PTD
    // from the base vcruntime DLL.
    extern"C" __ucxxrt_ptd* __cdecl __ucxxrt_getptd();


    // Imports from AppCRT and DesktopCRT that need to be forwarded to the OS-mode global state counterparts when this dll is built as vcruntime_win
#if defined _VCRT_WINDOWS_BUILD && defined _CRT_GLOBAL_STATE_ISOLATION
    _ACRTIMP int __cdecl _o__callnewh(
        _In_ size_t _Size
    );

#define _callnewh _o__callnewh
#endif

    _CRT_END_C_HEADER

}
