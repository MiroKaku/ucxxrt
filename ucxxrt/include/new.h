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


// replaceable allocation functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count
    );


// replaceable usual deallocation functions
void __CRTDECL operator delete(
    void* ptr
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr
    ) noexcept;

void __CRTDECL operator delete(
    void* ptr,
    size_t count
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr,
    size_t count
    ) noexcept;


// replaceable non-throwing allocation functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    std::nothrow_t const&
    ) noexcept;

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    std::nothrow_t const&
    ) noexcept;


// replaceable placement deallocation functions
void __CRTDECL operator delete(
    void* ptr,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr,
    std::nothrow_t const&
    ) noexcept;


// non-allocating placement allocation/deallocation functions
#pragma warning(push)
#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified
#pragma warning(disable: 4514) // 'operator new': unreferenced inline function has been removed
#ifndef __PLACEMENT_NEW_INLINE
#define __PLACEMENT_NEW_INLINE
_NODISCARD inline void* __CRTDECL operator new(size_t count, void* place) noexcept
{
    (void)count;
    return place;
}

inline void __CRTDECL operator delete(void* ptr, void* place) noexcept
{
    return;
}
#endif

#ifndef __PLACEMENT_VEC_NEW_INLINE
#define __PLACEMENT_VEC_NEW_INLINE
_NODISCARD inline void* __CRTDECL operator new[](size_t count, void* place) noexcept
{
    (void)count;
    return place;
}

inline void __CRTDECL operator delete[](void* ptr, void* place) noexcept
{
}
#endif
#pragma warning(pop)


// user-defined placement allocation functions
// void* operator new  (std::size_t count, user-defined-args...);
// void* operator new[](std::size_t count, user-defined-args...);

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool,
    ULONG tag
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool,
    ULONG tag
    );


// user-defined placement deallocation functions
// void operator delete  (void* ptr, args...);
// void operator delete[](void* ptr, args...);

void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE pool
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE pool
    ) noexcept;

void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE pool,
    ULONG tag
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE pool,
    ULONG tag
    ) noexcept;


// user-defined non-throwing placement allocation functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool,
    std::nothrow_t const&
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool,
    std::nothrow_t const&
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t count,
    POOL_TYPE pool,
    ULONG tag,
    std::nothrow_t const&
    );

_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t count,
    POOL_TYPE pool,
    ULONG tag,
    std::nothrow_t const&
    );


// user-defined non-throwing placement deallocation functions
void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE pool,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE pool,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete(
    void* ptr,
    POOL_TYPE pool,
    ULONG tag,
    std::nothrow_t const&
    ) noexcept;

void __CRTDECL operator delete[](
    void* ptr,
    POOL_TYPE pool,
    ULONG tag,
    std::nothrow_t const&
    ) noexcept;


#pragma warning(pop)
#endif
