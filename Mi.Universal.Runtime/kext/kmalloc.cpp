#include <corecrt_internal.h>
#include <new.h>
#include "kmalloc.h"
#include "knew.h"


extern"C" __declspec(noinline) void* __cdecl ExReallocatePoolWithTag(
    _In_ SIZE_T                                          OldSize,
    _In_ SIZE_T                                          NewSize,
    _In_ PVOID                                           OldBlock,
    _In_ __drv_strictTypeMatch(__drv_typeExpr) POOL_TYPE PoolType,
    _In_ ULONG                                           Tag
)
{
    if (OldSize == 0) {
        return nullptr;
    }

    #pragma warning(suppress: 4996)
    void* const NewBlock = ExAllocatePoolWithTag(PoolType, NewSize, Tag);
    if (NewBlock) {
        memset(NewBlock, 0, NewSize);
        memmove(NewBlock, OldBlock, NewSize < OldSize ? NewSize : OldSize);

        ExFreePoolWithTag(OldBlock, Tag);
        return NewBlock;
    }

    return nullptr;
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl kmalloc(
    size_t const  size,
    pool_t        pool,
    unsigned long tag
)
{
    // Ensure that the requested size is not too large:
    _VALIDATE_RETURN_NOEXC(_HEAP_MAXREQ >= size, ENOMEM, nullptr);

    // Ensure we request an allocation of at least one byte:
    size_t const actual_size = size == 0 ? 1 : size;

    for (;;) {
        #pragma warning(suppress: 4996)
        void* const block = ExAllocatePoolWithTag(pool, actual_size, tag);
        if (block)
            return block;

        // Otherwise, see if we need to call the new handler, and if so call it.
        // If the new handler fails, just return nullptr:
        if (_query_new_mode() == 0 || !_callnewh(actual_size)) {
            errno = ENOMEM;
            return nullptr;
        }

        // The new handler was successful; try to allocate again...
    }
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl kcalloc(
    size_t const  count,
    size_t const  size,
    pool_t        pool,
    unsigned long tag
)
{
    // Ensure that (count * size) does not overflow
    _VALIDATE_RETURN_NOEXC(count == 0 || (_HEAP_MAXREQ / count) >= size, ENOMEM, nullptr);

    // Ensure that we allocate a nonzero block size:
    size_t const requested_block_size = count * size;
    size_t const actual_block_size    = requested_block_size == 0 ? 1 : requested_block_size;

    for (;;) {
        void* const block = kmalloc(actual_block_size, pool, tag);

        // If allocation succeeded, return the pointer to the new block:
        if (block) {
            memset(block, 0, actual_block_size);
            return block;
        }

        // Otherwise, see if we need to call the new handler, and if so call it.
        // If the new handler fails, just return nullptr:
        if (_query_new_mode() == 0 || !_callnewh(actual_block_size)) {
            errno = ENOMEM;
            return nullptr;
        }

        // The new handler was successful; try to allocate aagain...
    }
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl krealloc(
    void* const   block,
    size_t const  old_size,
    size_t const  new_size,
    pool_t        pool,
    unsigned long tag
)
{
    // If the block is a nullptr, just call malloc:
    if (block == nullptr)
        return kmalloc(new_size, pool, tag);

    // If the new size is 0, just call free and return nullptr:
    if (new_size == 0) {
        kfree(block, tag);
        return nullptr;
    }

    // Ensure that the requested size is not too large:
    _VALIDATE_RETURN_NOEXC(_HEAP_MAXREQ >= new_size, ENOMEM, nullptr);

    for (;;) {
        void* const new_block = ExReallocatePoolWithTag(old_size, new_size, block, pool, tag);
        if (new_block) {
            return new_block;
        }

        // Otherwise, see if we need to call the new handler, and if so call it.
        // If the new handler fails, just return nullptr:
        if (_query_new_mode() == 0 || !_callnewh(new_size)) {
            errno = ENOMEM;
            return nullptr;
        }

        // The new handler was successful; try to allocate again...
    }
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl krecalloc(
    void* const   block,
    size_t const  count,
    size_t const  old_size,
    size_t const  new_size,
    pool_t        pool,
    unsigned long tag
)
{
    // Ensure that (count * size) does not overflow
    _VALIDATE_RETURN_NOEXC(count == 0 || (_HEAP_MAXREQ / count) >= new_size, ENOMEM, nullptr);

    size_t const old_block_size = block != nullptr ? (count * old_size) : 0;
    size_t const new_block_size = count * new_size;

    void* const new_block = krealloc(block, old_block_size, new_block_size, pool, tag);

    // If the reallocation succeeded and the new block is larger, zero-fill the
    // new bytes:
    if (new_block != nullptr && old_block_size < new_block_size) {
        memset(static_cast<char*>(new_block) + old_block_size, 0, new_block_size - old_block_size);
    }

    return new_block;
}
