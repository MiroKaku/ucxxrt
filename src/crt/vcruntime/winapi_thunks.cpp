 //
 // winapi_thunks.cpp
 //
 //      Copyright (c) Microsoft Corporation. All rights reserved.
 //
 // Definitions of wrappers for Windows API functions that cannot be called
 // by the statically-linked CRT code because they are not part of the MSDK.
 //
#include <vcruntime_internal.h>
#include "winapi_thunks.h"

extern"C" NTSTATUS NTAPI RtlFindAndFormatMessage(
    _In_        UINT32   Flags,
    _In_opt_    LPCVOID  Source,
    _In_        UINT32   MessageId,
    _In_        UINT32   LanguageId,
    _Out_       LPWSTR   Buffer,
    _Inout_     UINT32* Size,
    _In_opt_    va_list* Arguments
);

extern"C" PVOID __vcrt_EncodePointer(
    _In_ PVOID const ptr
)
{
    return __crt_fast_encode_pointer(ptr);
}

extern"C" PVOID __vcrt_DecodePointer(
    _In_ PVOID const ptr
)
{
    return __crt_fast_decode_pointer(ptr);
}

extern"C" DWORD __vcrt_FormatMessageA(
    _In_        DWORD   Flags,
    _In_opt_    LPCVOID Source,
    _In_        DWORD   MessageId,
    _In_        DWORD   LanguageId,
    _Out_       LPSTR   Buffer,
    _In_        DWORD   Size,
    _In_opt_    va_list * Arguments
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    UINT32   WrittenSize = Size;
    PWSTR    UnicodeSource = nullptr;
    PWSTR    UnicodeBuffer = nullptr;

    ANSI_STRING     AnsiString{};
    UNICODE_STRING  UnicodeString{};

    do
    {
        if (BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_STRING))
        {
            if (strlen((LPCSTR)Source) >= MAXSHORT)
            {
                Status = STATUS_INVALID_PARAMETER;
                break;
            }
            else
            {
                RtlInitAnsiString(&AnsiString, (LPCSTR)Source);
                Status = RtlAnsiStringToUnicodeString(&UnicodeString, &AnsiString, TRUE);
                if (!NT_SUCCESS(Status))
                {
                    break;
                }

                UnicodeSource = UnicodeString.Buffer;
            }
        }
        else
        {
            UnicodeSource = (LPWSTR)Source;
        }

        if (BooleanFlagOn(Flags, FORMAT_MESSAGE_ALLOCATE_BUFFER))
        {
            UnicodeBuffer = (LPWSTR)Buffer;
        }
        else
        {
            UnicodeBuffer = (LPWSTR)malloc(Size * sizeof(wchar_t));
        }

        if (UnicodeBuffer == nullptr)
        {
            Status = STATUS_NO_MEMORY;
            break;
        }

        SetFlag(Flags, FORMAT_MESSAGE_ARGUMENT_ANSI);

        Status = RtlFindAndFormatMessage(
            Flags,
            UnicodeSource,
            MessageId,
            LanguageId,
            UnicodeBuffer,
            &WrittenSize,
            Arguments);

        RtlFreeUnicodeString(&UnicodeString);

        if (!NT_SUCCESS(Status))
        {
            if (BooleanFlagOn(Flags, FORMAT_MESSAGE_ALLOCATE_BUFFER))
            {
                UnicodeBuffer = nullptr;
            }

            break;
        }

        if (WrittenSize >= MAXSHORT)
        {
            if (BooleanFlagOn(Flags, FORMAT_MESSAGE_ALLOCATE_BUFFER))
            {
                UnicodeBuffer = *(LPWSTR*)Buffer;
                *(LPWSTR*)Buffer = nullptr;
            }

            Status = STATUS_INVALID_PARAMETER;
            break;
        }

        if (BooleanFlagOn(Flags, FORMAT_MESSAGE_ALLOCATE_BUFFER))
        {
            UnicodeBuffer = *(LPWSTR*)Buffer;

            UnicodeString.Buffer = UnicodeBuffer;
            UnicodeString.Length = (USHORT)(WrittenSize * sizeof(WCHAR));
            UnicodeString.MaximumLength = (USHORT)(UnicodeString.Length + sizeof(UNICODE_NULL));

            Status = RtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, TRUE);
            if (!NT_SUCCESS(Status))
            {
                *(LPSTR*)Buffer = nullptr;
                break;
            }

            *(LPSTR*)Buffer = AnsiString.Buffer;
        }
        else
        {
            UnicodeString.Buffer = UnicodeBuffer;

            RtlInitEmptyAnsiString(&AnsiString, Buffer, (USHORT)Size);
            Status = RtlUnicodeStringToAnsiString(&AnsiString, &UnicodeString, FALSE);
            if (!NT_SUCCESS(Status))
            {
                break;
            }
        }

        WrittenSize = AnsiString.Length;

    } while (false);

    if (UnicodeBuffer)
    {
        free(UnicodeBuffer);
    }

    _set_doserrno(Status);
    return WrittenSize;
}

extern"C" DWORD __vcrt_FormatMessageW(
    _In_        DWORD    Flags,
    _In_opt_    LPCVOID  Source,
    _In_        DWORD    MessageId,
    _In_        DWORD    LanguageId,
    _Out_       LPWSTR   Buffer,
    _In_        DWORD    Size,
    _In_opt_    va_list * Arguments
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    UINT32   WrittenSize = Size;

    do
    {
        if (BooleanFlagOn(Flags, FORMAT_MESSAGE_ARGUMENT_ANSI))
        {
            Status = STATUS_INVALID_PARAMETER;
            break;
        }

        Status = RtlFindAndFormatMessage(
            Flags,
            Source,
            MessageId,
            LanguageId,
            Buffer,
            &WrittenSize,
            Arguments);

    } while (false);

    _set_doserrno(Status);
    return WrittenSize;
}
