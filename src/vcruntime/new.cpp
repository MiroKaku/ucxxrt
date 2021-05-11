/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      new.cpp
 * DATA:      2021/05/08
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines the operator new.
//

#include <stdlib.h>
#include <vcruntime_new.h>
#include <internal_shared.h>

#ifdef _KERNEL_MODE

// Enable the compiler to elide null checks during LTCG
#pragma comment(linker, "/ThrowingNew")

__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_alloc();
__declspec(noreturn) void __CRTDECL __scrt_throw_std_bad_array_new_length();

////////////////////////////////////
// new() Fallback Ordering
//
// +----------+
// |new_scalar<---------------+
// +----^-----+               |
//      |                     |
// +----+-------------+  +----+----+
// |new_scalar_nothrow|  |new_array|
// +------------------+  +----^----+
//                            |
//               +------------+----+
//               |new_array_nothrow|
//               +-----------------+

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new(size_t const size)
{
    for (;;)
    {
        if (void* const block = malloc(size))
        {
            return block;
        }

        if (_callnewh(size) == 0)
        {
            if (size == SIZE_MAX)
            {
                __scrt_throw_std_bad_array_new_length();
            }
            else
            {
                __scrt_throw_std_bad_alloc();
            }
        }

        // The new handler was successful; try to allocate again...
    }
}

// new_scalar_nothrow
void* __CRTDECL operator new(size_t const size, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new(size);
    }
    catch (...)
    {
        return nullptr;
    }
}

// new_array
void* __CRTDECL operator new[](size_t const size)
{
    return operator new(size);
}

// new_array_nothrow
void* __CRTDECL operator new[](size_t const size, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new[](size);
    }
    catch (...)
    {
        return nullptr;
    }
}

////////////////////////////////////////////////
// Aligned new() Fallback Ordering
//
// +----------------+
// |new_scalar_align<--------------+
// +----^-----------+              |
//      |                          |
// +----+-------------------+  +---+-----------+
// |new_scalar_align_nothrow|  |new_array_align|
// +------------------------+  +---^-----------+
//                                 |
//                     +-----------+-----------+
//                     |new_array_align_nothrow|
//                     +-----------------------+
//

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new(size_t const size, std::align_val_t const al)
{
    for (;;)
    {
        if (void* const block = _aligned_malloc(size, static_cast<size_t>(al)))
        {
            return block;
        }

        if (_callnewh(size) == 0)
        {
            if (size == SIZE_MAX)
            {
                __scrt_throw_std_bad_array_new_length();
            }
            else
            {
                __scrt_throw_std_bad_alloc();
            }
        }

        // The new handler was successful; try to allocate again...
    }
}

// new_scalar_align_nothrow
void* __CRTDECL operator new(size_t const size, std::align_val_t const al, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new(size, al);
    }
    catch (...)
    {
        return nullptr;
    }
}

// new_array_align
void* __CRTDECL operator new[](size_t const size, std::align_val_t const al)
{
    return operator new(size, al);
}

// new_array_align_nothrow
void* __CRTDECL operator new[](size_t const size, std::align_val_t const al, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new[](size, al);
    }
    catch (...)
    {
        return nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Pool new() Fallback Ordering
//
// +-------------------+
// |new_scalar_pool_tag<----+---------------------+-----------------------------+
// +--^----------------+    |                     |                             |
//    |                     |                     |                             |
// +--+---------------+  +--+------------+  +--+--------------------+  +--+------------------------+
// |new_array_pool_tag|  |new_scalar_pool|  |new_scalar_pool_nothrow|  |new_scalar_pool_tag_nothrow|
// +--^-----^-----^---+  +---------------+  +-----------------------+  +---------------------------+
//    |     |     |
//    |     |     +----------------------------------+
//    |     |                                        |
//    |     +---------------+                        |
//    |                     |                        |
// +--+-----------+  +------+---------------+  +--+-----------------------+
// |new_array_pool|  |new_array_pool_nothrow|  |new_array_pool_tag_nothrow|
// +--------------+  +----------------------+  +--------------------------+

// user-defined placement allocation functions
// void* operator new  (std::size_t count, user-defined-args...);
// void* operator new[](std::size_t count, user-defined-args...);

// global
namespace ucxxrt
{
    extern ULONG        DefaultPoolTag;
    extern POOL_TYPE    DefaultPoolType;
    extern ULONG        DefaultMdlProtection;
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl _malloc_pool_tag(
    _In_ _CRT_GUARDOVERFLOW size_t _Size,
    _In_ int _Pool,
    _In_ unsigned long _Tag
);

// new_scalar_pool_tag
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool, ULONG tag)
{
    for (;;)
    {
        if (void* const block = _malloc_pool_tag(size, pool, tag))
        {
            return block;
        }

        if (_callnewh(size) == 0)
        {
            if (size == SIZE_MAX)
            {
                __scrt_throw_std_bad_array_new_length();
            }
            else
            {
                __scrt_throw_std_bad_alloc();
            }
        }

        // The new handler was successful; try to allocate again...
    }
}

// new_scalar_pool
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool)
{
    return operator new(size, pool, ucxxrt::DefaultPoolTag);
}

// new_scalar_pool_nothrow
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new(size, pool, ucxxrt::DefaultPoolTag);
    }
    catch (...)
    {
        return nullptr;
    }
}

// new_scalar_pool_tag_nothrow
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new(size, pool, tag);
    }
    catch (...)
    {
        return nullptr;
    }
}

// new_array_pool_tag
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool, ULONG tag)
{
    return operator new(size, pool, tag);
}

// new_array_pool
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool)
{
    return operator new[](size, pool, ucxxrt::DefaultPoolTag);
}

// new_array_pool_nothrow
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new[](size, pool, ucxxrt::DefaultPoolTag);
    }
    catch (...)
    {
        return nullptr;
    }
}

// new_array_pool_tag_nothrow
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept
{
    try
    {
        return operator new[](size, pool, tag);
    }
    catch (...)
    {
        return nullptr;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// new_debug() Fallback Ordering
//
// +----------------+    +---------------+
// |new_scalar_debug<----|new_array_debug|
// +--^-------------+    +---------------+
//

#ifdef _DEBUG

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new(
    size_t const size,
    int const    block_use,
    char const*  file_name,
    int const    line_number
    )
{
    for (;;)
    {
        if (void* const block = _malloc_dbg(size, block_use, file_name, line_number))
        {
            return block;
        }

        if (_callnewh(size) == 0)
        {
            if (size == SIZE_MAX)
            {
                __scrt_throw_std_bad_array_new_length();
            }
            else
            {
                __scrt_throw_std_bad_alloc();
            }
        }
    }
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new[](
    size_t const size,
    int const    block_use,
    char const*  file_name,
    int const    line_number
    )
{
    return operator new(size, block_use, file_name, line_number);
}

#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv //

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new(
    size_t const size,
    int const    block_use,
    char const* file_name,
    int const    line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator new(size);
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new[](
    size_t const size,
    int const    block_use,
    char const* file_name,
    int const    line_number
    )
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator new[](size);
}

#endif // !_DEBUG

#endif
