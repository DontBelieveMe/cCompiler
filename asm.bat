@echo off

set Configuration=Debug
set BinaryName=cCompiler.exe

set TargetDir=build\bin\%Configuration%
set FullBinaryPath=%TargetDir%\%BinaryName%

if not exist %FullBinaryPath% (
	echo ======================================
	echo Executable Does Not Exist. Building...
	echo ======================================
	echo.

	call cb build

	echo =====================
	echo Running Executable...
	echo =====================
	echo.
)

call %FullBinaryPath% %*
