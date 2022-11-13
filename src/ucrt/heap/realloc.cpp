//
// realloc.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Implementation of realloc().
//
#include <corecrt_internal.h>
#include <malloc.h>
#include <new.h>



extern"C" __declspec(noinline) void* __cdecl ExReallocatePoolWithTag(
    _In_ SIZE_T OldSize,
    _In_ SIZE_T NewSize,
    _In_ PVOID  OldBlock,
    _In_ __drv_strictTypeMatch(__drv_typeExpr) POOL_TYPE PoolType,
    _In_ ULONG Tag
)
{
    if (OldSize == 0)
    {
        return nullptr;
    }

#pragma warning(suppress: 4996)
    void* const NewBlock = ExAllocatePoolWithTag(PoolType, NewSize, Tag);
    if (NewBlock)
    {
        memset(NewBlock, 0, NewSize);
        memmove(NewBlock, OldBlock, OldSize);

        ExFreePoolWithTag(OldBlock, Tag);
        return NewBlock;
    }

    return nullptr;
}

// Reallocates a block of memory in the heap.
//
// This function reallocates the block pointed to by 'block' such that it is
// 'size' bytes in size.  The new size may be either greater or less than the
// original size of the block.  The reallocation may result in the block being
// moved to a new location in memory.  If the block is moved, the contents of
// the original block are copied.
//
// Standard behavior notes:
// [1] realloc(nullptr, new_size) is equivalent to malloc(new_size)
// [2] realloc(p, 0) is equivalent to free(p), and nullptr is returned
// [3] If reallocation fails, the original block is left unchanged
//
// If 'block' is non-null, it must point to a valid block of memory allocated in
// the heap.
//
// This function supports patching and therefore must be marked noinline.
// Both _realloc_dbg and _realloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to realloc
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl realloc(
    void*  const block,
    size_t const size
    )
{
    // If the block is a nullptr, just call malloc:
    if (block == nullptr)
        return malloc(size);

    // If the new size is 0, just call free and return nullptr:
    if (size == 0)
    {
        free(block);
        return nullptr;
    }

    // Ensure that the requested size is not too large:
    _VALIDATE_RETURN_NOEXC(_HEAP_MAXREQ >= size, ENOMEM, nullptr);

    for (;;)
    {
        void* const new_block = ExReallocatePoolWithTag(_msize(block), size, block, NonPagedPool, __ucxxrt_tag);
        if (new_block)
        {
            return new_block;
        }

        // Otherwise, see if we need to call the new handler, and if so call it.
        // If the new handler fails, just return nullptr:
        if (_query_new_mode() == 0 || !_callnewh(size))
        {
            errno = ENOMEM;
            return nullptr;
        }

        // The new handler was successful; try to allocate again...
    }
}
