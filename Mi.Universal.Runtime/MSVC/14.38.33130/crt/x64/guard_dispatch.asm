;***
;guard_dispatch.asm
;
;    Copyright (c) Microsoft Corporation. All rights reserved.
;
;*******************************************************************************

include ksamd64.inc


        NESTED_ENTRY _guard_dispatch_icall_nop, _TEXT

        END_PROLOGUE

        jmp             rax

        NESTED_END _guard_dispatch_icall_nop, _TEXT

        END
