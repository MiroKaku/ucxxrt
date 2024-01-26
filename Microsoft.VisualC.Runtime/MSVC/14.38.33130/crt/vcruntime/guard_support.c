/***
*guard_support.c - logic and globals to support the Guard security feature
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Define the globals and default check routine for the Guard security
*       feature.
*
*       N.B.  Code and data declared by this module must be in the form of
*             selectany COMDATs, so that duplicate definitions between the
*             CRT and no_cfg_support are permitted for the restricted use
*             cases of the no_cfg_support module.
*
*******************************************************************************/

#include <Windows.h>
#include <intrin.h>
#include <castguard.h>

#ifndef FAST_FAIL_CAST_GUARD
#define FAST_FAIL_CAST_GUARD 65
#endif

typedef
void
(__fastcall *GUARDCF_CHECK_ROUTINE) (
    uintptr_t Target
    );

#if defined(_AMD64_) || defined(_ARM64_)

//
// Deliberately misdeclared as data and not code so that the free branch
// instruction is not itself considered a valid call target.
//

extern const unsigned char _guard_dispatch_icall_nop;

#endif

#if defined(_AMD64_)

//
// Deliberately misdeclared as data and not code so that the free branch
// instruction is not itself considered a valid call target.
//

extern const unsigned char _guard_xfg_dispatch_icall_nop;

#endif

//
// The CHPE version of the _guard_check_icall_nop function does not preserve
// ECX, as is required by the custom CFG calling convention on x86. Force the
// guest version of this function to be chosen instead.
//

#if defined(_M_HYBRID_X86_ARM64)

__declspec(hybrid_guest)
void
__fastcall
_guard_check_icall_nop (
    _In_ uintptr_t Target
    );

#else

extern
__inline
void
__fastcall
_guard_check_icall_nop (
    _In_ uintptr_t Target
    )

/*++

Routine Description:

    This function performs a no-op check when invoked by the compiler to check
    the integrity of a function pointer for Control Flow Guard (/guard).

Arguments:

    Target - Supplies the function pointer to check.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(Target);
    return;
}

#endif

#pragma section(".00cfg", read)

// Devdiv:1093613 - __guard_icall_check_fptr is only modified by the loader and
// so it appears to be read-only during compilation. We mark it volatile to
// ensure uses of it are not optimized away.
__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
volatile PVOID __guard_check_icall_fptr = (PVOID)_guard_check_icall_nop;

__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
volatile PVOID __guard_xfg_check_icall_fptr = (PVOID)_guard_check_icall_nop;

#if defined(_AMD64_) || defined(_ARM64_)

__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
volatile PVOID __guard_dispatch_icall_fptr = (PVOID)&_guard_dispatch_icall_nop;

#endif

#if defined(_AMD64_)

__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
volatile PVOID __guard_xfg_dispatch_icall_fptr = (PVOID)&_guard_xfg_dispatch_icall_nop;

__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
volatile PVOID __guard_xfg_table_dispatch_icall_fptr = (PVOID)&_guard_xfg_dispatch_icall_nop;

#endif

__declspec(allocate(".00cfg"))
DECLSPEC_SELECTANY
VOID (* volatile __castguard_check_failure_os_handled_fptr)(PVOID rhsVftablePtr) = NULL;

DECLSPEC_SELECTANY
volatile __check_guard_fp __castguard_check_failure_user_handled_fptr = NULL;

extern
__inline
int
_guard_icall_checks_enforced (
    VOID
    )

/*++

Routine Description:

    This function determines whether Control Flow Guard is enforced for the
    current module.

Arguments:

    None.

Return Value:

    A nonzero value is returned as the function value if Control Flow Guard
    is enforced for the current module.

--*/

{

    return (ReadPointerNoFence(&__guard_check_icall_fptr) != (PVOID)_guard_check_icall_nop);
}


#if defined(_AMD64_)
extern
__inline
int
__cdecl
_guard_rf_checks_enforced (
    VOID
    )

/*++

Routine Description:

    This function determines whether Return Flow Guard is enforced for the
    current module.

Arguments:

    None.

Return Value:

    A nonzero value is returned as the function value if Return Flow Guard
    is enforced for the current module.

--*/

{
    return 0;
}

#endif

#pragma section(".rdata$CastGuardVftablesA", read)
#pragma section(".rdata$CastGuardVftablesC", read)

