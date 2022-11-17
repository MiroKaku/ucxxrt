/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      kmalloc.h
 * DATA:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once
#include <malloc.h>


extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl kmalloc(
    _In_ _CRT_GUARDOVERFLOW size_t size,
    _In_ int pool,
    _In_ unsigned long tag
);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline)
void __cdecl kfree(
    _Pre_maybenull_ _Post_invalid_ void* block,
    _In_  unsigned long tag
);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl kcalloc(
    _In_ _CRT_GUARDOVERFLOW size_t const count,
    _In_ _CRT_GUARDOVERFLOW size_t const size,
    _In_ int pool,
    _In_ unsigned long tag
);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT
void* __cdecl krealloc(
    void* const block,
    size_t const size,
    int pool,
    unsigned long tag
);

extern "C" __declspec(noinline) _CRTRESTRICT
void* __cdecl krecalloc(
    void* const block,
    size_t const count,
    size_t const size,
    int pool,
    unsigned long tag
);
