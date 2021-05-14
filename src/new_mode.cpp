/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      new_mode.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// new_mode.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definition of _set_new_mode and _query_new_mode, which provide access to the
// _newmode global flag.
//

#include <new.h>
#include <internal_shared.h>


static volatile long __acrt_global_new_mode;


// Sets the _newmode flag to the new value 'mode' and return the old mode.
EXTERN_C int __cdecl _set_new_mode(int const mode)
{
    // The only valid values of _newmode are 0 and 1:
    _VALIDATE_RETURN(mode == 0 || mode == 1, EINVAL, -1);

    return static_cast<int>(_InterlockedExchange(&__acrt_global_new_mode, mode));
}

// Gets the current value of the _newmode flag.
EXTERN_C int __cdecl _query_new_mode()
{
    return static_cast<int>(__crt_interlocked_read(&__acrt_global_new_mode));
}
