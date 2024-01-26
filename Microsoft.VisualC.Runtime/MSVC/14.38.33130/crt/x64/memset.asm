    page    ,132
    title   memset - set sections of memory all to one byte
;***
;memset.asm - contains memset routine
;
;   Copyright (c) Microsoft Corporation. All rights reserved.
;
;Purpose:
;       sets all bytes in a memory block to a value
;
;Entry:
;       void *dst (rcx) - pointer to destination buffer
;       int value (rdx) - value to set
;       size_t count (r8) - number of bytes to set
;
;Exit:
;       Returns a pointer to the destination buffer (rax)
;
;*******************************************************************************

include ksamd64.inc
        subttl  "memset"

IFDEF _CRT_FFS_BUILD
memset_name       TEXTEQU <memset_ffs>
ELSE
memset_name       TEXTEQU <memset>
ENDIF

; the code implementing memory set via rep stosb (enhanced strings)
; needs to be in a separate routine to be able to generate EH records
; that can used to restore non-volatile register value during EH unwinding

; Note: This is placed in a separate coffgroup along with a few other
; mem routines. See https://devdiv.visualstudio.com/DevDiv/_workitems/edit/1720249
        NESTED_ENTRY memset_repstos, _TEXT$21

        push_reg rdi
        .endprolog
        mov      eax, edx
        mov      rdi, rcx
        mov      rcx, r8
        rep      stosb
        mov      rax, r9
        .beginepilog
        pop      rdi
        ret

        NESTED_END memset_repstos, _TEXT$21

;*******************************************************************************
;        Main memset routine implementation
;
;    the implementation follows this approach:
;   1. for blocks less than 16 bytes in size use stores through general registers
;   2. for blocks 16-32 bytes in size use two (possibly overlapping) stores through SSE registers
;   3. for blocks 33-8K bytes in size use AVX (if available) or SSE stores
;   4. for blocks 8K+1-256K bytes in size use enhanced strings (if available),
;      if not continue using AVX or SSE stores
;   5. for blocks greater than 256K size use non-temporal AVX stores (if available), or enhanced strings (if available),
;      or SSE stores.
;

        __FAVOR_ENFSTRG equ 1
        __ISA_AVAILABLE_AVX equ 3

; Note: This is placed in a separate coffgroup along with a few other
; mem routines. See https://devdiv.visualstudio.com/DevDiv/_workitems/edit/1720249
        LEAF_ENTRY_ARG3 memset_name, _TEXT$21, buf:ptr byte, value:byte, count:dword

        OPTION PROLOGUE:NONE, EPILOGUE:NONE

        extrn    __favor:byte
        extrn    __ImageBase:byte
        extrn    __isa_available:dword
        extrn    __memset_nt_threshold:qword                     ; defined in cpu_disp.c
        extrn    __memset_fast_string_threshold:qword            ; defined in cpu_disp.c

        mov      rax, rcx                                        ; pre-set the return value
        mov      r9, rcx                                         ; save the block address for setting the return value in the fast strings implementation
        lea      r10, OFFSET __ImageBase                         ; pre-set the image base to be used in table based branching
        movzx    edx, dl                                         ; set the byte fill pattern
        mov      r11, 0101010101010101h                          ; set replication mask for expanding into 64 bits
        imul     r11, rdx                                        ; expand into the 64-bit integer fill value for using in regular stores
        movq     xmm0, r11                                       ; expand into the 64-bit XMM fill value for using in SSE stores
        cmp      r8, 15                                          ; dispatch to code handling block sizes of 16 bytes or more
        ja       SetAbove15                                      ; fall-thru to code handling length of 15 bytes or less

;
;   set blocks of less than 16 bytes in length
;
;   Entry:
;      r8 - length < 16
;      rcx - destination block address
;      rdx - fill value
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the block length as an index
;      into the table of code labels where each entry points to the code block implementing the set of this particular length
;      each block implements memory set using the appropriate combination of stores through general registers staggering entry points
;      where possible to recycle store instructions to cover more than one block length
;

        align    16
IFDEF _VCRUNTIME_BUILD_QSPECTRE
         and     r8, 0Fh
