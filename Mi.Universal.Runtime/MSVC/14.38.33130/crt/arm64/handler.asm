;***
;handler.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

#include "ksarm64.h"
#include "arm64/arm64ASMsymbolname.h"

    TEXTAREA

;;++
;;
;;extern "C" LONG
;;__C_ExecuteExceptionFilter(
;;    _In_ PEXCEPTION_POINTERS ExceptionPointers,
;;    _In_ PVOID EstablisherFrame,
;;    _In_ PEXCEPTION_FILTER ExceptionFilter,
;;    _In_ PUCHAR NonVolatileRegisters
;;    );
;;
;;--

    NESTED_ENTRY A64NAME(__C_ExecuteExceptionFilter)

    PROLOG_SAVE_REG_PAIR x19, x20, #-0x60!
    PROLOG_SAVE_REG_PAIR x21, x22, #0x10
    PROLOG_SAVE_REG_PAIR x23, x24, #0x20
    PROLOG_SAVE_REG_PAIR x25, x26, #0x30
    PROLOG_SAVE_REG_PAIR x27, x28, #0x40
    PROLOG_SAVE_REG_PAIR x29, x30, #0x50

    LDP     X19, X20, [X3]
    LDP     X21, X22, [X3, #0x10]
    LDP     X23, X24, [X3, #0x20]
    LDP     X25, X26, [X3, #0x30]
    LDP     X27, X28, [X3, #0x40]
    LDR     X1, [X3,#0x50]
    BLR     X2

    EPILOG_RESTORE_REG_PAIR x29, x30, #0x50
    EPILOG_RESTORE_REG_PAIR x27, x28, #0x40
    EPILOG_RESTORE_REG_PAIR x25, x26, #0x30
    EPILOG_RESTORE_REG_PAIR x23, x24, #0x20
    EPILOG_RESTORE_REG_PAIR x21, x22, #0x10
    EPILOG_RESTORE_REG_PAIR x19, x20, #0x60!

    EPILOG_RETURN

    NESTED_END A64NAME(__C_ExecuteExceptionFilter)

;;++
;;
;;extern "C" VOID
;;__C_ExecuteTerminationHandler(
;;    _In_ BOOLEAN AbnormalTermination,
;;    _In_ PVOID EstablisherFrame,
;;    _In_ PTERMINATION_HANDLER TerminationHandler,
;;    _In_ PUCHAR NonVolatileRegisters
;;    );
;;
;;--

    NESTED_ENTRY A64NAME(__C_ExecuteTerminationHandler)

    PROLOG_SAVE_REG_PAIR x19, x20, #-0x60!
    PROLOG_SAVE_REG_PAIR x21, x22, #0x10
    PROLOG_SAVE_REG_PAIR x23, x24, #0x20
    PROLOG_SAVE_REG_PAIR x25, x26, #0x30
    PROLOG_SAVE_REG_PAIR x27, x28, #0x40
    PROLOG_SAVE_REG_PAIR x29, x30, #0x50

    LDP     X19, X20, [X3]
    LDP     X21, X22, [X3, #0x10]
    LDP     X23, X24, [X3, #0x20]
    LDP     X25, X26, [X3, #0x30]
    LDP     X27, X28, [X3, #0x40]
    LDR     X1, [X3,#0x50]
    BLR     X2

    EPILOG_RESTORE_REG_PAIR x29, x30, #0x50
    EPILOG_RESTORE_REG_PAIR x27, x28, #0x40
    EPILOG_RESTORE_REG_PAIR x25, x26, #0x30
    EPILOG_RESTORE_REG_PAIR x23, x24, #0x20
    EPILOG_RESTORE_REG_PAIR x21, x22, #0x10
    EPILOG_RESTORE_REG_PAIR x19, x20, #0x60!

    EPILOG_RETURN

    NESTED_END A64NAME(__C_ExecuteTerminationHandler)

    END
