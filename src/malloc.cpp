/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      malloc.cpp
 * DATA:      2020/02/05
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <malloc.h>

namespace ucxxrt
{
    extern ULONG     DefaultPoolTag;
    extern POOL_TYPE DefaultPoolType;
}

_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(_Size)
_ACRTIMP _CRTALLOCATOR _CRT_JIT_INTRINSIC _CRTRESTRICT _CRT_HYBRIDPATCHABLE
void* __cdecl malloc(
    _In_ _CRT_GUARDOVERFLOW size_t _Size
)
{
    return ExAllocatePoolWithTag(ucxxrt::DefaultPoolType, _Size, ucxxrt::DefaultPoolTag);
}

_ACRTIMP _CRT_HYBRIDPATCHABLE
void __cdecl free(
    _Pre_maybenull_ _Post_invalid_ void* _Block
)
{
    ExFreePoolWithTag(_Block, ucxxrt::DefaultPoolTag);
}
