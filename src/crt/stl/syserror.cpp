// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// system_error message mapping

#include <system_error>

// TRANSITION, MSBuild
// MSBuild has a hard requirement against including the same file in both a DLL and its import lib, so we include
// the import lib .cpp here to make those functions available for internal use by other parts of our DLL.
#ifdef _CRTBLD
#include "syserror_import_lib.cpp"
#endif

namespace {
    using _STD errc;

    struct _Win_errtab_t { // maps Windows error to Posix error
        int _Windows;
        errc _Posix;
    };

    constexpr _Win_errtab_t _Win_errtab[] = {
        // table of Windows/Posix pairs
        {STATUS_ACCESS_DENIED, errc::permission_denied},
        {STATUS_OBJECT_NAME_EXISTS, errc::file_exists},
        {STATUS_BAD_NETWORK_PATH, errc::no_such_file_or_directory},
        {STATUS_BAD_DEVICE_TYPE, errc::no_such_device},
        {STATUS_PIPE_BROKEN, errc::broken_pipe},
        {STATUS_BUFFER_OVERFLOW, errc::filename_too_long},
        {STATUS_RESOURCE_IN_USE, errc::device_or_resource_busy},
        {STATUS_DEVICE_BUSY, errc::device_or_resource_busy},
        {STATUS_CANNOT_MAKE, errc::permission_denied},
        {STATUS_IO_DEVICE_ERROR, errc::io_error},
        {STATUS_REGISTRY_IO_FAILED, errc::io_error},
        {STATUS_DIRECTORY_NOT_EMPTY, errc::permission_denied},
        {STATUS_DEVICE_DOES_NOT_EXIST, errc::no_such_device},
        {STATUS_DIRECTORY_NOT_EMPTY, errc::directory_not_empty},
        {STATUS_OBJECT_NAME_INVALID, errc::invalid_argument},
        {STATUS_DISK_FULL, errc::no_space_on_device},
        {STATUS_OBJECT_NAME_NOT_FOUND, errc::no_such_file_or_directory},
        {STATUS_INVALID_ADDRESS, errc::permission_denied},
        {STATUS_INVALID_DEVICE_REQUEST, errc::no_such_device},
        {STATUS_NOT_IMPLEMENTED, errc::function_not_supported},
        {STATUS_INVALID_HANDLE, errc::invalid_argument},
        {STATUS_INVALID_PARAMETER, errc::invalid_argument},
        {STATUS_MUTANT_NOT_OWNED, errc::no_lock_available},
        {STATUS_ACCESS_VIOLATION, errc::permission_denied},
        {STATUS_INSUFFICIENT_RESOURCES, errc::not_enough_memory},
        {STATUS_DEVICE_NOT_READY, errc::resource_unavailable_try_again},
        {STATUS_NOT_SAME_DEVICE, errc::cross_device_link},
        {STATUS_NOT_SUPPORTED, errc::not_supported},
        {STATUS_OBJECT_PATH_NOT_FOUND, errc::no_such_file_or_directory},
        {STATUS_IO_REPARSE_TAG_INVALID, errc::invalid_argument},
        {STATUS_RETRY, errc::resource_unavailable_try_again},
        {STATUS_SHARING_VIOLATION, errc::permission_denied},
        {STATUS_TOO_MANY_OPENED_FILES, errc::too_many_files_open},
        {STATUS_FILE_TOO_LARGE, errc::file_too_large},
        {STATUS_INVALID_IMAGE_FORMAT, errc::executable_format_error},
        {STATUS_INTERRUPTED, errc::interrupted},
        {STATUS_FILE_IS_A_DIRECTORY, errc::is_a_directory},
        {STATUS_BUFFER_TOO_SMALL, errc::no_buffer_space},
        {STATUS_NOT_A_DIRECTORY, errc::not_a_directory},
        {STATUS_CANCELLED, errc::operation_canceled},
        {STATUS_OPERATION_IN_PROGRESS, errc::operation_in_progress},
        {STATUS_OPERATION_NOT_SUPPORTED_IN_TRANSACTION, errc::operation_not_supported},
        {STATUS_FILE_LOCKED_WITH_ONLY_READERS, errc::read_only_file_system},
        {STATUS_TIMEOUT, errc::timed_out},
        {STATUS_TOO_MANY_LINKS, errc::too_many_links},
    };

    struct _Sys_errtab_t { // maps error_code to NTBS
        errc _Errcode;
        const char* _Name;
    };

    constexpr _Sys_errtab_t _Sys_errtab[] = {
        // table of Posix code/name pairs
        {errc::broken_pipe, "broken pipe"},
        {errc::cross_device_link, "cross device link"},
        {errc::device_or_resource_busy, "device or resource busy"},
        {errc::directory_not_empty, "directory not empty"},
        {errc::executable_format_error, "executable format error"},
        {errc::file_exists, "file exists"},
        {errc::file_too_large, "file too large"},
        {errc::filename_too_long, "filename too long"},
        {errc::function_not_supported, "function not supported"},
        {errc::interrupted, "interrupted"},
        {errc::invalid_argument, "invalid argument"},
        {errc::io_error, "io error"},
        {errc::is_a_directory, "is a directory"},
        {errc::no_buffer_space, "no buffer space"},
        {errc::no_lock_available, "no lock available"},
        {errc::no_space_on_device, "no space on device"},
        {errc::no_such_device_or_address, "no such device or address"},
        {errc::no_such_device, "no such device"},
        {errc::no_such_file_or_directory, "no such file or directory"},
        {errc::not_a_directory, "not a directory"},
        {errc::not_enough_memory, "not enough memory"},
        {errc::not_supported, "not supported"},
        {errc::operation_canceled, "operation canceled"},
        {errc::operation_in_progress, "operation in progress"},
        {errc::operation_not_permitted, "operation not permitted"},
        {errc::operation_not_supported, "operation not supported"},
        {errc::operation_would_block, "operation would block"},
        {errc::permission_denied, "permission denied"},
        {errc::read_only_file_system, "read only file system"},
        {errc::timed_out, "timed out"},
        {errc::too_many_files_open, "too many files open"},
        {errc::too_many_links, "too many links"},
    };
} // unnamed namespace

_STD_BEGIN

_CRTIMP2_PURE int __CLRCALL_PURE_OR_CDECL _Winerror_map(int _Errcode) {
    // convert Windows error to Posix error if possible, otherwise 0
    for (const auto& _Entry : _Win_errtab) {
        if (_Entry._Windows == _Errcode) {
            return static_cast<int>(_Entry._Posix);
        }
    }

    return 0;
}

// TRANSITION, ABI: _Winerror_message() is preserved for binary compatibility
_CRTIMP2_PURE unsigned long __CLRCALL_PURE_OR_CDECL _Winerror_message(
    unsigned long _Message_id, char* _Narrow, unsigned long _Size) {
    // convert to name of Windows error, return 0 for failure, otherwise return number of chars written
    // pre: _Size < INT_MAX
    const unsigned long _Chars = __vcrt_FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, _Message_id, 0, _Narrow, _Size, nullptr);

    return static_cast<unsigned long>(_CSTD __std_get_string_size_without_trailing_whitespace(_Narrow, _Chars));
}

_CRTIMP2_PURE const char* __CLRCALL_PURE_OR_CDECL _Syserror_map(int _Errcode) { // convert to name of generic error
    for (const auto& _Entry : _Sys_errtab) {
        if (static_cast<int>(_Entry._Errcode) == _Errcode) {
            return _Entry._Name;
        }
    }

    return "unknown error";
}
_STD_END
