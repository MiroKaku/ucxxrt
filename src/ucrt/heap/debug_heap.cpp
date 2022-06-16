//
// debug_heap.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// The implementation of the CRT Debug Heap.
//
#ifndef _DEBUG
    #error This file is supported only in debug builds
    #define _DEBUG // For design-time support, when editing/viewing CRT sources
#endif

#include <corecrt_internal.h>
#include <malloc.h>
#include <minmax.h>
#include <new.h>
#include <stdio.h>
#include <stdlib.h>



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Constant Data
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#define _ALLOCATION_FILE_LINENUM "\nMemory allocated at %hs(%d).\n"

#if _FREE_BLOCK != 0 || _NORMAL_BLOCK != 1 || _CRT_BLOCK != 2 || _IGNORE_BLOCK != 3 || _CLIENT_BLOCK != 4
    #error Block numbers have changed!
#endif

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Public Debug Heap Allocation APIs
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(_Size)
_ACRTIMP _CRTALLOCATOR _CRT_HYBRIDPATCHABLE
extern "C" void* __cdecl _expand_base(
    _Pre_notnull_           void* _Block,
    _In_                    size_t _Size
);

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
extern "C" __declspec(noinline) void* __cdecl _malloc_dbg(
    size_t      const size,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _malloc_base(size);
}


// This function must be marked noinline, otherwise calloc and
// _calloc_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because calloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void* __cdecl _calloc_dbg(
    size_t      const count,
    size_t      const element_size,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _calloc_base(count, element_size);
}


// This function must be marked noinline, otherwise realloc and
// _realloc_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because realloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void* __cdecl _realloc_dbg(
    void*       const block,
    size_t      const requested_size,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _realloc_base(block, requested_size);
}


// This function must be marked noinline, otherwise recalloc and
// _recalloc_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because recalloc
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void* __cdecl _recalloc_dbg(
    void*       const block,
    size_t      const count,
    size_t      const element_size,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _recalloc_base(block, count, element_size);
}

// This function must be marked noinline, otherwise _expand and
// _expand_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because _expand
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void* __cdecl _expand_dbg(
    void*       const block,
    size_t      const requested_size,
    int         const block_use,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _expand_base(block, requested_size);
}



// This function must be marked noinline, otherwise free and
// _free_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because free
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void __cdecl _free_dbg(void* const block, int const block_use)
{
    UNREFERENCED_PARAMETER(block_use);

    return _free_base(block);
}


// This function must be marked noinline, otherwise _msize and
// _msize_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because _msize
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) size_t __cdecl _msize_dbg(void* const block, int const block_use)
{
    UNREFERENCED_PARAMETER(block_use);
    
    return _msize_base(block);
}



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Aligned Allocation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern "C" void __cdecl _aligned_free_base(
    _Pre_maybenull_ _Post_invalid_ void* block
);

_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size)
extern "C" void* __cdecl _aligned_malloc_base(
    _In_ size_t size,
    _In_ size_t alignment
);

_Check_return_
extern "C" size_t __cdecl _aligned_msize_base(
    _Pre_notnull_ void* block,
    _In_          size_t alignment,
    _In_          size_t offset
);

_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size)
extern "C" void* __cdecl _aligned_offset_malloc_base(
    _In_ size_t size,
    _In_ size_t alignment,
    _In_ size_t offset
);

_Success_(return != 0) _Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size)
extern "C" void* __cdecl _aligned_offset_realloc_base(
    _Pre_maybenull_ _Post_invalid_ void* block,
    _In_                           size_t size,
    _In_                           size_t alignment,
    _In_                           size_t offset
);

_Success_(return != 0) _Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size * count)
extern "C" void* __cdecl _aligned_offset_recalloc_base(
    _Pre_maybenull_ _Post_invalid_ void* block,
    _In_                           size_t count,
    _In_                           size_t size,
    _In_                           size_t alignment,
    _In_                           size_t offset
);

_Success_(return != 0) _Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size)
extern "C" void* __cdecl _aligned_realloc_base(
    _Pre_maybenull_ _Post_invalid_ void* block,
    _In_                           size_t size,
    _In_                           size_t alignment
);

_Success_(return != 0) _Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(size * count)
extern "C" void* __cdecl _aligned_recalloc_base(
    _Pre_maybenull_ _Post_invalid_ void* block,
    _In_                           size_t count,
    _In_                           size_t size,
    _In_                           size_t alignment
);


#define IS_2_POW_N(X)   ((X) != 0 && ((X) & ((X) - 1)) == 0)


extern "C" void* __cdecl _aligned_malloc_dbg(
    size_t      const size,
    size_t      const alignment,
    char const* const file_name,
    int         const line_number
    )
{
    return _aligned_offset_malloc_dbg(size, alignment, 0, file_name, line_number);
}

extern "C" void* __cdecl _aligned_offset_malloc_dbg(
    size_t      const size,
    size_t            alignment,
    size_t      const offset,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _aligned_offset_malloc_base(size, alignment, offset);
}

extern "C" void* __cdecl _aligned_realloc_dbg(
    void*       const block,
    size_t      const size,
    size_t      const alignment,
    char const* const file_name,
    int         const line_number
    )
{
    return _aligned_offset_realloc_dbg(block, size, alignment, 0, file_name, line_number);
}

extern "C" void* __cdecl _aligned_recalloc_dbg(
    void*       const block,
    size_t      const count,
    size_t      const size,
    size_t      const alignment,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _aligned_offset_recalloc_dbg(block, count, size, alignment, 0, file_name, line_number);
}

extern "C" void* __cdecl _aligned_offset_realloc_dbg(
    void * block,
    size_t size,
    size_t alignment,
    size_t offset,
    const char * file_name,
    int line_number
    )
{
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _aligned_offset_realloc_base(block, size, alignment, offset);
}

extern "C" size_t __cdecl _aligned_msize_dbg(
    void*  const block,
    size_t       alignment,
    size_t const offset
    )
{
    return _aligned_msize_base(block, alignment, offset);
}

extern "C" void* __cdecl _aligned_offset_recalloc_dbg(
    void*       const block,
    size_t      const count,
    size_t      const element_size,
    size_t      const alignment,
    size_t      const offset,
    char const* const file_name,
    int         const line_number
    )
{
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return _aligned_offset_recalloc_base(block, count, element_size, alignment, offset);
}

extern "C" void __cdecl _aligned_free_dbg(void* const block)
{
    return _aligned_free_base(block);
}
