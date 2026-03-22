@echo off
REM ============================================================================
REM Android APK Build Script for VGLite Demos
REM ============================================================================
REM Usage:
REM   build_android.bat           - Build ALL demos (default when double-clicked)
REM   build_android.bat <demo>    - Build specific demo
REM   build_android.bat list      - List available demos
REM   build_android.bat clean     - Clean build directories
REM   build_android.bat help      - Show this help
REM ============================================================================

setlocal EnableDelayedExpansion

REM Configuration - EDIT THESE PATHS
set "ANDROID_SDK=E:\Android\Sdk"
set "ANDROID_NDK=%ANDROID_SDK%\ndk\29.0.14206865"
set "ANDROID_CMAKE=%ANDROID_SDK%\cmake\3.22.1\bin\cmake.exe"
set "ANDROID_NINJA=%ANDROID_SDK%\cmake\3.22.1\bin\ninja.exe"
set "ANDROID_BUILD_TOOLS=%ANDROID_SDK%\build-tools\34.0.0"
set "ANDROID_PLATFORM=%ANDROID_SDK%\platforms\android-34"
set "KEYSTORE_PATH=%~dp0debug.keystore"
set "KEYSTORE_PASS=android"
set "KEY_ALIAS=androiddebugkey"

REM Parse arguments
if "%~1"=="" goto build_all
if "%~1"=="clean" goto do_clean
if "%~1"=="list" goto list_demos
if "%~1"=="help" goto show_help
goto do_build

:show_help
echo.
echo VGLite Android APK Builder
echo.
echo Usage:
echo   build_android.bat           - Build ALL demos (default when double-clicked)
echo   build_android.bat ^<demo^>    - Build specific demo
echo   build_android.bat list      - List available demos
echo   build_android.bat clean     - Clean build directories
echo   build_android.bat help      - Show this help
echo.
echo Examples:
echo   build_android.bat             - Build all demos
echo   build_android.bat clear_demo  - Build clear_demo only
echo   build_android.bat list        - Show available demos
echo.
pause
exit /b 0

:list_demos
echo.
echo Available demos with Android support:
echo.
set FOUND=0
for /d %%d in ("%~dp0demos\*") do (
    if exist "%%d\android" (
        echo   - %%~nxd
        set FOUND=1
    )
)
if "!FOUND!"=="0" (
    echo   (No demos with Android support found)
)
echo.
pause
exit /b 0

:build_all
echo.
echo ============================================================================
echo Building ALL Android APKs
echo ============================================================================
echo.

REM Check prerequisites first
if not exist "%ANDROID_SDK%" (
    echo Error: Android SDK not found at %ANDROID_SDK%
    echo Please edit build_android.bat and set ANDROID_SDK to your path.
    echo.
    pause
    exit /b 1
)

set BUILD_COUNT=0
set FAIL_COUNT=0

for /d %%d in ("%~dp0demos\*") do (
    if exist "%%d\android" (
        call :build_single "%%~nxd"
        if !ERRORLEVEL! EQU 0 (
            set /a BUILD_COUNT+=1
        ) else (
            set /a FAIL_COUNT+=1
        )
    )
)

echo.
echo ============================================================================
echo Build Complete: %BUILD_COUNT% succeeded, %FAIL_COUNT% failed
echo ============================================================================
echo.
pause
exit /b 0

:do_clean
echo.
echo Cleaning Android build directories...
echo.

for /d %%d in ("%~dp0demos\*") do (
    if exist "%%d\android\app\src\main\cpp\build_android" (
        echo Removing: %%~nxd build_android
        rmdir /s /q "%%d\android\app\src\main\cpp\build_android"
    )
    if exist "%%d\android\app\src\main\cpp\apk_build" (
        echo Removing: %%~nxd apk_build
        rmdir /s /q "%%d\android\app\src\main\cpp\apk_build"
    )
)
if exist "%~dp0build\demos" (
    echo Removing: build\demos
    rmdir /s /q "%~dp0build\demos"
)

