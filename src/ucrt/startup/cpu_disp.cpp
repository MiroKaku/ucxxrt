/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      cpu_disp.cpp
 * DATE:      2021/04/25
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/*
 * Reference: https://github.com/SpoilerScriptsGroup/RetrievAL/blob/master/SpoilerAL-winmm.dll/crt/msvc/isa_available_init.c
 */

#if defined(_M_IX86) || defined(_M_X64)

#include <intrin.h>
#include <immintrin.h>
#include <isa_availability.h>


// BitScanForward
// 
// for constant value
#define _BSF8(x, default) ( \
    ((x) & 0x01) ? 0 :      \
    ((x) & 0x02) ? 1 :      \
    ((x) & 0x04) ? 2 :      \
    ((x) & 0x08) ? 3 :      \
    ((x) & 0x10) ? 4 :      \
    ((x) & 0x20) ? 5 :      \
    ((x) & 0x40) ? 6 :      \
    ((x) & 0x80) ? 7 :      \
    (default))
#define _BSF16(x, default) _BSF8(x, _BSF8((x) >> 8, (default) - 8) + 8)
#define _BSF32(x, default) _BSF16(x, _BSF16((x) >> 16, (default) - 16) + 16)
#define _BSF64(x, default) _BSF32(x, _BSF32((x) >> 32, (default) - 32) + 32)
#define BSF8(x) _BSF8(x, -1)
#define BSF16(x) _BSF16(x, -1)
#define BSF32(x) _BSF32(x, -1)
#define BSF64(x) _BSF64(x, -1)

// for constant value
#define MASM_BSF32(x) (                                      -1 + \
    (  (x)         and 1                                 )      + \
    ((((x) shr  1) and 1) and (((x) and 0x00000001) eq 0)) *  2 + \
    ((((x) shr  2) and 1) and (((x) and 0x00000003) eq 0)) *  3 + \
    ((((x) shr  3) and 1) and (((x) and 0x00000007) eq 0)) *  4 + \
    ((((x) shr  4) and 1) and (((x) and 0x0000000F) eq 0)) *  5 + \
    ((((x) shr  5) and 1) and (((x) and 0x0000001F) eq 0)) *  6 + \
    ((((x) shr  6) and 1) and (((x) and 0x0000003F) eq 0)) *  7 + \
    ((((x) shr  7) and 1) and (((x) and 0x0000007F) eq 0)) *  8 + \
    ((((x) shr  8) and 1) and (((x) and 0x000000FF) eq 0)) *  9 + \
    ((((x) shr  9) and 1) and (((x) and 0x000001FF) eq 0)) * 10 + \
    ((((x) shr 10) and 1) and (((x) and 0x000003FF) eq 0)) * 11 + \
    ((((x) shr 11) and 1) and (((x) and 0x000007FF) eq 0)) * 12 + \
    ((((x) shr 12) and 1) and (((x) and 0x00000FFF) eq 0)) * 13 + \
    ((((x) shr 13) and 1) and (((x) and 0x00001FFF) eq 0)) * 14 + \
    ((((x) shr 14) and 1) and (((x) and 0x00003FFF) eq 0)) * 15 + \
    ((((x) shr 15) and 1) and (((x) and 0x00007FFF) eq 0)) * 16 + \
    ((((x) shr 16) and 1) and (((x) and 0x0000FFFF) eq 0)) * 17 + \
    ((((x) shr 17) and 1) and (((x) and 0x0001FFFF) eq 0)) * 18 + \
    ((((x) shr 18) and 1) and (((x) and 0x0003FFFF) eq 0)) * 19 + \
    ((((x) shr 19) and 1) and (((x) and 0x0007FFFF) eq 0)) * 20 + \
    ((((x) shr 20) and 1) and (((x) and 0x000FFFFF) eq 0)) * 21 + \
    ((((x) shr 21) and 1) and (((x) and 0x001FFFFF) eq 0)) * 22 + \
    ((((x) shr 22) and 1) and (((x) and 0x003FFFFF) eq 0)) * 23 + \
    ((((x) shr 23) and 1) and (((x) and 0x007FFFFF) eq 0)) * 24 + \
    ((((x) shr 24) and 1) and (((x) and 0x00FFFFFF) eq 0)) * 25 + \
    ((((x) shr 25) and 1) and (((x) and 0x01FFFFFF) eq 0)) * 26 + \
    ((((x) shr 26) and 1) and (((x) and 0x03FFFFFF) eq 0)) * 27 + \
    ((((x) shr 27) and 1) and (((x) and 0x07FFFFFF) eq 0)) * 28 + \
    ((((x) shr 28) and 1) and (((x) and 0x0FFFFFFF) eq 0)) * 29 + \
    ((((x) shr 29) and 1) and (((x) and 0x1FFFFFFF) eq 0)) * 30 + \
    ((((x) shr 30) and 1) and (((x) and 0x3FFFFFFF) eq 0)) * 31 + \
    ((((x) shr 31) and 1) and (((x) and 0x7FFFFFFF) eq 0)) * 32)

