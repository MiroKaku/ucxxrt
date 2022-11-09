//
// gs_report.c
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definitions of functions used to report a security check failure.  This file
// corresponds to gs_report.c in the Windows sources.
//
#include <vcruntime_internal.h>

_CRT_BEGIN_C_HEADER

#define STATUS_SECURITY_CHECK_FAILURE STATUS_STACK_BUFFER_OVERRUN



extern UINT_PTR __security_cookie;
extern UINT_PTR __security_cookie_complement;



// Use global memory for the exception and context records, and any other local
// state needed by __report_gsfailure and __report_securityfailure.  We're not
// worried about multithread issues here - we're going to die anyway, and it
// saves us from consuming a good chunk of stack, as well as potentially
// overwriting useful data in the stack memory dump.
static EXCEPTION_RECORD         GS_ExceptionRecord;
static CONTEXT                  GS_ContextRecord;
static EXCEPTION_POINTERS const GS_ExceptionPointers =
{
    &GS_ExceptionRecord,
    &GS_ContextRecord
};



// Force an EBP frame on x86; no stack packing.  Forcing a frame applies to all
// of the functions below, including __report_gsfailure, __report_securityfailure,
// and __report_securityfailureEx
BEGIN_PRAGMA_OPTIMIZE_DISABLE("", DevDivVSO:162582, "Optimizations must be disabled to enable us to walk the stack")



// __report_gsfailure() - Report security error.
//
// A /GS security error has been detected.  We save the registers in a CONTEXT
// struct & call UnhandledExceptionFilter to display a message to the user
// (invoke Watson handling) and terminate the program.
//
// NOTE: __report_gsfailure should not be called directly.  Instead, it should
// be entered due to a failure detected by __security_check_cookie.   That's
// because __security_check_cookie may perform more checks than just a mismatch
// against the global security cookie, and because the context captured by
// __report_gsfailure is unwound assuming that __security_check_cookie was
// used to detect the failure.
//
// ULONGLONG StackCookie - the local cookie.  On x86, the local cookie is not
// passed in and is instead available in ECX, but since __report_gsfailure isn't
// __fastcall, it isn't a true argument, and we must flush ECX to the context
// record quickly.
#if defined _M_IX86
    #define GSFAILURE_PARAMETER
#elif defined _M_X64
    #define GSFAILURE_PARAMETER _In_ ULONGLONG stack_cookie
#elif defined _M_ARM || defined _M_ARM64
    #define GSFAILURE_PARAMETER _In_ uintptr_t stack_cookie
#else
    #error Unsupported architecture
#endif



#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
__declspec(noreturn) void __cdecl __report_gsfailure(GSFAILURE_PARAMETER)
{
    __fastfail(FAST_FAIL_STACK_COOKIE_CHECK_FAILURE);
}
#pragma warning(pop)



// Reports a specific security failure condition
//
// Reports a specific security failure condition.  The type of failure that
// occurred is embodied in the failure_code that is provided as a parameter.
// A specific failure condition can optionally specify additional parameters
// that should be recorded as part of the exception that is generated.
//
// NOTE: Unlike __report_gsfailure, __report_securityfailureEx assumes that
// it is called directly by the function in which the failure occurred.

__declspec(noreturn) void __cdecl __report_securityfailureEx(
    _In_                            ULONG  failure_code,
    _In_                            ULONG  parameter_count,
    _In_reads_opt_(parameter_count) void** parameters
    )
{
    UNREFERENCED_PARAMETER(parameter_count);
    UNREFERENCED_PARAMETER(parameters);

    __fastfail(failure_code);
}



// Reports a specific security failure condition.  The type of failure that
// occurred is embodied in the failure_code that is provided as a parameter.
// If a failure condition needs to specify additional parameters then it
// should call __report_securityfailureEx instead.
//
// NOTE: Unlike __report_gsfailure, __report_securityfailure assumes that it is
// called directly by the function in which the failure occurred.   This also
// means that __security_reportfailure doesn't simply call
//
//     __report_securityfailureEx(failure_code, 0, 0);
//
// as that would alter the call stack.
__declspec(noinline) void __cdecl __report_securityfailure(ULONG failure_code)
{
    __fastfail(failure_code);
}



// Declare stub for rangecheckfailure, since these occur often enough that the
// code bloat of setting up the parameters hurts performance
__declspec(guard(nosspro)) __declspec(guard(nossepi))
__declspec(noreturn) void __cdecl __report_rangecheckfailure()
{
    __report_securityfailure(FAST_FAIL_RANGE_CHECK_FAILURE);
}

_CRT_END_C_HEADER