ENDIF
         add     rcx, r8
         mov     r9d, [(IMAGEREL SetSmall) + r10 + r8*4]         ; load the appropriate code label RVA from the table (r9) using the length (r8) as an index
         add     r9, r10                                         ; convert the RVA into the VA
         jmp     r9                                              ; dispatch to the code

SetSmall15::
        mov      (-15)[rcx], r11                                 ; handle 15 bytes (8+4+2+1)
SetSmall7::
        mov      (-7)[rcx], r11d                                 ; handle 7 bytes (4+2+1)
SetSmall3::
        mov      (-3)[rcx], r11w                                 ; handle 3 bytes (2+1)
SetSmall1::
        mov      (-1)[rcx], r11b                                 ; handle 1 byte ()
SetSmall0::
        ret

SetSmall14::
        mov     (-14)[rcx], r11                                  ; handle 14 bytes (8+4+2)
SetSmall6::
        mov     (-6)[rcx], r11d                                  ; handle 6 bytes (4+2)
SetSmall2::
        mov     (-2)[rcx], r11w                                  ; handle 2 bytes (2)
MoveSmall0::
        ret

        align   16
SetSmall13::
        mov     (-13)[rcx], r11                                  ; handle 13 bytes (8+4+1)
SetSmall5::
        mov     (-5)[rcx], r11d                                  ; handle 5 bytes (4+1)
        mov     (-1)[rcx], r11b
        ret

        align 16
SetSmall12::
        mov     (-12)[rcx], r11                                  ; handle 12 bytes (8+4)
SetSmall4::
        mov     (-4)[rcx], r11d                                  ; handle 4 bytes (4)
        ret

SetSmall11::
        mov     (-11)[rcx], r11                                  ; handle 11 bytes (8+2+1)
        mov     (-3)[rcx], r11w
        mov     (-1)[rcx], r11b
        ret

SetSmall9::
        mov     (-9)[rcx], r11                                   ; handle 9 bytes (8+1)
        mov     (-1)[rcx], r11b
        ret

        align   16
SetSmall10::
        mov     (-10)[rcx], r11                                  ; handle 10 bytes (8+2)
        mov     (-2)[rcx], r11w
        ret

SetSmall8::
        mov     (-8)[rcx], r11                                   ; handle 8 bytes (8)
        ret

;
;   set blocks of 16 bytes or more in length
;
        align    16
SetAbove15:
        punpcklqdq xmm0, xmm0                                    ; expand into the 128-bit XMM value for using in SSE stores
        cmp      r8, 32                                          ; dispatch to code handling blocks over 32 bytes in length
        ja       SetAbove32                                      ; fall-through to code handling blocks between 16 and 32 in length

;
;   set blocks between 16 and 32 bytes in length
;
;   Entry:
;      r8 - length (16-32 bytes)
;      rcx - destination block address
;      xmm0 - fill value
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented with two SSE stores using XMM registers
;      the first 16-byte store sets the first 16 bytes beginning at the blocks start addresses
;      the second 16-byte store sets the remaining bytes beginning at the adjusted address
;      with the store tail covering the remaining length
;
        movdqu   XMMWORD PTR [rcx], xmm0
        movdqu   XMMWORD PTR (-16)[rcx + r8], xmm0
        ret

;
;   set blocks of 32 bytes or more in length
;
SetAbove32:
        cmp      __isa_available, __ISA_AVAILABLE_AVX            ; check if the HW supports wider AVX copies
        jb       NoAVX                                           ; if not, use the SSE based implementation

;
;      AVX based implementation
;

        __AVX_LEN_BIT equ 5
        __AVX_STEP_LEN equ 1 SHL __AVX_LEN_BIT
        __AVX_LOOP_LEN equ __AVX_STEP_LEN*8

        cmp      r8, __memset_fast_string_threshold              ; it's best to use regular AVX stores to set blocks up to a certain threshold in length
        jbe      SetWithYMM                                      ; check the block length (r8) and dispatch to the regular AVX code

        cmp      r8, __memset_nt_threshold                       ; it's best to use non-temporal AVX stores to set blocks above a certain threshold in length
        ja       SetWithYMM                                      ; check the block length (r8) and dispatch to the AVX code that will take care of dispatching to the non-temporal version

        test    __favor, 1 SHL __FAVOR_ENFSTRG                   ; it's best to use "fast string" rep stosb (if present) to set blocks above the fast string and up to NT AVX thresholds in length
        jnz     memset_repstos                                   ; if "fast string" rep stosb is not present fall-through to regular AVX code

