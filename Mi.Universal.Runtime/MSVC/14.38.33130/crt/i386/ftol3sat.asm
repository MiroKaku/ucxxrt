        page    ,132
        title   ftol3sat    - truncate TOS to 32-bit integer
;*** 
;ftol3sat.asm - truncate TOS to 32-bit integer
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


                        public _ftoi3_sat
        _ftoi3_sat      proc near
                        xor     eax, eax
                        ucomiss xmm0, ds:f_two31
                        jp      short ftoi3s_except
                        mov     eax, 7FFFFFFFh
                        jnb     short ftoi3s_except
                        cvttss2si eax, xmm0
                        retn
        ftoi3s_except:
                        cvttss2si edx, ds:f_NaN
                        retn
        _ftoi3_sat      endp


                        public _ftoui3_sat
        _ftoui3_sat     proc near
                        xor     eax, eax
                        ucomiss xmm0, ds:f_negone
                        jbe     short ftoui3s_except
                        ucomiss xmm0, ds:f_two31
                        jnb     short ftoui3s_big
                        cvttss2si eax, xmm0
                        retn
        ftoui3s_big:
                        dec     eax
                        ucomiss xmm0, ds:f_two32
                        jnb     short ftoui3s_except
                        movd    eax, xmm0
                        shl     eax, 8
                        bts     eax, 1Fh
                        retn
        ftoui3s_except:
                        cvttss2si edx, ds:f_NaN
                        retn
        _ftoui3_sat     endp


                        public _ftol3_sat
        _ftol3_sat      proc near
                        xor     eax, eax
                        xor     edx, edx
                        ucomiss xmm0, ds:f_two31
                        jp      short ftols_except
                        jnb     short ftols_big
                        ucomiss xmm0, ds:f_negtwo31
                        jb      short ftols_veryneg
                        cvttss2si eax, xmm0
                        cdq
                        retn
        ftols_big:
                        mov     edx, 7FFFFFFFh
                        dec     eax
                        ucomiss xmm0, ds:f_two63
                        jnb     short ftols_except
        ftols_bigcvt:
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
        ftols_veryneg:
                        mov     edx, 80000000h
                        ucomiss xmm0, ds:f_negtwo63
                        ja      short ftols_bigcvt
                        jz      short ftols_ret
        ftols_except:
                        cvttss2si ecx, ds:f_NaN
        ftols_ret:
                        retn
        _ftol3_sat      endp


                        public _ftoul3_sat
        _ftoul3_sat     proc near
                        xor     eax, eax
                        xor     edx, edx
                        ucomiss xmm0, ds:f_negone
                        jbe     short ftoul3s_except
                        ucomiss xmm0, ds:f_two31
                        jnb     short ftoul3s_big
                        cvttss2si eax, xmm0
                        retn
        ftoul3s_big:
                        dec     eax
                        dec     edx
                        ucomiss xmm0, ds:f_two64
                        jnb     short ftoul3s_except
                        movd    eax, xmm0
                        mov     ecx, eax
                        bts     eax, 17h
                        shr     ecx, 17h
                        shl     eax, 8
                        sub     cl, 0BEh
                        jz      short ftoul3s_bigshift
                        xor     edx, edx
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        ftoul3s_bigshift:
                        mov     edx, eax
                        xor     eax, eax
                        retn
        ftoul3s_except:
                        cvttss2si ecx, ds:f_NaN
                        retn
        _ftoul3_sat     endp


                        public _dtoi3_sat
        _dtoi3_sat      proc near
                        xor     eax, eax
                        ucomisd xmm0, ds:d_two31
                        jp      short dtois_except
                        mov     eax, 7FFFFFFFh
                        jnb     short dtois_except
                        cvttsd2si eax, xmm0
                        retn
        dtois_except:
                        cvttss2si ecx, ds:f_NaN
                        retn
        _dtoi3_sat      endp


                        public _dtoui3_sat
        _dtoui3_sat     proc near
                        xor     eax, eax
                        ucomisd xmm0, ds:d_negone
                        jbe     short dtouis_except
                        ucomisd xmm0, ds:d_two31
                        jnb     short dtouis_big
                        cvttsd2si eax, xmm0
                        retn
        dtouis_big:
                        dec     eax
                        ucomisd xmm0, ds:d_two32
                        jnb     short dtouis_except
                        movd    eax, xmm0
                        psrldq  xmm0, 4
                        movd    edx, xmm0
                        test    eax, 1FFFFFh
                        jz      short dtoui3s_Exact
                        cvttss2si ecx, ds:f_onept5
        dtoui3s_Exact:
                        shrd    eax, edx, 15h
                        bts     eax, 1Fh
                        retn
        dtouis_except:
                        cvttss2si ecx, ds:f_NaN
                        retn
        _dtoui3_sat     endp


                        public _dtol3_sat
        _dtol3_sat      proc near
                        xor     eax, eax
                        xor     edx, edx
                        ucomisd xmm0, ds:d_two31
                        jp      short dtols_except
                        jnb     short dtols_big
                        ucomisd xmm0, ds:d_negtwo31
                        jb      short dtols_veryneg
                        cvttsd2si eax, xmm0
                        cdq
                        retn
        dtols_veryneg:
                        mov     edx, 80000000h
                        ucomisd xmm0, ds:d_negtwo63
                        jz      short dtols_ret
                        jb      short dtols_except
                        call    dtol3s_getbits
                        neg     eax
                        adc     edx, 0
                        neg     edx
                        retn
        dtols_except:
                        cvttss2si ecx, ds:f_NaN
        
        dtols_ret:
                        retn
        dtols_big:
                        mov     edx, 7FFFFFFFh
                        dec     eax
                        ucomisd xmm0, ds:d_two63
                        jnb     short dtols_except
        dtol3s_getbits:
                        movd    eax, xmm0
                        psrldq  xmm0, 4
                        movd    ecx, xmm0
                        btr     ecx, 1Fh
                        shr     ecx, 14h
                        sub     ecx, 433h
                        jge     short dtol3s_lshift
                        neg     ecx
                        xor     edx, edx
                        shrd    edx, eax, cl
                        jz      short dtol3s_exact
                        cvttss2si edx, ds:f_onept5
        dtol3s_exact:
                        movd    edx, xmm0
                        bts     edx, 14h
                        and     edx, 1FFFFFh
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        dtol3s_lshift:
                        movd    edx, xmm0
                        bts     edx, 14h
                        and     edx, 1FFFFFh
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _dtol3_sat      endp


                        public _dtoul3_sat
        _dtoul3_sat     proc near
                        xor     eax, eax
                        xor     edx, edx
                        ucomisd xmm0, ds:d_negone
                        jbe     short dtoul3s_except
                        ucomisd xmm0, ds:d_two31
                        jnb     short dtoul3s_big
                        cvttsd2si eax, xmm0
                        retn
        dtoul3s_big:
                        dec     eax
                        dec     edx
                        ucomisd xmm0, ds:d_two64
                        jnb     short dtoul3s_except
                        movd    eax, xmm0
                        psrldq  xmm0, 4
                        movd    ecx, xmm0
                        shr     ecx, 14h
                        sub     ecx, 433h
                        jge     short dtoul3s_bigshift
                        neg     ecx
                        xor     edx, edx
                        shrd    edx, eax, cl
                        jz      short dtoul3s_exact
                        cvttss2si edx, ds:f_onept5
        dtoul3s_exact:
                        movd    edx, xmm0
                        bts     edx, 14h
                        and     edx, 1FFFFFh
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        dtoul3s_bigshift:
                        movd    edx, xmm0
                        bts     edx, 14h
                        and     edx, 1FFFFFh
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        dtoul3s_except:
                        cvttss2si ecx, ds:f_NaN
                        retn
        _dtoul3_sat     endp


        end
