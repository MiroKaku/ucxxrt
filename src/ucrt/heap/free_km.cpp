/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      free_km.cpp
 * DATA:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <corecrt_internal.h>
#include <malloc.h>


// This function implements the logic of kfree().  It is called directly by the
// kfree() function in the Release CRT, and it is called by the debug heap in the
// Debug CRT.
//
// This function must be marked noinline, otherwise free and
// _kfree_base will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because free
// needs to support users patching in custom implementations.
extern "C" void __declspec(noinline) __cdecl _kfree_base(void* const block, unsigned long tag)
{
    if (block)
    {
        ExFreePoolWithTag(block, tag);
    }
}


#ifdef _DEBUG
// This function must be marked noinline, otherwise kfree and
// _kfree_dbg will have identical COMDATs, and the linker will fold
// them when calling one from the CRT. This is necessary because free
// needs to support users patching in custom implementations.
extern "C" __declspec(noinline) void __cdecl _kfree_dbg(void* const block, unsigned long tag, int const block_use)
{
    UNREFERENCED_PARAMETER(block_use);

    return _kfree_base(block, tag);
}
#endif


// Frees a block in the heap.  The 'block' pointer must either be a null pointer
// or must point to a valid block in the heap.
//
// This function supports patching and therefore must be marked noinline.
// Both _kfree_dbg and _kfree_base must also be marked noinline
// to prevent identical COMDAT folding from substituting calls to free
// with either other function or vice versa.
extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void __cdecl kfree(void* const block, unsigned long tag)
{
#ifdef _DEBUG
    _kfree_dbg(block, tag, _NORMAL_BLOCK);
#else
    _kfree_base(block, tag);
#endif
}
