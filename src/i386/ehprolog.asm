;/*
; * PROJECT:   Universal C++ RunTime (UCXXRT)
; * FILE:      ehprolog.asm
; * DATA:      2021/05/14
; *
; * PURPOSE:   Universal C++ RunTime
; *
; * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
; *
; * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
; */

;***
;ehprolog.asm   - defines __EH_prolog
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       EH prolog helper function. Sets up the frame for a C++ EH function
;       with unwinds, by creating a link in the __except_list, and by setting
;       up EBP as frame base pointer.
;
;*******************************************************************************

        title   ehprolog.asm
        .386
        .model FLAT

.code

        assume fs:nothing

        public  __EH_prolog
__EH_prolog proc near

        push    -1                      ; State index
        push    eax                     ; Push address of handler thunk
        mov     eax, DWORD PTR fs:[0]
        push    eax                     ; List link
        mov     eax, DWORD PTR [esp+12] ; Load return address
        mov     DWORD PTR fs:[0], esp
        mov     DWORD PTR [esp+12], ebp ; Save old ebp on the stack
        lea     ebp, DWORD PTR [esp+12] ; Set ebp to the base of the frame
        push    eax                     ; Push return addr on top of the stack
        bnd     ret 0                   ; JMP [eax] would be bad on P6
__EH_prolog endp

end
