/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ehprolg3.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/***
*ehprolg3.c - Define the _EH_prolog3* compiler helpers for unaligned frames
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       EH prologue helpers for the EH security cookie cases, where the cookie
*       must be initialized before the EH node is installed.  Also defines the
*       complementary EH epilogue helpers.
*
*       The helpers should be compiled /Gy, at least in retail CRTs, so they
*       will be compiled as comdats, and only the ones used will be pulled into
*       a release build.
*
*       These helpers are for setting up normal frames which do not require
*       8-byte (or more) alignment of the local stack allocation.
*
*       There are 4 different prologue helpers, to deal with the various
*       combinations of a 12-byte -vs- 16-byte exception registration node and
*       the presence or absence of a /GS local security cookie.
*
*       _EH_prolog3
*       _EH_prolog3_catch
*       _EH_prolog3_GS
*       _EH_prolog3_catch_GS
*
*       There are 3 different epilogue helpers:
*
*       _EH_epilog3
*       _EH_epilog3_GS
*       _EH_epilog3_catch_GS
****/

#ifdef _X86_

#include <vcruntime_internal.h>

/*
 * Ignore warnings about unreferenced formal parameters and unsafe FS:0
 * assignments.
 */

#pragma warning(disable:4100 4733)
#define bnd repne
/***
*_EH_prolog3 - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 12-byte EH node (function has unwinds, no catch blocks)
*   + does not have a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

EXTERN_C_START

void __declspec(naked) __cdecl _EH_prolog3(
    unsigned long LocalAllocation
)
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate locals allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH cookie
        push    eax
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

    bnd ret                     ; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_prolog3_catch - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 16-byte EH node (function has catch blocks)
*   + does not have a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This includes 4 bytes for the 4th field (the saved ESP) in the EH
*       registration node.
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_catch's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved final ESP return value
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

void __declspec(naked) __cdecl _EH_prolog3_catch(
    unsigned long LocalAllocation
)
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate end of EH node and the locals
                                ; allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   (uninitialized) saved ESP field of EH node
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH cookie
        push    eax
        mov     [ebp-16], esp   ; initialize the EH node's saved ESP field
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   saved ESP field of EH node
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

    bnd ret                     ; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_prolog3_GS - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 12-byte EH node (function has unwinds, no catch blocks)
*   + has a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This includes 4 bytes for the GS cookie.
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_GS's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    local GS cookie, XORed with value of EBP on exit
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

void __declspec(naked) __cdecl _EH_prolog3_GS(
    unsigned long LocalAllocation
)
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate space for the GS cookie, and the
                                ; locals allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   (uninitialized) local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH and GS cookies
        push    eax
        mov     [ebp-16], eax
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   saved ESP field of EH node
        ;   local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

    bnd ret                     ; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_prolog3_catch_GS - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 16-byte EH node (function has catch blocks)
*   + has a local /GS cookie
*   + uses an EBP frame (no FPO)
*   + does not need dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This includes 4 bytes for the 4th field (the saved ESP) in the EH
*       registration node, and 4 bytes for the GS cookie.
*
*   Incoming stack frame has:
*       [ESP+8]  callee's return address
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_catch_GS's return address
*
*Exit:
*   EAX = destroyed
*   EBP = ESP on entry + 4
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBP+4]     (entry [ESP+8]) callee's return address
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved final ESP return value
*       [EBP-20]    local GS cookie, XORed with value of EBP on exit
*       ...
*       [EBP-12-N]  base of locals allocation
*       [EBP-16-N]  saved EBX
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

void __declspec(naked) __cdecl _EH_prolog3_catch_GS(
    unsigned long LocalAllocation
)
{
    __asm
    {
        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr   <== ESP

        push    eax             ; save handler thunk address in its proper
                                ; final position, free EAX for use
        push    fs:[0]          ; set link to next node in EH chain

        ; stack has:
        ;   callee's return addr
        ;   allocation size
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0] <== ESP

        lea     eax, [esp+12]   ; calculate return value for EBP
        sub     esp, [esp+12]   ; allocate end of EH node, space for the GS
                                ; cookie, and the locals allocation area
        push    ebx             ; save the callee saves except for EBP
        push    esi
        push    edi
        mov     [eax], ebp      ; save old EBP in final location
        mov     ebp, eax        ; establish new EBP frame pointer

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   ret addr
        ;   handler thunk address
        ;   saved FS:[0]
        ;   (uninitialized) saved ESP field of EH node
        ;   (uninitialized) local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI   <== ESP

        mov     eax, __security_cookie
        xor     eax, ebp        ; generate local EH and GS cookies
        push    eax
        mov     [ebp-20], eax
        mov     [ebp-16], esp   ; initialize the EH node's saved ESP field
        push    [ebp-4]         ; move the return address to its final position
        mov     dword ptr [ebp-4], -1 ; initialize the EH state index

        ; stack has:
        ;   callee's return addr
        ;   saved EBP   <== EBP
        ;   initial EH state index of -1
        ;   handler thunk address
        ;   saved FS:[0]
        ;   saved ESP field of EH node
        ;   local GS security cookie
        ;   locals allocation area
        ;   saved EBX
        ;   saved ESI
        ;   saved EDI
        ;   local EH security cookie
        ;   ret addr    <== ESP

        lea     eax, [ebp-12]   ; all done - link in the EH node
        mov     fs:[0], eax

    bnd ret                     ; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_epilog3 - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3 or _EH_prolog3_catch.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3 or _EH_prolog3_catch
*   EBP = value it had on return from _EH_prolog3 or _EH_prolog3_catch
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

void __declspec(naked) __cdecl _EH_epilog3(void)
{
    __asm
    {
        mov     ecx, [ebp-12]   ; remove EH node from EH chain
        mov     fs:[0], ecx
        pop     ecx             ; save return address
        pop     edi             ; pop and ignore the EH cookie
        pop     edi             ; restore callee-saves
        pop     esi
        pop     ebx
        mov     esp, ebp        ; free stack
        pop     ebp             ; restore frame pointer
        push    ecx             ; push return address
    bnd ret     0               ; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_epilog3_GS - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3_GS, after first checking that
*   the local /GS security cookie is correct.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3_GS
*   EBP = value it had on return from _EH_prolog3_GS
*   GS cookie located at EBP-16
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

void __declspec(naked) __cdecl
_EH_epilog3_GS(void)
{
    __asm
    {
        mov     ecx, [ebp-16]   ; rematerialize the global cookie and check
        xor     ecx, ebp        ; * its validity
    bnd call    __security_check_cookie  ; adds 0xF2(BND)prefix to preserve bounds
    bnd jmp     _EH_epilog3     ; go tear down the frame
    }
}

/***
*_EH_epilog3_catch_GS - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3_catch_GS, after first checking
*   that the local /GS security cookie is correct.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3_catch_GS
*   EBP = value it had on return from _EH_prolog3_catch_GS
*   GS cookie located at EBP-20
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

void __declspec(naked) __cdecl
_EH_epilog3_catch_GS(void)
{
    __asm
    {
        mov     ecx, [ebp-20]   ; rematerialize the global cookie and check
        xor     ecx, ebp        ; * its validity
    bnd call    __security_check_cookie  ; adds 0xF2(BND)prefix to preserve bounds
    bnd jmp     _EH_epilog3     ; go tear down the frame
    }
}

/***
*_EH_prolog3_align - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 12-byte EH node (function has unwinds, no catch blocks)
*   + does not have a local /GS cookie
*   + uses an EBX/EBP frame (no FPO)
*   + needs dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This is the space required after EBX is pushed below the EH node.
*   Alignment = alignment requirement.
*
*   Incoming stack frame has:
*       [ESP+12] callee's return address
*       [ESP+8]  alignment requirement
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_align's return address
*
*Exit:
*   EAX = destroyed
*   EBX = ESP on entry + 4 (pointer to callee's parameters)
*   EBP = aligned pointer to callee's locals
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBX+4]     (entry [ESP+12]) callee's return address
*       [EBX+0]     saved EBX
*       padding to align stack (if needed)
*       [EBP+4]     callee's return address (copy of entry [ESP+12])
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved value of EBX on exit
*       padding to realign stack (if needed)
*       ...
*       [EBP-16-N]  base of locals allocation
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

void __declspec(naked) __cdecl
_EH_prolog3_align(
    unsigned long LocalAllocation,
    unsigned long Alignment
)
{
    __asm
    {
        ; stack has :
        ;   callee's return addr
            ;   alignment requirement
            ;   allocation size
            ;   ret addr <= = ESP
            push    ecx; we need an extra reg, so save ECX
            mov     ecx, [esp + 12]; get the alignment requirement
            mov[esp + 12], ebx; save EBX in its final position
            lea     ebx, [esp + 12]; *andset the new EBX for addressing params
            push    eax; save the handler thunk addr, free EAX for use
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   allocation size
            ;   ret addr
            ;   saved ECX
            ;   handler thunk addr <= = ESP
            ;
        ; Now calculate the aligned stack pointer, which will be returned in
            ; EBP.Depending on the padding required, it will point at the ret
            ; addr in the above stack, or somewhere below that.

            lea     eax, [esp + 8]
            neg     ecx
            and eax, ecx
            lea     esp, [eax - 8]

            ; ESP now points at or below its position in the previous comment,
            ;and ESP + 8 has the required alignment.Everything below the saved
            ; EBX is potentially overlapped by the aligned stack frame we're about
            ; to create, so move those items to safety.
            mov     eax, [ebx - 16]; the handler thunk address can be moved into
            mov[esp], eax; *its final position(might not move)
            mov     eax, [ebx - 8]; copy down the ret addr
            push    eax
            mov     eax, [ebx - 4]; get the allocation size for later
            mov     ecx, [ebx - 12]; restore the original ECX
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   (uninitialized)saved EBP(aligned stack location)
            ;   (uninitialized)EH state index
            ;   handler thunk addr
            ;   ret addr <= = ESP
            mov[esp + 12], ebp; save old EBPand set new EBP frame pointer
            lea     ebp, [esp + 12]; *for addressing locals
            mov     dword ptr[esp + 8], -1; initialize the EH state index
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr <= = ESP
            push    ebx; save copy of EBX at EBP - 16 for funclet use
            sub     esp, eax; allocate the locals allocation space
            push    esi; save callee - saved regs
            push    edi
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr
            ;   saved value of EBX on exit
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI <= = ESP
            mov     eax, __security_cookie
            xor eax, ebp; generate local EH cookie
            push    eax
            mov     eax, [ebx + 4]; copy callee's return addr above saved EBP
            mov[ebp + 4], eax; *so EBP - based stackwalks work better
            push[ebp - 12]; move the return address to final position
            mov     eax, fs: [0] ; set link to next node in EH chain
            mov[ebp - 12], eax

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   saved FS : [0]
            ;   saved value of EBX on exit
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI
            ;   local EH security cookie
            ;   ret addr <= = ESP

            lea     eax, [ebp - 12]; all done - link in the EH node
            mov     fs : [0] , eax

            bnd ret; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_prolog3_catch_align - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 16-byte EH node (function has catch blocks)
*   + does not have a local /GS cookie
*   + uses an EBX/EBP frame (no FPO)
*   + needs dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This is the space required after EBX is pushed below the EH node.
*   Alignment = alignment requirement.
*
*   Incoming stack frame has:
*       [ESP+12] callee's return address
*       [ESP+8]  alignment requirement
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_catch_align's return address
*
*Exit:
*   EAX = destroyed
*   EBX = ESP on entry + 4 (pointer to callee's parameters)
*   EBP = aligned pointer to callee's locals
*   ESP = EBP on exit - 32 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBX+4]     (entry [ESP+12]) callee's return address
*       [EBX+0]     saved EBX
*       padding to align stack (if needed)
*       [EBP+4]     callee's return address (copy of entry [ESP+12])
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved final ESP return value
*       [EBP-20]    saved value of EBX on exit
*       padding to realign stack (if needed)
*       ...
*       [EBP-20-N]  base of locals allocation
*       [EBP-24-N]  saved ESI
*       [EBP-28-N]  saved EDI
*       [EBP-32-N]  local EH cookie, XORed with value of EBP on exit
*
*******************************************************************************/

