/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ucxxrt.h
 * DATA:      2021/05/03
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once

#if __has_include(<wdm.h>)
#   ifndef  _KERNEL_MODE
#       error _KERNEL_MODE must be defined before all header files.
#   endif
#endif

#ifndef _CRTIMP
#   define _CRTIMP
#endif

#ifndef _VCRTIMP
#define _VCRTIMP _CRTIMP
#endif

#include <cstdint>

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