// for constant value
#define MASM_BSWAP16(value) (       \
    (((value) shr 8) and 0x00FF) or \
    (((value) shl 8) and 0xFF00))

// for constant value
#define MASM_BSWAP24(value) (            \
    (((value) shr 16) and 0x000000FF) or \
    ( (value)         and 0x0000FF00) or \
    (((value) shl 16) and 0x00FF0000))

// for constant value
#define MASM_BSWAP32(value) (            \
    (((value) shr 24) and 0x000000FF) or \
    (((value) shr  8) and 0x0000FF00) or \
    (((value) shl  8) and 0x00FF0000) or \
    (((value) shl 24) and 0xFF000000))


_CRT_BEGIN_C_HEADER

#define __ISA_ENABLED_X86           0x00000001
#define __ISA_ENABLED_SSE2          0x00000002
#define __ISA_ENABLED_SSE42         0x00000004
#define __ISA_ENABLED_AVX           0x00000008
#define __ISA_ENABLED_AVX2          0x00000020
#define __ISA_ENABLED_AVX512        0x00000040

#define __FAVOR_ATOM                0
#define __FAVOR_ENFSTRG             1
#define __FAVOR_XMMLOOP             2

/* Features in ecx for leaf 1 */
#define CF_SSE42                    0x00100000
#define CF_OSXSAVE                  0x08000000
#define CF_AVX                      0x10000000

/* Features in ebx for leaf 7 sub-leaf 0 */
#define CX_AVX2                     0x00000020
#define CX_ERMS                     0x00000200
#define CX_AVX512F                  0x00010000
#define CX_AVX512DQ                 0x00020000
#define CX_AVX512CD                 0x10000000
#define CX_AVX512BW                 0x40000000
#define CX_AVX512VL                 0x80000000

/* Get XCR_XFEATURE_ENABLED_MASK register with xgetbv. */
#define XCR_XFEATURE_ENABLED_MASK   0x00000000
#define XSTATE_FP                   0x00000001
#define XSTATE_SSE                  0x00000002
#define XSTATE_YMM                  0x00000004
#define XSTATE_OPMASK               0x00000020
#define XSTATE_ZMM                  0x00000040
#define XSTATE_HI_ZMM               0x00000080

unsigned int __isa_available = __ISA_AVAILABLE_X86;
unsigned int __isa_enabled   = __ISA_ENABLED_X86;
unsigned int __favor         = 0;

BOOLEAN __stdcall __IsProcessorFeaturePresent(_In_ ULONG ProcessorFeature)
{
    return (BOOLEAN)ExIsProcessorFeaturePresent(ProcessorFeature);
}

#if defined(_AMD64_)
#pragma intrinsic(__cpuid)
#pragma intrinsic(__cpuidex)

