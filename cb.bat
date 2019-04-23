@echo off

set PremakeTarget=vs2017

if "%1"=="premake" (
	echo ========================
	echo Generating Project Files
	echo ========================
	echo.

	call tools\premake5 %PremakeTarget%
)

if "%1"=="build" (
	if not exist "cCompiler.sln" (
		echo =========================================
		echo Project Files Do Not Exist. Generating...
		echo =========================================
		echo.
		call tools\premake5 %PremakeTarget%
		echo.
	)
		
	echo ================
	echo Building Project
	echo ================
	echo.

	call tools\msbuild .
)

if "%1"=="test" (
	call tests\build\bin\Debug\cCompiler-tests.exe
)

if "%1"=="devenv" (
	echo =======================
	echo Launching Visual Studio
	echo =======================
	echo.
	call tools\devenv 
)

if "%1"=="clean" (
	echo ====================
	echo Deleting Build Files
	echo ====================
	echo.
	
	if exist "build" rd build /s /q
	if exist "tests\build" rd tests\build /s /q
	
	if exist "cCompiler.sln" del cCompiler.sln
	if exist "cCompiler.vcxproj" del cCompiler.vcxproj
	if exist "cCompiler.vcxproj.filters" del cCompiler.vcxproj.filters
	if exist "cCompiler.vcxproj.user" del cCompiler.vcxproj.user

	if exist "tests\cCompiler-tests.vcxproj" del tests\cCompiler-tests.vcxproj
	if exist "tests\cCompiler-tests.vcxproj.filters" del tests\cCompiler-tests.vcxproj.filters
	if exist "tests\cCompiler-tests.vcxproj.user" del tests\cCompiler-tests.vcxproj.user

	if exist ".vs" rd .vs /s /q	
	echo All Done!
)
