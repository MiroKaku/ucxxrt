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

#ifdef __KERNEL_MODE
#   ifndef  _KERNEL_MODE
#       define  _KERNEL_MODE __KERNEL_MODE
#   endif
#endif

#ifndef _CRTIMP
#   define _CRTIMP
#endif

#ifndef _VCRTIMP
#define _VCRTIMP _CRTIMP
#endif


#include <cstdint>
#include <inc/malloc.h>
#include <inc/new_user.h>


namespace ucxxrt
{
#if (_MSVC_LANG < 201704L) && (__cplusplus < 201704L)
    constexpr char    __Version[] = u8"2.0.0.40";
#else
    constexpr char8_t __Version[] = u8"2.0.0.40";
#endif

#ifdef _KERNEL_MODE
    extern ULONG        DefaultPoolTag;
    extern POOL_TYPE    DefaultPoolType;
    extern ULONG        DefaultMdlProtection;
#endif
}


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