void __cdecl __isa_available_init()
{
    #define C1_AVX      (CF_OSXSAVE | CF_AVX)
    #define C7_AVX512   (CX_AVX512F | CX_AVX512DQ | CX_AVX512CD | CX_AVX512BW | CX_AVX512VL)
    #define XCR_AVX     (XSTATE_SSE | XSTATE_YMM)
    #define XCR_AVX512F (XSTATE_OPMASK | XSTATE_ZMM | XSTATE_HI_ZMM)

    struct {
        uint32_t eax;
        uint32_t ebx;
        uint32_t ecx;
        uint32_t edx;
    }        cpuInfo;
    uint32_t cpuid_0_eax;
    uint32_t cpuid_1_ecx;
    uint32_t cpuid_7_ebx;
    uint32_t xgetbv_eax;
    uint32_t intel_outside;

    __favor = 0;
    if (!__IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE))
        goto ISA_AVAILABLE_X86;
    __cpuid((int *)&cpuInfo, 0);
    cpuid_0_eax = cpuInfo.eax;
    intel_outside =
        (cpuInfo.ebx ^ _ByteSwap32('Genu')) |
        (cpuInfo.edx ^ _ByteSwap32('ineI')) |
        (cpuInfo.ecx ^ _ByteSwap32('ntel'));
    __cpuid((int *)&cpuInfo, 1);
    cpuid_1_ecx = cpuInfo.ecx;
    if (!intel_outside)
        switch (cpuInfo.eax & 0x0FFF3FF0) {
        case 0x000106C0:
        case 0x00020660:
        case 0x00020670:
        case 0x00030650:
        case 0x00030660:
        case 0x00030670:
            __favor = 1 << __FAVOR_ATOM;
        }
    cpuid_7_ebx = 0;
    if (cpuid_0_eax >= 7) {
        __cpuidex((int *)&cpuInfo, 7, 0);
        __favor |= ((cpuid_7_ebx = cpuInfo.ebx) & CX_ERMS) >> (BSF32(CX_ERMS) - __FAVOR_ENFSTRG);
    }
    if (cpuid_1_ecx & CF_SSE42)
        if ((cpuid_1_ecx & C1_AVX) == C1_AVX && ((xgetbv_eax = (uint32_t)_xgetbv(0)) & XCR_AVX) == XCR_AVX)
            if (cpuid_7_ebx & CX_AVX2)
                if ((cpuid_7_ebx & C7_AVX512) == C7_AVX512 && (xgetbv_eax & XCR_AVX512F) == XCR_AVX512F)
                    goto ISA_AVAILABLE_AVX512;
                else
                    goto ISA_AVAILABLE_AVX2;
            else
                goto ISA_AVAILABLE_AVX;
        else
            goto ISA_AVAILABLE_SSE42;
    else
        goto ISA_AVAILABLE_SSE2;

ISA_AVAILABLE_X86:
    __isa_available = __ISA_AVAILABLE_X86;
    __isa_enabled = __ISA_ENABLED_X86;
    return;

ISA_AVAILABLE_SSE2:
    __isa_available = __ISA_AVAILABLE_SSE2;
    __isa_enabled = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2;
    return;

ISA_AVAILABLE_SSE42:
    __isa_available = __ISA_AVAILABLE_SSE42;
    __isa_enabled = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42;
    return;

ISA_AVAILABLE_AVX:
    __isa_available = __ISA_AVAILABLE_AVX;
    __isa_enabled = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42 | __ISA_ENABLED_AVX;
    return;

ISA_AVAILABLE_AVX2:
    __isa_available = __ISA_AVAILABLE_AVX2;
    __isa_enabled = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42 | __ISA_ENABLED_AVX | __ISA_ENABLED_AVX2;
    return;