;
;   set memory with AVX stores
;
;   Entry:
;      r8 - length (> 32 bytes)
;      rcx - destination block address
;      xmm0 - fill value
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is based on a loop with eight AVX stores copying 256 bytes in an iteration with the remainder (or blocks themselves)
;      handled through a table based dispatch to the code that handles the remaining length with the appropriate number of AVX stores
;

SetWithYMM:
        vinsertf128 ymm0, ymm0, xmm0, 1                          ; expand into the 256-bit YMM value for using in AVX stores
        mov      r9, rcx                                         ; it is generally profitable to align the loop stores on the 32-byte boundary, this is done by shifting
        and      r9, __AVX_STEP_LEN-1                            ; the starting destination (rcx) set address by the number of bytes (complement r9) required
        sub      r9, __AVX_STEP_LEN                              ; to make the destination block aligned on the next 32-byte boundary and reducing the loop copy length accordingly,
        sub      rcx, r9                                         ; the skipped head bytes will be set in the remainder handling code;
        sub      rdx, r9
        add      r8, r9
        cmp      r8, __AVX_LOOP_LEN                              ; compare the block set length (which may have gotten shorter) to the loop iteration length,
        jbe      SetUpTo256WithYMM                               ; if it is less than it go to the loop remainder handling code

        cmp      r8, __memset_nt_threshold                       ; blocks greater than a certain threshold are best handled with non-temporal stores
        ja       YmmLoopNT                                       ; for blocks up to this threshold fall-through to the regular AVX loop

;
;    set > 256 bytes with an AVX based loop
;
;   Entry:
;      r8 - length (> 256 bytes)
;      rcx - destination block address (32-byte aligned)
;      ymm0 - fill value
;   Exit:
;      r8 - remaining length (< 256 bytes)
;      rcx - remaining destination block address
;      rax - destination block address
;
;   Implementation:
;      each iteration sets 256 bytes through 8 AVX stores using a YMM register
;
       align     16
YmmLoop:
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*0)[rcx], ymm0       ; set 256 bytes in destination
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*1)[rcx], ymm0
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*2)[rcx], ymm0
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*3)[rcx], ymm0
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*4)[rcx], ymm0
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*5)[rcx], ymm0
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*6)[rcx], ymm0
        vmovdqa  YMMWORD PTR (__AVX_STEP_LEN*7)[rcx], ymm0
        add      rcx, __AVX_LOOP_LEN                             ; advance destination address
        sub      r8, __AVX_LOOP_LEN                              ; adjust remaining length
        cmp      r8, __AVX_LOOP_LEN                              ; if at least 256 bytes remain to be set do another iteration
        jae      YmmLoop                                         ; otherwise fall-through to the remainder handling code

;
;   set up to 256 bytes using AVX stores
;
;   Entry:
;      r8 - length (<= 256 bytes)
;      rcx - remaining destination block address
;      ymm0 - fill value
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the number of remaining 32-byte copies as an index
;      into the table of staggered code labels where each entry points to the beginning code address that will handle the remaining length
;

SetUpTo256WithYMM:
        lea      r9, (__AVX_STEP_LEN-1)[r8]                      ; calculate the length of full 32-byte blocks (r9) required to handle the remaining length (r8), i.e. (0 for 0 length, 1 for 1-32,
        and      r9, -__AVX_STEP_LEN                             ; 2 for 33-64, etc.), this number will be used as a base to calculate the destination block address for remaining sets
        mov      r11, r9                                         ; also use this number to calculate the number of sets (r11) required to handle the remaining length
        shr      r11, __AVX_LEN_BIT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r11, 0Fh
ENDIF
        mov      r11d, [(IMAGEREL SetSmallYmm) + r10 + r11*4]    ; load the appropriate code label RVA from the table using the number of remaining sets (r11) as an index
        add      r11, r10                                        ; convert the RVA into the VA
        jmp      r11                                             ; dispatch to the code

