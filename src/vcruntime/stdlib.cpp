/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      stdlib.cpp
 * DATA:      2020/02/28
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include "vcruntime/ehdata_values.h"

#if __has_include(<wdm.h>)

extern ULONG     DefaultPoolTag;
extern POOL_TYPE DefaultPoolType;

_CRTIMP _CRTNOALIAS void   __cdecl free(_Pre_maybenull_ _Post_invalid_ void* _Memory)
{
    ExFreePoolWithTag(_Memory, DefaultPoolTag);
}
_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(_Size) _CRTIMP _CRT_JIT_INTRINSIC _CRTNOALIAS _CRTRESTRICT void* __cdecl malloc(_In_ size_t _Size)
{
    return ExAllocatePoolWithTag(DefaultPoolType, _Size, DefaultPoolTag);
}

#endif