        page    ,132
        title   ftol3    - truncate TOS to 32-bit integer
;*** 
;ftol3.asm - truncate TOS to 32-bit integer
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

                        ; _bias_low xmmword 4330000000000000h
                        _bias_low       dq 4330000000000000h
                        align 16

                        ; _bias_high xmmword 4530000000000000h
                        _bias_high      dq 4530000000000000h
                        align 16

                        _DP2to32        dq 41F0000000000000h
                        _NaN            dd 0FFFFFFFFh
                        _onept5         dd 3FC00000h
        _rdata  ends


        CODESEG


                        public _ftoui3
        _ftoui3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ftoui3_default
                        vcvttss2usi eax, xmm0
                        retn
        _ftoui3_default:
                        movd    eax, xmm0
                        shl     eax, 1
                        jb      short _ftoui3_neg
                        cmp     eax, 9E000000h
                        jnb     short _ftoui3_notsafe
        
        _ftoui3_cvt:
                        cvttss2si eax, xmm0
                        retn
        _ftoui3_notsafe:
                        cmp     eax, 9F000000h
                        jnb     short _ftoui3_NaN
                        shl     eax, 7
                        bts     eax, 1Fh
                        retn
        _ftoui3_neg:
                        cmp     eax, 7F000000h
                        jb      short _ftoui3_cvt
        _ftoui3_NaN:
                        cvttss2si ecx, ds:_NaN
                        cmc
                        sbb     eax, eax
                        retn
        _ftoui3         endp


                        public _ftoul3
        _ftoul3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ftoul3_default
                        vbroadcastss xmm0, xmm0
                        vcvttps2uqq xmm0, xmm0
                        vmovd   eax, xmm0
                        vpextrd edx, xmm0, 1
                        retn
        _ftoul3_default:
                        movd    eax, xmm0
                        shl     eax, 1
                        jb      short _ftoul3_neg
                        cmp     eax, 9E000000h
                        jnb     short _ftoul3_notsafe
        _ftoul3_cvt:
                        cvttss2si eax, xmm0
                        xor     edx, edx
                        retn
        _ftoul3_notsafe:
                        cmp     eax, 0BF000000h
                        jnb     short _ftoul3_NaN
                        mov     ecx, eax
                        bts     eax, 18h
                        shr     ecx, 18h
                        shl     eax, 7
                        sub     cl, 0BEh
                        jns     short _ftoul3_bigshift
                        xor     edx, edx
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _ftoul3_bigshift:
                        mov     edx, eax
                        xor     eax, eax
                        retn
        _ftoul3_neg:
                        cmp     eax, 7F000000h
                        jb      short _ftoul3_cvt
        _ftoul3_NaN:
                        cvttss2si ecx, ds:_NaN
                        cmc
                        sbb     eax, eax
                        cdq
                        retn
        _ftoul3         endp


                        public _ftoul3_legacy
        _ftoul3_legacy proc near
                        movd    eax, xmm0
                        cmp     eax, 4F000000h
                        jnb     short _ftoul3l_notsafe
                        cvttss2si eax, xmm0
                        xor     edx, edx
                        retn
        _ftoul3l_notsafe:
                        cmp     eax, 5F800000h
                        jnb     _ftol3
                        jmp     _ftoul3
        _ftoul3_legacy  endp


                        public _ftol3
        _ftol3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ftol3_default
                        vbroadcastss xmm0, xmm0
                        vcvttps2qq xmm0, xmm0
                        vmovd   eax, xmm0
                        vpextrd edx, xmm0, 1
                        retn
        _ftol3_default:
                        movd    eax, xmm0
                        cdq
                        shl     eax, 1
                        cmp     eax, 9E000000h
                        jnb     short _ftol3_notsafe
                        cvttss2si eax, xmm0
                        cdq
                        retn
        _ftol3_notsafe:
                        cmp     eax, 0BE000000h
                        jnb     short _ftol3_mincheck
                        mov     ecx, eax
                        bts     eax, 18h
                        shr     ecx, 18h
                        shl     eax, 7
                        sub     cl, 0BEh
                        xor     eax, edx
                        sub     eax, edx
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _ftol3_mincheck:
                        jnz     short _ftol3_NaN
                        test    edx, edx
                        js      short _ftol3_retmin
        _ftol3_NaN:
                        cvttss2si ecx, ds:_NaN
        _ftol3_retmin:
                        mov     edx, 80000000h
                        xor     eax, eax
                        retn
        _ftol3          endp


                        public _dtoui3
        _dtoui3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _dtoui3_default
                        vcvttsd2usi eax, xmm0
                        retn
        _dtoui3_default:
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        movsd   qword ptr [esp], xmm0
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        mov     esp, ecx
                        btr     edx, 1Fh
                        jb      short _dtoui3_neg
                        cmp     edx, 41E00000h
                        jnb     short _dtoui3_notsafe
                        cvttsd2si eax, xmm0
                        retn
        _dtoui3_notsafe:
                        cmp     edx, 41F00000h
                        jnb     short _dtoui3_NaN
                        test    eax, 1FFFFFh
                        jz      short _dtoui3_exact
                        cvttss2si ecx, ds:_onept5
        _dtoui3_exact:
                        shrd    eax, edx, 15h
                        bts     eax, 1Fh
                        retn
        _dtoui3_neg:
                        cmp     edx, 3FF00000h
                        jnb     short _dtoui3_NaN
                        cvttsd2si eax, xmm0
                        xor     eax, eax
                        retn
        _dtoui3_NaN:
                        cvttss2si ecx, ds:_NaN
                        xor     eax, eax
                        dec     eax
                        retn
        _dtoui3         endp


                        public _dtoul3
        _dtoul3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _dtoul3_default
                        vmovq   xmm0, xmm0
                        vcvttpd2uqq xmm0, xmm0
                        vmovd   eax, xmm0
                        vpextrd edx, xmm0, 1
                        retn
        _dtoul3_default:
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        movsd   qword ptr [esp], xmm0
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        mov     esp, ecx
                        btr     edx, 1Fh
                        jb      short _dtoul3_neg
                        cmp     edx, 41E00000h
                        jnb     short _dtoul3_notsafe
                        cvttsd2si eax, xmm0
                        xor     edx, edx
                        retn
        _dtoul3_notsafe:
                        mov     ecx, edx
                        bts     edx, 14h
                        shr     ecx, 14h
                        and     edx, 1FFFFFh
                        sub     ecx, 433h
                        jge     short _dtoul3_large
                        neg     ecx
                        push    ebx
                        xor     ebx, ebx
                        shrd    ebx, eax, cl
                        jz      short _dtoul3_exact
                        cvttss2si ebx, ds:_onept5
        _dtoul3_exact:
                        pop     ebx
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        _dtoul3_large:
                        cmp     ecx, 0Ch
                        jnb     short _dtoul3_NaN
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _dtoul3_neg:
                        cmp     edx, 3FF00000h
                        jnb     short _dtoul3_NaN
                        cvttsd2si eax, xmm0
                        xor     eax, eax
                        xor     edx, edx
                        retn
        _dtoul3_NaN:
                        cvttss2si ecx, ds:_NaN
                        xor     eax, eax
                        dec     eax
                        cdq
                        retn
        _dtoul3         endp


                        public _dtoul3_legacy
        _dtoul3_legacy proc near
                        add     esp, 0FFFFFFF8h
                        movsd   qword ptr [esp], xmm0
                        mov     edx, [esp+4]
                        add     esp, 8
                        cmp     edx, 41E00000h
                        jnb     short _dtoul3l_notsafe
                        cvttsd2si eax, xmm0
                        xor     edx, edx
                        retn
        _dtoul3l_notsafe:
                        cmp     edx, 43F00000h
                        jnb     _dtol3
                        jmp     _dtoul3
        _dtoul3_legacy  endp


                        public _dtol3
        _dtol3          proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _dtol3_default
                        vmovq   xmm0, xmm0
                        vcvttpd2qq xmm0, xmm0
                        vmovd   eax, xmm0
                        vpextrd edx, xmm0, 1
                        retn
        _dtol3_default:
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        movsd   qword ptr [esp], xmm0
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        mov     esp, ecx
                        btr     edx, 1Fh
                        sbb     ecx, ecx
                        cmp     edx, 41E00000h
                        jnb     short _dtol3_notsafe
                        cvttsd2si eax, xmm0
                        cdq
                        retn
        _dtol3_getbits:
                        mov     ecx, edx
                        bts     edx, 14h
                        shr     ecx, 14h
                        and     edx, 1FFFFFh
                        sub     ecx, 433h
                        jge     short _dtol3_lshift
                        neg     ecx
                        push    ebx
                        xor     ebx, ebx
                        shrd    ebx, eax, cl
                        jz      short _dtol3_trunc
                        cvttss2si ebx, ds:_onept5
        _dtol3_trunc:
                        pop     ebx
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        _dtol3_lshift:
                        shld    edx, eax, cl
                        shl     eax, cl
                        retn
        _dtol3_notsafe:
                        cmp     edx, 43E00000h
                        jnb     short _dtol3_large
                        test    ecx, ecx
                        jz      short _dtol3_getbits
                        call    _dtol3_getbits
                        neg     eax
                        adc     edx, 0
                        neg     edx
                        retn
        _dtol3_large:
                        jecxz   short _dtol3_NaN
                        ja      short _dtol3_NaN
                        test    eax, eax
                        jz      short _dtol3_retmin
        
        _dtol3_NaN:
                        cvttss2si ecx, ds:_NaN
        
        _dtol3_retmin:
                        mov     edx, 80000000h
                        xor     eax, eax
                        retn
        _dtol3          endp


                        public _ultod3
        _ultod3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ultod3_default
                        vmovd   xmm0, ecx
                        vpinsrd xmm0, xmm0, edx, 1
                        vcvtuqq2pd xmm0, xmm0
                        retn
        _ultod3_default:
                        movd    xmm0, ecx
                        orps    xmm0, xmmword ptr ds:_bias_low
                        subsd   xmm0, qword ptr ds:_bias_low
                        test    edx, edx
                        jz      short _ultod3_uint32
                        movd    xmm1, edx
                        orps    xmm1, xmmword ptr ds:_bias_high
                        subsd   xmm1, qword ptr ds:_bias_high
                        addsd   xmm0, xmm1
        _ultod3_uint32:
                        retn
        _ultod3         endp


                        public _ltod3
        _ltod3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ltod3_default
                        vmovd   xmm0, ecx
                        vpinsrd xmm0, xmm0, edx, 1
                        vcvtqq2pd xmm0, xmm0
                        retn
        _ltod3_default:
                        movd    xmm0, ecx
                        orps    xmm0, xmmword ptr ds:_bias_low
                        subsd   xmm0, qword ptr ds:_bias_low
                        test    edx, edx
                        jz      short _ltod3_lt33
                        cvtsi2sd xmm1, edx
                        mulsd   xmm1, ds:_DP2to32
                        addsd   xmm0, xmm1
        _ltod3_lt33:
                        retn
        _ltod3          endp


                        public _ultof3
        _ultof3         proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ultof3_default
                        vmovd   xmm0, ecx
                        vpinsrd xmm0, xmm0, edx, 1
                        vcvtuqq2ps xmm0, xmm0
                        retn
        _ultof3_default:
                        test    edx, edx
                        js      short _ultof3_mt63
                        jnz     short _ultof3_mt31
                        test    ecx, ecx
                        js      short _ultof3_mt31
                        cvtsi2ss xmm0, ecx
                        retn
        _ultof3_mt31:
                        mov     eax, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        mov     [esp], ecx
                        mov     [esp+4], edx
                        fild    qword ptr [esp]
                        fstp    dword ptr [esp]
                        movss   xmm0, dword ptr [esp]
                        mov     esp, eax
                        retn
        _ultof3_mt63:
                        push    ebp
                        mov     ebp, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        mov     eax, ecx
                        and     eax, 1
                        shrd    ecx, edx, 1
                        or      ecx, eax
                        shr     edx, 1
                        mov     [esp], ecx
                        mov     [esp+4], edx
                        fild    qword ptr [esp]
                        fstp    dword ptr [esp]
                        movss   xmm0, dword ptr [esp]
                        addss   xmm0, xmm0
                        mov     esp, ebp
                        pop     ebp
                        retn
        _ultof3         endp


                        public _ltof3
        _ltof3          proc near
                        cmp     ds:__isa_available, __ISA_AVAILABLE_AVX512
                        jl      short _ltof3_default
                        vmovd   xmm0, ecx
                        vpinsrd xmm0, xmm0, edx, 1
                        vcvtqq2ps xmm0, xmm0
                        retn
        _ltof3_default:
                        bt      ecx, 1Fh
                        sbb     eax, eax
                        cmp     eax, edx
                        jnz     short _ltof3_mt32
                        cvtsi2ss xmm0, ecx
                        retn
        _ltof3_mt32:
                        mov     eax, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        mov     [esp], ecx
                        mov     [esp+4], edx
                        fild    qword ptr [esp]
                        fstp    dword ptr [esp]
                        movss   xmm0, dword ptr [esp]
                        mov     esp, eax
                        retn
        _ltof3          endp


        end
