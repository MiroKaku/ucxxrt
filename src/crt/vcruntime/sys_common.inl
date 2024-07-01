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



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Common DriverMain() implementation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

static int __cdecl invoke_main(PDRIVER_OBJECT drvobj, PUNICODE_STRING regpath)
{
    return DriverMain(drvobj, regpath);
}

static int __cdecl pre_c_initialization()
{
#ifdef _M_IX86
    // Clear the x87 exception flags.  Any other floating point initialization
    // should already have taken place before this function is called.
    _asm { fnclex }
#endif

    return 0;
}

static void __cdecl pre_cpp_initialization()
{
    _set_new_mode(_get_startup_new_mode());
}



_CRTALLOC(".CRT$XIAA") static _PIFV pre_c_initializer    = pre_c_initialization;
_CRTALLOC(".CRT$XCAA") static _PVFV pre_cpp_initializer  = pre_cpp_initialization;


static PDRIVER_UNLOAD __drv_unload = nullptr;

static __declspec(noinline) void __scrt_common_exit(PDRIVER_OBJECT drvobj)
{
    if (__drv_unload && __drv_unload != &__scrt_common_exit)
    {
        __drv_unload(drvobj);
    }

    _cexit();

    __scrt_uninitialize_crt(true, true);
}

static __declspec(noinline) long __cdecl __scrt_common_main_seh(PDRIVER_OBJECT drvobj, PUNICODE_STRING regpath)
{
    if (!__scrt_initialize_crt())
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

    __try
    {
        if (_initterm_e(__xi_a, __xi_z) != 0)
            return STATUS_FAILED_DRIVER_ENTRY;

        _initterm(__xc_a, __xc_z);

        //
        // Initialization is complete; invoke main...
        //

        long const main_result = invoke_main(drvobj, regpath);
        if (NT_SUCCESS(main_result))
        {
            if (drvobj && drvobj->DriverUnload)
            {
                __drv_unload = drvobj->DriverUnload;
                drvobj->DriverUnload = &__scrt_common_exit;
            }
        }
        else
        {
            _cexit();

            // We terminate the CRT:
            __scrt_uninitialize_crt(true, false);
        }

        return main_result;
    }
    __except (_seh_filter_dll(GetExceptionCode(), GetExceptionInformation()))
    {
        // Note:  We should never reach this except clause.
        int const main_result = GetExceptionCode();

        _c_exit();

        return main_result;
    }
}



// This is the common main implementation to which all of the CRT main functions
// delegate (for executables; DLLs are handled separately).
static __forceinline int __cdecl __scrt_common_main(PDRIVER_OBJECT drvobj, PUNICODE_STRING regpath)
{
    // The /GS security cookie must be initialized before any exception handling
    // targeting the current image is registered.  No function using exception
    // handling can be called in the current image until after this call:
    //__security_init_cookie();

    return __scrt_common_main_seh(drvobj, regpath);
}
