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

#if defined DBG && __has_include(<wdm.h>)
#   define DisableCheckPoolTag 1
#elif !__has_include(<wdm.h>)
#   define DisableCheckPoolTag 1
#endif


#ifndef DisableCheckPoolTag

#pragma warning( push )
#pragma warning( disable: 4201)
#ifdef _AMD64_
struct PoolHeader
{
    union
    {
        struct
        {
            uint16_t PreviousSize : 8;
            uint16_t PoolIndex    : 8;
            uint16_t BlockSize    : 8;
            uint16_t POOL_TYPE     : 8;
        };
        uint32_t Filling;
    };
    uint32_t     PoolTag;
    union
    {
        struct _EPROCESS* ProcessBilled;
        struct
        {
            uint16_t AllocatorBackTraceIndex;
            uint16_t PoolTagHash;
        };
    };
};
#else
struct PoolHeader
{
    union
    {
        struct
        {
            uint16_t PreviousSize : 9;
            uint16_t PoolIndex : 7;
            uint16_t BlockSize : 9;
            uint16_t POOL_TYPE : 7;
        };
        uint32_t Filling;
    };
    union
    {
        uint32_t     PoolTag;
        struct
        {
            uint16_t AllocatorBackTraceIndex;
            uint16_t PoolTagHash;
        };
    };
};
#endif
#pragma warning( pop )

#define PageAlign$(Va) ((void*)((size_t)(Va) & ~(0x1000 - 1)))
static auto check_pooltag(void* _ptr, POOL_TYPE /*_pool_type*/, uint32_t _tag)
{
    if (PageAlign$(_ptr) == _ptr)
    {
        //
        // 不支持
        // 因为存储大页池的 nt!PoolBigPageTable 数组,
        // 以及描述数组大小的 nt!PoolBigPageTableSize 都未导出.
        //
        return;
    }

    const auto pool_header = reinterpret_cast<PoolHeader*>(size_t(_ptr) - sizeof(PoolHeader));
    if (_tag != pool_header->PoolTag)
    {
        // See https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/bug-check-0x19--bad-pool-header
        KeBugCheckEx(
            BAD_POOL_HEADER,
            0x50u,
            size_t(_ptr),
            size_t(pool_header->PoolTag),
            size_t(_tag));
    }
}
#undef PageAlign$

#else
#define check_pooltag(_ptr, _pool_type, _tag)
#endif

extern ULONG        DefaultPoolTag          = _ByteSwap32('ucrt'); // ucrt
extern POOL_TYPE    DefaultPoolType         = POOL_TYPE::NonPagedPoolNx;
extern ULONG        DefaultMdlProtection    = MdlMappingNoExecute;

namespace std
{
    [[noreturn]] void __cdecl _Xbad_alloc();
}

// replaceable usual deallocation functions
auto __cdecl operator new(size_t _size)
-> void *
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, _size, DefaultPoolTag))
    {
        return ptr;
    }

    std::_Xbad_alloc();
}

auto __cdecl operator new(size_t _size, POOL_TYPE _pool_type)
-> void *
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, DefaultPoolTag))
    {
        return ptr;
    }

    std::_Xbad_alloc();
}

auto __cdecl operator new(size_t _size, POOL_TYPE _pool_type, ULONG _tag)
-> void *
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, _tag))
    {
        return ptr;
    }

    std::_Xbad_alloc();
}

auto __cdecl operator new[](size_t _size)
-> void *
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, _size, DefaultPoolTag))
    {
        return ptr;
    }

    std::_Xbad_alloc();
}

auto __cdecl operator new[](size_t _size, POOL_TYPE _pool_type)
-> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, DefaultPoolTag))
    {
        return ptr;
    }

    std::_Xbad_alloc();
}

auto __cdecl operator new[](size_t _size, POOL_TYPE _pool_type, ULONG _tag)
-> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, _tag))
    {
        return ptr;
    }

    std::_Xbad_alloc();
}

auto __cdecl operator delete(void* _ptr)
-> void
{
    if (nullptr == _ptr) return;

    check_pooltag(_ptr, DefaultPoolType, DefaultPoolTag);
    return ExFreePoolWithTag(_ptr, DefaultPoolTag);
}

