/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      std_type_info_static.cpp
 * DATA:      2021/05/14
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

//
// std_type_info_static.cpp
//
//       Copyright (c) Microsoft Corporation. All rights reserved.
//
// Definition of the std::type_info destructor.  This symbol is needed in the
// import library.  Any class with virtual member functions compiled with /GR
// will depend on the type_info::`vftable` symbol.  This definition of the
// destructor ensures that this symbol will be found, even if no user source
// file includes <typeinfo>.
//
#undef _DLL
#undef _VCRT_BUILD
#include <vcruntime_typeinfo.h>

type_info::~type_info() noexcept
{
}
