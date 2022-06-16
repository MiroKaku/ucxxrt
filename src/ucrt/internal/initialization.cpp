//
// initialization.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This file defines the main initialization and uninitialization routines for
// the AppCRT, shared by both the static and dynamic AppCRT libraries.  In the
// dynamic AppCRT library, these are called by DllMain.  In the static AppCRT
// library, these are called by the initialization code.
//
#include <corecrt_internal.h>
#include <stdlib.h>
#include <stdio.h>

_CRT_BEGIN_C_HEADER

extern _onexit_table_t __acrt_atexit_table;
extern _onexit_table_t __acrt_at_quick_exit_table;


static bool __cdecl initialize_c()
{
    _initialize_onexit_table(&__acrt_atexit_table);
    _initialize_onexit_table(&__acrt_at_quick_exit_table);
    return true;
}

static bool __cdecl uninitialize_c(bool)
{
    return true;
}

static bool __cdecl initialize_pointers()
{
    void* const encoded_null = __crt_fast_encode_pointer(nullptr);
    __acrt_initialize_invalid_parameter_handler(encoded_null);
    __acrt_initialize_new_handler(encoded_null);
    return true;
}

// This is the table of initializer/uninitializer pairs that is used to perform
// AppCRT initialization.  Initializers are run first-to-last during AppCRT
// initialization, and uninitializers are run last-to-first during termination.
static __acrt_initializer const __acrt_initializers[] =
{
    { initialize_pointers,                     nullptr                                  },
    { __acrt_initialize_locks,                 __acrt_uninitialize_locks                },
    { __acrt_initialize_ptd,                   __acrt_uninitialize_ptd                  },
    { initialize_c,                            uninitialize_c                           },
};

__crt_bool __cdecl __acrt_initialize()
{
    return __acrt_execute_initializers(
        __acrt_initializers,
        __acrt_initializers + _countof(__acrt_initializers)
        );
}

__crt_bool __cdecl __acrt_uninitialize(__crt_bool const terminating)
{
    UNREFERENCED_PARAMETER(terminating);

    return __acrt_execute_uninitializers(
        __acrt_initializers,
        __acrt_initializers + _countof(__acrt_initializers)
        );
}

_CRT_END_C_HEADER