void __declspec(naked) __cdecl
_EH_prolog3_catch_align(
    unsigned long LocalAllocation,
    unsigned long Alignment
)
{
    __asm
    {
        ; stack has :
        ;   callee's return addr
            ;   alignment requirement
            ;   allocation size
            ;   ret addr <= = ESP
            push    ecx; we need an extra reg, so save ECX
            mov     ecx, [esp + 12]; get the alignment requirement
            mov[esp + 12], ebx; save EBX in its final position
            lea     ebx, [esp + 12]; *andset the new EBX for addressing params
            push    eax; save the handler thunk addr, free EAX for use
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   allocation size
            ;   ret addr
            ;   saved ECX
            ;   handler thunk addr <= = ESP
            ;
        ; Now calculate the aligned stack pointer, which will be returned in
            ; EBP.Depending on the padding required, it will point at the ret
            ; addr in the above stack, or somewhere below that.

            lea     eax, [esp + 8]
            neg     ecx
            and eax, ecx
            lea     esp, [eax - 8]

            ; ESP now points at or below its position in the previous comment,
            ;and ESP + 8 has the required alignment.Everything below the saved
            ; EBX is potentially overlapped by the aligned stack frame we're about
            ; to create, so move those items to safety.
            mov     eax, [ebx - 16]; the handler thunk address can be moved into
            mov[esp], eax; *its final position(might not move)
            mov     eax, [ebx - 8]; copy down the ret addr
            push    eax
            mov     eax, [ebx - 4]; get the allocation size for later
            mov     ecx, [ebx - 12]; restore the original ECX
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   (uninitialized)saved EBP(aligned stack location)
            ;   (uninitialized)EH state index
            ;   handler thunk addr
            ;   ret addr <= = ESP
            mov[esp + 12], ebp; save old EBPand set new EBP frame pointer
            lea     ebp, [esp + 12]; *for addressing locals
            mov     dword ptr[esp + 8], -1; initialize the EH state index
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr <= = ESP
            push    ecx; save space for saved ESP field of EH node
            push    ebx; save copy of EBX at EBP - 20 for funclet use
            sub     esp, eax; allocate the locals allocation space
            push    esi; save callee - saved regs
            push    edi
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr
            ;   (uninitialized)saved ESP field of EH node
            ;   saved value of EBX on exit
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI <= = ESP
            mov     eax, __security_cookie
            xor eax, ebp; generate local EH cookie
            push    eax
            mov[ebp - 16], esp; initialize the EH node's saved ESP field
            mov     eax, [ebx + 4]; copy callee's return addr above saved EBP
            mov[ebp + 4], eax; *so EBP - based stackwalks work better
            push[ebp - 12]; move the return address to final position
            mov     eax, fs: [0] ; set link to next node in EH chain
            mov[ebp - 12], eax
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   saved FS : [0]
            ;   saved ESP field of EH node
            ;   saved value of EBX on exit
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI
            ;   local EH security cookie
            ;   ret addr <= = ESP
            lea     eax, [ebp - 12]; all done - link in the EH node
            mov     fs : [0] , eax
            bnd ret; adds 0xF2(BND)prefix to preserve bounds
    }
}
/***
*_EH_prolog3_GS_align - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 12-byte EH node (function has unwinds, no catch blocks)
*   + has a local /GS cookie
*   + uses an EBX/EBP frame (no FPO)
*   + needs dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This is the space required after EBX is pushed below the EH node.
*   Alignment = alignment requirement.
*
*   Incoming stack frame has:
*       [ESP+12] callee's return address
*       [ESP+8]  alignment requirement
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_GS_align's return address
*
*Exit:
*   EAX = destroyed
*   EBX = ESP on entry + 4 (pointer to callee's parameters)
*   EBP = aligned pointer to callee's locals
*   ESP = EBP on exit - 28 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBX+4]     (entry [ESP+12]) callee's return address
*       [EBX+0]     saved EBX
*       padding to align stack (if needed)
*       [EBP+4]     callee's return address (copy of entry [ESP+12])
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved value of EBX on exit
*       padding to realign stack (if needed)
*       [EBP-???]   local GS cookie, XORed with value of EBP on exit
*       ...
*       [EBP-16-N]  base of locals allocation
*       [EBP-20-N]  saved ESI
*       [EBP-24-N]  saved EDI
*       [EBP-28-N]  local EH cookie, XORed with value of EBP on exit
*
*   The local GS cookie will be placed just below the first alignment point at
*   or below EBP-16.  That is, it is stored at [((EBP-16) & -Alignment)-4].
*
*******************************************************************************/
void __declspec(naked) __cdecl
_EH_prolog3_GS_align(
    unsigned long LocalAllocation,
    unsigned long Alignment
)
{
    __asm
    {
        ; stack has :
        ;   callee's return addr
            ;   alignment requirement
            ;   allocation size
            ;   ret addr <= = ESP

            push    ecx; we need an extra reg, so save ECX
            mov     ecx, [esp + 12]; get the alignment requirement
            mov[esp + 12], ebx; save EBX in its final position
            lea     ebx, [esp + 12]; *andset the new EBX for addressing params
            push    eax; save the handler thunk addr, free EAX for use

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   allocation size
            ;   ret addr
            ;   saved ECX
            ;   handler thunk addr <= = ESP
            ;
        ; Now calculate the aligned stack pointer, which will be returned in
            ; EBP.Depending on the padding required, it will point at the ret
            ; addr in the above stack, or somewhere below that.
            lea     eax, [esp + 8]
            neg     ecx
            and eax, ecx
            lea     esp, [eax - 8]
            ; ESP now points at or below its position in the previous comment,
            ;and ESP + 8 has the required alignment.Everything below the saved
            ; EBX is potentially overlapped by the aligned stack frame we're about
            ; to create, so move those items to safety.

            mov     eax, [ebx - 16]; the handler thunk address can be moved into
            mov[esp], eax; *its final position(might not move)
            mov     eax, [ebx - 8]; copy down the ret addr
            push    eax
            mov     eax, [ebx - 12]; copy down the original ECX
            push    eax
            mov     eax, [ebx - 4]; get the allocation size for later

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   (uninitialized)saved EBP(aligned stack location)
            ;   (uninitialized)EH state index
            ;   handler thunk addr
            ;   ret addr
            ;   saved ECX <= = ESP

            mov[esp + 16], ebp; save old EBPand set new EBP frame pointer
            lea     ebp, [esp + 16]; *for addressing locals
            mov     dword ptr[esp + 12], -1; initialize the EH state index

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr
            ;   saved ECX <= = ESP

            and ecx, esp; get address of realignment point
            sub     esp, eax; allocate the locals allocation space
            push    esi; save callee - saved regs
            push    edi

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr
            ;   saved ECX
            ;   ... possible padding to realign stack ... <= = ECX(aligned)
            ;   (uninitialized)local GS security cookie
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI <= = ESP

            mov     eax, __security_cookie
            xor eax, ebp; generate local EHand GS cookies
            push    eax
            mov[ecx - 4], eax
            mov     ecx, [ebp - 16]; restore the original ECX
            mov[ebp - 16], ebx; save copy of EBX for funclet use
            mov     eax, [ebx + 4]; copy callee's return addr above saved EBP
            mov[ebp + 4], eax; *so EBP - based stackwalks work better
            push[ebp - 12]; move the return address to final position
            mov     eax, fs: [0] ; set link to next node in EH chain
            mov[ebp - 12], eax
            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   saved FS : [0]
            ;   saved value of EBX on exit
            ;   ... possible padding to realign stack ...
            ;   local GS security cookie
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI
            ;   local EH security cookie
            ;   ret addr <= = ESP
            lea     eax, [ebp - 12]; all done - link in the EH node
            mov     fs : [0] , eax
            bnd ret; adds 0xF2(BND)prefix to preserve bounds
    }
}
/***
*_EH_prolog3_catch_GS_align - Set up C++ EH call frame variation
*
*Purpose:
*   Sets up the call frame for a C++ EH function that:
*   + uses a 16-byte EH node (function has catch blocks)
*   + has a local /GS cookie
*   + uses an EBX/EBP frame (no FPO)
*   + needs dynamic stack alignment
*
*Entry:
*   EAX = address of EH handler thunk.
*   LocalAllocation = local stack allocation requirement (hereafter, 'N').
*       This is the space required after EBX is pushed below the EH node.
*   Alignment = alignment requirement.
*
*   Incoming stack frame has:
*       [ESP+12] callee's return address
*       [ESP+8]  alignment requirement
*       [ESP+4]  local stack allocation requirement
*       [ESP+0]  _EH_prolog3_catch_GS_align's return address
*
*Exit:
*   EAX = destroyed
*   EBX = ESP on entry + 4 (pointer to callee's parameters)
*   EBP = aligned pointer to callee's locals
*   ESP = EBP on exit - 32 - N
*   All other registers except CC preserved
*
*   FS:[0] = set to EBP on exit - 12 to create new link in the EH chain
*
*   Stack frame has been set up as follows:
*       [EBX+4]     (entry [ESP+12]) callee's return address
*       [EBX+0]     saved EBX
*       padding to align stack (if needed)
*       [EBP+4]     callee's return address (copy of entry [ESP+12])
*       [EBP+0]     saved EBP
*       [EBP-4]     EH record state index, initialized to -1
*       [EBP-8]     address of EH handler thunk
*       [EBP-12]    saved FS:[0]
*       [EBP-16]    saved final ESP return value
*       [EBP-20]    saved value of EBX on exit
*       padding to realign stack (if needed)
*       [EBP-???]   local GS cookie, XORed with value of EBP on exit
*       ...
*       [EBP-20-N]  base of locals allocation
*       [EBP-24-N]  saved ESI
*       [EBP-28-N]  saved EDI
*       [EBP-32-N]  local EH cookie, XORed with value of EBP on exit
*
*   The local GS cookie will be placed just below the first alignment point at
*   or below EBP-16.  That is, it is stored at [((EBP-16) & -Alignment)-4].
*
*******************************************************************************/
void __declspec(naked) __cdecl
_EH_prolog3_catch_GS_align(
    unsigned long LocalAllocation,
    unsigned long Alignment
)
{
    __asm
    {
        ; stack has :
        ;   callee's return addr
            ;   alignment requirement
            ;   allocation size
            ;   ret addr <= = ESP

            push    ecx; we need an extra reg, so save ECX
            mov     ecx, [esp + 12]; get the alignment requirement
            mov[esp + 12], ebx; save EBX in its final position
            lea     ebx, [esp + 12]; *andset the new EBX for addressing params
            push    eax; save the handler thunk addr, free EAX for use

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   allocation size
            ;   ret addr
            ;   saved ECX
            ;   handler thunk addr <= = ESP
            ;
        ; Now calculate the aligned stack pointer, which will be returned in
            ; EBP.Depending on the padding required, it will point at the ret
            ; addr in the above stack, or somewhere below that.
            lea     eax, [esp + 8]
            neg     ecx
            and eax, ecx
            lea     esp, [eax - 8]
            ; ESP now points at or below its position in the previous comment,
            ;and ESP + 8 has the required alignment.Everything below the saved
            ; EBX is potentially overlapped by the aligned stack frame we're about
            ; to create, so move those items to safety.

            mov     eax, [ebx - 16]; the handler thunk address can be moved into
            mov[esp], eax; *its final position(might not move)
            mov     eax, [ebx - 8]; copy down the ret addr
            push    eax
            mov     eax, [ebx - 12]; copy down the original ECX
            push    eax
            mov     eax, [ebx - 4]; get the allocation size for later

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   (uninitialized)saved EBP(aligned stack location)
            ;   (uninitialized)EH state index
            ;   handler thunk addr
            ;   ret addr
            ;   saved ECX <= = ESP

            mov[esp + 16], ebp; save old EBPand set new EBP frame pointer
            lea     ebp, [esp + 16]; *for addressing locals
            mov     dword ptr[esp + 12], -1; initialize the EH state index

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr
            ;   saved ECX <= = ESP

            push    ebx; save copy of EBX at EBP - 16 for funclet use
            and ecx, esp; get address of realignment point
            sub     esp, eax; allocate the locals allocation space
            push    esi; save callee - saved regs
            push    edi

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   (uninitialized)callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   ret addr
            ;   saved ECX
            ;   saved value of EBX on exit
            ;   ... possible padding to realign stack ... <= = ECX(aligned)
            ;   (uninitialized)local GS security cookie
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI <= = ESP

            mov     eax, __security_cookie
            xor eax, ebp; generate local EHand GS cookies
            push    eax
            mov[ecx - 4], eax
            mov     ecx, [ebp - 16]; restore the original ECX
            mov[ebp - 16], esp; initialize the EH node's saved ESP field
            mov     eax, [ebx + 4]; copy callee's return addr above saved EBP
            mov[ebp + 4], eax; *so EBP - based stackwalks work better
            push[ebp - 12]; move the return address to final position
            mov     eax, fs: [0] ; set link to next node in EH chain
            mov[ebp - 12], eax

            ; stack has :
        ;   callee's return addr
            ;   saved EBX <= = EBX
            ;   ... possible padding to align stack ...
            ;   callee's return addr
            ;   saved EBP <= = EBP(aligned stack location)
            ;   initial EH state index of - 1
            ;   handler thunk addr
            ;   saved FS : [0]
            ;   saved ESP field of EH node
            ;   saved value of EBX on exit
            ;   ... possible padding to realign stack ...
            ;   local GS security cookie
            ;   locals allocation area
            ;   saved ESI
            ;   saved EDI
            ;   local EH security cookie
            ;   ret addr <= = ESP

            lea     eax, [ebp - 12]; all done - link in the EH node
            mov     fs : [0] , eax

            bnd ret; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_epilog3_align - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3_align or _EH_prolog3_catch_align.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3_align or
*         _EH_prolog3_catch_align
*   EBP = value it had on return from _EH_prolog3_align or
*         _EH_prolog3_catch_align
*   EBX = value it had on return from _EH_prolog3_align or
*         _EH_prolog3_catch_align
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

void __declspec(naked) __cdecl
_EH_epilog3_align(
    void
)
{
    __asm
    {
        mov     ecx, [ebp - 12]; remove EH node from EH chain
        mov     fs : [0] , ecx
        pop     ecx; save return address
        pop     edi; popand ignore the EH cookie
        pop     edi; restore callee - saves
        pop     esi
        mov     esp, ebp; free aligned stack
        pop     ebp; restore local frame pointer
        mov     esp, ebx; free unaligned stack
        pop     ebx; restore parameter frame pointer
        push    ecx; push return address
        bnd ret     0; adds 0xF2(BND)prefix to preserve bounds
    }
}

/***
*_EH_epilog3_GS_align - Tear down C++ EH call frame variation
*
*Purpose:
*   Destroy a call frame set up by _EH_prolog3_GS_align or
*   _EH_prolog3_catch_align, after first checking that the local /GS security
*   cookie is correct.
*
*   This helper version does not load the cookie itself because it doesn't
*   know where it's located.  It would need to be passed the alignment to know
*   that, so it might as well be passed the cookie value directly.
*
*Entry:
*   ESP = value it had on return from _EH_prolog3_GS
*   EBP = value it had on return from _EH_prolog3_GS
*   EBX = value it had on return from _EH_prolog3_GS
*   ECX = GS cookie from callee's frame
*
*Exit:
*   ESP = pointer to the callee's return address
*   EBX, ESI, EDI, EBP restored from frame
*   ECX destroyed
*   All other registers except CC preserved
*
*******************************************************************************/

void __declspec(naked) __cdecl
_EH_epilog3_GS_align(
    void
)
{
    __asm
    {
        xor ecx, ebp; rematerialize the global cookieand
        bnd call    __security_check_cookie; *check its validity
        bnd jmp     _EH_epilog3_align; go tear down the frame
    }
}

EXTERN_C_END

#endif
