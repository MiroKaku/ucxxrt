/***
*gs_cookie.c - defines buffer overrun security cookie
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines per-module global variable __security_cookie, which is used
*       by the /GS compile switch to detect local buffer variable overrun
*       bugs/attacks.
*
*       When compiling /GS, the compiler injects code to detect when a local
*       array variable has been overwritten, potentially overwriting the
*       return address (on machines like x86 where the return address is on
*       the stack).  A local variable is allocated directly before the return
*       address and initialized on entering the function.  When exiting the
*       function, the compiler inserts code to verify that the local variable
*       has not been modified.  If it has, then an error reporting routine
*       is called.
*
*******************************************************************************/

// do not include windows.h as we are intentionally redefining the security
// cookie so that it is on its own cache line
#ifndef _UINTPTR_T_DEFINED
    #define _UINTPTR_T_DEFINED
    #ifdef _WIN64
        typedef unsigned __int64  uintptr_t;
    #else
        typedef unsigned int uintptr_t;
    #endif
#endif

/*
 * Default value used for the global /GS security cookie, defined here and
 * in gs_support.c (since standalone SDK build can't use CRT's internal.h).
 */

#ifdef _WIN64
#define DEFAULT_SECURITY_COOKIE ((uintptr_t)0x00002B992DDFA232)
#else  /* _WIN64 */
#define DEFAULT_SECURITY_COOKIE ((uintptr_t)0xBB40E64E)
#endif  /* _WIN64 */

/*
 * The global security cookie.  This name is known to the compiler.
 * Initialize to a garbage non-zero value just in case we have a buffer overrun
 * in any code that gets run before __security_init_cookie() has a chance to
 * initialize the cookie to the final value.
 */

// vso1820362: We want the security cookie on its own cache line so that it
// doesn't false share with other globals, as it is read a lot. This means we
// need to both align it to a cache line and pad it to cache line size even
// though __security_cookie is only a uintptr_t.
#pragma warning( push )
#pragma warning( disable : 4324 ) // we want it to auto pad the struct based on alignment
#define CACHE_LINE_SIZE 64
union __declspec(align(CACHE_LINE_SIZE)) {
    uintptr_t value;
} __security_cookie = { DEFAULT_SECURITY_COOKIE };
static_assert(sizeof(__security_cookie) == CACHE_LINE_SIZE);
#pragma warning( pop )

uintptr_t __security_cookie_complement = ~(DEFAULT_SECURITY_COOKIE);
