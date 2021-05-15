/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      except.cpp
 * DATA:      2020/02/11
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <intrin.h>
#include <ehassert.h>
#include <ehhooks.h>


[[noreturn]] static void __RaiseException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT /*ContextRecord*/
) {

    RtlRaiseException(ExceptionRecord);
}

[[noreturn]] void __stdcall __CxxRaiseException(
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

[[noreturn]] void __stdcall __CxxDispatchException(
    _In_ PEXCEPTION_RECORD ExceptionRecord,
    _In_ PCONTEXT ContextRecord
) {
    auto ScopeIndex = 0ul;
    auto NestedFrame = 0Ui64;

    auto UnwindHistoryTableBuf = UNWIND_HISTORY_TABLE{};
    auto UnwindHistoryTable = &UnwindHistoryTableBuf;

    auto NonContinuableExceptionRecord = EXCEPTION_RECORD{};
    for (;;)
    {
        DWORD64 ImageBase = ExceptionRecord->ExceptionInformation[3];
        DWORD64 ControlPc = ContextRecord->Rip;
        PVOID   HandlerData = nullptr;
        DWORD64 EstablisherFrame = 0;

        auto RuntimeFunctionItem = RtlLookupFunctionEntry(ContextRecord->Rip, &ImageBase, UnwindHistoryTable);
        if (!RuntimeFunctionItem)
        {
            if (ContextRecord->Rip == 0)
            {
                NonContinuableExceptionRecord.ExceptionCode = EXCEPTION_NONCONTINUABLE_EXCEPTION;
                NonContinuableExceptionRecord.ExceptionFlags |= EXCEPTION_NONCONTINUABLE | EXCEPTION_UNWINDING;
                NonContinuableExceptionRecord.ExceptionRecord = ExceptionRecord;
                NonContinuableExceptionRecord.NumberParameters = 0;

                __RaiseException(&NonContinuableExceptionRecord, ContextRecord);
                break;
            }

            //
            // If we don't have a RUNTIME_FUNCTION, then we've encountered
            // a leaf function.  Adjust the stack approprately.
            //

            ContextRecord->Rip = (ULONG64)(*(PULONG64)ContextRecord->Rsp);
            ContextRecord->Rsp += sizeof(void*);
            continue;
        }

        //
        // Otherwise, call upon RtlVirtualUnwind to execute the unwind for
        // us.
        //

        ControlPc = ContextRecord->Rip;

        auto FrameHandler = RtlVirtualUnwind(
            UNW_FLAG_EHANDLER, // let it find the FrameHandler
            ImageBase,
            ControlPc,
            RuntimeFunctionItem,
            ContextRecord,
            &HandlerData,
            &EstablisherFrame,
            nullptr);

        if (EstablisherFrame & STACK_MISALIGNMENT)
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

        if (FrameHandler == nullptr)
        {
            continue;
        }

        auto DispatcherContext = DISPATCHER_CONTEXT{};
        DispatcherContext.ControlPc = ControlPc;
        DispatcherContext.ImageBase = ImageBase;
        DispatcherContext.FunctionEntry = RuntimeFunctionItem;
        DispatcherContext.EstablisherFrame = EstablisherFrame;
        DispatcherContext.TargetIp = 0;
        DispatcherContext.ContextRecord = ContextRecord;
        DispatcherContext.LanguageHandler = FrameHandler;
        DispatcherContext.HandlerData = HandlerData;
        DispatcherContext.HistoryTable = UnwindHistoryTable;
        DispatcherContext.ScopeIndex = ScopeIndex;

        auto Disposition = __CxxExecuteHandlerForException(
            ExceptionRecord,
            (PVOID)EstablisherFrame,
            ContextRecord,
            &DispatcherContext,
            FrameHandler);

        if (NestedFrame == EstablisherFrame)
        {
            ExceptionRecord->ExceptionFlags &= ~EXCEPTION_NESTED_CALL;
            NestedFrame = 0;
        }

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
        case ExceptionNestedException:
        {
            /* Nested exception */

            /* Turn the nested flag on */
            ExceptionRecord->ExceptionFlags |= EXCEPTION_NESTED_CALL;

            /* Update the current nested frame */
            if (DispatcherContext.EstablisherFrame > NestedFrame)
            {
                /* Get the frame from the dispatcher context */
                NestedFrame = DispatcherContext.EstablisherFrame;
            }
            break;
        }
        case ExceptionContinueSearch:
        {
            /* Continue searching */
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
    }
}

EXCEPTION_DISPOSITION __stdcall __CxxExecuteHandlerForException(
    PEXCEPTION_RECORD ExceptionRecord,
    PVOID EstablisherFrame,
    PCONTEXT Context,
    PVOID DispatcherContext,
    PEXCEPTION_ROUTINE ExceptionHandler
) {
    return ExceptionHandler(ExceptionRecord, EstablisherFrame, Context, DispatcherContext);
}

