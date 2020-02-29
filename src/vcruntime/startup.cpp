/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      startup.cpp
 * DATA:      2020/02/26
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 //
 // Copyright (c) Microsoft Corporation. All rights reserved.
 //

#include "vcruntime/startup.h"


#if __has_include(<wdm.h>)


 // Need to put the following marker variables into the .CRT section.
 // The .CRT section contains arrays of function pointers.
 // The compiler creates functions and adds pointers to this section
 // for things like C++ global constructors.
 //
 // The XIA, XCA etc are group names with in the section.
 // The compiler sorts the contributions by the group name.
 // For example, .CRT$XCA followed by .CRT$XCB, ... .CRT$XCZ.
 // The marker variables below let us get pointers
 // to the beginning/end of the arrays of function pointers.
 //
 // For example, standard groups are
 //  XCA used here, for begin marker
 //  XCC "compiler" inits
 //  XCL "library" inits
 //  XCU "user" inits
 //  XCZ used here, for end marker
 //

#pragma section(".CRT$XIA", long, read)      // C Initializer
#pragma section(".CRT$XIZ", long, read)

#pragma section(".CRT$XCA", long, read)      // C++ Initializer
#pragma section(".CRT$XCZ", long, read)

#pragma section(".CRT$XPA", long, read)      // C pre-terminators
#pragma section(".CRT$XPZ", long, read)

#pragma section(".CRT$XTA", long, read)      // C terminators
#pragma section(".CRT$XTZ", long, read)

#define _CRTAlloc$(x) __declspec(allocate(x))
typedef void(__cdecl* _PVFV)(void);
typedef int (__cdecl* _PIFV)(void);
extern "C" _CRTAlloc$(".CRT$XIA") _PIFV __xi_a[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XIZ") _PIFV __xi_z[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XCA") _PVFV __xc_a[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XCZ") _PVFV __xc_z[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XPA") _PVFV __xp_a[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XPZ") _PVFV __xp_z[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XTA") _PVFV __xt_a[] = { nullptr };
extern "C" _CRTAlloc$(".CRT$XTZ") _PVFV __xt_z[] = { nullptr };
#undef _CRTAlloc$

#pragma comment(linker, "/merge:.CRT=.rdata")


extern ULONG     DefaultPoolTag;
extern POOL_TYPE DefaultPoolType;
extern ULONG     DefaultMdlProtection;

namespace ucxxrt
{
    extern "C" void __cdecl _initalloc()
    {
        RTL_OSVERSIONINFOW ver_info{};

        auto status = RtlGetVersion(&ver_info);
        if (!NT_SUCCESS(status))
        {
            return;
        }

        if ((ver_info.dwMajorVersion < 6) ||
            (ver_info.dwMajorVersion == 6 && ver_info.dwMinorVersion < 2))
        {
            DefaultPoolType = POOL_TYPE::NonPagedPool;
            DefaultMdlProtection = 0;
        }
    }

    // Calls each function in [first, last).  [first, last) must be a valid range of
    // function pointers.  Each function is called, in order.
    extern "C" void __cdecl _initterm(_PVFV* const first, _PVFV* const last)
    {
        for (_PVFV* it = first; it != last; ++it)
        {
            if (*it == nullptr)
                continue;

            (**it)();
        }
    }

    // Calls each function in [first, last).  [first, last) must be a valid range of
    // function pointers.  Each function must return zero on success, nonzero on
    // failure.  If any function returns nonzero, iteration stops immediately and
    // the nonzero value is returned.  Otherwise all functions are called and zero
    // is returned.
    //
    // If a nonzero value is returned, it is expected to be one of the runtime error
    // values (_RT_{NAME}, defined in the internal header files).
    extern "C" int __cdecl _initterm_e(_PIFV* const first, _PIFV* const last)
    {
        for (_PIFV* it = first; it != last; ++it)
        {
            if (*it == nullptr)
                continue;

            int const result = (**it)();
            if (result != 0)
                return result;
        }

        return 0;
    }

    struct onexit_entry
    {
        onexit_entry*   _next = nullptr;
        _PVFV           _destructor = nullptr;

        onexit_entry(onexit_entry* next, _PVFV destructor)
            : _next{ next }
            , _destructor{ destructor }
        { }

        ~onexit_entry()
        {
            _destructor();
        }
    };
    static onexit_entry* s_onexit_table = nullptr;

    static int __cdecl register_onexit(onexit_entry* table, _PVFV const function)
    {
        const auto entry = new onexit_entry(table, function);
        if (nullptr == entry)
        {
            return -1;
        }
        s_onexit_table = entry;

        return 0;
    }

    static int __cdecl execute_onexit(onexit_entry* table)
    {
        for (auto entry = table; entry;)
        {
            const auto next = entry->_next;
            delete entry;
            entry = next;
        }

        return 0;
    }

    extern "C" int __cdecl atexit(_PVFV const function)
    {
        return register_onexit(s_onexit_table, reinterpret_cast<_PVFV const>(function));
    }

    // This function executes a table of atexit() functions.  The Terminators 
    // are executed in reverse order, to give the required LIFO execution order.  
    // If the table is uninitialized, this function has no effect.  
    // After executing the terminators, this function resets the table
    // so that it is uninitialized.  Returns 0 on success; -1 on failure.
    extern "C" onexit_t __cdecl onexit(_In_opt_ onexit_t _Func)
    {
        return execute_onexit(s_onexit_table), _Func;
    }

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
            // do allocator initializions
            _initalloc();

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
}

#endif
