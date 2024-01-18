/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      stdalloc.cpp
 * DATE:      2022/11/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_internal.h>


extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl std_malloc(size_t const size)
{
    #pragma warning(suppress: 4996)
    return ExAllocatePoolWithTag(NonPagedPool, size, __ucxxrt_tag);
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void __cdecl std_free(void* const block)
{
    if (block)
    {
        ExFreePoolWithTag(block, __ucxxrt_tag);
    }
}

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) size_t __cdecl std_msize(void* const block)
{
    size_t  size = 0;
    BOOLEAN quota_charged = FALSE;

    // Validation section
    _VALIDATE_RETURN(block != nullptr, EINVAL, static_cast<size_t>(-1));

    size = ExQueryPoolBlockSize(block, &quota_charged);
    if (size == PAGE_SIZE && PAGE_ALIGN(block))
    {
        // NOTE: If the entry is bigger than a page, the value PAGE_SIZE is returned
        //       rather than the correct number of bytes.

        size = 0;
        KdBreakPoint();
    }

    return size;
}
