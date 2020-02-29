/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      stdexcept.cpp
 * DATA:      2020/02/28
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

#include <exception>

#if !__has_include(<vcruntime_exception.h>)
#if _HAS_EXCEPTIONS

#pragma pack(push, _CRT_PACKING)

exception::exception()
    : _m_what(nullptr)
    , _m_doFree(0)
{
}

exception::exception(const __exString& _What)
    : exception()
{
    if (_What != nullptr)
    {
        size_t _Buf_size = strlen(_What) + 1;
#pragma warning(push)
#pragma warning(disable:4996)
        _m_what = static_cast<char*>(malloc(_Buf_size));
#pragma warning(pop)
        if (_m_what != nullptr)
        {
            memcpy(const_cast<char*>(_m_what), _What, _Buf_size);
        }
    }
    else
    {
        _m_what = nullptr;
    }
    _m_doFree = 1;
}

exception::exception(const __exString& _What, int)
    : exception()
{
    _m_what = _What;
    _m_doFree = 0;
}

exception::exception(const exception& _That)
    : exception()
{
    _m_doFree = _That._m_doFree;
    if (_m_doFree)
    {
        if (_That._m_what != NULL)
        {
            size_t _Buf_size = strlen(_That._m_what) + 1;
#pragma warning(push)
#pragma warning(disable:4996)
            _m_what = static_cast<char*>(malloc(_Buf_size));
#pragma warning(pop)
            if (_m_what != NULL) {
                memcpy(const_cast<char*>(_m_what), _That._m_what, _Buf_size);
            }
        }
        else
        {
            _m_what = NULL;
        }
    }
    else
    {
        _m_what = _That._m_what;
    }
}

exception& exception::operator= (const exception& _That)
{
    if (this != &_That)
    {
        this->exception::~exception();
        this->exception::exception(_That);
    }
    return *this;
}

exception::~exception()
{
    if (_m_doFree)
    {
#pragma warning(push)
#pragma warning(disable:4996)
        free(const_cast<char*>(_m_what));
#pragma warning(pop)
    }
}

exception::__exString exception::what() const
{
    if (_m_what != nullptr)
        return _m_what;
    else
        return "Unknown exception";
}

#pragma pack(pop)

#endif
#endif
