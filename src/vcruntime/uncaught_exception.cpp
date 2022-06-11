/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      uncaught_exception.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/***
*uncaught_exception.cpp - Some target-independent helper routines used by the EH frame handler.
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
****/

#include <vcruntime_internal.h>


///////////////////////////////////////////////////////////////////////////////
//
// __uncaught_exception() - Returns true while a throw is in progress, between
//                          the construction of the thrown object and the
//                          initialization of the caught object in a catch
//                          block.
//

EXTERN_C bool __cdecl __uncaught_exception()
{
    RENAME_BASE_PTD(__vcrt_ptd)* const ptd = RENAME_UCXXRT(RENAME_BASE_PTD(__vcrt_getptd_noinit))();
    return ptd ? ptd->_ProcessingThrow > 0 : false;
}
