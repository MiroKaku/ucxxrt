        page    ,132
        title   ftol2sat    - truncate TOS to 32-bit integer
;*** 
;ftol2sat.asm - truncate TOS to 32-bit integer
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

                        public _ftoi2_sat
        _ftoi2_sat      proc near
                        fcomi   st, st
                        jp      short ftoi2s_Nan
                        push    ebp
                        mov     ebp, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        fld     st
                        fstp    tbyte ptr [esp]
                        mov     edx, [esp]
                        mov     eax, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        leave
                        cmp     cx, 401Eh
                        jge     short ftoi2s_Max
                        cmp     cx, 0C01Eh
                        jnb     short ftoi2s_CheckMin
                        btr     ecx, 0Fh
                        sbb     edx, edx
                        cmp     cx, 3FFFh
                        jb      short ftoi2s_RetSmall
                        test    eax, eax
                        jns     short ftoi2s_Nan
                        neg     cl
                        add     cl, 3Eh
                        frndint
                        fstp    st
                        shr     eax, cl
                        xor     eax, edx
                        sub     eax, edx
                        retn
        ftoi2s_RetSmall:
                        frndint
                        fstp    st
                        xor     eax, eax
                        retn
        ftoi2s_CheckMin:
                        ja      short ftoi2s_Min
                        lea     ecx, ds:0[eax*2]
                        or      ecx, edx
                        jz      short ftoi2s_RetMin
        ftoi2s_Min:
                        fcomp   ds:_NaN
        ftoi2s_RetMin:
                        mov     eax, 80000000h
                        retn
        ftoi2s_Max:
                        fcomp   ds:_NaN
                        mov     eax, 7FFFFFFFh
                        retn
        ftoi2s_Nan:
                        fcomp   ds:_NaN
                        xor     eax, eax
                        retn
        _ftoi2_sat      endp


                        public _ftoui2_sat
        _ftoui2_sat     proc near
                        fldz
                        fucomip st, st(1)
                        jp      short ftoui2s_Min
                        ja      short ftoui2s_Neg
                        fld     ds:two32
                        fucomip st, st(1)
                        jbe     short ftoui2s_Max
                        cmp     ds:__isa_available, __ISA_AVAILABLE_SSE42
                        jl      short ftoui2s_Default
                        mov     ecx, esp
                        add     esp, 0FFFFFFF8h
                        and     esp, 0FFFFFFF8h
                        fisttp  qword ptr [esp]
                        mov     eax, [esp]
                        mov     esp, ecx
                        retn
        ftoui2s_Default:
                        push    ebp
                        mov     ebp, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        fld     st
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        leave
                        cmp     cx, 3FFFh
                        jb      short ftoui2s_RetSmall
                        test    eax, eax
                        jns     short ftoui2s_Min
                        cmp     cx, 401Fh
                        jnb     short ftoui2s_Max
                        neg     cx
                        add     cx, 403Eh
                        frndint
                        fstp    st
                        shr     eax, cl
                        retn
        ftoui2s_RetSmall:
                        frndint
                        fstp    st
                        xor     eax, eax
                        retn
        ftoui2s_Neg:
                        fabs
                        fld1
                        fucomip st, st(1)
                        ja      short ftoui2s_RetSmall
        ftoui2s_Min:
                        fcomp   ds:_NaN
                        xor     eax, eax
                        retn
        ftoui2s_Max:
                        fcomp   ds:_NaN
                        or      eax, 0FFFFFFFFh
                        retn
        _ftoui2_sat     endp


                        public _ftol2_sat
        _ftol2_sat      proc near
                        fcomi   st, st
                        jp      ftol2s_Nan
                        push    ebp
                        mov     ebp, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        fld     st
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        movsx   ecx, word ptr [esp+8]
                        leave
                        btr     ecx, 0Fh
                        cmp     cx, 3FFFh
                        jb      short ftol2s_RetSmall
                        test    edx, edx
                        jns     short ftol2s_Nan
                        cmp     cx, 403Eh
                        jnb     short ftol2s_Large
                        neg     cl
                        add     cl, 3Eh ; '>'
                        frndint
                        fstp    st
                        cmp     cl, 20h ; ' '
                        jb      short ftol2s_ShftLT32
                        mov     eax, edx
                        xor     edx, edx
        ftol2s_ShftLT32:
                        shrd    eax, edx, cl
                        shr     edx, cl
                        test    ecx, ecx
                        jns     short ftol2s_RetPos
                        neg     eax
                        adc     edx, 0
                        neg     edx
        ftol2s_RetPos:
                        retn
        ftol2s_RetSmall:
                        frndint
                        fstp    st
                        xor     eax, eax
                        xor     edx, edx
                        retn
        ftol2s_Large:
                        cmp     ecx, 0FFFF403Eh
                        jb      short ftol2s_Max
                        jnz     short ftol2s_Min
                        lea     ecx, ds:0[edx*2]
                        or      ecx, eax
                        jnz     short ftol2s_Min
                        fstp    st
                        retn
        ftol2s_Min:
                        fcomp   ds:_NaN
                        mov     edx, 80000000h
                        xor     eax, eax
                        retn
        ftol2s_Max:
                        fcomp   ds:_NaN
                        mov     edx, 7FFFFFFFh
                        or      eax, 0FFFFFFFFh
                        retn
        ftol2s_Nan:
                        fcomp   ds:_NaN
                        xor     edx, edx
                        xor     eax, eax
                        retn
        _ftol2_sat      endp


                        public _ftoul2_sat
        _ftoul2_sat     proc near
                        fldz
                        fucomip st, st(1)
                        jp      short ftoul2s_Min
                        ja      short ftoul2s_Neg
                        push    ebp
                        mov     ebp, esp
                        add     esp, 0FFFFFFF0h
                        and     esp, 0FFFFFFF0h
                        fld     st
                        fstp    tbyte ptr [esp]
                        mov     eax, [esp]
                        mov     edx, [esp+4]
                        movzx   ecx, word ptr [esp+8]
                        leave
                        cmp     cx, 3FFFh
                        jb      short ftol2s_RetSmall_0
                        test    edx, edx
                        jns     short ftoul2s_Min
                        cmp     cx, 403Fh
                        jnb     short ftoul2s_Max
                        neg     cx
                        add     cx, 403Eh
                        frndint
                        fstp    st
                        cmp     cl, 20h ; ' '
                        jb      short ftoul2s_ShftLT32
                        mov     eax, edx
                        xor     edx, edx
        
        ftoul2s_ShftLT32:
                        shrd    eax, edx, cl
                        shr     edx, cl
                        retn
        ftol2s_RetSmall_0:
                        frndint
                        fstp    st
                        xor     eax, eax
                        xor     edx, edx
                        retn
        ftoul2s_Neg:
                        fabs
                        fld1
                        fucomip st, st(1)
                        ja      short ftol2s_RetSmall_0
        
        ftoul2s_Min:
                        fcomp   ds:_NaN
                        xor     eax, eax
                        xor     edx, edx
                        retn
        ftoul2s_Max:
                        fcomp   ds:_NaN
                        or      eax, 0FFFFFFFFh
                        or      edx, 0FFFFFFFFh
                        retn
        _ftoul2_sat     endp


        end