Set8YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*8)[rcx + r9], ymm0 ; entry point to perform 8 remaining sets
Set7YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*7)[rcx + r9], ymm0 ; entry point to perform 7 remaining sets
Set6YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*6)[rcx + r9], ymm0 ; entry point to perform 6 remaining sets
Set5YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*5)[rcx + r9], ymm0 ; entry point to perform 5 remaining sets
Set4YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*4)[rcx + r9], ymm0 ; entry point to perform 4 remaining sets
Set3YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*3)[rcx + r9], ymm0 ; entry point to perform 3 remaining sets
Set2YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*2)[rcx + r9], ymm0 ; entry point to perform 2 remaining sets
Set1YmmBlocks::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*1)[rcx + r8], ymm0 ; entry point to perform 1 remaining set
Set0YmmBlocks::
        vmovdqu  YMMWORD PTR [rax], ymm0                         ; set the first 32 byte chunk that may have been skipped by aligning the destination
        vzeroupper                                               ; clean up upper AVX
        ret

;
;   set > 256 bytes with an AVX based loop using non-temporal stores
;
;   Entry:
;      r8 - length (> 256 bytes)
;      rcx - destination block address (32-byte aligned)
;      ymm0 - source block address
;   Exit:
;      r8 - remaining length (< 256 bytes)
;      rcx - remaining destination block address
;      rax - destination block address
;
;   Implementation:
;      each iteration sets 256 bytes through 8 non-temporal AVX stores using a YMM register
;
SetWithYMMNT:
       align     16
YmmLoopNT:
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*0)[rcx], ymm0       ; set 256 bytes in destination
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*1)[rcx], ymm0
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*2)[rcx], ymm0
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*3)[rcx], ymm0
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*4)[rcx], ymm0
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*5)[rcx], ymm0
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*6)[rcx], ymm0
        vmovntdq YMMWORD PTR (__AVX_STEP_LEN*7)[rcx], ymm0
        add      rcx, __AVX_LOOP_LEN                             ; advance destination address
        sub      r8, __AVX_LOOP_LEN                              ; adjust remaining length
        cmp      r8, __AVX_LOOP_LEN                              ; if at least 256 bytes remain to be set do another iteration
        jae      YmmLoopNT                                       ; otherwise fall-through to the remainder handling code

;
;   set up to 256 bytes using non-temporal AVX stores
;
;   Entry:
;      r8 - length (<= 256 bytes)
;      rcx - remaining destination block address
;      ymm0 - fill value
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the number of remaining 32-byte copies as an index
;      into the table of staggered code labels where each entry points to the beginning code address that will handle the remaining length
;
        lea      r9, (__AVX_STEP_LEN-1)[r8]                      ; calculate the length of full 32-byte blocks (r9) required to handle the remaining length (r8), i.e. (0 for 0 length, 1 for 1-32,
        and      r9, -__AVX_STEP_LEN                             ; 2 for 33-64, etc.), this number will be used as a base to calculate the destination block address for remaining sets
        mov      r11, r9                                         ; also use this number to calculate the number of sets (r11) required to handle the remaining length
        shr      r11, __AVX_LEN_BIT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r11, 0Fh
ENDIF
        mov      r11d, [(IMAGEREL SetSmallYmmNT) + r10 + r11*4]  ; load the appropriate code label RVA from the table using the number of remaining sets (r11) as an index
        add      r11, r10                                        ; convert the RVA into the VA
        jmp      r11                                             ; dispatch to the code

Set8YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*8)[rcx + r9], ymm0 ; entry point to perform 8 remaining sets
Set7YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*7)[rcx + r9], ymm0 ; entry point to perform 7 remaining sets
Set6YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*6)[rcx + r9], ymm0 ; entry point to perform 6 remaining sets
Set5YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*5)[rcx + r9], ymm0 ; entry point to perform 5 remaining sets
Set4YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*4)[rcx + r9], ymm0 ; entry point to perform 4 remaining sets
Set3YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*3)[rcx + r9], ymm0 ; entry point to perform 3 remaining sets
Set2YmmBlocksNT::
        vmovntdq YMMWORD PTR (-__AVX_STEP_LEN*2)[rcx + r9], ymm0 ; entry point to perform 2 remaining sets
Set1YmmBlocksNT::
        vmovdqu  YMMWORD PTR (-__AVX_STEP_LEN*1)[rcx + r8], ymm0 ; entry point to perform 1 remaining set
