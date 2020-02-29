/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ptd_downlevel.h
 * DATA:      2020/02/08
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once
#include "vcruntime/vcruntime.h"

namespace ucxxrt
{
    extern"C" __ucxxrt_ptd * __cdecl __ucxxrt_getptd();
}
