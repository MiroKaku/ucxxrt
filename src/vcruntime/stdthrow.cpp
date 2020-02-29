/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      stdthrow.cpp
 * DATA:      2020/02/28
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 // Copyright (c) Microsoft Corporation.
 // SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 //
 // exception handling support functions


#if __has_include(<wdm.h>)

namespace std
{
    [[noreturn]] void __cdecl _Xbad_alloc() { // report a bad_alloc error
        throw bad_alloc{};
    }

#if __has_include(<stdexcept>)
    [[noreturn]] void __cdecl _Xinvalid_argument(
        _In_z_ const char* _Message) { // report an invalid_argument error
        throw invalid_argument(_Message);
    }

    [[noreturn]] void __cdecl _Xlength_error(
        _In_z_ const char* _Message) { // report a length_error
        throw length_error(_Message);
    }

    [[noreturn]] void __cdecl _Xout_of_range(
        _In_z_ const char* _Message) { // report an out_of_range error
        throw out_of_range(_Message);
    }

    [[noreturn]] void __cdecl _Xoverflow_error(
        _In_z_ const char* _Message) { // report an overflow error
        throw overflow_error(_Message);
    }

    [[noreturn]] void __cdecl _Xruntime_error(
        _In_z_ const char* _Message) { // report a runtime_error
        throw runtime_error(_Message);
    }
#endif

#if __has_include(<functional>)
    [[noreturn]] void __cdecl _Xbad_function_call() { // report a bad_function_call error
        throw bad_function_call{};
    }
#endif

#if __has_include(<regex>)
    [[noreturn]] void __cdecl _Xregex_error(regex_constants::error_type _Code) { // report a regex_error
        throw regex_error(_Code);
    }
#endif
}

#endif
