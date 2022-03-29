/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      startup.cpp
 * DATA:      2020/05/08
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

#include <internal_shared.h>

_CRT_BEGIN_C_HEADER

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

_CRTALLOC(".CRT$XIA") _PIFV __xi_a[] = { nullptr }; // C initializers (first)
_CRTALLOC(".CRT$XIZ") _PIFV __xi_z[] = { nullptr }; // C initializers (last)
_CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { nullptr }; // C++ initializers (first)
_CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { nullptr }; // C++ initializers (last)
_CRTALLOC(".CRT$XPA") _PVFV __xp_a[] = { nullptr }; // C pre-terminators (first)
_CRTALLOC(".CRT$XPZ") _PVFV __xp_z[] = { nullptr }; // C pre-terminators (last)
_CRTALLOC(".CRT$XTA") _PVFV __xt_a[] = { nullptr }; // C terminators (first)
_CRTALLOC(".CRT$XTZ") _PVFV __xt_z[] = { nullptr }; // C terminators (last)

#pragma comment(linker, "/merge:.CRT=.rdata")


// Calls each function in [first, last).  [first, last) must be a valid range of
// function pointers.  Each function is called, in order.
void __cdecl _initterm(_PVFV* const first, _PVFV* const last)
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
int __cdecl _initterm_e(_PIFV* const first, _PIFV* const last)
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

extern "C++" struct onexit_entry
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
static onexit_entry* s_quick_onexit_table = nullptr;

int __cdecl _register_onexit_function(onexit_entry* table, _PVFV const function)
{
    const auto entry = new onexit_entry(table, function);
    if (nullptr == entry)
    {
        return -1;
    }
    s_onexit_table = entry;

    return 0;
}

int __cdecl _execute_onexit_table(onexit_entry* table)
{
    for (auto entry = table; entry;)
    {
        const auto next = entry->_next;
        delete entry;
        entry = next;
    }

    return 0;
}

int __cdecl atexit(_PVFV const function)
{
    return _register_onexit_function(s_onexit_table, reinterpret_cast<_PVFV const>(function));
}

int __cdecl at_quick_exit(_PVFV const function)
{
    return _register_onexit_function(s_quick_onexit_table, reinterpret_cast<_PVFV const>(function));
}

onexit_t __cdecl onexit(_In_opt_ onexit_t function)
{
    return atexit((_PVFV)function) == 0
        ? function
        : nullptr;
}

onexit_t __cdecl _onexit(_In_opt_ onexit_t function)
{
    return onexit(function);
}

int __cdecl _do_onexit()
{
    return _execute_onexit_table(s_onexit_table);
}

int __cdecl _do_quick_onexit()
{
    return _execute_onexit_table(s_quick_onexit_table);
}

_CRT_END_C_HEADER
