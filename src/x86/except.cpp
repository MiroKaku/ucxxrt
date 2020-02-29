/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      except.cpp
 * DATA:      2020/02/21
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <intrin.h>
#include "vcruntime/ehassert.h"
#include "vcruntime/ehhooks.h"


namespace ucxxrt 
{
    /*End of Exception List*/
#define EXCEPTION_CHAIN_END ((PEXCEPTION_REGISTRATION_RECORD)-1)

    typedef struct _DISPATCHER_CONTEXT
    {
        PEXCEPTION_REGISTRATION_RECORD RegistrationPointer;
    } DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

    EXTERN_C void __GetStackLimits(PULONG_PTR LowLimit, PULONG_PTR HighLimit)
    {
#ifdef _KERNEL_MODE
        IoGetStackLimits(LowLimit, HighLimit);
#else
        *LowLimit  = (ULONG_PTR)((NT_TIB*)NtCurrentTeb())->StackLimit;
        *HighLimit = (ULONG_PTR)((NT_TIB*)NtCurrentTeb())->StackBase;
#endif
    }

    EXTERN_C PEXCEPTION_REGISTRATION_RECORD __GetExceptionList()
    {
        return (PEXCEPTION_REGISTRATION_RECORD)__readfsdword(0);
    }

    EXTERN_C [[noreturn]] void __RaiseException(
        _In_ PEXCEPTION_RECORD ExceptionRecord,
        _In_ PCONTEXT /*ContextRecord*/
    ) {

        RtlRaiseException(ExceptionRecord);
    }

    EXTERN_C [[noreturn]] void __CxxRaiseException(
        _In_ DWORD dwExceptionCode,
        _In_ DWORD dwExceptionFlags,
        _In_ DWORD nNumberOfArguments,
        _In_reads_opt_(nNumberOfArguments) CONST ULONG_PTR* lpArguments
    ) {
        /*
         *  Exception Record
         */

        EXCEPTION_RECORD ExceptionRecord{};

        ExceptionRecord.ExceptionCode = dwExceptionCode;
        ExceptionRecord.ExceptionRecord = nullptr;
        ExceptionRecord.ExceptionAddress = _ReturnAddress();
        ExceptionRecord.ExceptionFlags = dwExceptionFlags & EXCEPTION_NONCONTINUABLE;

        /* Check if we have arguments */
        if (!lpArguments)
        {
            /* We don't */
            ExceptionRecord.NumberParameters = 0;
        }
        else
        {
            /* We do, normalize the count */
            if (nNumberOfArguments > EXCEPTION_MAXIMUM_PARAMETERS)
                nNumberOfArguments = EXCEPTION_MAXIMUM_PARAMETERS;

            /* Set the count of parameters and copy them */
            ExceptionRecord.NumberParameters = nNumberOfArguments;
            RtlCopyMemory(ExceptionRecord.ExceptionInformation,
                lpArguments,
                nNumberOfArguments * sizeof(ULONG_PTR));
        }

        /*
         *  Exception Context
         */

        CONTEXT Context{};
        RtlCaptureContext(&Context);

        /* Write the context flag */
        Context.ContextFlags = CONTEXT_FULL;

        __CxxDispatchException(&ExceptionRecord, &Context);
    }

