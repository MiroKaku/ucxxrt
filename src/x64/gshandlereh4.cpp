/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      gshandlereh4.cpp
 * DATA:      2021/05/15
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/***
*gshandlereh4.cpp - Defines __GSHandlerCheck_EH4 for X64
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines __GSHandlerCheck_EH4, the X64 exception handler for functions
*       with /GS-protected buffers as well as C++ EH constructs based on __CxxFrameHandler4.
*******************************************************************************/

// Disable /GL: gshandlereh4.cpp -> Properties -> C/C++ -> Optimization -> Whole Program Optimization: NO

#ifdef _AMD64_

#include <ehdata.h>
#include <vcruntime_internal.h>

typedef struct _GS_HANDLER_DATA *PGS_HANDLER_DATA;

EXTERN_C void __cdecl
__GSHandlerCheckCommon (
    IN PVOID EstablisherFrame,
    IN PDISPATCHER_CONTEXT DispatcherContext,
    IN PGS_HANDLER_DATA GSHandlerData
    );

EXTERN_C EXCEPTION_DISPOSITION __cdecl
__CxxFrameHandler4 (
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
);

EXTERN_C EXCEPTION_DISPOSITION __cdecl
__GSHandlerCheck_EH4(
    IN PEXCEPTION_RECORD ExceptionRecord,
    IN PVOID EstablisherFrame,
    IN OUT PCONTEXT ContextRecord,
    IN OUT PDISPATCHER_CONTEXT DispatcherContext
)
{
    PGS_HANDLER_DATA GSHandlerData;
    ULONG GSUnwindInfo;
    EXCEPTION_DISPOSITION Disposition;

    //
    // Retrieve a pointer to the start of that part of the handler data used
    // to locate the local security cookie in the local frame.  That is found
    // following the image-relative offset to the FuncInfo table used by
    // __CxxFrameHandler4.
    //

    GSHandlerData = (PGS_HANDLER_DATA)((PULONG)DispatcherContext->HandlerData + 1);

    //
    // Perform the actual cookie check.
    //

    __GSHandlerCheckCommon(
        EstablisherFrame,
        DispatcherContext,
        GSHandlerData
    );

    //
    // If the cookie check succeeds, call the normal C++ EH handler if we're
    // supposed to on this exception pass.  Find the EHANDLER/UHANDLER flags
    // controlling that in the first ULONG of our part of the handler data.
    //

    GSUnwindInfo = *(PULONG)GSHandlerData;
    if (IS_DISPATCHING(ExceptionRecord->ExceptionFlags)
        ? (GSUnwindInfo & UNW_FLAG_EHANDLER)
        : (GSUnwindInfo & UNW_FLAG_UHANDLER))
    {
        Disposition = __CxxFrameHandler4(
            ExceptionRecord,
            EstablisherFrame,
            ContextRecord,
            DispatcherContext
        );
    }
    else
    {
        Disposition = ExceptionContinueSearch;
    }

    return Disposition;
}

#endif
