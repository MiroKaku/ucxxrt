;/*
; * PROJECT:   Universal C++ RunTime (UCXXRT)
; * FILE:      exsup.asm
; * DATA:      2021/05/15
; *
; * PURPOSE:   Universal C++ RunTime
; *
; * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
; *
; * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
; */

.code

;extern "C" VOID
;_NLG_Notify (
;    IN PVOID uoffDestination,
;    IN PVOID uoffFramePointer,
;    IN ULONG dwCode
;    );
;
;extern "C" VOID
;__NLG_Return2 (
;    VOID
;    );

                public _NLG_Notify
_NLG_Notify     proc
                mov     [rsp+8], rcx
                mov     [rsp+24], rdx
                mov     [rsp+16], r8d
                mov     r9, 19930520h
                jmp     short __NLG_Dispatch2
_NLG_Notify     endp


                public __NLG_Dispatch2
__NLG_Dispatch2 proc
                ret
__NLG_Dispatch2 endp


                public __NLG_Return2
__NLG_Return2   proc
                ret
__NLG_Return2   endp


                end
