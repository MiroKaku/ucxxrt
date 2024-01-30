#pragma once
#include <malloc.h>


using pool_t = POOL_TYPE;

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl kmalloc(
    _In_ _CRT_GUARDOVERFLOW size_t size,
    _In_ pool_t                    pool,
    _In_ unsigned long             tag
);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void __cdecl kfree(
    _Pre_maybenull_ _Post_invalid_ void* block,
    _In_ unsigned long                   tag
);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl kcalloc(
    _In_ _CRT_GUARDOVERFLOW size_t count,
    _In_ _CRT_GUARDOVERFLOW size_t size,
    _In_ pool_t                    pool,
    _In_ unsigned long             tag
);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl krealloc(
    void*         block,
    size_t        old_size,
    size_t        new_size,
    pool_t        pool,
    unsigned long tag
);

extern "C" __declspec(noinline) _CRTRESTRICT void* __cdecl krecalloc(
    void*         block,
    size_t        count,
    size_t        old_size,
    size_t        new_size,
    pool_t        pool,
    unsigned long tag
);
