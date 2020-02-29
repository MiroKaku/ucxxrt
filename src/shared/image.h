/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      image.h
 * DATA:      2020/02/05
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once


namespace ucxxrt
{
#if __has_include(<ntimage.h>)
#include <ntimage.h>

    typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY RUNTIME_FUNCTION, * PRUNTIME_FUNCTION;

    //
    // Define unwind information flags.
    //

#define UNW_FLAG_NHANDLER       0x0
#define UNW_FLAG_EHANDLER       0x1
#define UNW_FLAG_UHANDLER       0x2
#define UNW_FLAG_CHAININFO      0x4

#define UNW_FLAG_NO_EPILOGUE    0x80000000UL    // Software only flag

    //
    // Define unwind history table structure.
    //

#define UNWIND_HISTORY_TABLE_SIZE 12

    typedef struct _UNWIND_HISTORY_TABLE_ENTRY {
        ULONG_PTR ImageBase;
        PRUNTIME_FUNCTION FunctionEntry;
    } UNWIND_HISTORY_TABLE_ENTRY, * PUNWIND_HISTORY_TABLE_ENTRY;

    typedef struct _UNWIND_HISTORY_TABLE {
        DWORD Count;
        BYTE  LocalHint;
        BYTE  GlobalHint;
        BYTE  Search;
        BYTE  Once;
        ULONG_PTR LowAddress;
        ULONG_PTR HighAddress;
        UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
    } UNWIND_HISTORY_TABLE, * PUNWIND_HISTORY_TABLE;

#else
#include <winnt.h>
#endif
}

namespace ucxxrt
{
    extern "C" const IMAGE_DOS_HEADER __ImageBase;
}
