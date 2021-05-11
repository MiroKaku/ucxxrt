        page    ,132
        title   ftol2    - truncate TOS to 32-bit integer

;/*
; * PROJECT:   Universal C++ RunTime (UCXXRT)
; * FILE:      vcruntime.inc
; * DATA:      2021/04/25
; *
; * PURPOSE:   Universal C++ RunTime
; *
; * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
; *
; * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
; */


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

two32           dd 4F800000h
two63           dd 5F000000h
_NaN            dd 0FFFFFFFFh

_rdata          ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftol2_sse_excpt
_ftol2_sse_excpt proc near
                jmp     _ftoi2
_ftol2_sse_excpt endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftol2_sse
_ftol2_sse     proc near
                jmp     _ftoi2
_ftol2_sse     endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftoi2
_ftoi2         proc near
                cmp     dword ptr ds:__isa_available, 2
                jl      short L3B
                sub     esp, 4
                fisttp  dword ptr [esp]
                pop     eax
                retn
L3B:
                push    ebp
                mov     ebp, esp
                add     esp, 0FFFFFFF0h
                and     esp, 0FFFFFFF0h
                fld     st
                fstp    tbyte ptr [esp]
                mov     eax, [esp+4]
                movzx   ecx, word ptr [esp+8]
                btr     ecx, 0Fh
                sbb     edx, edx
                cmp     cx, 3FFFh
                jb      short L3F
                test    eax, eax
                jns     short L41
                cmp     cx, 401Eh
                jnb     short L40
                neg     cx
                add     cx, 403Eh
                frndint
                fstp    st
                shr     eax, cl
                xor     eax, edx
                sub     eax, edx
                leave
                retn
L3F:
                frndint
                fstp    st
                xor     eax, eax
                leave
                retn
L40:
                ja      short L41
                test    edx, edx
                jns     short L41
                cmp     eax, 80000000h
                jnz     short L41
                frndint
                fstp    st
                leave
                retn
L41:
                fcomp   ds:_NaN
                leave
                mov     eax, 80000000h
                retn
_ftoi2         endp
_text$mn        ends


; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing
                
                public _ftoui2
_ftoui2        proc near
                cmp     dword ptr ds:__isa_available, 2
                jl      short L93
                fldz
                fucomip st, st(1)
                jp      short L91
                ja      short L90
                fld     ds:two32
                fucomip st, st(1)
                jbe     short L91
L8F:
                mov     ecx, esp
                add     esp, 0FFFFFFF8h
                and     esp, 0FFFFFFF8h
                fisttp  qword ptr [esp]
                mov     eax, [esp]
                mov     esp, ecx
                retn
L90:
                fabs
                fld1
                fucomip st, st(1)
                jbe     short L91
                fisttp  dword ptr [esp-4]
                xor     eax, eax
                retn
L91:
                fcomp   ds:_NaN
L92:
                mov     eax, 0FFFFFFFFh
                retn
L93:
                push    ebp
                mov     ebp, esp
                add     esp, 0FFFFFFF0h
                and     esp, 0FFFFFFF0h
                fld     st
                fstp    tbyte ptr [esp]
                mov     eax, [esp+4]
                movzx   ecx, word ptr [esp+8]
loc_FE:
                btr     ecx, 0Fh
                jb      short L96
                cmp     cx, 3FFFh
                jb      short L97
                test    eax, eax
                jns     short L99
                cmp     cx, 401Fh
                jnb     short L99
                neg     cx
                add     cx, 403Eh
                frndint
                fstp    st
                shr     eax, cl
                leave
                retn
L96:
                cmp     cx, 3FFFh
                jnb     short L99
L97:
                frndint
                fstp    st
                xor     eax, eax
                leave
                retn
L99:
                fcomp   ds:_NaN
                leave
                mov     eax, 0FFFFFFFFh
                retn
