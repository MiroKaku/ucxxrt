#include <isa_availability.h>


unsigned int __isa_available = __ISA_AVAILABLE_X86;

#if defined(_M_ARM)

int __cdecl __isa_available_init()
{
    if (IsProcessorFeaturePresent(PF_ARM_NEON_INSTRUCTIONS_AVAILABLE))
        __isa_available = __ISA_AVAILABLE_NEON;
    return 0;
}

#elif defined(_M_ARM64)

int __cdecl __isa_available_init()
{
    return 0;
}

#else // _M_AMD64 || _M_X86

#include <intrin.h>
#include <immintrin.h>

typedef enum _CPUID
{
    CPUID_BASIC_INFORMATION                         = 0x00000000,
    CPUID_PROCESSOR_INFO_AND_FEATURE_BITS           = 0x00000001,
    CPUID_CACHE_AND_TLB_DESCRIPTOR_INFORMATION      = 0x00000002,
    CPUID_PROCESSOR_SERIAL_NUMBER                   = 0X00000003,
    CPUID_INTEL_THREAD_CORE_AND_CACHE_TOPOLOGY      = 0x00000004,
    CPUID_MONITOR_MWAIT_INFORMATION                 = 0x00000005,
    CPUID_THERMAL_POWER_MANAGEMENT                  = 0x00000005,
    CPUID_EXTENDED_FEATURES                         = 0x00000007,
    CPUID_ARCHITECTURAL_PERFORMANCE_MONITORING      = 0x0000000A,
    CPUID_HYPERVISOR_VENDOR                         = 0x40000000,
    CPUID_GET_HIGHEST_EXTENDED_FUNCTION_IMPLEMENTED = 0x80000000,
    CPUID_EXTENDED_PROCESSOR_INFO_AND_FEATURE_BITS  = 0x80000001,
    CPUID_PROCESSOR_BRAND_STRING_2                  = 0x80000002,
    CPUID_PROCESSOR_BRAND_STRING_3                  = 0x80000003,
    CPUID_PROCESSOR_BRAND_STRING_4                  = 0x80000004,
    CPUID_L1_CACHE_AND_TLB_IDENTIFIERS              = 0x80000005,
    CPUID_EXTENDED_L2_CACHE_FEATURES                = 0x80000006,
    CPUID_ADVANCED_POWER_MANAGEMENT_INFORMATION     = 0x80000007,
    CPUID_VIRTUAL_AND_PHYSICAL_ADDRESS_SIZES        = 0x80000008,
    CPUID_AMD_EASTER_EGG                            = 0x8FFFFFFF,
    CPUID_SECURE_VIRTUAL_MACHINE_SPECIFICATIONS     = 0x8000000A,
} CPUID;

typedef union _CPUID_INFO
{
    struct
    {
        int EAX;
        int EBX;
        int ECX;
        int EDX;

    #pragma warning(suppress: 4201)
    };

    int Data[4];
} CPUID_INFO;

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

#define __ISA_ENABLED_X86           (1 << __ISA_AVAILABLE_X86)
#define __ISA_ENABLED_SSE2          (1 << __ISA_AVAILABLE_SSE2)
#define __ISA_ENABLED_SSE42         (1 << __ISA_AVAILABLE_SSE42)
#define __ISA_ENABLED_AVX           (1 << __ISA_AVAILABLE_AVX)
#define __ISA_ENABLED_AVX2          (1 << __ISA_AVAILABLE_AVX2)
#define __ISA_ENABLED_AVX512        (1 << __ISA_AVAILABLE_AVX512)

unsigned int __favor = __FAVOR_ATOM;

#if defined(_M_AMD64) || defined(_M_IX86)

unsigned int __isa_enabled = __ISA_ENABLED_X86;

#if defined(_M_AMD64)
size_t __memset_nt_threshold          = 0x2000000;
size_t __memset_fast_string_threshold = 0x80000;
#endif

