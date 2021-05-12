/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      malloc.cpp
 * DATA:      2020/02/05
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 /***
  * Copyright (c) Microsoft Corporation. All rights reserved.
  *
  * Purpose:
  *       Defines the malloc(),
  *                   free(),
  *                   calloc(),
  *                   realloc(),
  *                   recalloc(),
  *                   _msize(),
  *                   _expand(),
  *                   _malloc_pool_tag(),
  *                   _free_pool_tag() functions.
  *
  *******************************************************************************/

#include <internal_shared.h>
#include <malloc.h>
#include <new.h>

#ifdef _KERNEL_MODE

 // global
namespace ucxxrt
{
    ULONG       DefaultPoolTag       = _ByteSwap32('ucrt');
    POOL_TYPE   DefaultPoolType      = POOL_TYPE::NonPagedPoolNx;
    ULONG       DefaultMdlProtection = MdlMappingNoExecute;
}

extern "C" void __cdecl __initialize_memory()
{
    RTL_OSVERSIONINFOW ver_info{};

    auto status = RtlGetVersion(&ver_info);
    if (!NT_SUCCESS(status))
    {
        return;
    }

    if ((ver_info.dwMajorVersion < 6) ||
        (ver_info.dwMajorVersion == 6 && ver_info.dwMinorVersion < 2))
    {
        ucxxrt::DefaultPoolType = POOL_TYPE::NonPagedPool;
        ucxxrt::DefaultMdlProtection = 0;
    }
}

// Allocates a block of memory of size 'size' bytes in the heap.  If allocation
// fails, nullptr is returned.
//
// This function supports patching and therefore must be marked noinline.
// Both _malloc_dbg and _malloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to malloc
// with either other function or vice versa.
extern "C" __declspec(noinline) _CRTRESTRICT
void* __cdecl _malloc_base(size_t const size)
{
    // Ensure that the requested size is not too large:
    _VALIDATE_RETURN_NOEXC(_HEAP_MAXREQ >= size, ENOMEM, nullptr);

    // Ensure we request an allocation of at least one byte:
    size_t const actual_size = size == 0 ? 1 : size;

    for (;;)
    {
        void* const block = ExAllocatePoolWithTag(ucxxrt::DefaultPoolType, actual_size, ucxxrt::DefaultPoolTag);
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

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl malloc(
    _In_ _CRT_GUARDOVERFLOW size_t size
)
{
    return _malloc_base(size);
}

// Frees a block in the heap.  The 'block' pointer must either be a null pointer
// or must point to a valid block in the heap.
//
// This function supports patching and therefore must be marked noinline.
// Both _free_dbg and _free_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to free
// with either other function or vice versa.
extern "C" __declspec(noinline)
void __cdecl _free_base(void* const block)
{
    if (block)
    {
        ExFreePoolWithTag(block, ucxxrt::DefaultPoolTag);
    }
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline)
void __cdecl free(
    _Pre_maybenull_ _Post_invalid_ void* block
)
{
    return _free_base(block);
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl _malloc_pool_tag(
    _In_ _CRT_GUARDOVERFLOW size_t _Size,
    _In_ int _Pool,
    _In_ unsigned long _Tag
)
{
    if (_Size == 0)
        _Size = 1;

    return ExAllocatePoolWithTag((POOL_TYPE)_Pool, _Size, _Tag);
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline)
void __cdecl _free_pool_tag(
    _Pre_maybenull_ _Post_invalid_ void* _Block,
    _In_ int /*_Pool*/,
    _In_ unsigned long _Tag
)
{
    if (_Block)
    {
        ExFreePoolWithTag(_Block, _Tag);
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

// This function must be marked noinline, otherwise malloc and
// _malloc_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because malloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline)
void* __cdecl _malloc_dbg(
    size_t      const size,
    int         const /*block_use*/,
    char const* const /*file_name*/,
    int         const /*line_number*/
)
{
    // TODO
    return malloc(size);
}

// This function must be marked noinline, otherwise free and
// _free_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because free
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline)
void __cdecl _free_dbg(void* const block, int const /*block_use*/)
{
    // TODO
    return free(block);
}
#endif // _DEBUG

// Allocates a block of memory of size 'count * size' in the heap.  The newly
// allocated block is zero-initialized.  If allocation fails, nullptr is
// returned.
//
// This function supports patching and therefore must be marked noinline.
// Both _calloc_dbg and _calloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to calloc
// with either other function or vice versa.
extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _calloc_base(
    size_t const count,
    size_t const size
)
{
    // Ensure that (count * size) does not overflow
    _VALIDATE_RETURN_NOEXC(count == 0 || (_HEAP_MAXREQ / count) >= size, ENOMEM, nullptr);

    return malloc(count * size);
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl calloc(
    _In_ _CRT_GUARDOVERFLOW size_t count,
    _In_ _CRT_GUARDOVERFLOW size_t size
)
{
    return _calloc_base(count, size);
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
extern "C" __declspec(noinline) _CRTRESTRICT
void* __cdecl _realloc_base(
    void* const block,
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
        void* const new_block = ExAllocatePoolWithTag(ucxxrt::DefaultPoolType, size, ucxxrt::DefaultPoolTag);
        if (new_block)
        {
            memmove(new_block, block, _msize_base(block));
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

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl realloc(
    void* const block,
    size_t const size
)
{
    return _realloc_base(block, size);
}

// Reallocates a block of memory in the heap.
//
// This function reallocates the block pointed to by 'block' such that it is
// 'count * size' bytes in size.  The new size may be either greater or less
// than the original size of the block.  If the new size is greater than the
// original size, the new bytes are zero-filled.  This function shares its
// implementation with the realloc() function; consult the comments of that
// function for more information about the implementation.
//
// This function supports patching and therefore must be marked noinline.
// Both _recalloc_dbg and _recalloc_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to _recalloc
// with either other function or vice versa.
extern "C" __declspec(noinline) _CRTRESTRICT
void* __cdecl _recalloc_base(
    void* const block,
    size_t const count,
    size_t const size
)
{
    // Ensure that (count * size) does not overflow
    _VALIDATE_RETURN_NOEXC(count == 0 || (_HEAP_MAXREQ / count) >= size, ENOMEM, nullptr);

    return _realloc_base(block, count * size);
}

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl _recalloc(
    void* const block,
    size_t const count,
    size_t const size
)
{
    return _recalloc_base(block, count, size);
}

// This function implements the logic of _msize().  It is called only in the
// Release CRT.  The Debug CRT has its own implementation of this function.
//
// This function must be marked noinline, otherwise _msize and
// _msize_base will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because _msize
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) size_t __cdecl _msize_base(void* const block)
{
    // Validation section
    _VALIDATE_RETURN(block != nullptr, EINVAL, static_cast<size_t>(-1));

    // TODO
    return 0;
}

// Calculates the size of the specified block in the heap.  'block' must be a
// pointer to a valid block of heap-allocated memory (it must not be nullptr).
//
// This function supports patching and therefore must be marked noinline.
// Both _msize_dbg and _msize_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to _msize
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline)
size_t __cdecl _msize(void* const block)
{
    return _msize_base(block);
}

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
extern "C" __declspec(noinline) void* __cdecl _expand_base(void* const block, size_t const size)
{
    // Validation section
    _VALIDATE_RETURN(block != nullptr, EINVAL, nullptr);
    _VALIDATE_RETURN_NOEXC(size <= _HEAP_MAXREQ, ENOMEM, nullptr);

    // TODO
    return nullptr;
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline)
void* __cdecl _expand(void* const block, size_t const size)
{
    return _expand_base(block, size);
}

#endif