auto __cdecl operator delete(void* _ptr, POOL_TYPE _pool_type)
-> void
{
    _pool_type;

    if (nullptr == _ptr) return;

    check_pooltag(_ptr, _pool_type, DefaultPoolTag);
    return ExFreePoolWithTag(_ptr, DefaultPoolTag);
}

auto __cdecl operator delete(void* _ptr, POOL_TYPE _pool_type, ULONG _tag)
-> void
{
    _pool_type;

    if (nullptr == _ptr) return;

    check_pooltag(_ptr, _pool_type, _tag);
    return ExFreePoolWithTag(_ptr, _tag);
}

auto __cdecl operator delete[](void * _ptr)
-> void
{
    if (nullptr == _ptr) return;

    check_pooltag(_ptr, DefaultPoolType, DefaultPoolTag);
    return ExFreePoolWithTag(_ptr, DefaultPoolTag);
}

auto __cdecl operator delete[](void * _ptr, POOL_TYPE _pool_type) -> void
{
    _pool_type;

    if (nullptr == _ptr) return;

    check_pooltag(_ptr, _pool_type, DefaultPoolTag);
    return ExFreePoolWithTag(_ptr, DefaultPoolTag);
}

auto __cdecl operator delete[](void * _ptr, POOL_TYPE _pool_type, ULONG _tag)
-> void
{
    _pool_type;

    if (nullptr == _ptr) return;

    check_pooltag(_ptr, _pool_type, _tag);
    return ExFreePoolWithTag(_ptr, _tag);
}

// replaceable placement deallocation functions
//auto __cdecl operator new   (size_t /*_size*/, void* _ptr) noexcept
//-> void*
//{
//    return _ptr;
//}
//
//auto __cdecl operator new[](size_t /*_size*/, void* _ptr) noexcept
//-> void*
//{
//    return _ptr;
//}

// T::~T()
//auto __cdecl operator delete  (void*, void*)
//-> void
//{
//    return ;
//}
//
//auto __cdecl operator delete[](void*, void*)
//-> void
//{
//    return ;
//}

// sized class - specific deallocation functions
auto __cdecl operator delete  (void* _ptr, size_t /*_size*/)
-> void
{
    if (nullptr == _ptr) return;

    check_pooltag(_ptr, DefaultPoolType, DefaultPoolTag);
    return ExFreePoolWithTag(_ptr, DefaultPoolTag);
}

auto __cdecl operator delete[](void* _ptr, size_t /*_size*/)
-> void
{
    if (nullptr == _ptr) return;

    check_pooltag(_ptr, DefaultPoolType, DefaultPoolTag);
    return ExFreePoolWithTag(_ptr, DefaultPoolTag);
}

// replaceable usual deallocation functions (noexcept)
auto __cdecl operator new(size_t _size, const std::nothrow_t& ) noexcept -> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, _size, DefaultPoolTag))
    {
        return ptr;
    }
    return nullptr;
}

auto __cdecl operator new(size_t _size, POOL_TYPE _pool_type, const std::nothrow_t& ) noexcept -> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, DefaultPoolTag))
    {
        return ptr;
    }
    return nullptr;
}

auto __cdecl operator new(size_t _size, POOL_TYPE _pool_type, ULONG _tag, const std::nothrow_t& ) noexcept -> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, _tag))
    {
        return ptr;
    }
    return nullptr;
}

auto __cdecl operator new[](size_t _size, const std::nothrow_t& ) noexcept -> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, _size, DefaultPoolTag))
    {
        return ptr;
    }

    return nullptr;
}

auto __cdecl operator new[](size_t _size, POOL_TYPE _pool_type, const std::nothrow_t& ) noexcept -> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, DefaultPoolTag))
    {
        return ptr;
    }

    return nullptr;
}

auto __cdecl operator new[](size_t _size, POOL_TYPE _pool_type, ULONG _tag, const std::nothrow_t& ) noexcept -> void*
{
    if (0 == _size) _size = 1;

    if (auto ptr = ExAllocatePoolWithTag(_pool_type, _size, _tag))
    {
        return ptr;
    }

    return nullptr;
}

#endif