//
// Count (in number of pointers) of the alignment of the CastGuard region.
// This translates to 128-byte alignment on 64-bit platforms and 64-bytes
// on 32-bit platforms.
//

#define CAST_GUARD_SECTION_ALIGNMENT_IN_PTRS 16

#if defined(_WIN64)
#define CAST_GUARD_SECTION_ALIGNMENT 128
#else
#define CAST_GUARD_SECTION_ALIGNMENT 64
#endif

static_assert(CAST_GUARD_SECTION_ALIGNMENT_IN_PTRS * sizeof(void*) == CAST_GUARD_SECTION_ALIGNMENT, "Invalid alignment");

//
// CastGuard checks lowered by the compiler, for efficiency, have an assumption that vftables laid out
// by CastGuard start at a 128-byte alignment for 64-bit and 64-byte alignment for 32-bit. The section these
// vftables are placed have this alignment specified by the compiler.
//
// __CastGuardVftableStart is a very special symbol. In order to do AppCompat checks correctly,
// we need to know the precise distance of __CastGuardVftablesStart variable from the first
// vftable laid out by CastGuard. The CastGuardVftablesB section (created by the compiler)
// is aligned by 16*sizeof(void*). The start variable therefore must have the exact same
// alignment as the section such that there's always 16*sizeof(void*) bytes between the address
// of this variable and the address of the first vftable.
//
// If no vftables are laid out, __CastGuardVftablesStart will be 64/128 bytes away from the
// __CastGuardVftablesEnd global variable. The code in __castguard_compat_check explicitly
// adds this 64/128 bytes to the __CastGuardVftablesStart, so it's important to ensure that the
// alignment is always respected.
//

struct __declspec(align(CAST_GUARD_SECTION_ALIGNMENT)) CastGuardVftables {
    char padding[CAST_GUARD_SECTION_ALIGNMENT];
};

__declspec(allocate(".rdata$CastGuardVftablesA"))
DECLSPEC_SELECTANY
volatile struct CastGuardVftables __CastGuardVftablesStart;

__declspec(allocate(".rdata$CastGuardVftablesC"))
DECLSPEC_SELECTANY
volatile struct CastGuardVftables __CastGuardVftablesEnd;

static
inline
BOOL
__cdecl __castguard_compat_check(PVOID rhsVftablePtr)
{
    ULONG_PTR realVftableRangeStart = (ULONG_PTR)&__CastGuardVftablesStart + sizeof(struct CastGuardVftables);
    ULONG_PTR realVftableRangeEnd = (ULONG_PTR)&__CastGuardVftablesEnd;
    ULONG_PTR vftableRangeSize = realVftableRangeEnd - realVftableRangeStart;

    return (ULONG_PTR)rhsVftablePtr - realVftableRangeStart <= vftableRangeSize;
}

extern
inline
__check_guard_fp
__cdecl __castguard_set_user_handler(__check_guard_fp new_handler) 
{
    // We are doing an atomic operation to set the new value and get the old value.
    // Therefore, we're using the InterlockedExchangePointer to make it synchronized.

    __check_guard_fp old_handler;
    old_handler = (__check_guard_fp) InterlockedExchangePointer((PVOID volatile*) &__castguard_check_failure_user_handled_fptr, 
                                                                (PVOID) new_handler);

    return old_handler;
}


static
inline
void
__declspec(guard(nocf))
__cdecl __castguard_check_failure_os_handled_wrapper(
    PVOID rhsVftablePtr
    )
{

    //
    // This function is opted out of CFG because the os handled function pointer
    // is allocated within ".00cfg" section. This section benefits from the same
    // level of protection as a CFG pointer would.
    //

    __check_guard_fp castguard_os_handled_fptr = (__check_guard_fp) ReadPointerNoFence((PVOID volatile*) &__castguard_check_failure_os_handled_fptr);

    if (castguard_os_handled_fptr != NULL)
    {
        castguard_os_handled_fptr(rhsVftablePtr);
    }

    return;
}

static
inline
void
__cdecl __castguard_check_failure_user_handled_wrapper(
    PVOID rhsVftablePtr
    )
{
    __check_guard_fp castguard_user_handled_fptr = (__check_guard_fp) ReadPointerNoFence((PVOID volatile*) &__castguard_check_failure_user_handled_fptr);

    if (castguard_user_handled_fptr != NULL)
    {
        castguard_user_handled_fptr(rhsVftablePtr);
    }

    return;
}

