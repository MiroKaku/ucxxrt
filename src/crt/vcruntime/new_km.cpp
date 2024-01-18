/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      new_km.cpp
 * DATE:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <stdlib.h>
#include <vcruntime_new.h>
#include <vcstartup_internal.h>
#include <kext/kmalloc.h>

// Enable the compiler to elide null checks during LTCG
#pragma comment(linker, "/ThrowingNew")

////////////////////////////////////
// new() Fallback Ordering
//
// +----------+
// |new_scalar<---------------+
// +----^-----+               |
//      |                     |
// +----+-------------+  +----+----+
// |new_scalar_nothrow|  |new_array|
// +------------------+  +----^----+
//                            |
//               +------------+----+
//               |new_array_nothrow|
//               +-----------------+

_CRT_SECURITYCRITICAL_ATTRIBUTE
void* __CRTDECL operator new(size_t const size, int pool_type, unsigned long tag)
{
    for (;;)
    {
        if (void* const block = kmalloc(size, pool_type, tag))
        {
            return block;
        }

        if (_callnewh(size) == 0)
        {
            if (size == SIZE_MAX)
            {
                __scrt_throw_std_bad_array_new_length();
            }
            else
            {
                __scrt_throw_std_bad_alloc();
            }
        }

        // The new handler was successful; try to allocate again...
    }
}

void* __CRTDECL operator new[](size_t const size, int pool_type, unsigned long tag)
{
    return operator new(size, pool_type, tag);
}
