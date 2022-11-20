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

 // Warnings which disabled for compiling
#if _MSC_VER >= 1200
#pragma warning(push)
// nonstandard extension used : nameless struct/union
#pragma warning(disable:4201)
// 'struct_name' : structure was padded due to __declspec(align())
#pragma warning(disable:4324)
// 'enumeration': a forward declaration of an unscoped enumeration must have an
// underlying type (int assumed)
#pragma warning(disable:4471)
#endif


#ifdef __KERNEL_MODE
#   ifndef  _KERNEL_MODE
#       define  _KERNEL_MODE __KERNEL_MODE
#   endif
#else
#   error user mode is not supported.
#endif

#ifndef _CRTIMP
#   define _CRTIMP
#endif

#ifndef _VCRTIMP
#define _VCRTIMP _CRTIMP
#endif


#include <fltKernel.h>
#include <stdint.h>


namespace ucxxrt
{
#ifdef _KERNEL_MODE
    extern PVOID PsSystemDllBase;
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


#if _MSC_VER >= 1200
#pragma warning(pop)
#endif
