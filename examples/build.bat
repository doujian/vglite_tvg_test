@echo off
REM ============================================================================
REM vg_lite_clear Demo Build Script
REM ============================================================================
REM 
REM This script builds the clear_demo executable using CMake and Visual Studio.
REM 
REM Prerequisites:
REM   - Visual Studio 18 2022 with C++ development tools
REM   - CMake 3.14 or later
REM   - Git (for GLFW FetchContent)
REM 
REM Usage:
REM   build.bat          - Build the demo
REM   build.bat clean    - Clean build directory
REM   build.bat run      - Build and run the demo
REM ============================================================================

setlocal

set BUILD_DIR=build
set CONFIG=Release

REM Parse command line arguments
if "%1"=="clean" goto :clean
if "%1"=="run" goto :build_and_run
goto :build

:clean
echo Cleaning build directory...
if exist %BUILD_DIR% rmdir /s /q %BUILD_DIR%
echo Clean complete.
goto :end

:build_and_run
echo Building and running...
call :do_build
if %ERRORLEVEL% neq 0 goto :error
echo.
echo Running clear_demo.exe...
%BUILD_DIR%\%CONFIG%\clear_demo.exe
goto :end

:build
call :do_build
goto :end

:do_build
echo ============================================================================
echo Building vg_lite_clear Demo
echo ============================================================================
echo.

REM Check if build directory exists
if not exist %BUILD_DIR% (
    echo Creating build directory...
    mkdir %BUILD_DIR%
)

echo Configuring with CMake...
cmake -B %BUILD_DIR% -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed!
    exit /b 1
)

echo.
echo Building...
cmake --build %BUILD_DIR% --config %CONFIG%
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed!
    exit /b 1
)

echo.
echo ============================================================================
echo Build Complete!
echo ============================================================================
echo Executable: %BUILD_DIR%\%CONFIG%\clear_demo.exe
echo Run: %BUILD_DIR%\%CONFIG%\clear_demo.exe
echo ============================================================================
exit /b 0

:error
echo.
echo Error: Build failed!
exit /b 1

:end
endlocal
