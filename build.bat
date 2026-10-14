@echo off
setlocal
cd /d "%~dp0"

if exist "C:\msys64\mingw64\bin" set "PATH=C:\msys64\mingw64\bin;C:\msys64\usr\bin;%PATH%"

where cmake >nul 2>nul
if errorlevel 1 (
    echo CMake was not found. Install CMake and make sure it is on PATH.
    exit /b 1
)

cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug
if errorlevel 1 (
    echo.
    echo Configure failed. If build\ already uses another CMake generator,
    echo rename that generated folder and run this script again.
    exit /b 1
)

cmake --build build --parallel 4
if errorlevel 1 (
    echo.
    echo Build failed. Read the error above, then try build.bat again.
    exit /b 1
)

echo.
echo Build succeeded: build\PalmIsland.exe
