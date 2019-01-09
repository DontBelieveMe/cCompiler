@echo off
echo === Linking ===
call link %1.obj

echo === Running ===
call %1.exe
echo Ran...
echo.

echo === Return Value ===
echo %errorlevel%
