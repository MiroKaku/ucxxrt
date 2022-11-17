//
// malloc.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Implementation of malloc().
//
#include <corecrt_internal.h>
#include <malloc.h>
#include <new.h>



// Allocates a block of memory of size 'size' bytes in the heap.  If allocation
// fails, nullptr is returned.
//
// This function supports patching and therefore must be marked noinline.
// Both _malloc_dbg and _malloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to malloc
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl malloc(size_t const size)
{
    // Ensure that the requested size is not too large:
    _VALIDATE_RETURN_NOEXC(_HEAP_MAXREQ >= size, ENOMEM, nullptr);

    // Ensure we request an allocation of at least one byte:
    size_t const actual_size = size == 0 ? 1 : size;

    for (;;)
    {
        void* const block = __override_malloc(size);
        if (block)
            return block;

        // Otherwise, see if we need to call the new handler, and if so call it.
        // If the new handler fails, just return nullptr:
        if (_query_new_mode() == 0 || !_callnewh(actual_size))
        {
            errno = ENOMEM;
            return nullptr;
        }

        // The new handler was successful; try to allocate again...
    }
}
