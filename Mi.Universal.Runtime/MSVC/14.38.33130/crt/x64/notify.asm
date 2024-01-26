;***
;notify.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

include ksamd64.inc


;;++
;;
;;extern "C" void _NLG_Notify(
;;    void* uoffDestination,
;;    void* uoffFramePointer,
;;    ULONG dwCode)
;;
;;--

CsFrame struct
    Return          dq      ?               ; Caller's return address
    Destination     dq      ?               ; Caller parameter save area
    FramePointer    dq      ?               ;
    NlgCode         dd      ?               ;
CsFrame ends


    NESTED_ENTRY _NLG_Notify, _TEXT

    END_PROLOGUE

    mov     CsFrame.Destination[rsp], rcx   ; move parameters to home slots
    mov     CsFrame.FramePointer[rsp], rdx  ;
    mov     CsFrame.NlgCode[rsp], r8d       ;

    mov     r9, 19930520h
    jmp     __NLG_Dispatch2

    NESTED_END _NLG_Notify, _TEXT

;;++
;;
;;extern "C" void __NLG_Dispatch2(
;;    void* uoffDestination,
;;    void* uoffFramePointer,
;;    ULONG dwCode,
;;    ULONG ehMagic)
;;
;;--

    NESTED_ENTRY __NLG_Dispatch2, _TEXT

    END_PROLOGUE

    ret

    NESTED_END __NLG_Dispatch2, _TEXT

;;++
;;
;;extern "C" void* __NLG_Return2()
;;
;;--

    NESTED_ENTRY __NLG_Return2, _TEXT

    END_PROLOGUE

    ret

    NESTED_END __NLG_Return2, _TEXT

END
