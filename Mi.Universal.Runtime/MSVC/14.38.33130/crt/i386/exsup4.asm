        page    ,132
        title   exsup4.asm
;***
;exsup4.asm - defines _EH4_GlobalUnwind2
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       Exception handling for i386.
;
;*******************************************************************************

        .686
        .model  flat

extrn __imp__RtlUnwind@16:dword

.code

ALIAS <@_EH4_GlobalUnwind2@8> = <_EH4_GlobalUnwind2>

;void __fastcall
;_EH4_GlobalUnwind2(
;    _In_opt_ PEXCEPTION_REGISTRATION_RECORD  EstablisherFrame,
;    _In_opt_ PEXCEPTION_RECORD               ExceptionRecord
;    );

_EH4_GlobalUnwind2 PROC
        push    ebp
        mov     ebp, esp
        push    ebx
        push    esi
        push    edi
        push    0                   ; ReturnValue
        push    edx                 ; ExceptionRecord
        push    offset ReturnPoint  ; TargetIp
        push    ecx                 ; TargetFrame
        call    __imp__RtlUnwind@16

ReturnPoint:
        pop     edi
        pop     esi
        pop     ebx
        pop     ebp
        retn
_EH4_GlobalUnwind2 ENDP

END
