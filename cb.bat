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
	
	if exist "cCompiler.sln" del cCompiler.sln
	if exist "cCompiler.vcxproj" del cCompiler.vcxproj
	if exist "cCompiler.vcxproj.filters" del cCompiler.vcxproj.filters
	
	echo All Done!
)
