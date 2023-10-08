/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      kfree.cpp
 * DATE:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_internal.h>
#include <malloc.h>


// Frees a block in the heap.  The 'block' pointer must either be a null pointer
// or must point to a valid block in the heap.
//
// This function supports patching and therefore must be marked noinline.
// Both _kfree_dbg and _kfree_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to free
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void __cdecl kfree(void* const block, unsigned long tag)
{
    if (block)
    {
        ExFreePoolWithTag(block, tag);
    }
}
