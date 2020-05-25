/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      new.h
 * DATA:      2020/02/28
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once

#if !__has_include(<wdm.h>)

#include <new>

#else

#undef _MSC_EXTENSIONS
#include <new.h>

 // disable tag warning
#pragma warning(push)
#pragma warning(disable: 28251)

// replaceable usual deallocation functions
auto __cdecl operator new (size_t _size)
    -> void*;
auto __cdecl operator new (size_t _size, POOL_TYPE _pool_type)
    -> void*;
auto __cdecl operator new (size_t _size, POOL_TYPE _pool_type, ULONG _tag)
-> void*;
auto __cdecl operator new[](size_t _size)
-> void*;
auto __cdecl operator new[](size_t _size, POOL_TYPE _pool_type)
-> void*;
auto __cdecl operator new[](size_t _size, POOL_TYPE _pool_type, ULONG _tag)
-> void*;

auto __cdecl operator delete (void* _ptr) noexcept
-> void;
auto __cdecl operator delete (void* _ptr, POOL_TYPE _pool_type) noexcept
-> void;
auto __cdecl operator delete (void* _ptr, POOL_TYPE _pool_type, ULONG _tag) noexcept
-> void;
auto __cdecl operator delete[](void* _ptr) noexcept
-> void;
auto __cdecl operator delete[](void *_ptr, POOL_TYPE _pool_type) noexcept
-> void;
auto __cdecl operator delete[](void *_ptr, POOL_TYPE _pool_type, ULONG _tag) noexcept
-> void;

// replaceable placement deallocation functions
#pragma warning(push)
#pragma warning(disable: 5043)
auto __cdecl operator new (size_t _size, void* _ptr)
    -> void*;
auto __cdecl operator new[] (size_t _size, void* _ptr)
    -> void*;
#pragma warning(pop)

// T::~T()
auto __cdecl operator delete (void*, void*) noexcept
    -> void;
auto __cdecl operator delete[](void*, void*) noexcept
    -> void;

// sized class - specific deallocation functions
auto __cdecl operator delete  (void* _ptr, size_t _size) noexcept
    -> void;
auto __cdecl operator delete[](void* _ptr, size_t _size) noexcept
    -> void;

// replaceable usual deallocation functions (noexcept)
auto __cdecl operator new (size_t _size, const std::nothrow_t&) noexcept
-> void*;
auto __cdecl operator new (size_t _size, POOL_TYPE _pool_type, const std::nothrow_t&) noexcept
-> void*;
auto __cdecl operator new (size_t _size, POOL_TYPE _pool_type, ULONG _tag, const std::nothrow_t&) noexcept
-> void*;
auto __cdecl operator new[] (size_t _size, const std::nothrow_t&) noexcept
-> void*;
auto __cdecl operator new[] (size_t _size, POOL_TYPE _pool_type, const std::nothrow_t&) noexcept
-> void*;
auto __cdecl operator new[] (size_t _size, POOL_TYPE _pool_type, ULONG _tag, const std::nothrow_t&) noexcept
-> void*;

#pragma warning(pop)

#endif
