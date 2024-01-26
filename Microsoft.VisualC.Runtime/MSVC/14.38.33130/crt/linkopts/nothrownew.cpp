//
// nothrownew.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Defines C++ new routines that return nullptr on failure instead of throwing.
//
#include <vcstartup_internal.h>
#include <malloc.h>
#include <new.h>

#pragma warning(push)
#pragma warning(disable: 6387 28196) // TRANSITION, VSO-1802891

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* operator new(size_t size)
{
    for (;;)
    {
        if (void* const block = malloc(size))
            return block;

        if (_callnewh(size) == 0)
            return nullptr;

        // The new handler was successful; try to allocate again...
    }
}

#pragma warning(pop)

_NODISCARD _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* operator new[](size_t size)
{
    return operator new(size);
}

_NODISCARD _Check_return_ _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* operator new(_In_ size_t size, _In_ int block_use, _In_z_ char const* file_name, _In_ int line_number)
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator new(size);
}

_NODISCARD _Check_return_ _Ret_notnull_ _Post_writable_byte_size_(size) _VCRT_ALLOCATOR
void* operator new[](_In_ size_t size, _In_ int block_use, _In_z_ char const* file_name, _In_ int line_number)
{
    UNREFERENCED_PARAMETER(block_use);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);

    return operator new[](size);
}
