       TTL ARM64 Control Flow Guard
;++
;
; Copyright (c) Microsoft Corporation
;
; Module Name:
;
;    guard_dispatch.asm
;
; Abstract:
;
;    ARM64 Control Flow Guard
;

#include "arm64/arm64ASMsymbolname.h"
#include "ksarm64.h"

        TEXTAREA

        NESTED_ENTRY A64NAME(_guard_dispatch_icall_nop)

        BR              X9

        NESTED_END A64NAME(_guard_dispatch_icall_nop)

        END
