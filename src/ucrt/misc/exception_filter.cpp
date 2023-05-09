//
// exception_filter.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The exception filter used by the startup code to transform various exceptions
// into C signals.
//
#include <corecrt_internal.h>
#include <float.h>
#include <signal.h>
#include <stddef.h>


// Wrapper that only calls the exception filter for C++ exceptions
extern "C" int __cdecl _seh_filter_dll(
    unsigned long       const xcptnum,
    PEXCEPTION_POINTERS const /*pxcptinfoptrs*/
    )
{
    if (xcptnum != ('msc' | 0xE0000000))
        return EXCEPTION_CONTINUE_SEARCH;

    return EXCEPTION_CONTINUE_EXECUTION;
}
