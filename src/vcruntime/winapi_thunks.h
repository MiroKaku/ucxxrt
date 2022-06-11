/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      winapi_thunks.h
 * DATA:      2022/02/27
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 //
 // winapi_thunks.cpp
 //
 //      Copyright (c) Microsoft Corporation. All rights reserved.
 //
 // Definitions of wrappers for Windows API functions that cannot be called
 // by the statically-linked CRT code because they are not part of the MSDK.
 //

#pragma once

_CRT_BEGIN_C_HEADER

DWORD __vcrt_FormatMessageA(
    _In_        DWORD   Flags,
    _In_opt_    LPCVOID Source,
    _In_        DWORD   MessageId,
    _In_        DWORD   LanguageId,
    _Out_       LPSTR   Buffer,
    _In_        DWORD   Size,
    _In_opt_    va_list* Arguments
);

DWORD __vcrt_FormatMessageW(
    _In_        DWORD    Flags,
    _In_opt_    LPCVOID  Source,
    _In_        DWORD    MessageId,
    _In_        DWORD    LanguageId,
    _Out_       LPWSTR   Buffer,
    _In_        DWORD    Size,
    _In_opt_    va_list* Arguments
);

_CRT_END_C_HEADER