int __cdecl __isa_available_init()
{
#define C1_AVX      (CF_OSXSAVE | CF_AVX)
#define C7_AVX512   (CX_AVX512F | CX_AVX512DQ | CX_AVX512CD | CX_AVX512BW | CX_AVX512VL)
#define XCR_AVX     (XSTATE_SSE | XSTATE_YMM)
#define XCR_AVX512F (XSTATE_OPMASK | XSTATE_ZMM | XSTATE_HI_ZMM)

    CPUID_INFO CpuId = { 0 };

    int  Features   = 0;
    int  FeaturesEx = 0;
    int  XFeatures  = 0;
    int  MaxId      = 0;
    bool Intel      = false;

    if (!IsProcessorFeaturePresent(PF_XMMI64_INSTRUCTIONS_AVAILABLE)) {
        goto ISA_AVAILABLE_X86;
    }

    __cpuid(CpuId.Data, CPUID_BASIC_INFORMATION);

    MaxId = CpuId.EAX;
    Intel = !(CpuId.EBX ^ 'uneG' | CpuId.EDX ^ 'Ieni' | CpuId.ECX ^ 'letn'); // GenuineIntel

    __cpuid(CpuId.Data, CPUID_PROCESSOR_INFO_AND_FEATURE_BITS);

    if (Intel) {
    #if defined(_M_AMD64)
        __memset_fast_string_threshold = 0x8000;
        __memset_nt_threshold          = ~0ull;
    #endif

        switch (CpuId.EAX & 0x0FFF3FF0) {
        default:
        {
            break;
        }
        case 0x000106C0:
        case 0x00020660:
        case 0x00020670:
        case 0x00030650:
        case 0x00030660:
        case 0x00030670:
        {
            __favor = 1 << __FAVOR_ATOM;
            break;
        }
        }
    }

    Features = CpuId.ECX;

    if (MaxId >= 7) {
        __cpuid(CpuId.Data, CPUID_EXTENDED_FEATURES);

        FeaturesEx = CpuId.EBX;
        if (FeaturesEx & CX_ERMS) {
            __favor = 1 << __FAVOR_ENFSTRG;
        }
    }

    if (Features & CF_SSE42) {
        if ((Features & C1_AVX) == C1_AVX) {
            XFeatures = (uint32_t)_xgetbv(_XCR_XFEATURE_ENABLED_MASK);
            if ((XFeatures & XCR_AVX) == XCR_AVX) {
                if (FeaturesEx & CX_AVX2) {
                    if ((FeaturesEx & C7_AVX512) == C7_AVX512 && (XFeatures & XCR_AVX512F) == XCR_AVX512F) {
                        goto ISA_AVAILABLE_AVX512;
                    }
                    goto ISA_AVAILABLE_AVX2;
                }
                goto ISA_AVAILABLE_AVX;
            }
            // goto ISA_AVAILABLE_SSE42;
        }
        goto ISA_AVAILABLE_SSE42;
    }
    goto ISA_AVAILABLE_SSE2;

ISA_AVAILABLE_X86:
    __isa_available = __ISA_AVAILABLE_X86;
    __isa_enabled   = __ISA_ENABLED_X86;
    return 0;

ISA_AVAILABLE_SSE2:
    __isa_available = __ISA_AVAILABLE_SSE2;
    __isa_enabled   = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2;
    return 0;

ISA_AVAILABLE_SSE42:
    __isa_available = __ISA_AVAILABLE_SSE42;
    __isa_enabled   = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42;
    return 0;

ISA_AVAILABLE_AVX:
    __isa_available = __ISA_AVAILABLE_AVX;
    __isa_enabled   = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42 | __ISA_ENABLED_AVX;
    return 0;

ISA_AVAILABLE_AVX2:
    __isa_available = __ISA_AVAILABLE_AVX2;
    __isa_enabled   = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42 | __ISA_ENABLED_AVX | __ISA_ENABLED_AVX2;
    return 0;

ISA_AVAILABLE_AVX512:
    __isa_available = __ISA_AVAILABLE_AVX512;
    __isa_enabled   = __ISA_ENABLED_X86 | __ISA_ENABLED_SSE2 | __ISA_ENABLED_SSE42 | __ISA_ENABLED_AVX | __ISA_ENABLED_AVX2 | __ISA_ENABLED_AVX512;
    return 0;

#undef C1_AVX
#undef C7_AVX512
#undef XCR_AVX
#undef XCR_AVX512F
}

#endif
#endif
