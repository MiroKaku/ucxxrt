// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// mutex support

#include "xmtx.hpp"

//#include "awint.hpp"

_EXTERN_C_UNLESS_PURE

// Win32 critical sections are recursive

void __CLRCALL_PURE_OR_CDECL _Mtxinit(_Rmtx* _Mtx) noexcept { // initialize mutex
    (void)ExInitializeResourceLite(_Mtx);
}

void __CLRCALL_PURE_OR_CDECL _Mtxdst(_Rmtx* _Mtx) noexcept { // delete mutex
    (void)ExDeleteResourceLite(_Mtx);
}

_RELIABILITY_CONTRACT
void __CLRCALL_PURE_OR_CDECL _Mtxlock(_Rmtx* _Mtx) noexcept { // lock mutex
    ExEnterCriticalRegionAndAcquireResourceExclusive(_Mtx);
}

_RELIABILITY_CONTRACT
void __CLRCALL_PURE_OR_CDECL _Mtxunlock(_Rmtx* _Mtx) noexcept { // unlock mutex
    ExReleaseResourceAndLeaveCriticalRegion(_Mtx);
}

_END_EXTERN_C_UNLESS_PURE
