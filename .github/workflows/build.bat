@echo off
pushd "%~dp0"

if exist bin rd /s /q bin
if exist lib rd /s /q lib
if exist ucxxrt rd /s /q ucxxrt

"%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\msbuild.exe" /p:Configuration=Release /p:Platform=x64 ..\..\ucxxrt.sln -t:kernel-mode\ucxxrt
"%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\msbuild.exe" /p:Configuration=Release /p:Platform=x86 ..\..\ucxxrt.sln -t:kernel-mode\ucxxrt

"%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\msbuild.exe" /p:Configuration=Debug /p:Platform=x64 ..\..\ucxxrt.sln -t:kernel-mode\ucxxrt
"%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\msbuild.exe" /p:Configuration=Debug /p:Platform=x86 ..\..\ucxxrt.sln -t:kernel-mode\ucxxrt

:exit
popd
@echo on
