/***
*ehassert.h - our own little versions of the assert macros.
*
*       Copyright (c) 1993-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Versions of the assert macros for exception handling.
* 
****/

#pragma once
#include <assert.h>

#define EHTRACE_RESET
#define EHTRACE_SAVE_LEVEL
#define EHTRACE_RESTORE_LEVEL(x)
#define EHTRACE_ENTER
#define EHTRACE_ENTER_MSG(x)
#define EHTRACE_ENTER_FMT1(x,y)
#define EHTRACE_ENTER_FMT2(x,y,z)
#define EHTRACE_MSG(x)
#define EHTRACE_FMT1(x,y)
#define EHTRACE_FMT2(x,y,z)
#define EHTRACE_EXCEPT(x)       (x)
#define EHTRACE_EXIT
#define EHTRACE_HANDLER_EXIT(x)

#define _VCRT_VERIFY assert
