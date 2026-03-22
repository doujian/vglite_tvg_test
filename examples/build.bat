@echo off
REM ============================================================================
REM VGLite Examples Build Script
REM ============================================================================
REM 
REM This script builds VGLite demo applications using CMake and Visual Studio.
REM Supports multiple demos with both GL and SW backends, plus Android APK.
REM 
REM Prerequisites:
REM   - Visual Studio 2022 with C++ development tools
REM   - CMake 3.14 or later
REM   - Git (for GLFW FetchContent)
REM   - Android SDK/NDK (for Android builds)
REM 
REM Usage:
REM   build.bat                    - Build all demos (GL + SW)
REM   build.bat <demo>             - Build specific demo (GL + SW)
REM   build.bat <demo> gl          - Build specific demo (GL only)
REM   build.bat <demo> sw          - Build specific demo (SW only)
REM   build.bat <demo> apk         - Build Android APK for demo
REM   build.bat clean              - Clean build directory
REM   build.bat run <demo>         - Build and run demo
REM   build.bat list               - List available demos
REM ============================================================================

setlocal

REM ============================================================================
REM Visual Studio 2022 Configuration
REM ============================================================================
set "VS_INSTALL_DIR=D:\Program Files\Microsoft Visual Studio\18\Community"
set "VCVARSALL=%VS_INSTALL_DIR%\VC\Auxiliary\Build\vcvarsall.bat"

REM ============================================================================
REM Android SDK/NDK Configuration
REM ============================================================================
set "ANDROID_SDK=E:\Android\Sdk"
set "ANDROID_NDK=%ANDROID_SDK%\ndk\29.0.14206865"
set "ANDROID_CMAKE=%ANDROID_SDK%\cmake\3.22.1\bin\cmake.exe"
set "ANDROID_NINJA=%ANDROID_SDK%\cmake\3.22.1\bin\ninja.exe"
set "ANDROID_BUILD_TOOLS=%ANDROID_SDK%\build-tools\34.0.0"
set "ANDROID_PLATFORM=%ANDROID_SDK%\platforms\android-34"
set "KEYSTORE_PATH=%~dp0\debug.keystore"
set "KEYSTORE_PASS=android"
set "KEY_ALIAS=androiddebugkey"

REM Parse command line arguments first (before VS check)
REM This allows Android builds to skip VS environment setup
if "%1"=="apk" goto :build_apk

REM Check if Visual Studio exists (only for Windows builds)
if not exist "%VCVARSALL%" (
    echo Error: Visual Studio not found at:
    echo   %VS_INSTALL_DIR%
    echo.
    echo Please edit build.bat and set VS_INSTALL_DIR to your Visual Studio path.
    goto :error_no_pause
)

echo ============================================================================
echo Setting up Visual Studio 2022 environment...
echo ============================================================================
call "%VCVARSALL%" x64
if %ERRORLEVEL% neq 0 (
    echo Error: Failed to initialize Visual Studio environment!
    goto :error_no_pause
)

set BUILD_DIR=build
set CONFIG=Release
set GENERATOR=Ninja
set ARCH=x64

REM Parse command line arguments
if "%1"=="" goto :build_all
if "%1"=="clean" goto :clean
if "%1"=="list" goto :list
if "%1"=="run" goto :run_demo
if "%1"=="apk" goto :build_apk
if "%1"=="help" goto :help
if "%1"=="--help" goto :help
if "%1"=="-h" goto :help
goto :build_demo

:help
echo ============================================================================
echo VGLite Examples Build Script
echo ============================================================================
echo.
echo Usage: build.bat [command] [options]
echo.
echo Commands:
echo   build.bat                    Build all demos (GL + SW versions)
echo   build.bat ^<demo^>            Build specific demo (GL + SW)
echo   build.bat ^<demo^> gl         Build specific demo (GL only)
echo   build.bat ^<demo^> sw         Build specific demo (SW only)
echo   build.bat ^<demo^> apk        Build Android APK for demo
echo   build.bat clean              Clean build directory
echo   build.bat list               List available demos
echo   build.bat run ^<demo^>        Build and run demo
echo   build.bat help               Show this help message
echo.
echo Examples:
echo   build.bat                    Build all demos
echo   build.bat clear_demo         Build clear_demo (GL + SW)
echo   build.bat clear_demo gl      Build clear_demo (GL only)
echo   build.bat clear_demo apk     Build Android APK
echo   build.bat run clear_demo     Build and run clear_demo
echo ============================================================================
goto :end

