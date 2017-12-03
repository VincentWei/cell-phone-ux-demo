@echo off

set VCVARS="C:\\Program Files\\Microsoft Visual Studio 8\\VC\\vcvarsall.bat"
REM set MSYS="E:\\home\\cos\msys\msys.bat"
set BASH="E:\home\cos\msys\bin\bash.exe"

call %VCVARS%
REM call %MSYS%

cd ..
%BASH% scripts\shell
