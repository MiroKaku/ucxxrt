// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
// 
// CoreSTL
// 
// Copyright (C) MeeSong. All rights reserved.
// 	    Author : MeeSong 
//	    Email  : meesong@live.cn
// 	    Github : https://github.com/meesong
//      License: GNU Library General Public License(LGPL) - Version 3
// 
// This file is part of Idea
// 
// Idea is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Idea is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with Idea.  If not, see <http://www.gnu.org/licenses/>.
//
// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

#include "include/new.h"


#if __has_include(<wdm.h>)


// global
namespace ucxxrt
{
    ULONG       DefaultPoolTag          = _ByteSwap32('ucrt');
    POOL_TYPE   DefaultPoolType         = POOL_TYPE::NonPagedPoolNx;
    ULONG       DefaultMdlProtection    = MdlMappingNoExecute;
}

namespace std
{
    [[noreturn]] void __cdecl _Xbad_alloc();
}

using namespace ucxxrt;

// replaceable allocation functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, count, DefaultPoolTag))
        return ptr;

    std::_Xbad_alloc();
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, count, DefaultPoolTag))
        return ptr;

    std::_Xbad_alloc();
}


// replaceable usual deallocation functions
void __CRTDECL operator delete(
    void* ptr
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete[](
    void* ptr
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete(
    void* ptr,
    size_t /*count*/
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    size_t /*count*/
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}


// replaceable non-throwing allocation functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    std::nothrow_t const&
    ) noexcept
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, count, DefaultPoolTag))
        return ptr;

    return nullptr;
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    std::nothrow_t const&
    ) noexcept
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, count, DefaultPoolTag))
        return ptr;

    return nullptr;
}


// replaceable placement deallocation functions
void __CRTDECL operator delete(
    void* ptr,
    std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}


// user-defined placement allocation functions
// void* operator new  (std::size_t count, user-defined-args...);
// void* operator new[](std::size_t count, user-defined-args...);

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, DefaultPoolTag))
        return ptr;

    std::_Xbad_alloc();
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, DefaultPoolTag))
        return ptr;

    std::_Xbad_alloc();
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool,
    ULONG tag
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, tag))
        return ptr;

    std::_Xbad_alloc();
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool,
    ULONG tag
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, tag))
        return ptr;

    std::_Xbad_alloc();
}


// user-defined placement deallocation functions
// void operator delete  (void* ptr, args...);
// void operator delete[](void* ptr, args...);

void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE /*pool*/
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE /*pool*/
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE /*pool*/,
    ULONG tag
    ) noexcept
{
    if (ptr)
    {
        ExFreePoolWithTag(ptr, tag);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE /*pool*/,
    ULONG tag
    ) noexcept
{
    if (ptr)
    {
        ExFreePoolWithTag(ptr, tag);
    }
}


// user-defined non-throwing placement allocation functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool,
    std::nothrow_t const&
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, DefaultPoolTag))
        return ptr;

    return nullptr;
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool,
    std::nothrow_t const&
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, DefaultPoolTag))
        return ptr;

    return nullptr;
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool,
    ULONG tag,
    std::nothrow_t const&
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, tag))
        return ptr;

    return nullptr;
}

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool,
    ULONG tag,
    std::nothrow_t const&
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(pool, count, tag))
        return ptr;

    return nullptr;
}


// user-defined non-throwing placement deallocation functions
void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE /*pool*/,
    std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE /*pool*/,
    std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE /*pool*/,
    ULONG tag,
    std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePoolWithTag(ptr, tag);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE /*pool*/,
    ULONG tag,
    std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePoolWithTag(ptr, tag);
    }
}


#endif
