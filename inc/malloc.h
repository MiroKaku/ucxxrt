/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      malloc.h
 * DATA:      2021/05/08
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once
#include <malloc.h>

_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(_Size)
_ACRTIMP _CRTALLOCATOR _CRT_JIT_INTRINSIC _CRTRESTRICT _CRT_HYBRIDPATCHABLE
void* __cdecl _malloc_pool_tag(
    _In_ _CRT_GUARDOVERFLOW size_t _Size,
    _In_ int _Pool,
    _In_ unsigned long _Tag
);

_ACRTIMP _CRT_HYBRIDPATCHABLE
void __cdecl _free_pool_tag(
    _Pre_maybenull_ _Post_invalid_ void* _Block,
    _In_ int /*_Pool*/,
    _In_ unsigned long _Tag
);
