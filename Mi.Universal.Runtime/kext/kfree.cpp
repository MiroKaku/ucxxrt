#include <corecrt_internal.h>
#include "kmalloc.h"


extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void __cdecl kfree(
    void* const   block,
    unsigned long tag
)
{
    if (block) {
        ExFreePoolWithTag(block, tag);
    }
}
