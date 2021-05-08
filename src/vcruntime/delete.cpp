/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      delete.cpp
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
// Defines the operator delete.
//

#include <malloc.h>
#include <vcruntime_new.h>
#include <internal_shared.h>

////////////////////////////////////////////////////////////////
// delete() Fallback Ordering
//
// +-------------+
// |delete_scalar<----+-----------------------+
// +--^----------+    |                       |
//    |               |                       |
// +--+---------+  +--+---------------+  +----+----------------+
// |delete_array|  |delete_scalar_size|  |delete_scalar_nothrow|
// +--^----^----+  +------------------+  +---------------------+
//    |    |
//    |    +-------------------+
//    |                        |
// +--+--------------+  +------+-------------+
// |delete_array_size|  |delete_array_nothrow|
// +-----------------+  +--------------------+

// delete_scalar
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(void* const block) noexcept
{
    free(block);
}

// delete_scalar_size
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(void* const block, size_t const) noexcept
{
    operator delete(block);
}

// delete_scalar_nothrow
void __CRTDECL operator delete(void* const block, std::nothrow_t const&) noexcept
{
    operator delete(block);
}

// delete_array
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete[](void* const block) noexcept
{
    operator delete(block);
}

// delete_array_size
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete[](void* const block, size_t const) noexcept
{
    operator delete[](block);
}

// delete_array_nothrow
void __CRTDECL operator delete[](void* const block, std::nothrow_t const&) noexcept
{
    operator delete[](block);
}

//////////////////////////////////////////////////////////////////////////////////
// Aligned delete() Fallback Ordering
//
// +-------------------+
// |delete_scalar_align<----+---------------------------+
// +--^----------------+    |                           |
//    |                     |                           |
// +--+---------------+  +--+---------------------+  +--+------------------------+
// |delete_array_align|  |delete_scalar_size_align|  |delete_scalar_align_nothrow|
// +--^-----^---------+  +------------------------+  +---------------------------+
//    |     |
//    |     +------------------------+
//    |                              |
// +--+--------------------+  +------+-------------------+
// |delete_array_size_align|  |delete_array_align_nothrow|
// +-----------------------+  +--------------------------+

// delete_scalar_align
_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(void* const block, std::align_val_t const) noexcept
{
    _aligned_free(block);
}

// delete_scalar_size_align
void __CRTDECL operator delete(void* const block, size_t const, std::align_val_t const al) noexcept
{
    operator delete(block, al);
}

// delete_scalar_align_nothrow
void __CRTDECL operator delete(void* const block, std::align_val_t const al, std::nothrow_t const&) noexcept
{
    operator delete(block, al);
}

// delete_array_align
void __CRTDECL operator delete[](void* const block, std::align_val_t const al) noexcept
{
    operator delete(block, al);
}

// delete_array_size_align
void __CRTDECL operator delete[](void* const block, size_t const, std::align_val_t const al) noexcept
{
    operator delete[](block, al);
}

// delete_array_align_nothrow
void __CRTDECL operator delete[](void* const block, std::align_val_t const al, std::nothrow_t const&) noexcept
{
    operator delete[](block, al);
}

//////////////////////////////////////////////////////////////////////////////////
// Pool delete() Fallback Ordering
//
// +----------------------+
// |delete_scalar_pool_tag<----+---------------------+-----------------------------+
// +--^-------------------+    |                     |                             |
//    |                        |                     |                             |
// +--+------------------+  +--+---------------+  +--+-----------------------+  +--+---------------------------+
// |delete_array_pool_tag|  |delete_scalar_pool|  |delete_scalar_pool_nothrow|  |delete_scalar_pool_tag_nothrow|
// +--^-----^-----^------+  +------------------+  +--------------------------+  +------------------------------+
//    |     |     |
//    |     |     +-------------------------------------+
//    |     |                                           |
//    |     +------------------+                        |
//    |                        |                        |
// +--+--------------+  +------+------------------+  +--+--------------------------+
// |delete_array_pool|  |delete_array_pool_nothrow|  |delete_array_pool_tag_nothrow|
// +-----------------+  +-------------------------+  +-----------------------------+

// user-defined placement deallocation functions
// void operator delete  (void* block, args...);
// void operator delete[](void* block, args...);

// global
namespace ucxxrt
{
    extern ULONG        DefaultPoolTag;
    extern POOL_TYPE    DefaultPoolType;
    extern ULONG        DefaultMdlProtection;
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline)
void __cdecl _free_pool_tag(
    _Pre_maybenull_ _Post_invalid_ void* _Block,
    _In_ int /*_Pool*/,
    _In_ unsigned long _Tag
);

// delete_scalar_pool_tag
void __CRTDECL operator delete(void* block, POOL_TYPE pool, ULONG tag) noexcept
{
    _free_pool_tag(block, pool, tag);
}

// delete_scalar_pool
void __CRTDECL operator delete(void* block, POOL_TYPE pool) noexcept
{
    operator delete(block, pool, ucxxrt::DefaultPoolTag);
}

// delete_scalar_pool_nothrow
void __CRTDECL operator delete(void* block, POOL_TYPE pool, std::nothrow_t const&) noexcept
{
    operator delete(block, pool, ucxxrt::DefaultPoolTag);
}

// delete_scalar_pool_tag_nothrow
void __CRTDECL operator delete(void* block, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept
{
    operator delete(block, pool, tag);
}

// delete_array_pool_tag
void __CRTDECL operator delete[](void* block, POOL_TYPE pool, ULONG tag) noexcept
{
    operator delete(block, pool, tag);
}

// delete_array_pool
void __CRTDECL operator delete[](void* block, POOL_TYPE pool) noexcept
{
    operator delete[](block, pool, ucxxrt::DefaultPoolTag);
}

// delete_array_pool_nothrow
void __CRTDECL operator delete[](void* block, POOL_TYPE pool, std::nothrow_t const&) noexcept
{
    operator delete[](block, pool, ucxxrt::DefaultPoolTag);
}

// delete_array_pool_tag_nothrow
void __CRTDECL operator delete[](void* block, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept
{
    operator delete[](block, pool, tag);
}

//////////////////////////////////////////////////////////////////////////////////
// delete_debug() Fallback Ordering
//
// +-------------------+    +------------------+
// |delete_scalar_debug<----|delete_array_debug|
// +-------------------+    +------------------+
//

#ifdef _DEBUG

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(
    void* block,
    int const   block_use,
    char const* file_name,
    int const   line_number
    ) noexcept
{
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    _free_dbg(block, block_use);
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete[](
    void* block,
    int const   block_use,
    char const* file_name,
    int const   line_number
    ) noexcept
{
    return operator delete(block, block_use, file_name, line_number);
}

#else // ^^^ _DEBUG ^^^ // vvv !_DEBUG vvv //

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(
    void* block,
    int const   block_use,
    char const* file_name,
    int const   line_number
    ) noexcept
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator delete(block);
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete[](
    void* block,
    int const   block_use,
    char const* file_name,
    int const   line_number
    ) noexcept
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator delete[](block);
}

#endif
