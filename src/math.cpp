/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      math.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <intrin.h>

#ifdef __cplusplus
extern"C" {
#endif
#if     !defined(__assembler)

#pragma function(ceil)
#pragma function(floor)

_CRTIMP double  __cdecl ceil(_In_ double x)
{
    return __ceil(x);
}

_CRTIMP double  __cdecl floor(_In_ double x)
{
    return __floor(x);
}

#endif
#ifdef __cplusplus
}
#endif
