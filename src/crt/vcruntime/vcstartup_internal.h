//
// vcstartup_internal.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Declarations and utilities used by the statically-linked CRT startup code.
//
#pragma once

#include <corecrt_startup.h>
#include <internal_shared.h>
#include <intrin.h>
#include <vcruntime_startup.h>



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Exception Filters for main()
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern "C" int __cdecl _seh_filter_sys(
    _In_ unsigned long               _ExceptionNum,
    _In_ struct _EXCEPTION_POINTERS* _ExceptionPtr
);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Utility Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// The _VCRT_DECLARE_ALTERNATE_NAME macro provides an architecture-neutral way
// of specifying /alternatename comments to the linker.  It prepends the leading
// decoration character for x86 and hybrid and leaves names unmodified for other
// architectures.

#if defined _M_IX86
    #if defined _M_HYBRID
        #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "#"
    #else
        #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "_"
    #endif
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA "_"
#elif defined _M_ARM64EC
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX "#"
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA ""
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX ""
    #define _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA ""
#else
    #error Unsupported architecture
#endif

#if defined _M_ARM64EC

#define _VCRT_DECLARE_ALTERNATE_NAME(name, alternate_name)  \
    __pragma(comment(linker,                                \
        "/alternatename:"                                   \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #name           \
        "="                                                 \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #alternate_name \
        ))

#else

#define _VCRT_DECLARE_ALTERNATE_NAME(name, alternate_name)  \
    __pragma(comment(linker,                                \
        "/alternatename:"                                   \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #name           \
        "="                                                 \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX #alternate_name \
        ))

#endif

#define _VCRT_DECLARE_ALTERNATE_NAME_DATA(name, alternate_name)     \
    __pragma(comment(linker,                                        \
        "/alternatename:"                                           \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA #name              \
        "="                                                         \
        _VCRT_DECLARE_ALTERNATE_NAME_PREFIX_DATA #alternate_name    \
        ))


// The _VCRT_DEFINE_IAT_SYMBOL macro provides an architecture-neutral way of
// defining IAT symbols (__imp_- or _imp__-prefixed symbols).
#ifdef _M_IX86
    #define _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(_imp__, f)
#else
    #define _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) _CRT_CONCATENATE(__imp_, f)
#endif

#define _VCRT_DEFINE_IAT_SYMBOL(f)                                                          \
    extern "C" __declspec(selectany) void const* const _VCRT_DEFINE_IAT_SYMBOL_MAKE_NAME(f) \
        = reinterpret_cast<void const*>(f)



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Client Entry Point Declarations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern "C" DRIVER_INITIALIZE DriverMain;

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// General Startup Utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" int  __cdecl _get_startup_new_mode();
extern "C" bool __cdecl __scrt_is_nonwritable_in_current_image(void const* target);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Static / Dynamic CRT Library Specific Functionality
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// These functions have different implementations for the static CRT and the CRT
// DLLs.  Startup and termination are subtly different between the two kinds of
// CRTs.

// These are called during startup and termination to initialize or uninitialize
// the CRT.  When the CRT is statically linked, these actually perform complete
// CRT initialization, calling into each part of the CRT initialize them.  When
// the CRT DLLs are used, the CRT DLLs initialize themselves when they are loaded.
// However, some initialization is still required, to bind the module-specific
// VCRuntime DLL to the global AppCRT DLL.
extern "C" bool __cdecl __scrt_initialize_crt();
extern "C" bool __cdecl __scrt_uninitialize_crt(bool is_terminating, bool from_exit);


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Desktop / Windows Store App Specific Functionality
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// In Windows Store apps, and on computers where __fastfail is supported, this
// function invokes __fastfail.  Otherwise, this function resets the unhandled
// exception filter and calls the default unhandled exception filter with a
// noncontinuable exception, which is as close as we can get to __fastfail.
extern "C" __declspec(noreturn) void __cdecl __scrt_fastfail(unsigned code);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Heap Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_alloc();
__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_array_new_length();
