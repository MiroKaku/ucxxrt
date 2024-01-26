        page    ,132
        title   ftol3fast    - truncate TOS to 32-bit integer
;*** 
;ftol3fast.asm - truncate TOS to 32-bit integer
;
;       Copyright (c) 1985-2001, Microsoft Corporation. All rights reserved.
;
;Purpose:
;
;*******************************************************************************


.xlist
    include cruntime.inc
.list

        .xmm

        __ISA_AVAILABLE_AVX512 equ 6

        EXTERN __isa_available:DWORD

        _rdata  segment para public 'DATA' use32
                        assume cs:_rdata

                        f_two31         dd 4F000000h
                        f_negtwo31      dd 0CF000000h
                        f_negone        dd 0BF800000h
                        f_two32         dd 4F800000h
                        f_negtwo63      dd 0DF000000h
                        f_two63         dd 5F000000h
                        f_two64         dd 5F800000h
                        f_NaN           dd 0FFFFFFFFh
                        f_onept5        dd 3FC00000h

                                        align 10h

                        d_two31         dq 41E0000000000000h
                        d_negtwo31      dq 0C1E0000000000000h
                        d_negone        dq 0BFF0000000000000h
                        d_two32         dq 41F0000000000000h
                        d_negtwo63      dq 0C3E0000000000000h
                        d_two63         dq 43E0000000000000h
                        d_two64         dq 43F0000000000000h
        _rdata  ends


        CODESEG

                        public _ftoui3_fast
        _ftoui3_fast    proc near
                        ucomiss xmm0, ds:f_two31
                        jnb     short ftoui3f_big
                        cvttss2si eax, xmm0
                        retn
        ftoui3f_big:
                        movd    eax, xmm0
                        shl     eax, 8
                        bts     eax, 1Fh
                        retn
        _ftoui3_fast    endp


                        public _ftol3_fast
        _ftol3_fast     proc near
                        ucomiss xmm0, ds:f_two31
                        jnb     short ftolf_big
                        ucomiss xmm0, ds:f_negtwo31
                        jb      short ftolf_big
                        cvttss2si eax, xmm0
                        cdq
                        retn
        ftolf_big:
                        movd    eax, xmm0
                        cdq
                        mov     ecx, eax
                        bts     eax, 17h
                        shr     ecx, 17h
                        shl     eax, 8
                        sub     cl, 0BEh
                        xor     eax, edx
                        sub     eax, edx
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _ftol3_fast     endp


                        public _ftoul3_fast
        _ftoul3_fast    proc near
                        ucomiss xmm0, ds:f_two31
                        jnb     short ftoul3f_big
                        cvttss2si eax, xmm0
                        xor     edx, edx
                        retn
        ftoul3f_big:
                        movd    eax, xmm0
                        mov     ecx, eax
                        bts     eax, 17h
                        shr     ecx, 17h
                        shl     eax, 8
                        sub     cl, 0BEh
                        jz      short ftoul3f_bigshift
                        xor     edx, edx
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        ftoul3f_bigshift:
                        mov     edx, eax
                        xor     eax, eax
                        retn
        _ftoul3_fast    endp


                        public _dtoui3_fast
        _dtoui3_fast    proc near
                        ucomisd xmm0, ds:d_two31
                        jnb     short dtouif_big
                        cvttsd2si eax, xmm0
                        retn
        dtouif_big:
                        movd    eax, xmm0
                        psrldq  xmm0, 4
                        movd    edx, xmm0
                        shrd    eax, edx, 15h
                        bts     eax, 1Fh
                        retn
        _dtoui3_fast    endp


                        public _dtol3_fast
        _dtol3_fast     proc near
                        ucomisd xmm0, ds:d_two31
                        jnb     short dtol3f_getbits
                        ucomisd xmm0, ds:d_negtwo31
                        jb      short dtolf_veryneg
                        cvttsd2si eax, xmm0
                        cdq
                        retn
        dtolf_veryneg:
                        call    dtol3f_getbits
                        neg     eax
                        adc     edx, 0
                        neg     edx
                        retn
        dtol3f_getbits:
                        movd    eax, xmm0
                        psrldq  xmm0, 4
                        movd    edx, xmm0
                        mov     ecx, edx
                        btr     ecx, 1Fh
                        bts     edx, 14h
                        shr     ecx, 14h
                        and     edx, 1FFFFFh
                        sub     ecx, 433h
                        jge     short dtol3f_bigshift
                        neg     cl
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        dtol3f_bigshift:
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _dtol3_fast     endp


                        public _dtoul3_fast
        _dtoul3_fast    proc near
                        ucomisd xmm0, ds:d_two31
                        jnb     short dtoul3f_big
                        cvttsd2si eax, xmm0
                        xor     edx, edx
                        retn
        dtoul3f_big:
                        movd    eax, xmm0
                        psrldq  xmm0, 4
                        movd    edx, xmm0
                        mov     ecx, edx
                        bts     edx, 14h
                        shr     ecx, 14h
                        and     edx, 1FFFFFh
                        sub     ecx, 433h
                        jge     short dtoul3f_bigshift
                        neg     cl
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        dtoul3f_bigshift:
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _dtoul3_fast    endp


        end
