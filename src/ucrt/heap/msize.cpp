//
// msize.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Implementation of msize().
//
#include <corecrt_internal.h>
#include <malloc.h>



// Calculates the size of the specified block in the heap.  'block' must be a
// pointer to a valid block of heap-allocated memory (it must not be nullptr).
//
// This function supports patching and therefore must be marked noinline.
// Both _msize_dbg and _msize_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to _msize
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) size_t __cdecl _msize(void* const block)
{
    return std_msize(block);
}
