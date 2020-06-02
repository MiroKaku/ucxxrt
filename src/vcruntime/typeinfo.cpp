/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      typeinfo.cpp
 * DATA:      2020/02/10
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

 //
 // Copyright (c) Microsoft Corporation. All rights reserved.
 //
 // Definitions of the std::type_info implementation functions, used for
 // Run-Time Type Information (RTTI).
 //

#include "include/typeinfo.h"

#if __has_include(<wdm.h>)
namespace ucxxrt
{

    _CRT_BEGIN_C_HEADER;

    _VCRTIMP int __cdecl __std_type_info_compare(
        _In_ const __std_type_info_data* _Lhs,
        _In_ const __std_type_info_data* _Rhs
    )
    {
        if (_Lhs == _Rhs)
        {
            return 0;
        }

        return strcmp(_Lhs->_DecoratedName + 1, _Rhs->_DecoratedName + 1);
    }

    _VCRTIMP size_t __cdecl __std_type_info_hash(
        _In_ const __std_type_info_data* _Data
    )
    {
        // FNV-1a hash function for the undecorated name

#ifdef _WIN64
        static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        size_t const fnv_offset_basis = 14695981039346656037ULL;
        size_t const fnv_prime = 1099511628211ULL;
#else
        static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
        size_t const fnv_offset_basis = 2166136261U;
        size_t const fnv_prime = 16777619U;
#endif

        size_t value = fnv_offset_basis;
        for (char const* it = _Data->_DecoratedName + 1; *it != '\0'; ++it)
        {
            value ^= static_cast<size_t>(static_cast<unsigned char>(*it));
            value *= fnv_prime;
        }

#ifdef _WIN64
        static_assert(sizeof(size_t) == 8, "This code is for 64-bit size_t.");
        value ^= value >> 32;
#else
        static_assert(sizeof(size_t) == 4, "This code is for 32-bit size_t.");
#endif

        return value;
    }

    _CRT_END_C_HEADER

}

type_info::~type_info() noexcept
{

}
#endif
