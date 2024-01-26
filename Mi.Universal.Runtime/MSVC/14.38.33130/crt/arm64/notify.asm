;***
;notify.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

#include "ksarm64.h"
#include "arm64/arm64ASMsymbolname.h"

    TEXTAREA

;;++
;;
;;extern "C" void _NLG_Notify(
;;    void* uoffDestination,
;;    void* uoffFramePointer,
;;    ULONG dwCode)
;;
;;--

    NESTED_ENTRY A64NAME(_NLG_Notify)

    PROLOG_STACK_ALLOC 32

    movz    x3, #0x0520
    movk    x3, #0x1993, lsl 16
    stp     x3, x0, [sp]
    stp     x1, x2, [sp,#0x10]

    NESTED_END A64NAME(_NLG_Notify)

;;++
;;
;;extern "C" void __NLG_Dispatch2(
;;    void* uoffDestination,
;;    void* uoffFramePointer,
;;    ULONG dwCode,
;;    ULONG ehMagic)
;;
;;--

    NESTED_ENTRY A64NAME(__NLG_Dispatch2)

    EPILOG_STACK_FREE 32

    EPILOG_RETURN

    NESTED_END A64NAME(__NLG_Dispatch2)

;;++
;;
;;extern "C" void* __NLG_Return2()
;;
;;--

    NESTED_ENTRY A64NAME(__NLG_Return2)

    EPILOG_RETURN

    NESTED_END A64NAME(__NLG_Return2)

    END
