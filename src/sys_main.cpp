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

_CRT_BEGIN_C_HEADER

int  __cdecl _do_onexit();
int  __cdecl _do_quick_onexit();

void __cdecl __sysruntime_init();
void __cdecl __initialize_memory();
void __cdecl __acrt_initialize_new_handler(_In_opt_ void* encoded_null);

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Client Entry Point Declarations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

DRIVER_INITIALIZE DriverMain;

static int __cdecl invoke_main(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry)
{
    return DriverMain(DriverObject, Registry);
}

static PDRIVER_UNLOAD __DriverUnload = nullptr;
static VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
    if (__DriverUnload && __DriverUnload != &DriverUnload)
    {
        __DriverUnload(DriverObject);
    }

    // do exit() of atexit()
    _do_onexit();

    // do pre terminations
    _initterm(__xp_a, __xp_z);

    // do terminations
    _initterm(__xt_a, __xt_z);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING Registry)
{
    long main_result = STATUS_UNSUCCESSFUL;

    // Nx
    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

    // do feature initializions
    __isa_available_init();

    // do sysruntime initializions
    __sysruntime_init();

    // do memory initializions
    __initialize_memory();

    // do pointer initializions
    void* const encoded_null = __crt_fast_encode_pointer(nullptr);
    __acrt_initialize_new_handler(encoded_null);

    __try
    {
        // do C initializions
        if (_initterm_e(__xi_a, __xi_z) != 0)
            return 255;

        // do C++ initializions
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
            _do_onexit();

            // do pre terminations
            _initterm(__xp_a, __xp_z);

            // do terminations
            _initterm(__xt_a, __xt_z);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        main_result = GetExceptionCode();
    }

    return main_result;
}

_CRT_END_C_HEADER