Set0YmmBlocksNT::
        vmovdqu  YMMWORD PTR [rax], ymm0                         ; set the first 32 byte chunk that may have been skipped by aligning the destination
        sfence                                                   ; non-temporal stores require sfence to maintain memory semantics
        vzeroupper                                               ; clean up upper AVX
        ret

;
;      SSE based implementation
;

        align   16
NoAVX:
        __SSE_LEN_BIT equ 4
        __SSE_STEP_LEN equ 1 SHL __SSE_LEN_BIT
        __SSE_LOOP_LEN equ __SSE_STEP_LEN*8

        cmp      r8, __memset_fast_string_threshold              ; it's best to use regular SSE stores to set blocks up to a certain threshold in length
        jbe      SetWithXMM                                      ; check the block length (r8) and dispatch to regular SSE code

        test     __favor, 1 SHL __FAVOR_ENFSTRG                  ; it's best to use "fast string" rep stosb (if present) to set blocks above the threshold
        jnz      memset_repstos                                  ; if "fast string" rep stosb is not present fall-through to regular SSE code

SetWithXMM:
        mov      r9, rcx                                         ; it is generally profitable to align the loop stores on the 16-byte boundary, this is done by shifting
        and      r9, __SSE_STEP_LEN-1                            ; the starting destination (rcx) set address by the number of bytes (complement r9) required
        sub      r9, __SSE_STEP_LEN                              ; to make the destination block aligned on the next 16-byte boundary and reducing the loop copy length accordingly,
        sub      rcx, r9                                         ; the skipped head bytes will be set in the remainder handling code;
        sub      rdx, r9
        add      r8, r9
        cmp      r8, __SSE_LOOP_LEN                              ; compare the block set length (which may have gotten shorter) to the loop iteration length,
        jbe      SetUpTo128WithXMM                               ; if it is less than it go to the loop remainder handling code

;
;    set > 128 bytes with an SSE based loop
;
;   Entry:
;      r8 - length (> 256 bytes)
;      rcx - destination block address (32-byte aligned)
;      xmm0 - fill value
;   Exit:
;      r8 - remaining length (< 128 bytes)
;      rcx - remaining destination block address
;      rax - destination block address
;
;   Implementation:
;      each iteration sets 128 bytes through 8 SSE stores using a XMM register
;
        align 16
XmmLoop:
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*0)[rcx], xmm0       ; set 128 bytes in destination
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*1)[rcx], xmm0
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*2)[rcx], xmm0
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*3)[rcx], xmm0
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*4)[rcx], xmm0
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*5)[rcx], xmm0
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*6)[rcx], xmm0
        movdqa   XMMWORD PTR (__SSE_STEP_LEN*7)[rcx], xmm0
        add      rcx, __SSE_LOOP_LEN                             ; advance destination address
        sub      r8, __SSE_LOOP_LEN                              ; adjust remaining length
        cmp      r8, __SSE_LOOP_LEN                              ; if at least 128 bytes remain to be set do another iteration
        jae      XmmLoop                                         ; otherwise fall-through to the remainder handling code

;
;   set up to 128 bytes using SSE stores
;
;   Entry:
;      r8 - length (<= 128 bytes)
;      rcx - remaining destination block address
;      xmm0 - fill value
;   Exit:
;      rax - destination block address
;
;   Implementation:
;      the operation is implemented through an indirect branch dispatch using the number of remaining 16-byte copies as an index
;      into the table of staggered code labels where each entry points to the beginning code address that will handle the remaining length
;

SetUpTo128WithXMM:
        lea      r9, (__SSE_STEP_LEN-1)[r8]                      ; calculate the length of full 16-byte blocks (r9) required to handle the remaining length (r8), i.e. (0 for 0 length, 1 for 1-16,
        and      r9, -__SSE_STEP_LEN                             ; 2 for 17-32, etc.), this number will be used as a base to calculate the destination block address for remaining sets
        mov      r11, r9                                         ; also use this number to calculate the number of sets (r11) required to handle the remaining length
        shr      r11, __SSE_LEN_BIT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
        and      r11, 0Fh
ENDIF
        mov      r11d, [(IMAGEREL SetSmallXmm) + r10 + r11*4]    ; load the appropriate code label RVA from the table using the number of remaining sets (r11) as an index
        add      r11, r10                                        ; convert the RVA into the VA
        jmp      r11                                             ; dispatch to the code