:list
echo ============================================================================
echo Available Demos
echo ============================================================================
echo Scanning demos directory...
echo.
for /d %%D in (demos\*) do (
    echo   - %%~nD
)
echo ============================================================================
goto :end

:clean
echo ============================================================================
echo Cleaning build directory...
echo ============================================================================
if exist %BUILD_DIR% (
    rmdir /s /q %BUILD_DIR%
    echo Build directory cleaned.
) else (
    echo Build directory does not exist.
)
goto :end

:run_demo
set DEMO=%2
if "%DEMO%"=="" (
    echo Error: No demo specified for run command!
    echo Usage: build.bat run ^<demo^>
    goto :error
)

echo ============================================================================
echo Building and running %DEMO%...
echo ============================================================================
call :do_build
if %ERRORLEVEL% neq 0 goto :error

REM Check for both versions, prefer GL
echo.
echo Checking for executable...
if exist %BUILD_DIR%\%DEMO%_gl.exe (
    echo Running %DEMO%_gl.exe...
    %BUILD_DIR%\%DEMO%_gl.exe
) else if exist %BUILD_DIR%\%DEMO%_sw.exe (
    echo Running %DEMO%_sw.exe...
    %BUILD_DIR%\%DEMO%_sw.exe
) else (
    echo Error: Executable not found!
    goto :error
)
goto :end

:build_all
echo ============================================================================
echo Building All Demos
echo ============================================================================
call :do_build
if %ERRORLEVEL% neq 0 goto :error
echo ============================================================================
echo Build Complete! Check %BUILD_DIR%\ for executables.
echo ============================================================================
goto :end

:build_demo
set DEMO=%1
set BACKEND=%2

REM Validate backend if specified
if not "%BACKEND%"=="" (
    if /i not "%BACKEND%"=="gl" if /i not "%BACKEND%"=="sw" (
        echo Error: Invalid backend '%BACKEND%'!
        echo Valid backends: gl, sw
        goto :error
    )
)

REM Configure if not already done (same as :do_build)
if not exist %BUILD_DIR%\CMakeCache.txt (
    echo Configuring with CMake...
    cmake -B %BUILD_DIR% -G %GENERATOR%
    if %ERRORLEVEL% neq 0 (
        echo Error: CMake configuration failed!
        goto :error
    )
)

echo ============================================================================
if "%BACKEND%"=="" (
    echo Building %DEMO% (GL + SW)...
) else (
    echo Building %DEMO% (%BACKEND% backend)...
)
echo ============================================================================

REM Build specific targets
if "%BACKEND%"=="" (
    echo Building %DEMO%_gl and %DEMO%_sw...
    cmake --build %BUILD_DIR% --config %CONFIG% --target %DEMO%_gl %DEMO%_sw
) else (
    echo Building %DEMO%_%BACKEND%...
    cmake --build %BUILD_DIR% --config %CONFIG% --target %DEMO%_%BACKEND%
)

if %ERRORLEVEL% neq 0 (
    echo Error: Build failed!
    goto :error
)

echo ============================================================================
echo Build Complete!
echo ============================================================================
if "%BACKEND%"=="" (
    echo Executables:
    echo   %BUILD_DIR%\%DEMO%_gl.exe
    echo   %BUILD_DIR%\%DEMO%_sw.exe
) else (
    echo Executable: %BUILD_DIR%\%DEMO%_%BACKEND%.exe
)
echo ============================================================================
goto :end

:do_build
echo ============================================================================
echo Configuring and Building
echo ============================================================================

if not exist %BUILD_DIR% (
    echo Creating build directory...
    mkdir %BUILD_DIR%
)

echo Configuring with CMake...
cmake -B %BUILD_DIR% -G %GENERATOR%
if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed!
    exit /b 1
)

echo.
echo Building all demos...
cmake --build %BUILD_DIR% --config %CONFIG%
if %ERRORLEVEL% neq 0 (
    echo Error: Build failed!
    exit /b 1
)

exit /b 0

