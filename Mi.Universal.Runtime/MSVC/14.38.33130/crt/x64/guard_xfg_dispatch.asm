;***
;guard_xfg_dispatch.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

include ksamd64.inc


        EXTRN   __guard_dispatch_icall_fptr:QWORD


        NESTED_ENTRY _guard_xfg_dispatch_icall_nop, _TEXT

        END_PROLOGUE

        jmp             __guard_dispatch_icall_fptr

        NESTED_END _guard_xfg_dispatch_icall_nop, _TEXT

        END
