/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      typeinfo.h
 * DATA:      2020/02/08
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

#pragma once


#if __has_include(<vcruntime_typeinfo.h>)

#include <vcruntime_typeinfo.h>

#else

namespace ucxxrt
{

    _CRT_BEGIN_C_HEADER;

    struct __cxx_type_info_data
    {
        const char* _UndecoratedName;
        const char   _DecoratedName[1];
        __cxx_type_info_data() = delete;
        __cxx_type_info_data(const __cxx_type_info_data&) = delete;
        __cxx_type_info_data(__cxx_type_info_data&&) = delete;

        __cxx_type_info_data& operator=(const __cxx_type_info_data&) = delete;
        __cxx_type_info_data& operator=(__cxx_type_info_data&&) = delete;
    };

    _VCRTIMP int __cdecl __cxx_type_info_compare(
        _In_ const __cxx_type_info_data* _Lhs,
        _In_ const __cxx_type_info_data* _Rhs
    );

    _VCRTIMP size_t __cdecl __cxx_type_info_hash(
        _In_ const __cxx_type_info_data* _Data
    );

    _CRT_END_C_HEADER

}


#pragma warning(push)
#pragma warning(disable: 4577) // 'noexcept' used with no exception handling mode specified
class type_info
{
public:

    type_info(const type_info&) = delete;
    type_info& operator=(const type_info&) = delete;

    size_t hash_code() const noexcept
    {
        return ucxxrt::__cxx_type_info_hash(&_Data);
    }

    bool operator==(const type_info& _Other) const noexcept
    {
        return ucxxrt::__cxx_type_info_compare(&_Data, &_Other._Data) == 0;
    }

    bool operator!=(const type_info& _Other) const noexcept
    {
        return ucxxrt::__cxx_type_info_compare(&_Data, &_Other._Data) != 0;
    }

    bool before(const type_info& _Other) const noexcept
    {
        return ucxxrt::__cxx_type_info_compare(&_Data, &_Other._Data) < 0;
    }

    const char* name() const noexcept
    {
        return _Data._DecoratedName; // _Data._UndecoratedName;
    }

    const char* raw_name() const noexcept
    {
        return _Data._DecoratedName;
    }

    virtual ~type_info() noexcept;

private:

    mutable ucxxrt::__cxx_type_info_data _Data;
};
#pragma warning(pop)

namespace std 
{
    using ::type_info;
}
#endif