    EXTERN_C [[noreturn]] void __CxxDispatchException(
        _In_ PEXCEPTION_RECORD ExceptionRecord,
        _In_ PCONTEXT ContextRecord
    ) {
        PEXCEPTION_REGISTRATION_RECORD RegistrationFrame, NestedFrame = nullptr;
        DISPATCHER_CONTEXT DispatcherContext{};
        EXCEPTION_RECORD NonContinuableExceptionRecord{};
        ULONG_PTR StackLow  = 0;
        ULONG_PTR StackHigh = 0;
        ULONG_PTR RegistrationFrameEnd = 0;

        /* Get the current stack limits and registration frame */
        __GetStackLimits(&StackLow, &StackHigh);
        RegistrationFrame = __GetExceptionList();

        /* Now loop every frame */
        while (RegistrationFrame != EXCEPTION_CHAIN_END)
        {
            /* Find out where it ends */
            RegistrationFrameEnd = (ULONG_PTR)RegistrationFrame + sizeof(EXCEPTION_REGISTRATION_RECORD);

            /* Make sure the registration frame is located within the stack */
            if ((RegistrationFrameEnd > StackHigh) ||
                ((ULONG_PTR)RegistrationFrame < StackLow) ||
                ((ULONG_PTR)RegistrationFrame & STACK_MISALIGNMENT))
            {
                /* Set up the exception record */
                NonContinuableExceptionRecord.ExceptionCode = EXCEPTION_NONCONTINUABLE_EXCEPTION;
                NonContinuableExceptionRecord.ExceptionFlags |= EXCEPTION_NONCONTINUABLE | EXCEPTION_STACK_INVALID | EXCEPTION_UNWINDING;
                NonContinuableExceptionRecord.ExceptionRecord = ExceptionRecord;
                NonContinuableExceptionRecord.NumberParameters = 0;

                /* Raise the exception */
                __RaiseException(&NonContinuableExceptionRecord, ContextRecord);
                break;
            }

            DispatcherContext.RegistrationPointer = RegistrationFrame;

            /* Call the handler */
            auto Disposition = __CxxExecuteHandlerForException(
                ExceptionRecord,
                RegistrationFrame,
                ContextRecord,
                &DispatcherContext,
                RegistrationFrame->Handler);

            /* Check if this is a nested frame */
            if (RegistrationFrame == NestedFrame)
            {
                /* Mask out the flag and the nested frame */
                ExceptionRecord->ExceptionFlags &= ~EXCEPTION_NESTED_CALL;
                NestedFrame = nullptr;
            }

            /* Handle the dispositions */
            switch (Disposition)
            {
            case ExceptionContinueExecution:
            {
                /* Continue execution */

                /* Check if it was non-continuable */
                if (ExceptionRecord->ExceptionFlags & EXCEPTION_NONCONTINUABLE)
                {
                    /* Set up the exception record */
                    NonContinuableExceptionRecord.ExceptionCode = EXCEPTION_NONCONTINUABLE_EXCEPTION;
                    NonContinuableExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
                    NonContinuableExceptionRecord.ExceptionRecord = ExceptionRecord;
                    NonContinuableExceptionRecord.NumberParameters = 0;

                    /* Raise the exception */
                    __RaiseException(&NonContinuableExceptionRecord, ContextRecord);
                }
                break;
            }
            case ExceptionContinueSearch:
            {
                /* Continue searching */
                break;
            }
            case ExceptionNestedException:
            {
                /* Nested exception */

                /* Turn the nested flag on */
                ExceptionRecord->ExceptionFlags |= EXCEPTION_NESTED_CALL;

                /* Update the current nested frame */
                if (DispatcherContext.RegistrationPointer > NestedFrame)
                {
                    /* Get the frame from the dispatcher context */
                    NestedFrame = DispatcherContext.RegistrationPointer;
                }
                break;
            }
            default:
            {
                /* Anything else */

                /* Set up the exception record */
                NonContinuableExceptionRecord.ExceptionCode = STATUS_INVALID_DISPOSITION;
                NonContinuableExceptionRecord.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
                NonContinuableExceptionRecord.ExceptionRecord = ExceptionRecord;
                NonContinuableExceptionRecord.NumberParameters = 0;

                /* Raise the exception */
                __RaiseException(&NonContinuableExceptionRecord, ContextRecord);
                break;
            }
            }

            /* Go to the next frame */
            RegistrationFrame = RegistrationFrame->Next;
        }
    }

    EXTERN_C EXCEPTION_DISPOSITION __CxxExecuteHandlerForException(
        PEXCEPTION_RECORD ExceptionRecord,
        PVOID EstablisherFrame,
        PCONTEXT Context,
        PVOID DispatcherContext,
        PEXCEPTION_ROUTINE ExceptionHandler
    ) {
        return ExceptionHandler(ExceptionRecord, EstablisherFrame, Context, DispatcherContext);
    }
}
