/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      calloc_km.cpp
 * DATA:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_internal.h>
#include <inc/malloc_km.h>
#include <new.h>


// This function implements the logic of calloc().
//
// This function must be marked noinline, otherwise calloc and
// _calloc_base will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because calloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _kcalloc_base(
    size_t const count,
    size_t const size,
    int          pool_type,
    unsigned long tag
)
{
    // Ensure that (count * size) does not overflow
    _VALIDATE_RETURN_NOEXC(count == 0 || (_HEAP_MAXREQ / count) >= size, ENOMEM, nullptr);

    // Ensure that we allocate a nonzero block size:
    size_t const requested_block_size = count * size;
    size_t const actual_block_size = requested_block_size == 0
        ? 1
        : requested_block_size;

    for (;;)
    {
        void* const block = kmalloc(actual_block_size, pool_type, tag);

        // If allocation succeeded, return the pointer to the new block:
        if (block)
        {
            memset(block, 0, actual_block_size);
            return block;
        }

        // Otherwise, see if we need to call the new handler, and if so call it.
        // If the new handler fails, just return nullptr:
        if (_query_new_mode() == 0 || !_callnewh(actual_block_size))
        {
            errno = ENOMEM;
            return nullptr;
        }

        // The new handler was successful; try to allocate aagain...
    }
}


// This function must be marked noinline, otherwise calloc and
// _calloc_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because calloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void* __cdecl _kcalloc_dbg(
    size_t      const count,
    size_t      const element_size,
    int               pool_type,
    unsigned    long  tag,
    int         const block_use,
    char const* const file_name,
    int         const line_number
)
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _kcalloc_base(count, element_size, pool_type, tag);
}


// Allocates a block of memory of size 'count * size' in the heap.  The newly
// allocated block is zero-initialized.  If allocation fails, nullptr is
// returned.
//
// This function supports patching and therefore must be marked noinline.
// Both _calloc_dbg and _calloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to calloc
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl kcalloc(
    size_t const count,
    size_t const size,
    int          pool_type,
    unsigned long tag
    )
{
#ifdef _DEBUG
    return _kcalloc_dbg(count, size, pool_type, tag, _NORMAL_BLOCK, nullptr, 0);
#else
    return _kcalloc_base(count, size, pool_type, tag);
#endif
}
