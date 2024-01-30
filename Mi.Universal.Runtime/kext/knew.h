#pragma once
#include "kmalloc.h"


//
// Defines the user-defined operator new.
//

// user-defined operator new functions
_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR void* __CRTDECL operator new(
    size_t        size,
    pool_t        pool_type,
    unsigned long tag
);

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR void* __CRTDECL operator new[](
    size_t        size,
    pool_t        pool_type,
    unsigned long tag
);

// user-defined operator deallocation functions
void __CRTDECL operator delete(
    void*         block,
    pool_t        pool_type,
    unsigned long tag
) noexcept;

void __CRTDECL operator delete[](
    void*         block,
    pool_t        pool_type,
    unsigned long tag
) noexcept;