Set8XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*8)[rcx + r9], xmm0 ; entry point to perform 8 remaining sets
Set7XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*7)[rcx + r9], xmm0 ; entry point to perform 7 remaining sets
Set6XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*6)[rcx + r9], xmm0 ; entry point to perform 6 remaining sets
Set5XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*5)[rcx + r9], xmm0 ; entry point to perform 5 remaining sets
Set4XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*4)[rcx + r9], xmm0 ; entry point to perform 4 remaining sets
Set3XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*3)[rcx + r9], xmm0 ; entry point to perform 3 remaining sets
Set2XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*2)[rcx + r9], xmm0 ; entry point to perform 2 remaining sets
Set1XmmBlocks::
        movdqu   XMMWORD PTR (-__SSE_STEP_LEN*1)[rcx + r8], xmm0 ; entry point to perform 1 remaining set
Set0XmmBlocks::
        movdqu   XMMWORD PTR [rax], xmm0                         ; set the first 16 byte chunk that may have been skipped by aligning the destination
        ret

        LEAF_END memset_name, _TEXT$21

_RDATA   segment read para alias('.rdata$00') 'DATA'

;
;  Code address table for handling small (< 16 bytes) sets
;

SetSmall        dd       IMAGEREL SetSmall0
                dd       IMAGEREL SetSmall1
                dd       IMAGEREL SetSmall2
                dd       IMAGEREL SetSmall3
                dd       IMAGEREL SetSmall4
                dd       IMAGEREL SetSmall5
                dd       IMAGEREL SetSmall6
                dd       IMAGEREL SetSmall7
                dd       IMAGEREL SetSmall8
                dd       IMAGEREL SetSmall9
                dd       IMAGEREL SetSmall10
                dd       IMAGEREL SetSmall11
                dd       IMAGEREL SetSmall12
                dd       IMAGEREL SetSmall13
                dd       IMAGEREL SetSmall14
                dd       IMAGEREL SetSmall15
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 0    dup (IMAGEREL SetSmall0) ; padding
ENDIF

;
;  Code address table for handling small (<= 256 byte) sets using regular AVX stores
;

SetSmallYmm     dd        IMAGEREL Set0YmmBlocks
                dd        IMAGEREL Set1YmmBlocks
                dd        IMAGEREL Set2YmmBlocks
                dd        IMAGEREL Set3YmmBlocks
                dd        IMAGEREL Set4YmmBlocks
                dd        IMAGEREL Set5YmmBlocks
                dd        IMAGEREL Set6YmmBlocks
                dd        IMAGEREL Set7YmmBlocks
                dd        IMAGEREL Set8YmmBlocks
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 7      dup (IMAGEREL Set0YmmBlocks) ; padding
ENDIF

;
;  Code address table for handling small (<= 256 byte) sets using non-temporal AVX stores
;

SetSmallYmmNT   dd        IMAGEREL Set0YmmBlocksNT
                dd        IMAGEREL Set1YmmBlocksNT
                dd        IMAGEREL Set2YmmBlocksNT
                dd        IMAGEREL Set3YmmBlocksNT
                dd        IMAGEREL Set4YmmBlocksNT
                dd        IMAGEREL Set5YmmBlocksNT
                dd        IMAGEREL Set6YmmBlocksNT
                dd        IMAGEREL Set7YmmBlocksNT
                dd        IMAGEREL Set8YmmBlocksNT
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 7      dup (IMAGEREL Set0YmmBlocksNT) ; padding
ENDIF

;
;  Code address table for handling small (<= 128 byte) sets using regular SSE stores
;

SetSmallXmm     dd        IMAGEREL Set0XmmBlocks
                dd        IMAGEREL Set1XmmBlocks
                dd        IMAGEREL Set2XmmBlocks
                dd        IMAGEREL Set3XmmBlocks
                dd        IMAGEREL Set4XmmBlocks
                dd        IMAGEREL Set5XmmBlocks
                dd        IMAGEREL Set6XmmBlocks
                dd        IMAGEREL Set7XmmBlocks
                dd        IMAGEREL Set8XmmBlocks
IFDEF _VCRUNTIME_BUILD_QSPECTRE
                dd 7      dup (IMAGEREL Set0XmmBlocks) ; padding
ENDIF

_RDATA  ends

    end
