;/*
; * PROJECT:   Universal C++ RunTime (UCXXRT)
; * FILE:      exsup.asm
; * DATA:      2021/05/14
; *
; * PURPOSE:   Universal C++ RunTime
; *
; * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
; *
; * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
; */

;***
;exsup.asm
;
;   Copyright (c) 1993-2001, Microsoft Corporation. All rights reserved.
;
;Purpose:
;   Exception handling for i386.  This file contains those routines
;   common to both C8.0 and C9.0.
;
;*******************************************************************************

.686p
.mmx
.model flat

.xlist
include exsup.inc
.list



;typedef struct _EXCEPTION_REGISTRATION PEXCEPTION_REGISTRATION;
;struct _EXCEPTION_REGISTRATION{
;     struct _EXCEPTION_REGISTRATION *prev;
;     void (*handler)(PEXCEPTION_RECORD, PEXCEPTION_REGISTRATION, PCONTEXT, PEXCEPTION_RECORD);
;     struct scopetable_entry *scopetable;
;     int trylevel;
;};
_EXCEPTION_REGISTRATION_COMMON struc    ; C8.0/C9.0 common only
    prev                dd      ?       ; prev (OS-req, def'd in exsup.inc)
    handler             dd      ?       ; handler (ditto)
;private:
    scopetable          dd      ?       ; C8/C9 common
    trylevel            dd      ?       ; C8/C9 common
_EXCEPTION_REGISTRATION_COMMON ends

;#define EXCEPTION_MAXIMUM_PARAMETERS 4
;typedef struct _EXCEPTION_RECORD EXCEPTION_RECORD;
;typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;
;struct _EXCEPTION_RECORD{
;    NTSTATUS ExceptionCode;
;    ULONG ExceptionFlags;
;    struct _EXCEPTION_RECORD *ExceptionRecord;
;    PVOID ExceptionAddress;
;    ULONG NumberParameters;
;    ULONG ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
;};
_EXCEPTION_RECORD struc
    exception_number    dd      ?
    exception_flags     dd      ?
    exception_record    dd      ?
    exception_address   dd      ?
    number_parameters   dd      ?
    exception_information dd 4 dup(?)
_EXCEPTION_RECORD ends
SIZEOF_EXCEPTION_RECORD equ 36



public __except_list
__except_list equ 0



;struct _SCOPETABLE_ENTRY{
;     int enclosing_level;              /* lexical level of enclosing scope */
;     int (*filter)(PEXCEPTION_RECORD); /* NULL for a termination handler */
;     void (*specific_handler)(void);   /* xcpt or termination handler */
;};
;struct _SCOPETABLE_ENTRY Scopetable[NUMTRYS];
_SCOPETABLE_ENTRY struc
    enclosing_level     dd      ?
    filter              dd      ?
    specific_handler    dd      ?
_SCOPETABLE_ENTRY ends



; Segment type: Zero-length
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32

extrn _RtlUnwind@16:near
extrn ___security_cookie:near
extrn @__security_check_cookie@4:near

_text$mn        ends



; Segment type: Pure data
; Segment permissions: Read/Write

_data           segment para public 'DATA' use32
                assume cs:_data

__NLG_Destination _NLG_INFO <>
PUBLIC __NLG_Destination

_data           ends


; Segment type: Pure code
; Segment permissions: Read/Execute
_text$mn        segment para public 'CODE' use32
                assume cs:_text$mn
                assume es:nothing, ss:nothing, ds:_data, fs:nothing, gs:nothing

; int __cdecl _global_unwind2(PVOID TargetFrame)
                public __global_unwind2
__global_unwind2 proc near

TargetFrame     = dword ptr  8

                push    ebp
                mov     ebp, esp
                push    ebx
                push    esi
                push    edi
                push    ebp
                push    0                           ; ReturnValue
                push    0                           ; ExceptionRecord
                push    offset flat:_gu_return      ; TargetIp
                push    dword ptr [ebp+TargetFrame] ; TargetFrame

                call    _RtlUnwind@16
_gu_return:
                pop     ebp
                pop     edi
                pop     esi
                pop     ebx
                mov     esp, ebp
                pop     ebp
                retn
__global_unwind2 endp



;_unwind_handler(
;  PEXCEPTION_RECORD xr,
;  PREGISTRATION_RECORD establisher,
;  PCONTEXT context,
;  PREGISTRATION_RECORD dispatcher);
;
;this is a special purpose handler used to guard our local unwinder.
; its job is to catch collided unwinds.
;
;NB: this code is basically stolen from the NT routine xcptmisc.asm
; and is basically the same method used by the system unwinder (RtlUnwind).
;

__unwind_handler proc near

                mov     ecx, dword ptr [esp+4]
                test    dword ptr [ecx + _EXCEPTION_RECORD.exception_flags], EXCEPTION_UNWIND_CONTEXT
                mov     eax, DISPOSITION_CONTINUE_SEARCH
                jz      short _uh_return

                mov     eax, [esp+20]
                mov     ecx, [eax-4]
                xor     ecx, eax
                call    @__security_check_cookie@4  ; __security_check_cookie(x)
                push    ebp
                mov     ebp, [eax+16]
                mov     edx, [eax+40]
                push    edx
                mov     edx, [eax+36]
                push    edx
                call    __local_unwind2
                add     esp, 8
                pop     ebp

                mov     eax, [esp+8]                ; Our establisher is the one
                                                    ; in front of the local one

                mov     edx, [esp+16]
                mov     [edx], eax                  ; set dispatcher to local_unwind2

                mov     eax, DISPOSITION_COLLIDED_UNWIND
_uh_return:
                retn
__unwind_handler endp



;/* _LOCAL_UNWIND2 - run all termination handlers listed in the scope table
; * associated with the given registration record, from the current lexical
; * level through enclosing levels up to, but not including the given 'stop'
; * level.
; */
;void _local_unwind2(PEXCEPTION_REGISTRATION xr, int stop)
;{
;    int ix;
;
;    for(ix=xr->trylevel; ix!=-1 && ix!=stop; ix=xr->xscope[i].enclosing_level){
;       /* NULL indicates that this entry is for a termination handler */
;       if(xr->xscope[i].filter==NULL){
;           /* NB: call to the termination handler may trash callee save regs */
;           (*xr->xscope[i].specific_handler)();
;       }
;    }
;    xr->trylevel=stop;
;}
;/* NOTE: frame (ebp) is setup by caller of __local_unwind2 */

                public  __NLG_Call
                public  __NLG_Return2
                public  __local_unwind2

__local_unwind2 proc near

                push    ebx
                push    esi
                push    edi     ;call to the handler may trash, so we must save it

                mov     eax, [esp+16]               ; (eax) = PEXCEPTION_REGISTRATION

                ;link in a handler to guard our unwind
                push    ebp
                push    eax
                push    TRYLEVEL_INVALID
                push    OFFSET FLAT:__unwind_handler
                push    fs:__except_list
                mov     eax, dword ptr ds:___security_cookie
                xor     eax, esp
                push    eax
                lea     eax, [esp+4]
                mov     fs:__except_list, eax

_lu_top:
                mov     eax, [esp+40]               ; (eax) = PEXCEPTION_REGISTRATION
                mov     ebx, [eax + _EXCEPTION_REGISTRATION_COMMON.scopetable]
                mov     esi, [eax + _EXCEPTION_REGISTRATION_COMMON.trylevel]

                cmp     esi, -1                     ; REVIEW: do we need this extra check?
                je      short _lu_done
                cmp     dword ptr [esp+44], -1
                jz      _continue_
                cmp     esi, [esp+44]
                je      short _lu_done

_continue_:
                lea     esi, [esi+esi*2]            ; esi*= 3

                mov     ecx, [(ebx+esi*4) + _SCOPETABLE_ENTRY.enclosing_level]
                mov     [esp+12], ecx               ; save enclosing level
                mov     [eax + _EXCEPTION_REGISTRATION_COMMON.trylevel], ecx

                cmp     dword ptr [(ebx+esi*4) + _SCOPETABLE_ENTRY.filter], 0
                jnz     short _lu_continue

                push    0101h
                mov     eax, [(ebx+esi*4) + _SCOPETABLE_ENTRY.specific_handler]
                call    __NLG_Notify

                mov     eax, [(ebx+esi*4) + _SCOPETABLE_ENTRY.specific_handler]
                call    __NLG_Call

_lu_continue:
                jmp     short _lu_top

_lu_done:
                mov     ecx, [esp+4]
                mov     fs:__except_list, ecx
                add     esp, 4*6                    ; cleanup stack

                pop     edi                         ; restore c-runtime registers
                pop     esi
                pop     ebx
                retn
__local_unwind2 endp



;/* _ABNORMAL_TERMINATION - return TRUE if __finally clause entered via
; * _local_unwind2.
; */
;int __cdecl _abnormal_termination()

                public __abnormal_termination

__abnormal_termination proc near
                xor     eax, eax                    ; assume FALSE

                mov     ecx, fs:__except_list
                cmp     [ecx + _EXCEPTION_REGISTRATION_COMMON.handler], offset FLAT:__unwind_handler
                jne     short _at_done              ; UnwindHandler first?

                mov     edx, [ecx+12]               ; establisher of local_unwind2
                mov     edx, [edx + _EXCEPTION_REGISTRATION_COMMON.trylevel]         ; is trylevel the same as the
                cmp     [ecx+8], edx                ; local_unwind level?
                jne     short _at_done              ; no - then FALSE

                mov     eax, 1                      ; currently in _abnormal_termination
_at_done:
                retn
__abnormal_termination endp

;
; NLG entrypoints, for debugger support
; On entry: address of non-local goto in eax
;

                public __NLG_Dispatch
                public __NLG_Notify1

__NLG_Notify1   proc near
                push    ebx
                push    ecx
                mov     ebx, OFFSET __NLG_Destination
                jmp     __NLG_Go                    ; ecx is already set
__NLG_Notify1   endp


                public __NLG_Notify

__NLG_Notify    proc near
                push    ebx
                push    ecx
                mov     ebx, OFFSET __NLG_Destination
                mov     ecx, [esp+12]               ; arg1:dwInCode
__NLG_Go::
                mov     [ebx+_NLG_INFO.dwCode], ecx
                mov     [ebx+_NLG_INFO.uoffDestination], eax
                mov     [ebx+_NLG_INFO.uoffFramePointer], ebp
                push    ebp
                push    ecx
                push    eax

__NLG_Dispatch::
                pop     eax                         ; __NLG_Dispatch2
                pop     ecx
                pop     ebp
                pop     ecx
                pop     ebx
                retn    4
__NLG_Notify    endp


                public __NLG_Call

__NLG_Call      proc near
        call    eax
__NLG_Call      endp


                public __NLG_Return2

__NLG_Return2   proc near
                retn
__NLG_Return2   endp

_text$mn        ends
end