echo.
echo Done.
echo.
pause
exit /b 0

:do_build
call :build_single "%~1"
if errorlevel 1 (
    pause
    exit /b 1
)
pause
exit /b 0

REM ============================================================================
REM build_single: Build a single demo APK
REM Args: %%1 = demo name
REM Returns: 0 on success, 1 on failure
REM ============================================================================
:build_single
set "DEMO=%~1"
set "ANDROID_DIR=%~dp0demos\%DEMO%\android"

if not exist "%ANDROID_DIR%" (
    echo.
    echo Error: Demo '%DEMO%' not found or has no Android support.
    echo.
    exit /b 1
)

echo.
echo ============================================================================
echo Building Android APK for %DEMO%
echo ============================================================================

REM Check prerequisites
if not exist "%ANDROID_SDK%" (
    echo Error: Android SDK not found at %ANDROID_SDK%
    echo Please edit build_android.bat and set ANDROID_SDK to your path.
    echo.
    exit /b 1
)

if not exist "%ANDROID_NDK%" (
    echo Error: Android NDK not found at %ANDROID_NDK%
    echo.
    exit /b 1
)

if not exist "%ANDROID_CMAKE%" (
    echo Error: CMake not found at %ANDROID_CMAKE%
    echo.
    exit /b 1
)

set "ANDROID_CPP_DIR=%ANDROID_DIR%\app\src\main\cpp"
set "ANDROID_BUILD_DIR=%ANDROID_CPP_DIR%\build_android"
set "APK_OUTPUT=%ANDROID_CPP_DIR%\apk_build"
set "FINAL_APK=%ANDROID_CPP_DIR%\%DEMO%.apk"

REM Step 1: Configure CMake (incremental)
echo.
echo [1/5] CMake configuration...

if not exist "%ANDROID_BUILD_DIR%\CMakeCache.txt" (
    echo Configuring for arm64-v8a...
    if not exist "%ANDROID_BUILD_DIR%" mkdir "%ANDROID_BUILD_DIR%"
    
    "%ANDROID_CMAKE%" -S "%ANDROID_CPP_DIR%" -B "%ANDROID_BUILD_DIR%" -G Ninja -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK%\build\cmake\android.toolchain.cmake" -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-21 -DANDROID_STL=c++_static -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM="%ANDROID_NINJA%"
    
    if errorlevel 1 (
        echo Error: CMake configuration failed.
        echo.
        exit /b 1
    )
) else (
    echo Already configured, skipping.
)

REM Step 2: Build
echo.
echo [2/5] Building native library...

"%ANDROID_CMAKE%" --build "%ANDROID_BUILD_DIR%" --config Release

if errorlevel 1 (
    echo Error: Build failed.
    echo.
    exit /b 1
)

echo Built: %ANDROID_BUILD_DIR%\libvglite_demo.so

REM Step 3: Create APK structure
echo.
echo [3/5] Creating APK structure...

if exist "%APK_OUTPUT%" rmdir /s /q "%APK_OUTPUT%"
mkdir "%APK_OUTPUT%"
mkdir "%APK_OUTPUT%\lib\arm64-v8a"

copy "%ANDROID_BUILD_DIR%\libvglite_demo.so" "%APK_OUTPUT%\lib\arm64-v8a\" >nul
echo Copied: libvglite_demo.so

REM Create DEX
set "DEX_SRC=%ANDROID_CPP_DIR%\dex_src"
if not exist "%DEX_SRC%\classes.dex" (
    echo Creating DEX file...
    if not exist "%DEX_SRC%\com\example\vglitedemo" mkdir "%DEX_SRC%\com\example\vglitedemo"
    echo package com.example.vglitedemo; > "%DEX_SRC%\com\example\vglitedemo\Stub.java"
    echo public class Stub {} >> "%DEX_SRC%\com\example\vglitedemo\Stub.java"
    javac -source 1.8 -target 1.8 -d "%DEX_SRC%" "%DEX_SRC%\com\example\vglitedemo\Stub.java" >nul 2>&1
    "%ANDROID_BUILD_TOOLS%\d8.bat" --output "%DEX_SRC%" "%DEX_SRC%\com\example\vglitedemo\Stub.class" >nul 2>&1
)

