/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      knew.h
 * DATA:      2022/06/17
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

#include <vcruntime_new.h>

using pool_t = POOL_TYPE;

// user-defined operator new functions
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* __CRTDECL operator new  (size_t size, pool_t pool_type, unsigned long tag);

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* __CRTDECL operator new[](size_t size, pool_t pool_type, unsigned long tag);

// user-defined operator deallocation functions
void __CRTDECL operator delete  (void* block, pool_t pool_type, unsigned long tag) noexcept;
void __CRTDECL operator delete[](void* block, pool_t pool_type, unsigned long tag) noexcept;
