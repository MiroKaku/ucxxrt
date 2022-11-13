//
// corecrt_internal.h
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// CoreCRT Internals
//
#pragma once

#include <corecrt.h>
#include <corecrt_startup.h>
#include <corecrt_terminate.h>
#include <corecrt_wctype.h>
#include <crtdbg.h>
#include <ctype.h>
#include <errno.h>
#include <excpt.h>
#include <internal_shared.h>
#include <limits.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <vcruntime_startup.h>


_CRT_BEGIN_C_HEADER

#define _DEFINE_SET_FUNCTION(function_name, type, variable_name) \
    __inline void function_name(type value)                      \
    {                                                            \
        __pragma(warning(push))                                  \
        __pragma(warning(disable:4996))                          \
        variable_name = value;                                   \
        __pragma(warning(pop))                                   \
    }



#if defined _M_IX86
    #define _CRT_LINKER_SYMBOL_PREFIX "_"
#elif defined _M_X64 || defined _M_ARM || defined _M_ARM64
    #define _CRT_LINKER_SYMBOL_PREFIX ""
#else
    #error Unsupported architecture
#endif

#define _CRT_LINKER_FORCE_INCLUDE(name) \
    __pragma(comment(linker,            \
        "/include:"                     \
        _CRT_LINKER_SYMBOL_PREFIX #name \
        ))



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CRT SAL Annotations
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// This macro can be used to annotate a buffer when it has the option that
// _CRT_UNBOUNDED_BUFFER_SIZE may be passed as its size in order to invoke unsafe behavior.
// void example(
//    _Maybe_unsafe_(_Out_writes_z_, buffer_count) char * const buffer,
//    _In_                                         size_t const buffer_size
// )
#define _CRT_UNBOUNDED_BUFFER_SIZE (static_cast<size_t>(-1))
#define _Maybe_unsafe_(buffer_annotation, expr)                                                    \
        _When_((expr < _CRT_UNBOUNDED_BUFFER_SIZE), buffer_annotation(expr))                       \
        _When_((expr >= _CRT_UNBOUNDED_BUFFER_SIZE), buffer_annotation(_Inexpressible_("unsafe")))



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CoreCRT Shared Initialization Support
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef bool(__cdecl* __acrt_initialize_pft)(void);
typedef bool(__cdecl* __acrt_uninitialize_pft)(bool);

typedef struct __acrt_initializer
{
    __acrt_initialize_pft   _initialize;
    __acrt_uninitialize_pft _uninitialize;
} __acrt_initializer;

bool __cdecl __acrt_execute_initializers(
    _In_reads_(last - first) _In_ __acrt_initializer const* first,
    _In_reads_(0) __acrt_initializer const* last
);

bool __cdecl __acrt_execute_uninitializers(
    _In_reads_(last - first) __acrt_initializer const* first,
    _In_reads_(0) __acrt_initializer const* last
);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Initialization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
bool __cdecl __acrt_initialize_locks(void);
bool __cdecl __acrt_uninitialize_locks(_In_ bool terminating);

bool __cdecl __acrt_initialize_ptd(void);
bool __cdecl __acrt_uninitialize_ptd(_In_ bool terminating);

void __cdecl __acrt_initialize_invalid_parameter_handler(_In_opt_ void* encoded_null);
void __cdecl __acrt_initialize_new_handler(_In_opt_ void* encoded_null);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Per-Thread Data
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct __acrt_ptd
{
    EXCEPTION_POINTERS* _tpxcptinfoptrs;    // Pointer to the exception info pointers

    terminate_handler   _terminate;         // terminate() routine

    int                 _terrno;            // errno value
    unsigned long       _tdoserrno;         // _doserrno value

    // Per-thread error message data:
    char*               _strerror_buffer;   // Pointer to strerror()  / _strerror()  buffer
    wchar_t*            _wcserror_buffer;   // Pointer to _wcserror() / __wcserror() buffer

    // The thread-local invalid parameter handler
    _invalid_parameter_handler _thread_local_iph;

} __acrt_ptd;

__acrt_ptd* __cdecl __acrt_getptd(void);
__acrt_ptd* __cdecl __acrt_getptd_noexit(void);
void        __cdecl __acrt_freeptd(void);

