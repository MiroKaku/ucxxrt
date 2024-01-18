/*
 * PROJECT:   Universal C++ RunTime (UCXXRT)
 * FILE:      sys_main.cpp
 * DATE:      2021/05/03
 *
 * PURPOSE:   Universal C++ RunTime
 *
 * LICENSE:   Relicensed under The MIT License from The CC BY 4.0 License
 *
 * DEVELOPER: MiroKaku (miro.kaku AT Outlook.com)
 */

#include <sys_common.inl>


extern"C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    return __scrt_common_main(DriverObject, RegistryPath);
}
