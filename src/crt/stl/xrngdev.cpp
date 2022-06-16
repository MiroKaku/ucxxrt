// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// implement random_device

#include <stdexcept> // for out_of_range

#include <random>

_STD_BEGIN
_CRTIMP2_PURE unsigned int __CLRCALL_PURE_OR_CDECL _Random_device();

_CRTIMP2_PURE unsigned int __CLRCALL_PURE_OR_CDECL _Random_device() { // return a random value
    unsigned long ans = 0;
    return RtlRandomEx(&ans);
}

_STD_END
