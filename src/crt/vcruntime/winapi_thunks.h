 //
 // winapi_thunks.cpp
 //
 //      Copyright (c) Microsoft Corporation. All rights reserved.
 //
 // Definitions of wrappers for Windows API functions that cannot be called
 // by the statically-linked CRT code because they are not part of the MSDK.
 //

#pragma once


extern"C" PVOID __vcrt_EncodePointer(
    _In_ PVOID const ptr
);

extern"C" PVOID __vcrt_DecodePointer(
    _In_ PVOID const ptr
);

extern"C" DWORD __vcrt_FormatMessageA(
    _In_        DWORD   Flags,
    _In_opt_    LPCVOID Source,
    _In_        DWORD   MessageId,
    _In_        DWORD   LanguageId,
    _Out_       LPSTR   Buffer,
    _In_        DWORD   Size,
    _In_opt_    va_list * Arguments
);

extern"C" DWORD __vcrt_FormatMessageW(
    _In_        DWORD    Flags,
    _In_opt_    LPCVOID  Source,
    _In_        DWORD    MessageId,
    _In_        DWORD    LanguageId,
    _Out_       LPWSTR   Buffer,
    _In_        DWORD    Size,
    _In_opt_    va_list * Arguments
);
