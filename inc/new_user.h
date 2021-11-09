/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      new_user.h
 * DATA:      2021/11/09
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 //
 // Defines the user-defined operator new.
 //

#ifdef _KERNEL_MODE

#include <vcruntime_new.h>

// user-defined operator new functions
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool, ULONG tag);
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool);
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool, std::nothrow_t const&) noexcept;
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new(size_t size, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept;
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool, ULONG tag);
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool);
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool, std::nothrow_t const&) noexcept;
_NODISCARD _VCRT_ALLOCATOR void* __CRTDECL operator new[](size_t size, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept;

// user-defined operator deallocation functions
void __CRTDECL operator delete(void* block, POOL_TYPE pool, ULONG tag) noexcept;
void __CRTDECL operator delete(void* block, POOL_TYPE pool) noexcept;
void __CRTDECL operator delete(void* block, POOL_TYPE pool, std::nothrow_t const&) noexcept;
void __CRTDECL operator delete(void* block, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept;
void __CRTDECL operator delete[](void* block, POOL_TYPE pool, ULONG tag) noexcept;
void __CRTDECL operator delete[](void* block, POOL_TYPE pool) noexcept;
void __CRTDECL operator delete[](void* block, POOL_TYPE pool, std::nothrow_t const&) noexcept;
void __CRTDECL operator delete[](void* block, POOL_TYPE pool, ULONG tag, std::nothrow_t const&) noexcept;

#endif
