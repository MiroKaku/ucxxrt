/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      ucxxrt.cpp
 * DATE:      2022/06/27
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */


// Section
#pragma comment(linker, "/merge:_TEXT=.text")
#pragma comment(linker, "/merge:_DATA=.data")
#pragma comment(linker, "/merge:_RDATA=.rdata")


// Global
extern"C" ULONG __ucxxrt_tag = static_cast<ULONG>(_ByteSwap32('ucrt'));
