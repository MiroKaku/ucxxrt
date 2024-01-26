//
// sys_main.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The DriverEntry(...) entry point, linked into client executables that
// uses DriverMain(...).
//

#include "sys_common.inl"

_CRT_BEGIN_C_HEADER

_ACRTIMP int __cdecl _seh_filter_sys(
    _In_ unsigned long       ExceptionNum,
    _In_ PEXCEPTION_POINTERS ExceptionPtr
);

_CRT_END_C_HEADER

static DRIVER_UNLOAD* __scrt_drv_unload = nullptr;

static VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
    if (__scrt_drv_unload) {
        __scrt_drv_unload(DriverObject);
    }

    _cexit();

    __scrt_uninitialize_crt(true, true);
}

extern"C" DRIVER_INITIALIZE DriverMain;

extern"C" NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    if (!__scrt_initialize_crt(__scrt_module_type_sys))
        __scrt_fastfail(FAST_FAIL_FATAL_APP_EXIT);

    __try {
        if (_initterm_e(__xi_a, __xi_z) != 0)
            return STATUS_DRIVER_INTERNAL_ERROR;

        _initterm(__xc_a, __xc_z);

        // If this module has any dynamically initialized __declspec(thread)
        // variables, then we invoke their initialization for the primary thread
        // used to start the process:
        _tls_callback_type const* const tls_init_callback = __scrt_get_dyn_tls_init_callback();
        if (*tls_init_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_init_callback)) {
            (*tls_init_callback)(nullptr, DLL_THREAD_ATTACH, nullptr);
        }

        // If this module has any thread-local destructors, register the
        // callback function with the Unified CRT to run on exit.
        _tls_callback_type const* const tls_dtor_callback = __scrt_get_dyn_tls_dtor_callback();
        if (*tls_dtor_callback != nullptr && __scrt_is_nonwritable_in_current_image(tls_dtor_callback)) {
            _register_thread_local_exe_atexit_callback(*tls_dtor_callback);
        }

        //
        // Initialization is complete; invoke main...
        //

        auto const main_result = DriverMain(DriverObject, RegistryPath);
        if (NT_SUCCESS(main_result)) {
            __scrt_drv_unload = static_cast<PDRIVER_UNLOAD>(InterlockedExchangePointer(
                reinterpret_cast<PVOID volatile*>(&DriverObject->DriverUnload), &DriverUnload));
        }
        else {
            _cexit();

            // We terminate the CRT:
            __scrt_uninitialize_crt(true, false);
        }

        return main_result;
    }
    __except (_seh_filter_sys(GetExceptionCode(), GetExceptionInformation())) {
        // Note:  We should never reach this except clause.
        int const main_result = GetExceptionCode();

        _exit(main_result);
    }
}
