/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      malloc_km.cpp
 * DATA:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_internal.h>
#include <malloc.h>
#include <new.h>


// This function implements the logic of kmalloc().  It is called directly by the
// kmalloc() function in the Release CRT and is called by the debug heap in the
// Debug CRT.
//
// This function must be marked noinline, otherwise malloc and
// _kmalloc_base will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because malloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _kmalloc_base(size_t const size, int pool_type, unsigned long tag)
{
    // Ensure that the requested size is not too large:
    _VALIDATE_RETURN_NOEXC(_HEAP_MAXREQ >= size, ENOMEM, nullptr);

    // Ensure we request an allocation of at least one byte:
    size_t const actual_size = size == 0 ? 1 : size;

    for (;;)
    {
#pragma warning(suppress: 4996)
        void* const block = ExAllocatePoolWithTag((POOL_TYPE)pool_type, actual_size, tag);
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

#ifdef _DEBUG
// These are the allocation functions that allocate, manipulate, and free blocks
// from the debug heap.  They are equivalent to the main allocation functions,
// which deal with blocks from the process heap.  Most of the debug allocation
// functions accept a block use, file name, and/or line number which are used to
// track where allocations originated.
//
// Documentation comments for these functions describe only the material
// differences between them and the corresponding main allocation functions.

// This function must be marked noinline, otherwise kmalloc and
// _kmalloc_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because malloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void* __cdecl _kmalloc_dbg(
    size_t      const size,
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

    return _kmalloc_base(size, pool_type, tag);
}
#endif

// Allocates a block of memory of size 'size' bytes in the heap.  If allocation
// fails, nullptr is returned.
//
// This function supports patching and therefore must be marked noinline.
// Both _kmalloc_dbg and _kmalloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to malloc
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl kmalloc(
    size_t      const   size,
    int                 pool_type,
    unsigned    long    tag
)
{
#ifdef _DEBUG
    return _kmalloc_dbg(size, pool_type, tag, _NORMAL_BLOCK, nullptr, 0);
#else
    return _kmalloc_base(size, pool_type, tag);
#endif
}
