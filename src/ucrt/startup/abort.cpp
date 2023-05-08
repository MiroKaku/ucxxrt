/***
*abort.c - abort a program by raising SIGABRT
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines abort() - print a message and raise SIGABRT.
*
*******************************************************************************/

#include <corecrt_internal.h>
#include <stdlib.h>

#ifdef _DEBUG
#define _INIT_ABORT_BEHAVIOR _WRITE_ABORT_MSG
#else
#define _INIT_ABORT_BEHAVIOR _CALL_REPORTFAULT
#endif

extern "C" unsigned int __abort_behavior = _INIT_ABORT_BEHAVIOR;

/***
*void abort() - abort the current program by raising SIGABRT
*
*Purpose:
*   print out an abort message and raise the SIGABRT signal.  If the user
*   hasn't defined an abort handler routine, terminate the program
*   with exit status of 3 without cleaning up.
*
*   Multi-thread version does not raise SIGABRT -- this isn't supported
*   under multi-thread.
*
*Entry:
*   None.
*
*Exit:
*   Does not return.
*
*Uses:
*
*Exceptions:
*
*******************************************************************************/

extern "C" void __cdecl _UCXXRT__abort()
{
#ifdef _DEBUG
    if (__abort_behavior & _WRITE_ABORT_MSG)
    {
        DbgPrintEx(DPFLTR_DEFAULT_ID, DPFLTR_ERROR_LEVEL,
            "Microsoft Visual C++ Runtime Library\n"
            "\n"
            "Runtime Error!\n"
            "\n"
            "abort() has been called\n");
    }
#endif

    __fastfail(FAST_FAIL_FATAL_APP_EXIT);
}


/***
*unsigned int _set_abort_behavior(unsigned int, unsigned int) - set the behavior on abort
*
*Purpose:
*
*Entry:
*   unsigned int flags - the flags we want to set
*   unsigned int mask - mask the flag values
*
*Exit:
*   Return the old behavior flags
*
*Exceptions:
*   None
*
*******************************************************************************/

extern "C" unsigned int __cdecl _set_abort_behavior(unsigned int flags, unsigned int mask)
{
    unsigned int oldflags = __abort_behavior;
    __abort_behavior = oldflags & (~mask) | flags & mask;
    return oldflags;
}
