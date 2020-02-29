/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      gshandlereh4.cpp - Defines __GSHandlerCheck_EH4 for X64
 * DATA:      2020/02/05
 *
 * PURPOSE:   Defines __GSHandlerCheck_EH4, the X64 exception handler for functions
 *            with /GS-protected buffers as well as C++ EH constructs based on __CxxFrameHandler4.
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <excpt.h>
#include "vcruntime/ehassert.h"
#include "shared/image.h"

namespace ucxxrt
{

    typedef struct _GS_HANDLER_DATA* PGS_HANDLER_DATA;


    extern "C" void
        __GSHandlerCheckCommon(
            _In_ void* EstablisherFrame,
            _In_ PDISPATCHER_CONTEXT DispatcherContext,
            _In_ PGS_HANDLER_DATA GSHandlerData
        );

    extern "C" EXCEPTION_DISPOSITION
        __CxxFrameHandler4(
            _In_ PEXCEPTION_RECORD ExceptionRecord,
            _In_ void* EstablisherFrame,
            _Inout_ PCONTEXT ContextRecord,
            _Inout_ PDISPATCHER_CONTEXT DispatcherContext
        );

    extern "C" EXCEPTION_DISPOSITION
        __GSHandlerCheck_EH4(
            _In_ PEXCEPTION_RECORD ExceptionRecord,
            _In_ void* EstablisherFrame,
            _Inout_ PCONTEXT ContextRecord,
            _Inout_ PDISPATCHER_CONTEXT DispatcherContext
        )
    {
        PGS_HANDLER_DATA GSHandlerData;
        uint32_t GSUnwindInfo;
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
}
