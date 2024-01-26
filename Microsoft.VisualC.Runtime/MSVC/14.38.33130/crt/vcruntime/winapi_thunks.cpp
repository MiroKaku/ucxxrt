//
// winapi_thunks.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of wrappers for Windows API functions that cannot be called
// by the statically-linked CRT code because they are not part of the MSDK.
//
#include <vcruntime_internal.h>

#if defined _CRT_APP && !defined _DEBUG
    #define USE_ONLY_MSDK_APIS
#endif

extern "C" _Success_(return != 0) _Ret_range_(1, buffer_count) DWORD __cdecl __vcrt_GetModuleFileNameW(
    _In_opt_ HMODULE const module_handle,
    _Out_writes_to_(buffer_count, ((return < buffer_count) ? (return + 1) : buffer_count)) LPWSTR  const buffer,
    _In_ DWORD   const buffer_count
    )
{
    #ifdef USE_ONLY_MSDK_APIS

    UNREFERENCED_PARAMETER(module_handle);
    UNREFERENCED_PARAMETER(buffer);
    UNREFERENCED_PARAMETER(buffer_count);

    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;

    #else // ^^^ MSDK ^^^ // vvv !MSDK vvv //

    return GetModuleFileNameW(module_handle, buffer, buffer_count);

    #endif // !MSDK
}

extern "C"
#if defined(_CRT_APP) && !defined(_DEBUG)
    _Ret_null_
#else // ^^^ MSDK-constrained ^^^ // vvv GetModuleHandleW is available vvv //
    _When_(file_name == NULL, _Ret_notnull_)
    _When_(file_name != NULL, _Ret_maybenull_)
#endif // ^^^ GetModuleHandleW is available ^^^ //
HMODULE __cdecl __vcrt_GetModuleHandleW(
    _In_opt_ LPCWSTR const file_name
    )
{
    #ifdef USE_ONLY_MSDK_APIS

    UNREFERENCED_PARAMETER(file_name);

    SetLastError(ERROR_NOT_SUPPORTED);
    return nullptr;

    #else // ^^^ MSDK ^^^ // vvv !MSDK vvv //

    return GetModuleHandleW(file_name);

    #endif // !MSDK
}

extern "C" _Ret_maybenull_ HMODULE __cdecl __vcrt_LoadLibraryExW(
    _In_ LPCWSTR const file_name,
    _Reserved_ HANDLE  const file_handle,
    _In_ DWORD   const flags
    )
{
    #ifdef USE_ONLY_MSDK_APIS

    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(file_handle);
    UNREFERENCED_PARAMETER(flags);

    SetLastError(ERROR_NOT_SUPPORTED);
    return nullptr;

    #else // ^^^ MSDK ^^^ // vvv !MSDK vvv //

    return LoadLibraryExW(file_name, file_handle, flags);

    #endif // !MSDK
}
