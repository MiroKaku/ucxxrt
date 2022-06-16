//
// initialization.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This file defines the main initialization and uninitialization routines for
// the VCRuntime, shared by both the static and dynamic VCRuntime libraries.  In
// the dynamic VCRuntime library, these are called by DllMain.  In the static
// VCRuntime library, these are called by the initialization code.
//
#include <vcruntime_internal.h>

extern "C" {

bool __cdecl __vcrt_initialize()
{
    if (!__vcrt_initialize_locks())
    {
        return false;
    }

    if (!__vcrt_initialize_ptd())
    {
        __vcrt_uninitialize_locks();
        return false;
    }

    return true;
}

bool __cdecl __vcrt_uninitialize(_In_ bool terminating)
{
    UNREFERENCED_PARAMETER(terminating);

    __vcrt_uninitialize_ptd();
    __vcrt_uninitialize_locks();

    return true;
}

bool __cdecl __vcrt_thread_attach()
{
    // Create a per-thread data structure for this thread (getptd will attempt
    // to create a new per-thread data structure if one does not already exist
    // for this thread):
    if (__vcrt_getptd_noexit() == nullptr)
        return false;

    return true;
}

bool __cdecl __vcrt_thread_detach()
{
    // Free the per-thread data structure for this thread:
    __vcrt_freeptd(nullptr);
    return true;
}

}
