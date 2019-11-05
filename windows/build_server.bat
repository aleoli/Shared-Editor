@echo off

cd ..\src\server
mkdir build > nul 2>&1
cd build

cmake .. -G "MinGW Makefiles"
mingw32-make

cd ..\..\..\
mkdir bin > nul 2>&1
xcopy src\server\build\server.exe .\bin\ /q /y /i > nul

set /p=Hit ENTER to close the window..
