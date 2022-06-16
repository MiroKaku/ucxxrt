// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// pointer to raise handler

#pragma push_macro("_HAS_EXCEPTIONS")
#    undef  _HAS_EXCEPTIONS
#    define _HAS_EXCEPTIONS 0
#    include <exception>
#pragma pop_macro("_HAS_EXCEPTIONS")

_STD_BEGIN

_Prhand _Raise_handler = nullptr; // define raise handler pointer

_STD_END
