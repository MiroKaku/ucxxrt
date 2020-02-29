;/*
; * PROJECT:   Universal C++ RunTime (UCXXRT)
; * FILE:      handlers.asm
; * DATA:      2020/02/28
; *
; * PURPOSE:   Universal C++ RunTime
; *
; * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
; *
; * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
; */

;***
;lowhelpr.asm
;
;    Copyright (C) Microsoft Corporation. All rights reserved.
;
;Purpose:
;    Contains _CallSettingFrame(), which must be in asm for NLG purposes.
;
;Notes:
;
;*******************************************************************************

    title	handlers.asm

.xlist
    include vcruntime.inc
.list

PUBLIC	_CallSettingFrame


CODESEG

;////////////////////////////////////////////////////////////////////////////
;/
;/ _CallSettingFrame - sets up EBP and calls the specified funclet.  Restores
;/                      EBP on return.
;/
;/ Return value is return value of funclet (whatever is in EAX).
;/


    public _CallSettingFrame

_CallSettingFrame proc stdcall, funclet:IWORD, pRN:IWORD, dwInCode:DWORD
    ; FPO = 0 dwords locals allocated in prolog
    ;       3 dword parameters
    ;       8 bytes in prolog
    ;       4 registers saved (includes locals to work around debugger bug)
    ;       1 EBP is used
    ;       0 frame type = FPO
    .FPO    (0,3,8,4,1,0)

    sub     esp,4
    push    ebx
    push    ecx
    mov     eax, pRN
    add     eax, 0Ch                ; sizeof(EHRegistrationNode) -- assumed to equal 0Ch
    mov     dword ptr [ebp-4],eax
    mov     eax,funclet
    push    ebp                     ; Save our frame pointer
    push    dwInCode
    mov     ecx,dwInCode
    mov     ebp,dword ptr [ebp-4]   ; Load target frame pointer
    push    esi
    push    edi
    call    eax                     ; Call the funclet
    add     esp, 0Ch
    pop     ebp                     ; Restore our frame pointer
    pop     ecx
    pop     ebx
    ret     0Ch
_CallSettingFrame ENDP

END