REM ============================================================================
REM Build Android APK
REM ============================================================================
:build_apk
set DEMO=%2
if "%DEMO%"=="" (
    echo Error: No demo specified for APK build!
    echo Usage: build.bat apk ^<demo^>
    goto :error
)

echo ============================================================================
echo Building Android APK for %DEMO%...
echo ============================================================================

REM Check Android SDK/NDK
if not exist "%ANDROID_SDK%" (
    echo Error: Android SDK not found at %ANDROID_SDK%
    echo Please edit build.bat and set ANDROID_SDK to your SDK path.
    goto :error
)

if not exist "%ANDROID_NDK%" (
    echo Error: Android NDK not found at %ANDROID_NDK%
    echo Please install NDK via Android Studio SDK Manager.
    goto :error
)

REM Check demo has Android support
set "ANDROID_DIR=demos\%DEMO%\android"
if not exist "%ANDROID_DIR%" (
    echo Error: Demo '%DEMO%' does not have Android support!
    echo Expected: %ANDROID_DIR%
    goto :error
)

set "ANDROID_CPP_DIR=%ANDROID_DIR%\app\src\main\cpp"
set "ANDROID_BUILD_DIR=%ANDROID_CPP_DIR%\build_android"
set "APK_OUTPUT=%ANDROID_CPP_DIR%\apk_build"

echo.
echo [1/5] Configuring CMake for Android (arm64-v8a)...
if not exist "%ANDROID_BUILD_DIR%" mkdir "%ANDROID_BUILD_DIR%"

"%ANDROID_CMAKE%" -S "%ANDROID_CPP_DIR%" -B "%ANDROID_BUILD_DIR%" ^
    -G "Ninja" ^
    -DCMAKE_TOOLCHAIN_FILE="%ANDROID_NDK%\build\cmake\android.toolchain.cmake" ^
    -DANDROID_ABI=arm64-v8a ^
    -DANDROID_PLATFORM=android-21 ^
    -DANDROID_STL=c++_static ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_MAKE_PROGRAM="%ANDROID_NINJA%"

if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed!
    goto :error
)

echo.
echo [2/5] Building native library...
"%ANDROID_CMAKE%" --build "%ANDROID_BUILD_DIR%" --config Release
if %ERRORLEVEL% neq 0 (
    echo Error: Native build failed!
    goto :error
)

echo.
echo [3/5] Creating APK structure...
if exist "%APK_OUTPUT%" rmdir /s /q "%APK_OUTPUT%"
mkdir "%APK_OUTPUT%"
mkdir "%APK_OUTPUT%\lib\arm64-v8a"
mkdir "%APK_OUTPUT%\res"

REM Copy native library
copy "%ANDROID_BUILD_DIR%\libvglite_demo.so" "%APK_OUTPUT%\lib\arm64-v8a\" >nul

REM Create AndroidManifest.xml
echo ^<?xml version="1.0" encoding="utf-8"?^> > "%APK_OUTPUT%\AndroidManifest.xml"
echo ^<manifest xmlns:android="http://schemas.android.com/apk/res/android" >> "%APK_OUTPUT%\AndroidManifest.xml"
echo     package="com.example.vglitedemo"^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo     ^<uses-feature android:glEsVersion="0x00030000" android:required="true" /^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo     ^<application >> "%APK_OUTPUT%\AndroidManifest.xml"
echo         android:allowBackup="true" >> "%APK_OUTPUT%\AndroidManifest.xml"
echo         android:label="VGLite %DEMO%" >> "%APK_OUTPUT%\AndroidManifest.xml"
echo         android:supportsRtl="true"^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo         ^<activity android:name="android.app.NativeActivity" >> "%APK_OUTPUT%\AndroidManifest.xml"
echo             android:label="VGLite %DEMO%" >> "%APK_OUTPUT%\AndroidManifest.xml"
echo             android:exported="true"^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo             ^<meta-data android:name="android.app.lib_name" android:value="vglite_demo" /^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo             ^<intent-filter^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo                 ^<action android:name="android.intent.action.MAIN" /^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo                 ^<category android:name="android.intent.category.LAUNCHER" /^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo             ^</intent-filter^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo         ^</activity^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo     ^</application^> >> "%APK_OUTPUT%\AndroidManifest.xml"
echo ^</manifest^> >> "%APK_OUTPUT%\AndroidManifest.xml"