extern
inline
void __cdecl __castguard_check_failure_nop(PVOID rhsVftablePtr)
{
    (void)rhsVftablePtr;
    //
    // Don't bother with the compat check, we'll do nothing either way.
    //
    return;
}

extern
inline
void __cdecl __castguard_check_failure_debugbreak(PVOID rhsVftablePtr)
{
    if (__castguard_compat_check(rhsVftablePtr))
    {
        __debugbreak();
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range.
    //
    return;
}

extern
inline
void __cdecl __castguard_check_failure_fastfail(PVOID rhsVftablePtr)
{
    if (__castguard_compat_check(rhsVftablePtr))
    {
        __fastfail(FAST_FAIL_CAST_GUARD);
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range.
    //
    return;
}

extern
inline
void __cdecl __castguard_check_failure_os_handled(PVOID rhsVftablePtr)
{
    if (__castguard_compat_check(rhsVftablePtr))
    {
        __castguard_check_failure_os_handled_wrapper(rhsVftablePtr);
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range or the OS handler returned.
    //
    return;
}

extern
inline
void __cdecl __castguard_check_failure_user_handled(PVOID rhsVftablePtr)
{
    if (__castguard_compat_check(rhsVftablePtr))
    {
        __castguard_check_failure_user_handled_wrapper(rhsVftablePtr);
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range or the user handler returned.
    //
    return;
}

static
inline
BOOL
__cdecl __castguard_slow_path_compat_check(PVOID rhsVftablePtr, SIZE_T compareVftableOffset, SIZE_T maxIndex)
{
    ULONG_PTR realVftableRangeStart = (ULONG_PTR)&__CastGuardVftablesStart + sizeof(struct CastGuardVftables);
    ULONG_PTR compareBase = realVftableRangeStart + compareVftableOffset;

    return ((ULONG_PTR)rhsVftablePtr - compareBase) > maxIndex;
}

extern
inline
void __cdecl __castguard_slow_path_check_debugbreak(PVOID rhsVftablePtr, SIZE_T compareVftableOffset, SIZE_T maxIndex)
{
    if (__castguard_slow_path_compat_check(rhsVftablePtr, compareVftableOffset, maxIndex) && __castguard_compat_check(rhsVftablePtr)) {
        __debugbreak();
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range.
    //
    return;
}

extern
inline
void __cdecl __castguard_slow_path_check_fastfail(PVOID rhsVftablePtr, SIZE_T compareVftableOffset, SIZE_T maxIndex)
{
    if (__castguard_slow_path_compat_check(rhsVftablePtr, compareVftableOffset, maxIndex) && __castguard_compat_check(rhsVftablePtr)) {
        __fastfail(FAST_FAIL_CAST_GUARD);
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range.
    //
    return;
}

extern
inline
void __cdecl __castguard_slow_path_check_nop(PVOID rhsVftablePtr, SIZE_T compareVftableOffset, SIZE_T maxIndex)
{
    UNREFERENCED_PARAMETER(rhsVftablePtr);
    UNREFERENCED_PARAMETER(compareVftableOffset);
    UNREFERENCED_PARAMETER(maxIndex);
    //
    // Don't bother with the compat check, we'll do nothing either way.
    //
    return;
}

extern
inline
void __cdecl __castguard_slow_path_check_os_handled(PVOID rhsVftablePtr, SIZE_T compareVftableOffset, SIZE_T maxIndex)
{
    if (__castguard_slow_path_compat_check(rhsVftablePtr, compareVftableOffset, maxIndex) &&
        __castguard_compat_check(rhsVftablePtr))
    {
        __castguard_check_failure_os_handled_wrapper(rhsVftablePtr);
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range or the OS handler returned.
    //
    return;
}

extern
inline
void __cdecl __castguard_slow_path_check_user_handled(PVOID rhsVftablePtr, SIZE_T compareVftableOffset, SIZE_T maxIndex)
{
    if (__castguard_slow_path_compat_check(rhsVftablePtr, compareVftableOffset, maxIndex) &&
        __castguard_compat_check(rhsVftablePtr))
    {
        __castguard_check_failure_user_handled_wrapper(rhsVftablePtr);
    }

    //
    // Fail open if vftable is not in the CastGuard vftable range or the user handler returned.
    //
    return;
}