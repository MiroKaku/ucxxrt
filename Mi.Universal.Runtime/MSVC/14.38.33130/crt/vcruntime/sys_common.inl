//
// exe_common.inl
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The implementation of the common executable entry point code.  There are four
// executable entry points defined by the CRT, one for each of the user-definable
// entry points:
//
//  * mainCRTStartup     => main
//  * wmainCRTStartup    => wmain
//  * WinMainCRTStartup  => WinMain
//  * wWinMainCRTStartup => wWinMain
//
// These functions all behave the same, except for which user-definable main
// function they call and whether they accumulate and pass narrow or wide string
// arguments.  This file contains the common code shared by all four of those
// entry points.
//
// The actual entry points are defined in four .cpp files alongside this .inl
// file.  At most one of these .cpp files will be linked into the resulting
// executable, so we can treat this .inl file as if its contents are only linked
// into the executable once as well.
//
#include <vcstartup_internal.h>
#include <vcruntime_internal.h>
#include <locale.h>
#include <math.h>
#include <new.h>
#include <process.h>
#include <rtcapi.h>
#include <stdio.h>
#include <stdlib.h>

#define __scrt_module_type_sys ((__scrt_module_type)3)

static int __cdecl pre_c_initialization()
{
    if (!__scrt_initialize_onexit_tables(__scrt_module_type_sys))
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

    #ifdef _M_IX86
    // Clear the x87 exception flags.  Any other floating point initialization
    // should already have taken place before this function is called.
    _asm { fnclex }
    #endif

    __scrt_initialize_type_info();

    _initialize_invalid_parameter_handler();
    _initialize_denormal_control();

    #ifdef _M_IX86
    _initialize_default_precision();
    #endif

    return 0;
}

static void __cdecl pre_cpp_initialization()
{
    // Before we begin C++ initialization, set the unhandled exception
    // filter so that unhandled C++ exceptions result in std::terminate
    // being called:
    __scrt_set_unhandled_exception_filter();

    _set_new_mode(_get_startup_new_mode());
}

// When both the PGO instrumentation library and the CRT are statically linked,
// PGO will initialize itself in XIAB.  We do most pre-C initialization before
// PGO is initialized, but defer some initialization steps to after.  See the
// commentary in post_pgo_initialization for details.
_CRTALLOC(".CRT$XIAA") static _PIFV pre_c_initializer    = pre_c_initialization;
_CRTALLOC(".CRT$XCAA") static _PVFV pre_cpp_initializer  = pre_cpp_initialization;