ISA_AVAILABLE_AVX512:
    __isa_available = __ISA_AVAILABLE_AVX512;
    __isa_enabled = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42 | __ISA_ENABLED_AVX | __ISA_ENABLED_AVX2 | __ISA_ENABLED_AVX512;
    return;

    #undef C1_AVX
    #undef C7_AVX512
    #undef XCR_AVX
    #undef XCR_AVX512F
}
#elif defined(_X86_)
__declspec(naked) void __cdecl __isa_available_init()
{
    __asm
    {
        #define C1_AVX      (CF_OSXSAVE or CF_AVX)
        #define C7_AVX512   (CX_AVX512F or CX_AVX512DQ or CX_AVX512CD or CX_AVX512BW or CX_AVX512VL)
        #define XCR_AVX     (XSTATE_SSE or XSTATE_YMM)
        #define XCR_AVX512F (XSTATE_OPMASK or XSTATE_ZMM or XSTATE_HI_ZMM)

        #define cpuid_0_eax eax
        #define cpuid_1_ecx ecx
        #define cpuid_7_ebx ebx

        mov     dword ptr [__favor], 0
        push    PF_XMMI64_INSTRUCTIONS_AVAILABLE
        call    __IsProcessorFeaturePresent
        test    eax, eax
        jz      ISA_AVAILABLE_X86
        push    ebx
        xor     eax, eax
        cpuid
        xor     ebx, MASM_BSWAP32('Genu')
        xor     edx, MASM_BSWAP32('ineI')
        xor     ecx, MASM_BSWAP32('ntel')
        or      ebx, edx
        push    cpuid_0_eax
        or      ebx, ecx
        mov     eax, 1
        push    ebx
        cpuid
        pop     ebx
        mov     edx, eax
        and     eax, 0FFF3FF0H and (not (000106C0H or 00020660H or 00020670H or 00030650H or 00030660H or 00030670H))
        and     edx, 000106C0H or 00020660H or 00020670H or 00030650H or 00030660H or 00030670H
        or      eax, ebx
        jnz     L2
        mov     eax, edx
        and     edx, 000106C0H and 00020660H and 00020670H and 00030650H and 00030660H and 00030670H
        cmp     edx, 000106C0H and 00020660H and 00020670H and 00030650H and 00030660H and 00030670H
        jne     L2
        sub     eax, 000106C0H
        jz      L1
        sub     eax, 00020660H - 000106C0H
        jz      L1
        sub     eax, 00020670H - 00020660H
        jz      L1
        sub     eax, 00030650H - 00020670H
        jz      L1
        sub     eax, 00030660H - 00030650H
        jz      L1
        cmp     eax, 00030670H - 00030660H
        jne     L2
    L1:
        mov     dword ptr [__favor], 1 shl __FAVOR_ATOM
    L2:
        pop     cpuid_0_eax
        xor     cpuid_7_ebx, cpuid_7_ebx
        cmp     cpuid_0_eax, 7
        jb      L3
        push    cpuid_1_ecx
        mov     eax, 7
        xor     ecx, ecx
        cpuid
        mov     ecx, cpuid_7_ebx
        and     ebx, CX_ERMS
        shr     ebx, MASM_BSF32(CX_ERMS) - __FAVOR_ENFSTRG
        mov     eax, dword ptr [__favor]
        or      eax, ebx
        mov     cpuid_7_ebx, ecx
        mov     dword ptr [__favor], eax
        pop     cpuid_1_ecx
    L3:
        mov     eax, cpuid_1_ecx
        and     ecx, C1_AVX
        test    eax, CF_SSE42
        jz      ISA_AVAILABLE_SSE2
        xor     ecx, C1_AVX
        jnz     ISA_AVAILABLE_SSE42
        xgetbv
        mov     ecx, eax
        and     eax, XCR_AVX
        cmp     eax, XCR_AVX
        jne     ISA_AVAILABLE_SSE42
        test    cpuid_7_ebx, CX_AVX2
        jz      ISA_AVAILABLE_AVX
        and     ebx, C7_AVX512
        and     ecx, XCR_AVX512F
        cmp     ebx, C7_AVX512
        jne     ISA_AVAILABLE_AVX2
        cmp     ecx, XCR_AVX512F
        jne     ISA_AVAILABLE_AVX2
        pop     ebx
        jmp     ISA_AVAILABLE_AVX512

        align   16
    ISA_AVAILABLE_X86:
        mov     dword ptr [__isa_available], __ISA_AVAILABLE_X86
        mov     dword ptr [__isa_enabled], __ISA_ENABLED_X86
        ret

        align   16
    ISA_AVAILABLE_SSE2:
        mov     dword ptr [__isa_available], __ISA_AVAILABLE_SSE2
        mov     dword ptr [__isa_enabled], __ISA_ENABLED_X86 or __ISA_ENABLED_SSE2
        pop     ebx
        ret

        align   16
    ISA_AVAILABLE_SSE42:
        mov     dword ptr [__isa_available], __ISA_AVAILABLE_SSE42
        mov     dword ptr [__isa_enabled], __ISA_ENABLED_X86 or __ISA_ENABLED_SSE2 or __ISA_ENABLED_SSE42
        pop     ebx
        ret

        align   16
    ISA_AVAILABLE_AVX:
        mov     dword ptr [__isa_available], __ISA_AVAILABLE_AVX
        mov     dword ptr [__isa_enabled], __ISA_ENABLED_X86 or __ISA_ENABLED_SSE2 or __ISA_ENABLED_SSE42 or __ISA_ENABLED_AVX
        pop     ebx
        ret

        align   16
    ISA_AVAILABLE_AVX2:
        mov     dword ptr [__isa_available], __ISA_AVAILABLE_AVX2
        mov     dword ptr [__isa_enabled], __ISA_ENABLED_X86 or __ISA_ENABLED_SSE2 or __ISA_ENABLED_SSE42 or __ISA_ENABLED_AVX or __ISA_ENABLED_AVX2
        pop     ebx
        ret

        align   16
    ISA_AVAILABLE_AVX512:
        mov     dword ptr [__isa_available], __ISA_AVAILABLE_AVX512
        mov     dword ptr [__isa_enabled], __ISA_ENABLED_X86 or __ISA_ENABLED_SSE2 or __ISA_ENABLED_SSE42 or __ISA_ENABLED_AVX or __ISA_ENABLED_AVX2 or __ISA_ENABLED_AVX512
        ret

        #undef C1_AVX
        #undef C7_AVX512
        #undef XCR_AVX
        #undef XCR_AVX512F
        #undef cpuid_0_eax
        #undef cpuid_1_ecx
        #undef cpuid_7_ebx
    }
}
#endif

_CRT_END_C_HEADER

#endif
