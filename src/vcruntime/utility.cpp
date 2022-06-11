/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      utility.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// utility.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Common functionality shared by both the EXE and DLL startup code.
//
#include <internal_shared.h>
#include <limits.h>


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// PE Image Utilities
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// Tests whether a PE image is located at the given image base.  Returns true if
// the given image base potentially points to a loaded PE image; false otherwise.
static bool __cdecl is_potentially_valid_image_base(void* const image_base) noexcept
{
    if (!image_base)
    {
        return false;
    }

    auto const dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);
    if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return false;
    }

    auto const nt_header_address = reinterpret_cast<PBYTE>(dos_header) + dos_header->e_lfanew;
    auto const nt_header         = reinterpret_cast<PIMAGE_NT_HEADERS>(nt_header_address);
    if (nt_header->Signature != IMAGE_NT_SIGNATURE)
    {
        return false;
    }

    auto const optional_header = &nt_header->OptionalHeader;
    if (optional_header->Magic != IMAGE_NT_OPTIONAL_HDR_MAGIC)
    {
        return false;
    }

    return true;
}



// Given an RVA, finds the PE section in the pointed-to image that includes the
// RVA.  Returns null if no such section exists or the section is not found.
static PIMAGE_SECTION_HEADER __cdecl find_pe_section(unsigned char* const image_base, uintptr_t const rva) noexcept
{
    auto const dos_header        = reinterpret_cast<PIMAGE_DOS_HEADER>(image_base);
    auto const nt_header_address = reinterpret_cast<PBYTE>(dos_header) + dos_header->e_lfanew;
    auto const nt_header         = reinterpret_cast<PIMAGE_NT_HEADERS>(nt_header_address);

    // Find the section holding the RVA.  We make no assumptions here about the
    // sort order of the section descriptors, though they always appear to be
    // sorted by ascending section RVA.
    PIMAGE_SECTION_HEADER const first_section = IMAGE_FIRST_SECTION(nt_header);
    PIMAGE_SECTION_HEADER const last_section  = first_section + nt_header->FileHeader.NumberOfSections;
    for (auto it = first_section; it != last_section; ++it)
    {
        if (rva >= it->VirtualAddress && rva < it->VirtualAddress + it->Misc.VirtualSize)
        {
            return it;
        }
    }

    return nullptr;
}


// Tests whether a target address is located within the current PE image (the
// image located at __ImageBase), that the target address is located in a proper
// section of the image, and that the section in which it is located is not
// writable.
EXTERN_C bool __cdecl __scrt_is_nonwritable_in_current_image(void const* const target)
{
    auto const target_address = reinterpret_cast<unsigned char const*>(target);
    auto const image_base     = reinterpret_cast<unsigned char*>(&__ImageBase);

    __try
    {
        // Make sure __ImageBase is the address of a valid PE image.  This is
        // likely an unnecessary check, since we should be executing in a
        // normal image, but it is fast, this routine is rarely called, and the
        // normal call is for security purposes.  If we don't have a PE image,
        // return failure:
        if (!is_potentially_valid_image_base(image_base))
        {
            return false;
        }

        // Convert the target address to an RVA within the image and find the
        // corresponding PE section.  Return failure if the target address is
        // not found within the current image:
        uintptr_t const rva_target = target_address - image_base;
        PIMAGE_SECTION_HEADER const section_header = find_pe_section(image_base, rva_target);
        if (!section_header)
        {
            return false;
        }

        // Check the section characteristics to see if the target address is
        // located within a writable section, returning a failure if it is:
        if (section_header->Characteristics & IMAGE_SCN_MEM_WRITE)
        {
            return false;
        }

        return true;
    }
    __except (GetExceptionCode() == STATUS_ACCESS_VIOLATION)
    {
        // If any of the above operations failed, assume that we do not have a
        // valid nonwritable address in the current image:
        return false;
    }
}
