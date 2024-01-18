@echo off
setlocal enabledelayedexpansion

pushd "%~dp0"

set msbuild="%PROGRAMFILES(x86)%\Microsoft Visual Studio\2019\Enterprise\MSBuild\Current\Bin\msbuild.exe"

for /f "usebackq tokens=*" %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
  set msbuild="%%i"
)

if exist bin rd /s /q bin
if exist lib rd /s /q lib
if exist ucxxrt rd /s /q ucxxrt

%msbuild% -m -p:Configuration=Debug   -p:Platform=x86 ..\..\ucxxrt.sln -t:ucxxrt
%msbuild% -m -p:Configuration=Release -p:Platform=x86 ..\..\ucxxrt.sln -t:ucxxrt

%msbuild% -m -p:Configuration=Debug   -p:Platform=x64 ..\..\ucxxrt.sln -t:ucxxrt
%msbuild% -m -p:Configuration=Release -p:Platform=x64 ..\..\ucxxrt.sln -t:ucxxrt

%msbuild% -m -p:Configuration=Debug   -p:Platform=ARM64 ..\..\ucxxrt.sln -t:ucxxrt
%msbuild% -m -p:Configuration=Release -p:Platform=ARM64 ..\..\ucxxrt.sln -t:ucxxrt

:exit
popd
@echo on
