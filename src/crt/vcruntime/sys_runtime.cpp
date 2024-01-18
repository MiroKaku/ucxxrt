/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      sys_runtime.cpp
 * DATE:      2022/05/22
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

namespace ucxxrt
{
    extern PVOID PsSystemDllBase = nullptr;

    extern constexpr UNICODE_STRING NtdllKnownDllName = RTL_CONSTANT_STRING(L"\\KnownDlls\\ntdll.dll");
}

/**
 * get known DLL module extents.
 * ref: https://github.com/processhacker/processhacker/blob/2b70d0a678dc06c330545085daf1f8375487b328/KProcessHacker/main.c#L374
 *
 * \param[in] SectionName  - Name of the section to open.
 * \param[out] BaseAddress - On success, populated with the module base.
 * \param[out] ImageSize   - On success, populated with the module size.
 *
 * \return Appropriate status.
*/
_IRQL_requires_max_(PASSIVE_LEVEL)
EXTERN_C NTSTATUS NTAPI RtlGetKnownDllExtents(
    _In_ PUNICODE_STRING SectionName,
    _Out_ PVOID* BaseAddress,
    _Out_ SIZE_T* ImageSize
)
{
    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE   SectionHandle = nullptr;
    PVOID    SectionObject = nullptr;
    PVOID    MappedBase = nullptr;
    SIZE_T   MappedSize = 0u;

    do
    {
        NT_ASSERT(PsInitialSystemProcess == PsGetCurrentProcess());

        OBJECT_ATTRIBUTES ObjectAttributes;
        InitializeObjectAttributes(
            &ObjectAttributes,
            SectionName,
            OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
            nullptr,
            nullptr);

        Status = ZwOpenSection(
            &SectionHandle,
            SECTION_MAP_READ | SECTION_QUERY,
            &ObjectAttributes);
        if (!NT_SUCCESS(Status))
        {
            break;
        }

        SECTION_IMAGE_INFORMATION SectionImageInfo{};

        Status = ZwQuerySection(
            SectionHandle,
            SectionImageInformation,
            &SectionImageInfo,
            sizeof(SectionImageInfo),
            nullptr);
        if (!NT_SUCCESS(Status))
        {
            break;
        }

        //
        // 21H2 no longer maps ntdll as an image in System. Querying the transfer
        // address to get the extents in this context will fail. Rather than rely
        // on ZwQueryVirtualMemory at all, we'll map it into system space to get
        // the extents.
        //
        // Note, in the future if Microsoft decides to relocate KnownDLLs in
        // every process we'll need to revisit this. That will likely involve
        // retrieving the module extents per-process, in our case this is used
        // for KPH communications validation so we'll need to the ntdll module
        // extents out of PH.
        //

        Status = ObReferenceObjectByHandle(
            SectionHandle,
            SECTION_MAP_READ | SECTION_QUERY,
            *MmSectionObjectType,
            KernelMode,
            &SectionObject,
            nullptr);
        if (!NT_SUCCESS(Status))
        {
            break;
        }

        Status = MmMapViewInSystemSpace(SectionObject, &MappedBase, &MappedSize);
        if (!NT_SUCCESS(Status))
        {
            break;
        }

        *BaseAddress = SectionImageInfo.TransferAddress;
        *ImageSize = MappedSize;

    } while (false);

    if (MappedBase)
    {
        MmUnmapViewInSystemSpace(MappedBase);
    }

    if (SectionObject)
    {
        ObDereferenceObject(SectionObject);
    }

    if (SectionHandle)
    {
        ZwClose(SectionHandle);
    }

    return Status;
}

extern "C" void __cdecl __scrt_initialize_system()
{
    using namespace ucxxrt;

    SIZE_T DllSize = 0u;
    RtlGetKnownDllExtents(const_cast<PUNICODE_STRING>(&NtdllKnownDllName), &PsSystemDllBase, &DllSize);
}
