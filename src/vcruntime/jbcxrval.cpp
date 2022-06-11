/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      jbcxrval.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/*++

Copyright (c) Microsoft Corporation

Module Name:

    jbcxrval.c

Abstract:

    This module implements C support code for jump buffer and context record
    validation.

Author:

    Ken Johnson (kejohns) 27-Apr-2013

Environment:

    Kernel mode only.

--*/

#include <vcruntime_internal.h>
#include <stdlib.h>


void __GetStackLimits(PULONG_PTR LowLimit, PULONG_PTR HighLimit)
{
    IoGetStackLimits(LowLimit, HighLimit);
}

// Using libcntpr.lib on X86 KernelMode
#if defined(_AMD64_)

#include "misc\cfg_support.inc"  // To inline _guard_icall_checks_enforced()

#if defined(_AMD64_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Rsp)
#elif defined(_ARM_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Sp)
#elif defined(_ARM64_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Sp)
#elif defined(_X86_)
#define JUMP_BUFFER_TO_STACK_POINTER(JumpBuffer) ((JumpBuffer)->Esp)
#define JUMP_BUFFER_NO_FRAME 1
#else
#error Unsupported architecture.
#endif

#define CONTEXT_TO_STACK_POINTER(Context) JUMP_BUFFER_TO_STACK_POINTER(Context)

EXTERN_C void __cdecl __except_validate_context_record (
    _In_ PCONTEXT ContextRecord
    )

/*++

Routine Description:

    This function validates a context record for exception handling support.

Arguments:

    ContextRecord - Supplies a pointer to the context record to validate.

Return Value:

    None.  If the context record was not valid, a fast fail event is raised if
    CFG was enforced.

--*/

{
    PVOID StackPointer   = nullptr;
    PVOID StackLowLimit  = nullptr;
    PVOID StackHighLimit = nullptr;


    //
    // If guard ICall checks are enforced, then validate the stack extents of
    // the context record and raise a fast fail exception if the extents are
    // invalid.  If checks are not enforced or the jump buffer was valid, then
    // return.
    //
    if (_guard_icall_checks_enforced()) {

        __GetStackLimits((PULONG_PTR)&StackLowLimit, (PULONG_PTR)&StackHighLimit);

        StackPointer = (PVOID)CONTEXT_TO_STACK_POINTER(ContextRecord);

        if ((StackPointer < StackLowLimit) ||
            (StackPointer > StackHighLimit)) {

            __fastfail(FAST_FAIL_INVALID_SET_OF_CONTEXT);
        }
    }
}

EXTERN_C void __cdecl __except_validate_jump_buffer (
    _In_ jmp_buf JumpBuffer
    )

/*++

Routine Description:

    This function validates a jump buffer for exception handling support.

Arguments:

    JumpBuffer - Supplies a pointer to the jump buffer to validate.

Return Value:

    None.  If the jump buffer was not valid, a fast fail event is raised if
    CFG was enforced.

--*/

{

    PVOID StackPointer   = nullptr;
    PVOID StackLowLimit  = nullptr;
    PVOID StackHighLimit = nullptr;


    //
    // If guard ICall checks are enforced, then validate the stack extents of
    // the jump buffer and raise a fast fail exception if the extents are
    // invalid.  If checks are not enforced or the jump buffer was valid, then
    // return.
    //

    if (_guard_icall_checks_enforced()) {

        __GetStackLimits((PULONG_PTR)&StackLowLimit, (PULONG_PTR)&StackHighLimit);

#pragma prefast(suppress:26007, "JumpBuffer is really a _JUMP_BUFFER in disguise and not a jmp_buf.  The code is correct.")
        StackPointer = (PVOID)JUMP_BUFFER_TO_STACK_POINTER((_JUMP_BUFFER*)JumpBuffer);

        if ((StackPointer < StackLowLimit) ||
            (StackPointer > StackHighLimit)) {

            __fastfail(FAST_FAIL_INVALID_SET_OF_CONTEXT);
        }

#ifndef JUMP_BUFFER_NO_FRAME
        if (((_JUMP_BUFFER*)JumpBuffer)->Frame == 0) {

            __fastfail(FAST_FAIL_INVALID_SET_OF_CONTEXT);
        }
#endif
    }
}

#endif // _AMD64_