echo.
echo [4/5] Packaging APK...
REM Create classes.dex (dummy for NativeActivity)
if not exist "%ANDROID_CPP_DIR%\classes.dex" (
    echo Creating dummy DEX file...
    echo ^<empty^> > "%ANDROID_CPP_DIR%\Dummy.java"
    "%ANDROID_BUILD_TOOLS%\d8.bat" --output "%ANDROID_CPP_DIR%" "%ANDROID_CPP_DIR%\Dummy.java" 2>nul
    del "%ANDROID_CPP_DIR%\Dummy.java" 2>nul
)

REM Copy classes.dex if exists
if exist "%ANDROID_CPP_DIR%\classes.dex" (
    copy "%ANDROID_CPP_DIR%\classes.dex" "%APK_OUTPUT%\" >nul
)

REM Create base APK using aapt2
set "BASE_APK=%APK_OUTPUT%\base.apk"
"%ANDROID_BUILD_TOOLS%\aapt2.exe" link -o "%BASE_APK%" ^
    -I "%ANDROID_PLATFORM%\android.jar" ^
    --manifest "%APK_OUTPUT%\AndroidManifest.xml" ^
    --auto-add-overlay

if %ERRORLEVEL% neq 0 (
    echo Warning: aapt2 failed, trying alternative method...
    REM Fallback: use existing APK structure if available
    if exist "%ANDROID_CPP_DIR%\apk_template" (
        xcopy /E /I /Y "%ANDROID_CPP_DIR%\apk_template\*" "%APK_OUTPUT%\"
    )
)

REM Add native library to APK using aapt (aapt2 doesn't have add command)
if exist "%BASE_APK%" (
    pushd "%APK_OUTPUT%"
    "%ANDROID_BUILD_TOOLS%\aapt.exe" add "%BASE_APK%" "lib/arm64-v8a/libvglite_demo.so"
    popd
)

echo.
echo [5/5] Signing APK...
set "FINAL_APK=%ANDROID_CPP_DIR%\%DEMO%.apk"

REM Generate debug keystore if not exists
if not exist "%KEYSTORE_PATH%" (
    echo Generating debug keystore...
    keytool -genkeypair -v -keystore "%KEYSTORE_PATH%" -alias %KEY_ALIAS% -keyalg RSA -keysize 2048 -validity 10000 -storepass %KEYSTORE_PASS% -keypass %KEYSTORE_PASS% -dname "CN=Debug, OU=Debug, O=Debug, L=Debug, ST=Debug, C=US" 2>nul
)

REM Align APK
set "ALIGNED_APK=%APK_OUTPUT%\aligned.apk"
if exist "%ANDROID_BUILD_TOOLS%\zipalign.exe" (
    "%ANDROID_BUILD_TOOLS%\zipalign.exe" -v -f 4 "%BASE_APK%" "%ALIGNED_APK%"
) else (
    copy "%BASE_APK%" "%ALIGNED_APK%" >nul
)

REM Sign APK
if exist "%ANDROID_BUILD_TOOLS%\apksigner.bat" (
    call "%ANDROID_BUILD_TOOLS%\apksigner.bat" sign --ks "%KEYSTORE_PATH%" --ks-pass pass:%KEYSTORE_PASS% --ks-key-alias %KEY_ALIAS% --key-pass pass:%KEYSTORE_PASS% --out "%FINAL_APK%" "%ALIGNED_APK%"
) else (
    copy "%ALIGNED_APK%" "%FINAL_APK%" >nul
    echo Warning: apksigner not found, APK is unsigned.
)

if exist "%FINAL_APK%" (
    echo.
    echo ============================================================================
    echo APK Build Complete!
    echo ============================================================================
    echo Output: %FINAL_APK%
    echo.
    echo To install on connected device:
    echo   adb install -r "%FINAL_APK%"
    echo ============================================================================
) else (
    echo Error: APK creation failed!
    goto :error
)

goto :end

:error_no_pause
pause
exit /b 1

:error
echo.
echo ============================================================================
echo Build Failed!
echo ============================================================================
pause
exit /b 1

:end
endlocal
echo.
echo Press any key to close...
pause >nul
exit /b 0