_ftoui2        endp

                align 4
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftol2
_ftol2         proc near
                cmp     dword ptr ds:__isa_available, 2
                jl      short L51
                mov     ecx, esp
                add     esp, 0FFFFFFF8h
                and     esp, 0FFFFFFF8h
                fisttp  qword ptr [esp]
                mov     eax, [esp]
                mov     edx, [esp+4]
                mov     esp, ecx
                retn
L51:
                push    ebp
                mov     ebp, esp
                add     esp, 0FFFFFFF0h
                and     esp, 0FFFFFFF0h
                fld     st
                fstp    tbyte ptr [esp]
                mov     eax, [esp]
                mov     edx, [esp+4]
                movzx   ecx, word ptr [esp+8]
                btr     ecx, 0Fh
                cmp     cx, 3FFFh
                jb      short L55
                test    edx, edx
                jns     short L57
                cmp     cx, 403Eh
                jnb     short L56
                neg     cx
                add     cx, 403Eh
                frndint
                fstp    st
                cmp     cl, 20h
                jb      short L52
                mov     eax, edx
                xor     edx, edx
L52:
                shrd    eax, edx, cl
                shr     edx, cl
                cmp     word ptr [esp+8], 0
                jge     short L53
                neg     eax
                adc     edx, 0
                neg     edx
L53:
                leave
                retn
L55:
                frndint
                fstp    st
                xor     eax, eax
                xor     edx, edx
                leave
                retn
L56:
                lea     ecx, ds:0[edx*2]
                ja      short L57
                or      ecx, eax
                jnz     short L57
                cmp     word ptr [esp+8], 0
                jge     short L57
                fstp    st
                leave
                retn
L57:
                fcomp   ds:_NaN
                leave
                mov     edx, 80000000h
                xor     eax, eax
                retn
_ftol2         endp

                align 10h
_text$mn        ends

; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

                public _ftoul2
_ftoul2        proc near
                cmp     dword ptr ds:__isa_available, 2
                jl      short LA9
                fldz
                fucomip st, st(1)
                jp      short LA7
                ja      short LA6
                fld     ds:two63
                fucomip st, st(1)
                jbe     short LA9
LA5:
                mov     ecx, esp
                add     esp, 0FFFFFFF8h
                and     esp, 0FFFFFFF8h
                fisttp  qword ptr [esp]
                mov     eax, [esp]
                mov     edx, [esp+4]
                mov     esp, ecx
                retn
LA6:
                fabs
                fld1
                fucomip st, st(1)
                jbe     short LA7
                fisttp  dword ptr [esp-4]
                xor     eax, eax
                retn
LA7:
                fcomp   ds:_NaN
                mov     eax, 0FFFFFFFFh
                cdq
                retn
LA9:
                push    ebp
                mov     ebp, esp
                add     esp, 0FFFFFFF0h
                and     esp, 0FFFFFFF0h
                fld     st
                fstp    tbyte ptr [esp]
                mov     eax, [esp]
                mov     edx, [esp+4]
                movzx   ecx, word ptr [esp+8]
                btr     ecx, 0Fh
                jb      short LAC
                cmp     cx, 3FFFh
                jb      short LAD
                test    edx, edx
                jns     short LAF
                cmp     cx, 403Fh
                jnb     short LAF
                neg     cx
                add     cx, 403Eh
                frndint
                fstp    st
                cmp     cl, 20h
                jb      short LAA
                mov     eax, edx
                xor     edx, edx
LAA:
                shrd    eax, edx, cl
                shr     edx, cl
                leave
                retn
LAC:
                cmp     cx, 3FFFh
                jnb     short LAF
LAD:
                frndint
                fstp    st
                xor     eax, eax
                xor     edx, edx
                leave
                retn
LAF:
                fcomp   ds:_NaN
                leave
                mov     eax, 0FFFFFFFFh
                cdq
                retn
_ftoul2        endp
_text$mn        ends

; Segment type: Externs
                extrn __isa_available:dword

                end
