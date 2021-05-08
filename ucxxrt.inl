/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ucxxrt.inl
 * DATA:      2021/05/03
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once

#define WINBASEAPI

#if __has_include(<wdm.h>)
#   ifndef  _KERNEL_MODE
#       define  _KERNEL_MODE 1
#   endif

#   include <ntddk.h>
#   include <wdm.h>
#   include <ntimage.h>
#else
#   include <Windows.h>
#endif

#include <corecrt.h>

#define _VCRT_BUILD

#include <cstddef>
#include <cstdlib>
#include <minwindef.h>

#include "ucxxrt.h"
