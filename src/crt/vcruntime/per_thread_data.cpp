//
// per_thread_data.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Per-Thread Data (PTD) used by the VCRuntime.
//
#include <vcruntime_internal.h>
#include <stdint.h>

#define PID_MAX_LIMIT USHRT_MAX

// In order to avoid dynamic allocation during CRT startup, the PTD for the
// startup thread is statically allocated:

struct __vcrt_ptd_km : public __vcrt_ptd
{
    void*       tid;
    long long   uid;
};

static NPAGED_LOOKASIDE_LIST __vcrt_startup_ptd_pools;
static RTL_AVL_TABLE         __vcrt_startup_ptd_table;
static KSPIN_LOCK            __vcrt_startup_ptd_table_lock;

static long long __get_thread_uid(_In_ PETHREAD thread)
{
    CLIENT_ID id = PsGetThreadClientId(thread);
    size_t high  = reinterpret_cast<size_t>(id.UniqueProcess) >> 2;
    size_t low   = reinterpret_cast<size_t>(id.UniqueThread ) >> 2;
    return (static_cast<long long>(high & ~uint32_t(0)) << 32) | (static_cast<long long>(low & ~uint32_t(0)));
}

RTL_GENERIC_COMPARE_RESULTS NTAPI __vcrt_ptd_table_compare(
    _In_ RTL_AVL_TABLE* /*table*/,
    _In_ PVOID first,
    _In_ PVOID second
)
{
    auto ptd1 = static_cast<__vcrt_ptd_km*>(first);
    auto ptd2 = static_cast<__vcrt_ptd_km*>(second);

    return
        (ptd1->tid < ptd2->tid) ? GenericLessThan :
        (ptd1->tid > ptd2->tid) ? GenericGreaterThan : GenericEqual;
}

PVOID NTAPI __vcrt_ptd_table_allocate(
    _In_ RTL_AVL_TABLE* /*table*/,
    _In_ CLONG /*size*/
)
{
    return ExAllocateFromNPagedLookasideList(&__vcrt_startup_ptd_pools);
}

VOID NTAPI __vcrt_ptd_table_free(
    _In_ RTL_AVL_TABLE* /*table*/,
    _In_ __drv_freesMem(Mem) _Post_invalid_ PVOID buffer
)
{
    return ExFreeToNPagedLookasideList(&__vcrt_startup_ptd_pools, buffer);
}

static __vcrt_ptd* __cdecl store_and_initialize_ptd(__vcrt_ptd* const ptd)
{
    BOOLEAN inserted = false;

    __vcrt_ptd* const new_ptd = static_cast<__vcrt_ptd*>(RtlInsertElementGenericTableAvl(
        &__vcrt_startup_ptd_table, ptd, sizeof __vcrt_ptd_km, &inserted));

    if (!new_ptd)
    {
        return nullptr;
    }

    // reuse outdated ptd.
    if (__get_thread_uid(PsGetCurrentThread()) != static_cast<__vcrt_ptd_km*>(new_ptd)->uid)
    {
        inserted = true;
        RtlSecureZeroMemory(new_ptd, sizeof __vcrt_ptd); // not reset pid/uid
    }

    if (inserted)
    {
#if defined _M_X64 || defined _M_ARM || defined _M_ARM64 || defined _M_HYBRID
        new_ptd->_CatchStateInParent = INVALID_CATCH_SPECIFIC_STATE;
#endif
    }

    return new_ptd;
}

extern "C" bool __cdecl __vcrt_initialize_ptd()
{
    constexpr auto size = ROUND_TO_SIZE(sizeof __vcrt_ptd_km + sizeof RTL_BALANCED_LINKS, sizeof(void*));

    ExInitializeNPagedLookasideList(&__vcrt_startup_ptd_pools, nullptr, nullptr,
        POOL_NX_ALLOCATION, size, __ucxxrt_tag, 0);

    KeInitializeSpinLock(&__vcrt_startup_ptd_table_lock);

    RtlInitializeGenericTableAvl(&__vcrt_startup_ptd_table, &__vcrt_ptd_table_compare,
        &__vcrt_ptd_table_allocate, &__vcrt_ptd_table_free, &__vcrt_startup_ptd_pools);

    __vcrt_ptd_km ptd{};
    ptd.tid = PsGetCurrentThreadId();
    ptd.uid = __get_thread_uid(PsGetCurrentThread());

    if (!store_and_initialize_ptd(&ptd))
    {
        __vcrt_uninitialize_ptd();
        return false;
    }

    return true;
}

extern "C" bool __cdecl __vcrt_uninitialize_ptd()
{
    auto ptd = static_cast<__vcrt_ptd*>(RtlGetElementGenericTableAvl(&__vcrt_startup_ptd_table, 0));

    while (ptd)
    {
        RtlDeleteElementGenericTableAvl(&__vcrt_startup_ptd_table, ptd);
        ptd = static_cast<__vcrt_ptd*>(RtlGetElementGenericTableAvl(&__vcrt_startup_ptd_table, 0));
    }

    ExDeleteNPagedLookasideList(&__vcrt_startup_ptd_pools);

    return true;
}

