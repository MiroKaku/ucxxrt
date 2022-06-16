//
// initializers.cpp
//
//      Copyright (c) Microsoft Corporation. All rights reserved.
//
// Data used in C and C++ initialization and termination.  Because this object
// is linked into every module that uses the CRT, we also use this module to
// pass flags to the linker to link with libraries upon which the CRT depends.
//
#include <vcstartup_internal.h>
#include <vcruntime_internal.h>


// Need to put the following marker variables into the .CRT section.
// The .CRT section contains arrays of function pointers.
// The compiler creates functions and adds pointers to this section
// for things like C++ global constructors.
//
// The XIA, XCA etc are group names with in the section.
// The compiler sorts the contributions by the group name.
// For example, .CRT$XCA followed by .CRT$XCB, ... .CRT$XCZ.
// The marker variables below let us get pointers
// to the beginning/end of the arrays of function pointers.
//
// For example, standard groups are
//  XCA used here, for begin marker
//  XCC "compiler" inits
//  XCL "library" inits
//  XCU "user" inits
//  XCZ used here, for end marker
//

extern "C" _CRTALLOC(".CRT$XIA") _PIFV __xi_a[] = { nullptr }; // C initializers (first)
extern "C" _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[] = { nullptr }; // C initializers (last)
extern "C" _CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { nullptr }; // C++ initializers (first)
extern "C" _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { nullptr }; // C++ initializers (last)
extern "C" _CRTALLOC(".CRT$XPA") _PVFV __xp_a[] = { nullptr }; // C pre-terminators (first)
extern "C" _CRTALLOC(".CRT$XPZ") _PVFV __xp_z[] = { nullptr }; // C pre-terminators (last)
extern "C" _CRTALLOC(".CRT$XTA") _PVFV __xt_a[] = { nullptr }; // C terminators (first)
extern "C" _CRTALLOC(".CRT$XTZ") _PVFV __xt_z[] = { nullptr }; // C terminators (last)

#pragma comment(linker, "/merge:.CRT=.rdata")
