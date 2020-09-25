@echo off
pushd "%~dp0"

powershell Compress-7Zip "ucxxrt" -ArchiveFileName "ucxxrt.zip" -Format Zip

:exit
popd
@echo on
