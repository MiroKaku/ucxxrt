;***
;guard_dispatch.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

include ksamd64.inc

EXTRN   __guard_dispatch_icall_fptr:qword

    LEAF_ENTRY _guard_dispatch_icall_nop, _TEXT

    jmp     rax

    LEAF_END _guard_dispatch_icall_nop, _TEXT

    LEAF_ENTRY _guard_xfg_dispatch_icall_nop, _TEXT

    jmp     __guard_dispatch_icall_fptr

    LEAF_END _guard_xfg_dispatch_icall_nop, _TEXT

END