extern "C" __vcrt_ptd* __cdecl __vcrt_getptd_noexit()
{
    // The EH runtime relies on the PTD.  If we attempt to allocate a PTD and
    // that allocation attempt fails by raising a structured exception, the EH
    // runtime may intercept that structured exception and re-attempt to allocate
    // a PTD.  This may result in infinite recursion that causes a stack overflow.
    //
    // To handle this case more gracefully, we set the PTD pointer to a
    // sentinel value for the duration of the allocation attempt.  This allows us
    // to detect this unwanted reentrancy.

    // First see if we've already created per-thread data for this thread:

    __vcrt_ptd* existing_ptd = nullptr;

    __vcrt_ptd_km ptd{};
    ptd.tid = PsGetCurrentThreadId();
    ptd.uid = __get_thread_uid(PsGetCurrentThread());

    KLOCK_QUEUE_HANDLE lock_state{};
    KeAcquireInStackQueuedSpinLock(&__vcrt_startup_ptd_table_lock, &lock_state);
    do
    {
        existing_ptd = static_cast<__vcrt_ptd*>(RtlLookupElementGenericTableAvl(&__vcrt_startup_ptd_table, &ptd));
        if (existing_ptd)
        {
            break;
        }

        // No per-thread data for this thread yet. Try to create one:
        existing_ptd = store_and_initialize_ptd(&ptd);

    } while (false);
    KeReleaseInStackQueuedSpinLock(&lock_state);

    return existing_ptd;
}

// Returns the PTD if one exists; null otherwise.  Does not attempt to allocate a new PTD.
extern "C" __vcrt_ptd* __cdecl __vcrt_getptd_noinit()
{
    __vcrt_ptd* existing_ptd = nullptr;

    __vcrt_ptd_km ptd{};
    ptd.tid = PsGetCurrentThreadId();
    ptd.uid = __get_thread_uid(PsGetCurrentThread());

    KLOCK_QUEUE_HANDLE lock_state{};
    KeAcquireInStackQueuedSpinLock(&__vcrt_startup_ptd_table_lock, &lock_state);
    do
    {
        existing_ptd = static_cast<__vcrt_ptd*>(RtlLookupElementGenericTableAvl(&__vcrt_startup_ptd_table, &ptd));

    } while (false);
    KeReleaseInStackQueuedSpinLock(&lock_state);

    return existing_ptd;
}

extern "C" __vcrt_ptd* __cdecl __vcrt_getptd()
{
    __vcrt_ptd* const ptd = __vcrt_getptd_noexit();
    if (ptd == nullptr)
    {
        abort();
    }

    return ptd;
}

#if defined _VCRT_SAT_1
// This function accesses the base vcruntime's PTD by inferring its location from internal fields
// Via the exported functions below.
extern "C"
{
    void** __cdecl __current_exception();
    int*   __cdecl __processing_throw();
}

// Disable optimizations to prevent UB of memory accesses from creeping in.
#pragma optimize ("", off)
extern "C" RENAME_BASE_PTD(__vcrt_ptd)* __cdecl RENAME_BASE_PTD(__vcrt_getptd)()
{
    uint8_t* const curexception_offset = reinterpret_cast<uint8_t *>(__current_exception());
    uint8_t* const processingthrow_offset = reinterpret_cast<uint8_t *>(__processing_throw());

    RENAME_BASE_PTD(__vcrt_ptd)* const infer_from_curexception = reinterpret_cast<RENAME_BASE_PTD(__vcrt_ptd) *>(curexception_offset - offsetof(RENAME_BASE_PTD(__vcrt_ptd), _curexception));
    RENAME_BASE_PTD(__vcrt_ptd)* const infer_from_processingthrow = reinterpret_cast<RENAME_BASE_PTD(__vcrt_ptd) *>(processingthrow_offset - offsetof(RENAME_BASE_PTD(__vcrt_ptd), _ProcessingThrow));

    if (infer_from_curexception != infer_from_processingthrow)
    {
        abort();
    }

    return infer_from_curexception;
}
#pragma optimize ("", on)

RENAME_BASE_PTD(__vcrt_ptd)* __cdecl RENAME_BASE_PTD(__vcrt_getptd_noinit)()
{
    return RENAME_BASE_PTD(__vcrt_getptd)();
}

#endif

extern "C" void __cdecl __vcrt_freeptd(_Inout_opt_ __vcrt_ptd* const ptd)
{
    // If the argument is null, get the pointer for this thread. Note that we
    // must not call __vcrt_getptd, because it will allocate a new per-thread
    // data if one does not already exist.

    __vcrt_ptd_km current_ptd{};
    current_ptd.tid = PsGetCurrentThreadId();
    current_ptd.uid = __get_thread_uid(PsGetCurrentThread());


    __vcrt_ptd* const block_to_free = (ptd == nullptr)
        ? &current_ptd
        : ptd;

    KLOCK_QUEUE_HANDLE lock_state{};
    KeAcquireInStackQueuedSpinLock(&__vcrt_startup_ptd_table_lock, &lock_state);
    do
    {
        RtlDeleteElementGenericTableAvl(&__vcrt_startup_ptd_table, block_to_free);

    } while (false);
    KeReleaseInStackQueuedSpinLock(&lock_state);
}
