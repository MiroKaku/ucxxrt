/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      new.cpp
 * DATA:      2020/02/05
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <new>
#include <stdexcept>

#ifdef _KERNEL_MODE

// global
namespace ucxxrt
{
    ULONG       DefaultPoolTag       = _ByteSwap32('ucrt');
    POOL_TYPE   DefaultPoolType      = POOL_TYPE::NonPagedPoolNx;
    ULONG       DefaultMdlProtection = MdlMappingNoExecute;
}

using namespace ucxxrt;

// replaceable allocation functions
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size)  _VCRT_ALLOCATOR
void* __CRTDECL operator new(
    size_t count
    )
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, count, DefaultPoolTag))
        return ptr;

    std::_Xbad_alloc();
}

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](
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
_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new(
    size_t count,
    ::std::nothrow_t const&
    ) noexcept
{
    if (count == 0)
        count = 1;

    if (auto ptr = ExAllocatePoolWithTag(DefaultPoolType, count, DefaultPoolTag))
        return ptr;

    return nullptr;
}

_NODISCARD _Ret_maybenull_ _Success_(return != NULL) _Post_writable_byte_size_(_Size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](
    size_t count,
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePool(ptr);
    }
}

void __CRTDECL operator delete[](
    void* ptr,
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
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
    ::std::nothrow_t const&
    ) noexcept
{
    if (ptr)
    {
        ExFreePoolWithTag(ptr, tag);
    }
}

#endif