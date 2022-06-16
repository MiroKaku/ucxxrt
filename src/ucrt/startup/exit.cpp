//
// exit.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The exit() implementation
//
#include <corecrt_internal.h>
#include <eh.h>

static long c_termination_complete = FALSE;

extern "C" extern _onexit_table_t __acrt_atexit_table;
extern "C" extern _onexit_table_t __acrt_at_quick_exit_table;


// CRT_REFACTOR TODO This needs to be declared somewhere more accessible and we
// need to clean up the static CRT exit coordination.
#if !defined CRTDLL && defined _DEBUG
    extern "C" bool __cdecl __scrt_uninitialize_crt(bool is_terminating, bool from_exit);
#endif


static int __cdecl atexit_exception_filter(unsigned long const _exception_code) throw()
{
    if (_exception_code == ('msc' | 0xE0000000))
    {
        return EXCEPTION_EXECUTE_HANDLER;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

extern PDRIVER_OBJECT __drvobj;
static void __cdecl common_exit(
    int                    const /*return_code*/,
    _crt_exit_cleanup_mode const cleanup_mode,
    _crt_exit_return_mode  const return_mode
    ) throw()
{
    // Run the C termination:
    __acrt_lock_and_call(__acrt_select_exit_lock(), [&]
    {
        if (c_termination_complete)
        {
            return;
        }

        InterlockedExchange(&c_termination_complete, TRUE);

        __try
        {
            if (cleanup_mode == _crt_exit_full_cleanup)
            {
                _execute_onexit_table(&__acrt_atexit_table);
            }
            else if (cleanup_mode == _crt_exit_quick_cleanup)
            {
                _execute_onexit_table(&__acrt_at_quick_exit_table);
            }
        }
        __except (atexit_exception_filter(GetExceptionCode()))
        {
            terminate();
        }

        if (cleanup_mode == _crt_exit_full_cleanup)
        {
            _initterm(__xp_a, __xp_z);
        }

        _initterm(__xt_a, __xt_z);
    });

    if (return_mode == _crt_exit_terminate_process)
    {
        __fastfail(FAST_FAIL_FATAL_APP_EXIT);
    }
}

extern "C" int __cdecl _is_c_termination_complete()
{
    return static_cast<int>(__crt_interlocked_read(&c_termination_complete));
}

extern "C" void __cdecl exit(int const return_code)
{
    common_exit(return_code, _crt_exit_full_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl _exit(int const return_code)
{
    common_exit(return_code, _crt_exit_no_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl _Exit(int const return_code)
{
    common_exit(return_code, _crt_exit_no_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl quick_exit(int const return_code)
{
    common_exit(return_code, _crt_exit_quick_cleanup, _crt_exit_terminate_process);
}

extern "C" void __cdecl _cexit()
{
    common_exit(0, _crt_exit_full_cleanup, _crt_exit_return_to_caller);
}

extern "C" void __cdecl _c_exit()
{
    common_exit(0, _crt_exit_no_cleanup, _crt_exit_return_to_caller);
}