void __cdecl __acrt_errno_map_os_error(long);
int  __cdecl __acrt_errno_from_os_error(long);



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Multi-Threading and Synchronization
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef enum __acrt_lock_id
{
    __acrt_exit_lock,
    __acrt_lock_count
} __acrt_lock_id;

#define _CORECRT_SPINCOUNT 4000

void __cdecl __acrt_lock(_In_ __acrt_lock_id lock);
void __cdecl __acrt_unlock(_In_ __acrt_lock_id lock);

__acrt_lock_id __cdecl __acrt_select_exit_lock();



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// CoreCRT SEH Encapsulation
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
#ifdef __cplusplus
extern "C++"
{
    template <typename Action>
    auto __acrt_lock_and_call(__acrt_lock_id const lock_id, Action&& action) throw()
        -> decltype(action())
    {
        return __crt_seh_guarded_call<decltype(action())>()(
            [lock_id]() { __acrt_lock(lock_id); },
            action,
            [lock_id]() { __acrt_unlock(lock_id); });
    }
}
#endif



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// AppCRT Heap Internals and Debug Heap Internals
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

// CRT Allocation macros:
#define _expand_crt    _expand
#define _strdup_crt    _strdup
#define _dupenv_s_crt  _dupenv_s
#define _wdupenv_s_crt _wdupenv_s



//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Invalid Parameter Macros
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// Invoke Watson if _ExpressionError is not 0; otherwise simply return
// _ExpressionError.
_CRT_SECURITYCRITICAL_ATTRIBUTE
__forceinline void _invoke_watson_if_error(
    _In_       errno_t        expression_error,
    _In_opt_z_ wchar_t const* expression,
    _In_opt_z_ wchar_t const* function_name,
    _In_opt_z_ wchar_t const* file_name,
    _In_       unsigned int   line_number,
    _In_       uintptr_t      reserved
    )
{
    if (expression_error == 0)
    {
        return;
    }

    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

// Invoke Watson if _ExpressionError is not 0 and equal to _ErrorValue1 or
// _ErrorValue2; otherwise simply return _EspressionError.
__forceinline errno_t _invoke_watson_if_oneof(
    _In_       errno_t        expression_error,
    _In_       errno_t        error_value1,
    _In_       errno_t        error_value2,
    _In_opt_z_ wchar_t const* expression,
    _In_opt_z_ wchar_t const* function_name,
    _In_opt_z_ wchar_t const* file_name,
    _In_       unsigned int   line_number,
    _In_       uintptr_t      reserved
    )
{
    if (expression_error == 0 || (expression_error != error_value1 && expression_error != error_value2))
    {
        return expression_error;
    }

    _invoke_watson(expression, function_name, file_name, line_number, reserved);
}

#ifdef _DEBUG
    #define _INVOKE_WATSON_IF_ERROR(expr) _invoke_watson_if_error((expr), _CRT_WIDE(#expr), __FUNCTIONW__, __FILEW__, __LINE__, 0)
    #define _INVOKE_WATSON_IF_ONEOF(expr, errvalue1, errvalue2) _invoke_watson_if_oneof(expr, (errvalue1), (errvalue2), _CRT_WIDE(#expr), __FUNCTIONW__, __FILEW__, __LINE__, 0)
#else
    #define _INVOKE_WATSON_IF_ERROR(expr) _invoke_watson_if_error(expr, NULL, NULL, NULL, 0, 0)
    #define _INVOKE_WATSON_IF_ONEOF(expr, errvalue1, errvalue2) _invoke_watson_if_oneof((expr), (errvalue1), (errvalue2), NULL, NULL, NULL, 0, 0)
#endif

#define _ERRCHECK(e)               _INVOKE_WATSON_IF_ERROR(e)
#define _ERRCHECK_EINVAL(e)        _INVOKE_WATSON_IF_ONEOF(e, EINVAL, EINVAL)
#define _ERRCHECK_EINVAL_ERANGE(e) _INVOKE_WATSON_IF_ONEOF(e, EINVAL, ERANGE)

#define _ERRCHECK_SPRINTF(_PrintfCall)      \
    {                                       \
        errno_t _SaveErrno = errno;         \
        errno = 0;                          \
        if ((_PrintfCall) < 0)              \
        {                                   \
            _ERRCHECK_EINVAL_ERANGE(errno); \
        }                                   \
        errno = _SaveErrno;                 \
    }

_CRT_END_C_HEADER
