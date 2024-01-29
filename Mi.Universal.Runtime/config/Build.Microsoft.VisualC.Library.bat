@setlocal
@echo off

@REM -------------------------------------------------------------------------
:parse_args
set __MI_BUILD_PARSE_ERROR=
set __MI_BUILD_HELP=
set __MI_BUILD_ARCH=
set __MI_BUILD_SPECTRE=
set __MI_BUILD_LIBRARY=
set __MI_BUILD_OUT=
set __MI_BUILD_EXCLUSION=

@REM Parse command line arguments.  This implementation does not care about argument order.

set __MI_BUILD_PARSE_ERROR=0
set "__MI_BUILD_ARGS_LIST=%*"
call :parse_loop
set __MI_BUILD_ARGS_LIST=

if "%__MI_BUILD_PARSE_ERROR%" NEQ "0" (
    goto :usage_error
)
if "%__MI_BUILD_HELP%" NEQ "" (
    goto :usage
)

@REM -------------------------------------------------------------------------
:build

set __MI_BUILD_VCVARSALL_ARCH=x64
if /I "%__MI_BUILD_ARCH%"=="x86" (
    set __MI_BUILD_VCVARSALL_ARCH=x64_x86
)^
else if /I "%__MI_BUILD_ARCH%"=="x64" (
    set __MI_BUILD_VCVARSALL_ARCH=x64
)^
else if /I "%__MI_BUILD_ARCH%"=="arm64" (
    set __MI_BUILD_VCVARSALL_ARCH=x64_arm64
)

@rem Initialize Visual Studio environment
if "%VSINSTALLDIR%"=="" (
    if "%VSAPPIDDIR%"=="" (
        call "%~dp0Build.InitializeEnvironment.bat" %__MI_BUILD_VCVARSALL_ARCH% ^-vcvars_spectre_libs=%__MI_BUILD_SPECTRE%
    )
)

@rem Building
lib %__MI_BUILD_LIBRARY% /NOLOGO /OUT:%__MI_BUILD_OUT%

if "%__MI_BUILD_EXCLUSION%" NEQ "" (
    for /F "delims=" %%1 in ('lib %__MI_BUILD_LIBRARY% /NOLOGO /LIST ^| findstr /g:%__MI_BUILD_EXCLUSION%') do (
        lib %__MI_BUILD_OUT% /NOLOGO /OUT:%__MI_BUILD_OUT% /REMOVE:%%1
    )
)

if %ERRORLEVEL%==0 (
    echo %~nx0 ^-^> %__MI_BUILD_OUT%
)

goto :end

@REM -------------------------------------------------------------------------
:parse_loop
for /F "tokens=1,* delims= " %%a in ("%__MI_BUILD_ARGS_LIST%") do (
    call :parse_argument %%a
    set "__MI_BUILD_ARGS_LIST=%%b"
    goto :parse_loop
)

exit /B 0

:parse_argument

@REM called by :parse_loop and expects the arguments to either be:
@REM 1. a single argument in %1
@REM 2. an argument pair from the command line specified as '%1=%2'

set __local_ARG_FOUND=
@REM Architecture
if /I "%1"=="x86" (
    set __MI_BUILD_ARCH=x86
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="x64" (
    set __MI_BUILD_ARCH=x64
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="arm64" (
    set __MI_BUILD_ARCH=arm64
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="debug" (
    set __MI_BUILD_CONFIG=arm64
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="release" (
    set __MI_BUILD_CONFIG=arm64
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="spectre" (
    set __MI_BUILD_SPECTRE=spectre
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="-lib" (
    set "__MI_BUILD_LIBRARY=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="/lib" (
    set "__MI_BUILD_LIBRARY=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="--lib" (
    set "__MI_BUILD_LIBRARY=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="-out" (
    set "__MI_BUILD_OUT=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="/out" (
    set "__MI_BUILD_OUT=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="--out" (
    set "__MI_BUILD_OUT=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="-exclusion_list" (
    set "__MI_BUILD_EXCLUSION=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="/exclusion_list" (
    set "__MI_BUILD_EXCLUSION=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="--exclusion_list" (
    set "__MI_BUILD_EXCLUSION=%2"
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="help" (
    set __VCVARSALL_HELP=1
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="/help" (
    set __VCVARSALL_HELP=1
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="-help" (
    set __VCVARSALL_HELP=1
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="/?" (
    set __VCVARSALL_HELP=1
    set __local_ARG_FOUND=1
)^
else if /I "%1"=="-?" (
    set __VCVARSALL_HELP=1
    set __local_ARG_FOUND=1
)

if "%__local_ARG_FOUND%" NEQ "1" (
    set /A __MI_BUILD_PARSE_ERROR=__MI_BUILD_PARSE_ERROR+1
    if "%2"=="" (
        @echo [ERROR:%~nx0] Invalid argument found : %1
    ) else (
        @echo [ERROR:%~nx0] Invalid argument found : %1=%2
    )
)
set __local_ARG_FOUND=
exit /B 0

@REM -------------------------------------------------------------------------
:usage_error
echo [ERROR:%~nx0] Error in script usage. The correct usage is:
goto :usage

:usage
echo Syntax:
echo     %~nx0 [arch] [spectre_mode] [-lib=filename] [-out=filename] [-exclusion_list=filename]
echo where :
echo     [arch]: x86 ^| x64 ^| ARM64
echo     [spectre_mode] : {none} for libraries without spectre mitigations ^|
echo                      "spectre" for libraries with spectre mitigations
echo     [lib] : static library filename
echo     [out] : output library filename
echo     [exclusion_list] : exclude specified obj files
echo:
echo For example:
echo     %~nx0 x64 libcmtd.lib -out=.\Output\Debug\x64\libcmtd.lib
goto :end

:missing
echo The specified configuration type is missing.  The tools for the
echo configuration might not be installed.
goto :end

@REM -------------------------------------------------------------------------
:end
set __MI_BUILD_PARSE_ERROR=
set __MI_BUILD_HELP=
set __MI_BUILD_ARCH=
set __MI_BUILD_SPECTRE=
set __MI_BUILD_LIBRARY=
set __MI_BUILD_OUT=
set __MI_BUILD_EXCLUSION=

@endlocal