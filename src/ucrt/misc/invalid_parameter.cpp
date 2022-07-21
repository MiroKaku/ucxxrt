//
// invalid_parameter.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// The invalid parameter handlers and related functionality
//
#include <corecrt_internal.h>
#include <vcstartup_internal.h>


_VCRT_DECLARE_ALTERNATE_NAME(_invoke_watson, _UCXXRT__invoke_watson);
#ifdef _DEBUG
_VCRT_DECLARE_ALTERNATE_NAME(_invalid_parameter, _UCXXRT__invalid_parameter);
#endif
_VCRT_DECLARE_ALTERNATE_NAME(_invalid_parameter_noinfo, _UCXXRT__invalid_parameter_noinfo);
_VCRT_DECLARE_ALTERNATE_NAME(_invalid_parameter_noinfo_noreturn, _UCXXRT__invalid_parameter_noinfo_noreturn);


static _invalid_parameter_handler __acrt_invalid_parameter_handler;


extern "C" void __cdecl __acrt_initialize_invalid_parameter_handler(void* const encoded_null)
{
    __acrt_invalid_parameter_handler = reinterpret_cast<_invalid_parameter_handler>(encoded_null);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// _invalid_parameter
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern "C" static void __cdecl _invalid_parameter_internal(
    wchar_t const*     const expression,
    wchar_t const*     const function_name,
    wchar_t const*     const file_name,
    unsigned int       const line_number,
    uintptr_t          const reserved
    )
{
    __acrt_ptd* const raw_ptd = __acrt_getptd_noexit();
    if (raw_ptd && raw_ptd->_thread_local_iph)
    {
        raw_ptd->_thread_local_iph(expression, function_name, file_name, line_number, reserved);
        return;
    }

    _invalid_parameter_handler const global_handler = __crt_fast_decode_pointer(__acrt_invalid_parameter_handler);
    if (global_handler)
    {
        global_handler(expression, function_name, file_name, line_number, reserved);
        return;
    }

    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

extern "C" void __cdecl _UCXXRT__invalid_parameter(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved
    )
{
    return _invalid_parameter_internal(expression, function_name, file_name, line_number, reserved);
}

extern "C" void __cdecl _UCXXRT__invalid_parameter_noinfo()
{
    _invalid_parameter_internal(nullptr, nullptr, nullptr, 0, 0);
}

// This is used by inline code in the C++ Standard Library and the SafeInt
// library.  Because it is __declspec(noreturn), the compiler can better
// optimize use of the invalid parameter handler for inline code.
extern "C" __declspec(noreturn) void __cdecl _UCXXRT__invalid_parameter_noinfo_noreturn()
{
    _invalid_parameter_internal(nullptr, nullptr, nullptr, 0, 0);
    _invoke_watson(nullptr, nullptr, nullptr, 0, 0);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// _invoke_watson
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

extern "C" __declspec(noreturn) void __cdecl _UCXXRT__invoke_watson(
    wchar_t const* const expression,
    wchar_t const* const function_name,
    wchar_t const* const file_name,
    unsigned int   const line_number,
    uintptr_t      const reserved
)
{
    UNREFERENCED_PARAMETER(expression);
    UNREFERENCED_PARAMETER(function_name);
    UNREFERENCED_PARAMETER(file_name);
    UNREFERENCED_PARAMETER(line_number);
    UNREFERENCED_PARAMETER(reserved);

    __fastfail(FAST_FAIL_INVALID_ARG);
}


//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Handler Accessors
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
extern "C" _invalid_parameter_handler __cdecl _set_invalid_parameter_handler(_invalid_parameter_handler const new_handler)
{
    _invalid_parameter_handler const old_handler = __crt_fast_decode_pointer(__acrt_invalid_parameter_handler);
    __acrt_invalid_parameter_handler = __crt_fast_encode_pointer(new_handler);
    return old_handler;
}

extern "C" _invalid_parameter_handler __cdecl _get_invalid_parameter_handler()
{
    return __crt_fast_decode_pointer(__acrt_invalid_parameter_handler);
}



extern "C" _invalid_parameter_handler __cdecl _set_thread_local_invalid_parameter_handler(_invalid_parameter_handler const new_handler)
{
    __acrt_ptd* const ptd = __acrt_getptd();

    _invalid_parameter_handler const old_handler = ptd->_thread_local_iph;
    ptd->_thread_local_iph = new_handler;
    return old_handler;
}

extern "C" _invalid_parameter_handler __cdecl _get_thread_local_invalid_parameter_handler()
{
    __acrt_ptd* const ptd = __acrt_getptd_noexit();
    if (!ptd)
    {
        return nullptr;
    }

    return ptd->_thread_local_iph;
}
