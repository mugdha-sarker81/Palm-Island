@echo off
setlocal
cd /d "%~dp0"

call build.bat
if errorlevel 1 (
    echo.
    pause
    exit /b 1
)

echo.
echo Starting Palm Island. Close the game window to exit.
build\PalmIsland.exe
echo.
pause
