/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      message.cpp
 * DATA:      2022/05/22
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

EXTERN_C NTSTATUS NTAPI RtlFindAndFormatMessage(
    _In_        UINT32   Flags,
    _In_opt_    LPCVOID  Source,
    _In_        UINT32   MessageId,
    _In_        UINT32   LanguageId,
    _Out_       LPWSTR   Buffer,
    _Inout_     UINT32*  Size,
    _In_opt_    va_list* Arguments
)
{
    NTSTATUS        Status = STATUS_SUCCESS;
    PVOID           AllocatedBuffer = nullptr;

    ANSI_STRING     AnsiMessage{};
    UNICODE_STRING  UnicodeMessage{};

    do
    {
        /* If this is a Win32 error wrapped as an OLE HRESULT then unwrap it */
        if (((MessageId & 0xffff0000) == 0x80070000) &&
             BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_SYSTEM) &&
            !BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_HMODULE) &&
            !BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_STRING))
        {
            MessageId &= 0x0000ffff;
        }

        if (Buffer == nullptr)
        {
            Status = STATUS_INVALID_PARAMETER;
            break;
        }

        if (Flags & FORMAT_MESSAGE_ALLOCATE_BUFFER)
        {
            *(PVOID*)Buffer = nullptr;
        }

        PVOID DllHandle     = nullptr;
        ULONG MaximumWidth  = 0ul;
        PWSTR MessageFormat = nullptr;
        PMESSAGE_RESOURCE_ENTRY MessageEntry = nullptr;

        __try
        {
            PVOID BaseDllHandle = ucxxrt::PsSystemDllBase;

            MaximumWidth = Flags & FORMAT_MESSAGE_MAX_WIDTH_MASK;
            if (MaximumWidth == FORMAT_MESSAGE_MAX_WIDTH_MASK)
            {
                MaximumWidth = ULONG_MAX;
            }

            if (BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_STRING))
            {
                MessageFormat = (PWSTR)Source;
            }
            else
            {
                if (BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_HMODULE))
                {
                    if (Source == nullptr)
                    {
                        DllHandle = BaseDllHandle;
                    }
                    else
                    {
                        DllHandle = (LPVOID)Source;
                    }
                }
                else if (BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_SYSTEM))
                {
                    DllHandle = BaseDllHandle;
                }
                else
                {
                    Status = STATUS_INVALID_PARAMETER;
                    break;
                }

                Status = RtlFindMessage(
                    DllHandle,
                    PtrToUlong(RT_MESSAGETABLE),
                    LanguageId,
                    MessageId,
                    &MessageEntry);

                if (Status == STATUS_MESSAGE_NOT_FOUND)
                {
                    if (BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_HMODULE) &&
                        BooleanFlagOn(Flags, FORMAT_MESSAGE_FROM_SYSTEM))
                    {
                        DllHandle = BaseDllHandle;
                        ClearFlag(Flags, FORMAT_MESSAGE_FROM_HMODULE);

                        Status = RtlFindMessage(
                            DllHandle,
                            PtrToUlong(RT_MESSAGETABLE),
                            LanguageId,
                            MessageId,
                            &MessageEntry);
                    }
                }

                if (!NT_SUCCESS(Status))
                {
                    break;
                }

                if (!BooleanFlagOn(MessageEntry->Flags, MESSAGE_RESOURCE_UNICODE))
                {
                    RtlInitAnsiString(&AnsiMessage, (PCSZ)MessageEntry->Text);
                    Status = RtlAnsiStringToUnicodeString(&UnicodeMessage, &AnsiMessage, TRUE);
                    if (!NT_SUCCESS(Status))
                    {
                        break;
                    }

                    MessageFormat = UnicodeMessage.Buffer;
                }
                else
                {
                    MessageFormat = (PWSTR)MessageEntry->Text;
                }
            }

            auto WrittenSize            = 256ul;
            bool IgnoreInserts          = BooleanFlagOn(Flags, FORMAT_MESSAGE_IGNORE_INSERTS);
            bool ArgumentsAreAnAnsi     = BooleanFlagOn(Flags, FORMAT_MESSAGE_ARGUMENT_ANSI);
            bool ArgumentsAreAnArray    = BooleanFlagOn(Flags, FORMAT_MESSAGE_ARGUMENT_ARRAY);

            do
            {
                if (AllocatedBuffer)
                {
                    free(AllocatedBuffer);
                }

                AllocatedBuffer = malloc(WrittenSize);
                if (AllocatedBuffer == nullptr)
                {
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                    break;
                }

                Status = RtlFormatMessage(
                    MessageFormat,
                    MaximumWidth,
                    IgnoreInserts,
                    ArgumentsAreAnAnsi,
                    ArgumentsAreAnArray,
                    Arguments,
                    (PWSTR)AllocatedBuffer,
                    WrittenSize,
                    &WrittenSize);
                if (NT_SUCCESS(Status))
                {
                    break;
                }

                if (Status != STATUS_BUFFER_OVERFLOW)
                {
                    break;
                }

                WrittenSize += 256;

            } while (true);

            if (!NT_SUCCESS(Status))
            {
                break;
            }

            if (BooleanFlagOn(Flags, FORMAT_MESSAGE_ALLOCATE_BUFFER))
            {
                *(PVOID*)Buffer = AllocatedBuffer;
                AllocatedBuffer = nullptr;
            }
            else if ((WrittenSize / sizeof(WCHAR)) > *Size)
            {
                Status = STATUS_BUFFER_TOO_SMALL;
                break;
            }
            else
            {
                RtlMoveMemory(Buffer, AllocatedBuffer, WrittenSize);
            }

            *Size = (WrittenSize - sizeof(WCHAR)) / sizeof(WCHAR);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            Status = GetExceptionCode();
            break;
        }

    } while (false);

    free(AllocatedBuffer);
    RtlFreeUnicodeString(&UnicodeMessage);

    return Status;
}
