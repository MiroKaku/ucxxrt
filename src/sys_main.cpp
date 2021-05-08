/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      sys_main.cpp
 * DATA:      2021/05/03
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_startup.h>
#include <vcruntime_internal.h>
#include <startup.h>

extern "C" void __cdecl _initialize_pool();

static int __cdecl invoke_main(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry)
{
    return DriverMain(DriverObject, Registry);
}

static PDRIVER_UNLOAD __DriverUnload = nullptr;
VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    if (__DriverUnload)
    {
        __DriverUnload(DriverObject);
    }

    // do exit() of atexit()
    onexit(nullptr);

    // do C initializations
    _initterm(__xp_a, __xp_z);

    // do C++ terminations
    _initterm(__xt_a, __xt_z);
}

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry)
{
    long main_result = STATUS_UNSUCCESSFUL;

    __try
    {
        __isa_available_init();

        // do allocator initializions
        _initialize_pool();

        if (_initterm_e(__xi_a, __xi_z) != 0)
            return 255;

        _initterm(__xc_a, __xc_z);

        //
        // Initialization is complete; invoke main...
        //

        main_result = invoke_main(DriverObject, Registry);
        if (NT_SUCCESS(main_result))
        {
            if (DriverObject->DriverUnload)
            {
                __DriverUnload = DriverObject->DriverUnload;
                DriverObject->DriverUnload = DriverUnload;
            }
        }
        else
        {
            // do exit() of atexit()
            onexit(nullptr);

            // do C initializations
            _initterm(__xp_a, __xp_z);

            // do C++ terminations
            _initterm(__xt_a, __xt_z);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        main_result = GetExceptionCode();
    }

    return main_result;
}
