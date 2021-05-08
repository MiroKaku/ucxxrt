/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      startup.h
 * DATA:      2020/02/26
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#pragma once


#if __has_include(<wdm.h>)

 //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 //
 // Client Entry Point Declarations
 //
 //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+


extern "C" NTSTATUS DriverMain(
    _In_ PDRIVER_OBJECT     DriverObject,
    _In_ PUNICODE_STRING    Registry
);


#endif
