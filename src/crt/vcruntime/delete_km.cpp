/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      delete_km.cpp
 * DATE:      2022/06/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <crtdbg.h>
#include <malloc.h>
#include <vcruntime_new.h>
#include <vcstartup_internal.h>
#include <kext/kmalloc.h>

////////////////////////////////////////////////////////////////
// delete() Fallback Ordering
//
// +-------------+
// |delete_scalar<----+-----------------------+
// +--^----------+    |                       |
//    |               |                       |
// +--+---------+  +--+---------------+  +----+----------------+
// |delete_array|  |delete_scalar_size|  |delete_scalar_nothrow|
// +--^----^----+  +------------------+  +---------------------+
//    |    |
//    |    +-------------------+
//    |                        |
// +--+--------------+  +------+-------------+
// |delete_array_size|  |delete_array_nothrow|
// +-----------------+  +--------------------+

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete(void* const block, int /*pool_type*/, unsigned long tag) noexcept
{
    kfree(block, tag);
}

_CRT_SECURITYCRITICAL_ATTRIBUTE
void __CRTDECL operator delete[](void* const block, int pool_type, unsigned long tag) noexcept
{
    operator delete(block, pool_type, tag);
}