if exist "%DEX_SRC%\classes.dex" copy "%DEX_SRC%\classes.dex" "%APK_OUTPUT%\" >nul

REM Create manifest
echo ^<?xml version="1.0" encoding="utf-8"?^> > "%APK_OUTPUT%\AndroidManifest.xml"
echo ^<manifest xmlns:android="http://schemas.android.com/apk/res/android" package="com.example.vglitedemo"^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo   ^<uses-feature android:glEsVersion="0x00030000" android:required="true" /^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo   ^<application android:allowBackup="true" android:label="VGLite %DEMO%" android:supportsRtl="true"^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo     ^<activity android:name="android.app.NativeActivity" android:label="VGLite %DEMO%" android:exported="true"^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo       ^<meta-data android:name="android.app.lib_name" android:value="vglite_demo" /^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo       ^<intent-filter^>^<action android:name="android.intent.action.MAIN" /^>^<category android:name="android.intent.category.LAUNCHER" /^>^</intent-filter^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo     ^</activity^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo   ^</application^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo ^</manifest^> >> "%APK_OUTPUT%\AndroidManifest.xml"

REM Step 4: Package
echo.
echo [4/5] Packaging APK...

set "BASE_APK=%APK_OUTPUT%\base.apk"

"%ANDROID_BUILD_TOOLS%\aapt2.exe" link -o "%BASE_APK%" -I "%ANDROID_PLATFORM%\android.jar" --manifest "%APK_OUTPUT%\AndroidManifest.xml" --auto-add-overlay

if errorlevel 1 (
    echo Error: aapt2 failed.
    echo.
    exit /b 1
)

pushd "%APK_OUTPUT%"
"%ANDROID_BUILD_TOOLS%\aapt.exe" add base.apk lib\arm64-v8a\libvglite_demo.so >nul
if exist classes.dex "%ANDROID_BUILD_TOOLS%\aapt.exe" add base.apk classes.dex >nul
popd

REM Step 5: Sign
echo.
echo [5/5] Signing APK...

if not exist "%KEYSTORE_PATH%" (
    echo Generating debug keystore...
    keytool -genkeypair -v -keystore "%KEYSTORE_PATH%" -alias %KEY_ALIAS% -keyalg RSA -keysize 2048 -validity 10000 -storepass %KEYSTORE_PASS% -keypass %KEYSTORE_PASS% -dname "CN=Debug, OU=Debug, O=Debug, L=Debug, ST=Debug, C=US" >nul 2>&1
)

set "ALIGNED_APK=%APK_OUTPUT%\aligned.apk"
"%ANDROID_BUILD_TOOLS%\zipalign.exe" -f 4 "%BASE_APK%" "%ALIGNED_APK%" >nul
call "%ANDROID_BUILD_TOOLS%\apksigner.bat" sign --ks "%KEYSTORE_PATH%" --ks-pass pass:%KEYSTORE_PASS% --ks-key-alias %KEY_ALIAS% --key-pass pass:%KEYSTORE_PASS% --out "%FINAL_APK%" "%ALIGNED_APK%" >nul 2>&1

REM Copy to build dir
set "BUILD_APK_DIR=%~dp0build\demos\%DEMO%"
if exist "%FINAL_APK%" (
    if not exist "%BUILD_APK_DIR%" mkdir "%BUILD_APK_DIR%"
    copy "%FINAL_APK%" "%BUILD_APK_DIR%\%DEMO%.apk" >nul
    echo.
    echo ============================================================================
    echo Success!
    echo ============================================================================
    echo APK: %BUILD_APK_DIR%\%DEMO%.apk
    echo.
    echo Install: adb install -r "%BUILD_APK_DIR%\%DEMO%.apk"
    echo ============================================================================
) else (
    echo Error: APK creation failed.
    exit /b 1
)

exit /b 0

endlocal