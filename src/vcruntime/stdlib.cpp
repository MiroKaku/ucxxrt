/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      stdlib.cpp
 * DATA:      2020/02/28
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include "vcruntime/ehdata_values.h"


#ifndef _CRTNOALIAS
#if     ( defined(_M_IA64) && defined(_MSC_VER_GREATER_THEN_13102050) ) || _MSC_VER >= 1400
#define _CRTNOALIAS __declspec(noalias)

#define _CRTRESTRICT __declspec(restrict)

#else  /* ( defined(_M_IA64) && defined(_MSC_VER_GREATER_THEN_13102050) ) || _MSC_VER >= 1400 */

#define _CRTNOALIAS

#define _CRTRESTRICT

#endif  /* ( defined(_M_IA64) && defined(_MSC_VER_GREATER_THEN_13102050) ) || _MSC_VER >= 1400 */
#endif


#if __has_include(<wdm.h>)

#if _HAS_FLOATPOINT
#pragma comment(lib, "libcntpr")
#endif

extern ULONG     DefaultPoolTag;
extern POOL_TYPE DefaultPoolType;

#pragma warning(push)
#pragma warning(disable: 4559)
_CRTIMP _CRTNOALIAS void __cdecl free(_Pre_maybenull_ _Post_invalid_ void* _Memory)
{
    ExFreePoolWithTag(_Memory, DefaultPoolTag);
}
_Check_return_ _Ret_maybenull_ _Post_writable_byte_size_(_Size) _CRTIMP _CRT_JIT_INTRINSIC _CRTNOALIAS _CRTRESTRICT void* __cdecl malloc(_In_ size_t _Size)
{
    return ExAllocatePoolWithTag(DefaultPoolType, _Size, DefaultPoolTag);
}
#pragma warning(pop)

#endif