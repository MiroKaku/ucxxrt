/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ucxxrt.h
 * DATA:      2020/02/05
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once

// If you use float-point please open _HAS_FLOATPOINT
// and link libcntpr.lib
//   1. Open the project's Property Pages dialog box.
//   2. Choose the Input property page in the Linker folder.
//   3. Modify the Additional Dependencies property to add the libcntpr.lib file.
#define _HAS_FLOATPOINT 1


#ifndef _CRT_BEGIN_C_HEADER
#define _CRT_BEGIN_C_HEADER            \
        __pragma(pack(push, _CRT_PACKING)) \
        extern "C" {
#endif // !_CRT_BEGIN_C_HEADER

#ifndef _CRT_END_C_HEADER
#define _CRT_END_C_HEADER \
        }                     \
        __pragma(pack(pop))
#endif

#ifndef _CRTIMP
#   define _CRTIMP
#endif

#ifndef _VCRTIMP
#define _VCRTIMP _CRTIMP
#endif

#ifndef _HAS_NODISCARD
#ifndef __has_cpp_attribute
#define _HAS_NODISCARD 0
#elif __has_cpp_attribute(nodiscard) >= 201603L // TRANSITION, VSO#939899 (need toolset update)
#define _HAS_NODISCARD 1
#else
#define _HAS_NODISCARD 0
#endif
#endif // _HAS_NODISCARD

#if _HAS_NODISCARD
    #define _NODISCARD [[nodiscard]]
#else // ^^^ CAN HAZ [[nodiscard]] / NO CAN HAZ [[nodiscard]] vvv
    #define _NODISCARD
#endif // _HAS_NODISCARD


#include "include/stdint.h"
#include "include/new.h"
#include "include/typeinfo.h"


namespace ucxxrt
{
#if (_MSVC_LANG < 201704L) && (__cplusplus < 201704L)
    constexpr char    __Version[] = u8"0.0.0.2";
#else
    constexpr char8_t __Version[] = u8"0.0.0.2";
#endif

#if __has_include(<wdm.h>)
    extern ULONG        DefaultPoolTag;
    extern POOL_TYPE    DefaultPoolType;
    extern ULONG        DefaultMdlProtection;
#endif
}


namespace ucxxrt
{

#ifndef _ByteSwap16
#define _ByteSwap16(x) (                            \
    ((uint16_t(x) & uint16_t(0xFF << 8)) >> 8) |    \
    ((uint16_t(x) & uint16_t(0xFF >> 0)) << 8)      \
)
#endif

#ifndef _ByteSwap32
#define _ByteSwap32(x) (                            \
    ((uint32_t(x) & uint32_t(0xFF << 24)) >> 24) |  \
    ((uint32_t(x) & uint32_t(0xFF << 16)) >>  8) |  \
    ((uint32_t(x) & uint32_t(0xFF <<  8)) <<  8) |  \
    ((uint32_t(x) & uint32_t(0xFF <<  0)) << 24)    \
)
#endif

#ifndef _ByteSwap64
#define _ByteSwap64(x) (                            \
    ((uint64_t(x) & uint64_t(0xFF << 56)) >> 56) |  \
    ((uint64_t(x) & uint64_t(0xFF << 48)) >> 40) |  \
    ((uint64_t(x) & uint64_t(0xFF << 40)) >> 24) |  \
    ((uint64_t(x) & uint64_t(0xFF << 32)) >>  8) |  \
    ((uint64_t(x) & uint64_t(0xFF << 24)) <<  8) |  \
    ((uint64_t(x) & uint64_t(0xFF << 16)) << 24) |  \
    ((uint64_t(x) & uint64_t(0xFF <<  8)) << 40) |  \
    ((uint64_t(x) & uint64_t(0xFF <<  0)) << 56) |  \
)
#endif

}
