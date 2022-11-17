/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      moverride.cpp
 * DATA:      2022/11/17
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

/* std allocator */

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) _CRTRESTRICT void* __cdecl std_malloc(size_t const size);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) void __cdecl std_free(void* const block);

extern "C" _CRT_HYBRIDPATCHABLE __declspec(noinline) size_t __cdecl std_msize(void* const block);

/* crt override */

extern "C" volatile _malloc_t   __override_malloc = nullptr;
extern "C" volatile _free_t     __override_free   = nullptr;
extern "C" volatile _msize_t    __override_msize  = nullptr;

extern "C" __declspec(noinline) void __cdecl _set_allocator(_malloc_t _malloc, _free_t _free, _msize_t _msize)
{
    if (_malloc == nullptr || _free == nullptr || _msize == nullptr) {

        _malloc = &std_malloc;
        _free   = &std_free;
        _msize  = &std_msize;
    }

    InterlockedExchangePointer((volatile PVOID*)&__override_malloc, _malloc);
    InterlockedExchangePointer((volatile PVOID*)&__override_free,   _free);
    InterlockedExchangePointer((volatile PVOID*)&__override_msize,  _msize);
}
