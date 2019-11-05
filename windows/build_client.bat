@echo off

cd ..\src\client
mkdir build > nul 2>&1
cd build

cmake .. -G "MinGW Makefiles"
mingw32-make

cd ..\..\..\
mkdir bin > nul 2>&1
xcopy src\client\build\client.exe .\bin\ /q /y /i > nul

set /p=Hit ENTER to close the window..
