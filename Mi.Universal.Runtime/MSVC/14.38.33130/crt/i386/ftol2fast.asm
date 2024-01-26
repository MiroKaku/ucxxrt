        page    ,132
        title   ftol2fast    - truncate TOS to 32-bit integer
;*** 
;ftol2fast.asm - truncate TOS to 32-bit integer
;
;       Copyright (c) 1985-2001, Microsoft Corporation. All rights reserved.
;
;Purpose:
;
;*******************************************************************************


.xlist
    include cruntime.inc
.list


        EXTERN __isa_available:DWORD

        _rdata  segment para public 'DATA' use32
                        assume cs:_rdata

                        two32   dd 4F800000h
                        two63   dd 5F000000h
                        _NaN    dd 0FFFFFFFFh
        _rdata  ends


        CODESEG

                        public _ftoi2_fast
        _ftoi2_fast     proc
                        cmp     ds:__isa_available, __ISA_AVAILABLE_SSE42
                        jl      short ftoi2f_nofisttp
                        sub     esp, 4
                        fisttp  dword ptr [esp]
                        pop     eax
                        retn
        ftoi2f_nofisttp:
                        add     esp, 0FFFFFFF0h
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        add     esp, 10h
                        btr     ecx, 0Fh
                        sbb     edx, edx
                        cmp     ecx, 3FFFh
                        jb      short ftoi2f_retzero
                        neg     cl
                        add     cl, 3Eh
                        shr     eax, cl
                        xor     eax, edx
                        sub     eax, edx
                        retn
        ftoi2f_retzero:
                        xor     eax, eax
                        retn
        _ftoi2_fast     endp


                        public _ftoui2_fast
        _ftoui2_fast    proc
                        cmp     ds:__isa_available, __ISA_AVAILABLE_SSE42
                        jl      short ftoui2f_nofisttp
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        fisttp  qword ptr [esp]
                        mov     eax, [esp]
                        mov     esp, ecx
                        retn
        ftoui2f_nofisttp:
                        add     esp, 0FFFFFFF0h
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        add     esp, 10h
                        cmp     ecx, 3FFFh
                        jb      short ftoui2f_retzero
                        neg     cl
                        add     cl, 3Eh ; '>'
                        shr     eax, cl
                        retn
        ftoui2f_retzero:
                        xor     eax, eax
                        retn
        _ftoui2_fast    endp


                        public _ftol2_fast
        _ftol2_fast     proc
                        cmp     ds:__isa_available, __ISA_AVAILABLE_SSE42
                        jl      short ftol2f_nofisttp
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        fisttp  qword ptr [esp]
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        mov     esp, ecx
                        retn
        ftol2f_nofisttp:
                        add     esp, 0FFFFFFF0h
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        add     esp, 0FFFFFFF0h
                        btr     ecx, 0Fh
                        jb      short ftol2f_neg
        ftol2f_convert:
                        cmp     ecx, 3FFFh
                        jb      short ftol2f_retzero
                        neg     cl
                        add     cl, 3Eh ; '>'
                        cmp     cl, 20h ; ' '
                        jb      short ftol2f_shftdw
                        mov     eax, edx
                        xor     edx, edx
        ftol2f_shftdw:
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        ftol2f_neg:
                        call    ftol2f_convert
                        neg     eax
                        adc     edx, 0
                        neg     edx
                        retn
        ftol2f_retzero:
                        xor     eax, eax
                        xor     edx, edx
                        retn
        _ftol2_fast     endp


                        public _ftoul2_fast
        _ftoul2_fast    proc
                        cmp     ds:__isa_available, __ISA_AVAILABLE_SSE42
                        jl      short ftoul2f_nofisttp
                        fld     ds:two63
                        fucomip st, st(1)
                        jbe     short ftoul2f_nofisttp
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        fisttp  qword ptr [esp]
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        mov     esp, ecx
                        retn
        ftoul2f_nofisttp:
                        add     esp, 0FFFFFFF0h
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        add     esp, 10h
                        cmp     ecx, 3FFFh
                        jb      short ftoul2f_retzero
                        neg     cl
                        add     cl, 3Eh ; '>'
                        cmp     cl, 20h ; ' '
                        jb      short ftoul2f_shftdw
                        mov     eax, edx
                        xor     edx, edx
        ftoul2f_shftdw:
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        ftoul2f_retzero:
                        xor     eax, eax
                        xor     edx, edx
                        retn
        _ftoul2_fast    endp


        end
