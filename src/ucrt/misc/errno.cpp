//
// errno.cpp
//
//      Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Defines _errno, _doserrno, and related functions
//
#include <internal_shared.h>
#include <corecrt_internal.h>
#include <vcstartup_internal.h>
#include <errno.h>

_CRT_BEGIN_C_HEADER

// This is the error table that defines the mapping between OS error codes and
// errno values.
namespace
{
    struct errentry
    {
        long oscode;    // OS return value
        int errnocode;  // System V error code
    };
}

static errentry const errtable[]
{
    { STATUS_NOT_IMPLEMENTED,       EINVAL    },  //    1
    { STATUS_OBJECT_NAME_NOT_FOUND, ENOENT    },  //    2
    { STATUS_OBJECT_PATH_NOT_FOUND, ENOENT    },  //    3
    { STATUS_TOO_MANY_OPENED_FILES, EMFILE    },  //    4
    { STATUS_ACCESS_DENIED,         EACCES    },  //    5
    { STATUS_INVALID_HANDLE,        EBADF     },  //    6
    { STATUS_BUFFER_OVERFLOW,       ENOMEM    },  //    7
    { STATUS_NO_MEMORY,             ENOMEM    },  //    8
    { STATUS_INSUFFICIENT_RESOURCES,ENOMEM    },  //    9
    { STATUS_IMAGE_SUBSYSTEM_NOT_PRESENT, E2BIG     },  //   10
    { STATUS_INVALID_IMAGE_FORMAT,  ENOEXEC   },  //   11
    { STATUS_ACCESS_VIOLATION,      EINVAL    },  //   12
    { STATUS_DATA_ERROR,            EINVAL    },  //   13
    { STATUS_VOLMGR_DISK_INVALID,   ENOENT    },  //   15
    { STATUS_DIRECTORY_NOT_RM,      EACCES    },  //   16
    { STATUS_NOT_SAME_DEVICE,       EXDEV     },  //   17
    { STATUS_NO_MORE_FILES,         ENOENT    },  //   18
    { STATUS_SHARING_VIOLATION,     EACCES    },  //   33
    { STATUS_BAD_NETWORK_PATH,      ENOENT    },  //   53
    { STATUS_NETWORK_ACCESS_DENIED, EACCES    },  //   65
    { STATUS_BAD_NETWORK_NAME,      ENOENT    },  //   67
    { STATUS_OBJECT_NAME_EXISTS,    EEXIST    },  //   80
    { STATUS_CANNOT_MAKE,           EACCES    },  //   82
    { STATUS_FAIL_FAST_EXCEPTION,   EACCES    },  //   83
    { STATUS_INVALID_PARAMETER,     EINVAL    },  //   87
    { STATUS_CHILD_PROCESS_BLOCKED, EAGAIN    },  //   89
    { STATUS_PIPE_BROKEN,           EPIPE     },  //  109
    { STATUS_DISK_FULL,             ENOSPC    },  //  112
    { STATUS_DIRECTORY_NOT_EMPTY,   ENOTEMPTY },  //  145
    { STATUS_NOT_LOCKED,            EACCES    },  //  158
    { STATUS_OBJECT_NAME_INVALID,   ENOENT    },  //  161
    { STATUS_TOO_MANY_THREADS,      EAGAIN    },  //  164
};

// Number of elements in the table
#define ERRTABLECOUNT (sizeof(errtable) / sizeof(errtable[0]))

int __cdecl __acrt_errno_from_os_error(long const oserrno);

// These map Windows error codes into errno error codes
void __cdecl __acrt_errno_map_os_error(long const oserrno)
{
    _doserrno = oserrno;
    errno     = __acrt_errno_from_os_error(oserrno);
}

int __cdecl __acrt_errno_from_os_error(long const oserrno)
{
    // Check the table for the OS error code
    for (unsigned i{0}; i < ERRTABLECOUNT; ++i)
    {
        if ((long)oserrno == errtable[i].oscode)
            return errtable[i].errnocode;
    }

    // The error code wasn't in the table.  We check for a range of
    // EACCES errors or exec failure errors (ENOEXEC).  Otherwise
    // EINVAL is returned.

    return EINVAL;
}

// These safely set and get the value of the calling thread's errno
errno_t __cdecl _UCXXRT__set_errno(_In_ int const value)
{
    errno = value;
    return 0;
}

errno_t __cdecl _UCXXRT__get_errno(_Out_ int* const result)
{
    _VALIDATE_RETURN_NOERRNO(result != nullptr, EINVAL);

    // Unlike most of our globals, this one is guaranteed to give some answer
    *result = errno;
    return 0;
}

_VCRT_DECLARE_ALTERNATE_NAME(_set_errno, _UCXXRT__set_errno);
_VCRT_DECLARE_ALTERNATE_NAME(_get_errno, _UCXXRT__get_errno);

// These safely set and get the value of the calling thread's doserrno
errno_t __cdecl _set_doserrno(_In_ unsigned long const value)
{
    _doserrno = value;
    return 0;
}

errno_t __cdecl _get_doserrno(_Out_ unsigned long* const result)
{
    _VALIDATE_RETURN_NOERRNO(result != nullptr, EINVAL);

    // Unlike most of our globals, this one is guaranteed to give some answer:
    *result = _doserrno;
    return 0;
}


// These return pointers to the calling thread's errno and doserrno values,
// respectively, and are used to implement errno and _doserrno in the header.
static int  errno_no_memory   { ENOMEM };
static long doserrno_no_memory{ STATUS_NO_MEMORY };

int* __cdecl _errno()
{
    __acrt_ptd* const ptd{ __acrt_getptd_noexit() };
    if (!ptd)
        return &errno_no_memory;

    return &ptd->_terrno;
}

unsigned long* __cdecl __doserrno()
{
    __acrt_ptd* const ptd{ __acrt_getptd_noexit() };
    if (!ptd)
        return (unsigned long*) & doserrno_no_memory;

    return &ptd->_tdoserrno;
}

_CRT_END_C_HEADER
