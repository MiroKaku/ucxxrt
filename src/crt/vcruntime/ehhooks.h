/***
*ehhook.h - declaration of exception handling hook variables
*
*       Copyright (c) 1993-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Declaration of E.H. hook variables
*
*       [Internal]
*
*Revision History:
*       05-26-93  BS    Module created.
*       09-02-94  SKS   This header file added.
*       12-15-94  XY    merged with mac header
*       02-14-95  CFW   Clean up Mac merge.
*       03-29-95  CFW   Add error message to internal headers.
*       12-14-95  JWM   Add "#pragma once".
*       02-24-97  GJF   Detab-ed.
*       05-17-99  PML   Remove all Macintosh support.
*
****/

#pragma once

#include <vcruntime_internal.h>



/* Thread specific pointers to handler functions */

#define __pUnexpected    (*((unexpected_function*) &(__vcrt_getptd()->_unexpected)))
#define __pSETranslator  (*((_se_translator_function*) &(__vcrt_getptd()->_translator)))

//
// This filter needs to be dragged in for any exe which has any eh.
//
// long NTAPI __CxxUnhandledExceptionFilter( EXCEPTION_POINTERS* );

