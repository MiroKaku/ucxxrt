#pragma once
#pragma warning(disable: 4005 4189 4245 4457 4499 4838)

// System Header
#include <corecrt.h>

#undef _CRT_NO_TIME_T
#ifdef _USE_32BIT_TIME_T
typedef __time32_t time_t;
#else
typedef __time64_t time_t;
#endif

#define NOMINMAX
#include <Veil/Veil.h>

// C/C++  Header

// Local  Header

// Global Variable

// Global Macro
#define __WARNING_NOT_SATISFIED     28020
#define __WARNING_UNUSED_ASSIGNMENT 28931
