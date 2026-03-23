@echo off
REM ============================================================================
REM VGLite Tests Build Script
REM ============================================================================

REM 加载环境配置
call "%~dp0..\check_env.bat" set
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to load environment configuration.
    echo Please run check_env.bat to verify your environment.
    pause
    exit /b 1
)

REM 初始化 Visual Studio 环境
if not exist "%VCVARSALL%" (
    echo Error: Visual Studio not found at %VS_INSTALL_DIR%
    echo Please check your environment configuration.
    pause
    exit /b 1
)

call "%VCVARSALL%" x64
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to initialize Visual Studio environment!
    pause
    exit /b 1
)

cd /d "%~dp0"
if exist build rmdir /s /q build
cmake -B build -G "Visual Studio 18" -A x64
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
