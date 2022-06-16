//
// free_base.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Implementation of _free_base().  This is defined in a different source file
// from the free() function to allow free() to be replaced by the user.
//
#include <corecrt_internal.h>
#include <malloc.h>



// This function implements the logic of free().  It is called directly by the
// free() function in the Release CRT, and it is called by the debug heap in the
// Debug CRT.
//
// This function must be marked noinline, otherwise free and
// _free_base will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because free
// needs to support users patching in custom implementations.
extern "C" void __declspec(noinline) __cdecl _free_base(void* const block)
{
    if (block)
    {
        ExFreePoolWithTag(block, __ucxxrt_tag);
    }
}

