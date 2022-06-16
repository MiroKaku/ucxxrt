    page    ,132
    title   ftol3 - truncate TOS to 32-bit integer

.xlist
        include cruntime.inc
.list

        CODESEG

; Segment type: Zero-length
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
_text$mn        ends

; Segment type: Zero-length
; Segment permissions: Read/Write
_data           segment para public 'DATA' use32
_data           ends

; Segment type: Pure data
; Segment permissions: Read
_rdata          segment para public 'DATA' use32
                assume cs:_rdata

_NaN            dd 0FFFFFFFFh
_onept5         dd 3FC00000h
_DP2to32        dq 0
qword_18        dq 41F0000000000000h

_rdata          ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftoui3
_ftoui3        proc near
                cmp     dword ptr ds:__isa_available, 6
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
_ftoui3        endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftoul3
_ftoul3        proc near
                cmp     dword ptr ds:__isa_available, 6
                jl      short _ftoul3_default
                mov     eax, 1
                kmovb   k1, eax
                vcvttps2uqq xmm0{k1}{z}, xmm0
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
_ftoul3        endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftol3
_ftol3         proc near
                cmp     dword ptr ds:__isa_available, 6
                jl      short _ftol3_default
                mov     eax, 1
                kmovb   k1, eax
                vcvttps2qq xmm0{k1}{z}, xmm0
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
_ftol3         endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _dtoui3
_dtoui3        proc near
                cmp     dword ptr ds:__isa_available, 6
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
_dtoui3        endp
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _dtoul3
_dtoul3        proc near
                cmp     dword ptr ds:__isa_available, 6
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
_dtoul3        endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _dtol3
_dtol3         proc near
                cmp     dword ptr ds:__isa_available, 6
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
_dtol3         endp

_dtol3_getbits  proc near
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
_dtol3_getbits  endp

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

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ultod3
_ultod3        proc near
                cmp     dword ptr ds:__isa_available, 6
                jl      short _ultod3_default
                vmovd   xmm0, ecx
                vpinsrd xmm0, xmm0, edx, 1
                vcvtuqq2pd xmm0, xmm0
                retn
_ultod3_default:
                xorps   xmm0, xmm0
                cvtsi2sd xmm0, ecx
                shr     ecx, 1Fh
                addsd   xmm0, ds:_DP2to32[ecx*8]
                test    edx, edx
                jz      short _ultod3_uint32
                xorps   xmm1, xmm1
                cvtsi2sd xmm1, edx
                shr     edx, 1Fh
                addsd   xmm1, ds:_DP2to32[edx*8]
                mulsd   xmm1, ds:qword_18
                addsd   xmm0, xmm1
_ultod3_uint32:
                retn
_ultod3        endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ltod3
_ltod3         proc near
                cmp     dword ptr ds:__isa_available, 6
                jl      short _ltod3_default
                vmovd   xmm0, ecx
                vpinsrd xmm0, xmm0, edx, 1
                vcvtqq2pd xmm0, xmm0
                retn
_ltod3_default:
                xorps   xmm1, xmm1
                cvtsi2sd xmm1, edx
                xorps   xmm0, xmm0
                cvtsi2sd xmm0, ecx
                shr     ecx, 1Fh
                mulsd   xmm1, ds:qword_18
                addsd   xmm0, ds:_DP2to32[ecx*8]
                addsd   xmm0, xmm1
                retn
_ltod3         endp
_text$mn        ends

; Segment type: Externs
                extrn __isa_available:dword

                end
