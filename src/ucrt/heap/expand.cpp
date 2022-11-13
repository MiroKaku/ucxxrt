//
// expand.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Implementation of _expand().
//
#include <corecrt_internal.h>
#include <malloc.h>


// Expands or contracts a block of memory in the heap.
//
// This function resizes a block of memory in the heap to 'size' bytes.  The
// new size may be either greater (expansion) or less (contraction) than the
// original size of the block.  This function never moves the block.  In the
// case of expansion, if the block cannot be expanded to 'size', it is expanded
// as much as possible.
//
// This function supports patching and therefore must be marked noinline.
// Both _expand_dbg and _expand_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to _expand
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void* __cdecl _expand(void* const block, size_t const size)
{
    // Validation section
    _VALIDATE_RETURN(block != nullptr, EINVAL, nullptr);
    _VALIDATE_RETURN_NOEXC(size <= _HEAP_MAXREQ, ENOMEM, nullptr);

    size_t const new_size = size == 0 ? 1 : size;

    void* new_block = realloc(block, new_size);
    if (new_block != nullptr)
        return new_block;

    errno = __acrt_errno_from_os_error(STATUS_INSUFFICIENT_RESOURCES);
    return nullptr;
}
