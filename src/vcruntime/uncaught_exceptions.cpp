/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      uncaught_exceptions.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/***
*uncaught_exceptions.cpp - Some target-independent helper routines used by the EH frame handler.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
****/

#include <vcruntime_internal.h>


///////////////////////////////////////////////////////////////////////////////
//
// __uncaught_exceptions() - Returns the number of exceptions that have been
//                           thrown but not yet caught.
//
// If a thread/fiber does not yet have thread/fiber locals, these functions
// just return false/0, without trying to allocate/initialize thread/fiber locals.
//

EXTERN_C int __cdecl __uncaught_exceptions()
{
    RENAME_BASE_PTD(__vcrt_ptd)* const ptd = RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd_noinit))();
    return ptd ? ptd->_ProcessingThrow : 0;
}
