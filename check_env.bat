@echo off

set PASS=0
set WARN=0
set FAIL=0

set NDK_VERSION=29.0.14206865
set CMAKE_VERSION=3.22.1
set BUILD_TOOLS_VERSION=34.0.0
set PLATFORM_VERSION=34

set MODE=default
if "%~1"=="check" set MODE=check
if "%~1"=="set" set MODE=set

set VS_INSTALL_DIR=
set ANDROID_SDK=
set JAVA_HOME=

if not exist "%~dp0env_config.txt" goto config_done
for /f "usebackq tokens=1,* delims==" %%a in ("%~dp0env_config.txt") do call :parse_config "%%a" "%%b"
:config_done

if not "%VS_INSTALL_DIR%"=="" set VCVARSALL=%VS_INSTALL_DIR%\VC\Auxiliary\Build\vcvarsall.bat

if "%ANDROID_SDK%"=="" goto skip_derived
set ANDROID_NDK=%ANDROID_SDK%\ndk\%NDK_VERSION%
set ANDROID_CMAKE=%ANDROID_SDK%\cmake\%CMAKE_VERSION%\bin\cmake.exe
set ANDROID_NINJA=%ANDROID_SDK%\cmake\%CMAKE_VERSION%\bin\ninja.exe
set ANDROID_BUILD_TOOLS=%ANDROID_SDK%\build-tools\%BUILD_TOOLS_VERSION%
set ANDROID_PLATFORM=%ANDROID_SDK%\platforms\android-%PLATFORM_VERSION%
:skip_derived

if not "%VS_INSTALL_DIR%"=="" goto vs_done1
if exist "D:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" set VS_INSTALL_DIR=D:\Program Files\Microsoft Visual Studio\18\Community
:vs_done1

if not "%VS_INSTALL_DIR%"=="" goto vs_done2
if exist "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" set VS_INSTALL_DIR=D:\Program Files\Microsoft Visual Studio\2022\Community
:vs_done2

if not "%VS_INSTALL_DIR%"=="" goto vs_done3
if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" set VS_INSTALL_DIR=C:\Program Files\Microsoft Visual Studio\2022\Community
:vs_done3

if not "%ANDROID_SDK%"=="" goto sdk_done1
if exist "D:\Android\Sdk\build-tools" set ANDROID_SDK=D:\Android\Sdk
:sdk_done1

if not "%ANDROID_SDK%"=="" goto sdk_done2
if exist "E:\Android\Sdk\build-tools" set ANDROID_SDK=E:\Android\Sdk
:sdk_done2

if not "%ANDROID_SDK%"=="" goto sdk_done3
if exist "C:\Users\%USERNAME%\AppData\Local\Android\Sdk\build-tools" set ANDROID_SDK=C:\Users\%USERNAME%\AppData\Local\Android\Sdk
:sdk_done3

if not "%JAVA_HOME%"=="" goto jdk_done1
if exist "D:\Programs\jdk-17.0.147\bin\javac.exe" set JAVA_HOME=D:\Programs\jdk-17.0.147
:jdk_done1

if not "%JAVA_HOME%"=="" goto jdk_done2
if exist "D:\Programs\jdk-17\bin\javac.exe" set JAVA_HOME=D:\Programs\jdk-17
:jdk_done2

if not "%JAVA_HOME%"=="" goto jdk_done3
if exist "C:\Program Files\Java\jdk-17\bin\javac.exe" set JAVA_HOME=C:\Program Files\Java\jdk-17
:jdk_done3

set KEYSTORE_PATH=%~dp0debug.keystore
set KEYSTORE_PASS=android
set KEY_ALIAS=androiddebugkey

if "%MODE%"=="set" exit /b 0

echo ============================================================================
echo VGLite ThorVG Environment Check
echo ============================================================================
if exist "%~dp0env_config.txt" echo Config: %~dp0env_config.txt
if not exist "%~dp0env_config.txt" echo Config: Not found
echo ============================================================================

echo.
echo [1/5] Basic Tools
echo ----------------------------------------------------------------------------

where cmake >nul 2>&1
if errorlevel 1 goto no_cmake
set /a PASS+=1
echo [PASS] CMake: Found
goto check_git
:no_cmake
set /a FAIL+=1
echo [FAIL] CMake: Not found

:check_git
where git >nul 2>&1
if errorlevel 1 goto no_git
set /a PASS+=1
echo [PASS] Git: Found
goto check_vs
:no_git
set /a WARN+=1
echo [WARN] Git: Not found

:check_vs
echo.
echo [2/5] Visual Studio
echo ----------------------------------------------------------------------------

if "%VS_INSTALL_DIR%"=="" goto no_vs
set /a PASS+=1
echo [PASS] VS_INSTALL_DIR: %VS_INSTALL_DIR%
if not exist "%VCVARSALL%" goto no_vcv
set /a PASS+=1
echo [PASS] vcvarsall.bat: Exists
goto check_jdk
:no_vs
set /a FAIL+=1
echo [FAIL] VS_INSTALL_DIR: Not found
goto check_jdk
:no_vcv
set /a FAIL+=1
echo [FAIL] vcvarsall.bat: Not found

:check_jdk
echo.
echo [3/5] JDK
echo ----------------------------------------------------------------------------

if "%JAVA_HOME%"=="" goto no_jdk
set /a PASS+=1
echo [PASS] JAVA_HOME: %JAVA_HOME%
goto check_keytool
:no_jdk
set /a FAIL+=1
echo [FAIL] JAVA_HOME: Not found

:check_keytool
where keytool >nul 2>&1
if errorlevel 1 goto no_keytool
set /a PASS+=1
echo [PASS] keytool: Found
goto check_sdk
:no_keytool
if "%JAVA_HOME%"=="" goto skip_keytool
if not exist "%JAVA_HOME%\bin\keytool.exe" goto skip_keytool
set /a PASS+=1
echo [PASS] keytool: Found in JAVA_HOME
goto check_sdk
:skip_keytool
set /a WARN+=1
echo [WARN] keytool: Not found

:check_sdk
echo.
echo [4/5] Android SDK/NDK
echo ----------------------------------------------------------------------------

if "%ANDROID_SDK%"=="" goto no_sdk
set /a PASS+=1
echo [PASS] Android SDK: %ANDROID_SDK%

if not exist "%ANDROID_NDK%" goto no_ndk
set /a PASS+=1
echo [PASS] Android NDK: %NDK_VERSION%
goto check_acmake
:no_ndk
set /a FAIL+=1
echo [FAIL] Android NDK: Not found

:check_acmake
if not exist "%ANDROID_CMAKE%" goto no_acmake
set /a PASS+=1
echo [PASS] Android CMake: %CMAKE_VERSION%
goto check_ninja
:no_acmake
set /a FAIL+=1
echo [FAIL] Android CMake: Not found

:check_ninja
if not exist "%ANDROID_NINJA%" goto no_ninja
set /a PASS+=1
echo [PASS] Android Ninja: Exists
goto check_bt
:no_ninja
set /a FAIL+=1
echo [FAIL] Android Ninja: Not found

:check_bt
if not exist "%ANDROID_BUILD_TOOLS%" goto no_bt
set /a PASS+=1
echo [PASS] Build Tools: %BUILD_TOOLS_VERSION%

if not exist "%ANDROID_BUILD_TOOLS%\aapt2.exe" goto no_aapt2
set /a PASS+=1
echo [PASS] aapt2: Exists
goto check_aapt
:no_aapt2
set /a FAIL+=1
echo [FAIL] aapt2: Not found

:check_aapt
if not exist "%ANDROID_BUILD_TOOLS%\aapt.exe" goto no_aapt
set /a PASS+=1
echo [PASS] aapt: Exists
goto check_d8
:no_aapt
set /a FAIL+=1
echo [FAIL] aapt: Not found

:check_d8
if not exist "%ANDROID_BUILD_TOOLS%\d8.bat" goto no_d8
set /a PASS+=1
echo [PASS] d8: Exists
goto check_zip
:no_d8
set /a FAIL+=1
echo [FAIL] d8: Not found

:check_zip
if not exist "%ANDROID_BUILD_TOOLS%\zipalign.exe" goto no_zip
set /a PASS+=1
echo [PASS] zipalign: Exists
goto check_sign
:no_zip
set /a FAIL+=1
echo [FAIL] zipalign: Not found

:check_sign
if not exist "%ANDROID_BUILD_TOOLS%\apksigner.bat" goto no_sign
set /a PASS+=1
echo [PASS] apksigner: Exists
goto check_plat
:no_sign
set /a FAIL+=1
echo [FAIL] apksigner: Not found

:check_plat
if not exist "%ANDROID_PLATFORM%\android.jar" goto no_plat
set /a PASS+=1
echo [PASS] Platform: android-%PLATFORM_VERSION%
goto check_deps
:no_plat
set /a FAIL+=1
echo [FAIL] Platform: Not found
goto check_deps

:no_bt
set /a FAIL+=1
echo [FAIL] Build Tools: Not found
goto check_deps

:no_sdk
set /a FAIL+=1
echo [FAIL] Android SDK: Not found

:check_deps
echo.
echo [5/5] Project Dependencies
echo ----------------------------------------------------------------------------

if not exist "%~dp0libs\thorvg-1.0.2\inc\thorvg.h" goto no_thorvg
set /a PASS+=1
echo [PASS] ThorVG 1.0.2: Exists
goto check_gtest
:no_thorvg
set /a FAIL+=1
echo [FAIL] ThorVG 1.0.2: Not found

:check_gtest
if not exist "%~dp0tests\third_party\googletest\CMakeLists.txt" goto no_gtest
set /a PASS+=1
echo [PASS] GoogleTest: Exists
goto check_glfw
:no_gtest
set /a WARN+=1
echo [WARN] GoogleTest: Not found

:check_glfw
if not exist "%~dp0examples\glfw-3.3.8\CMakeLists.txt" goto no_glfw
set /a PASS+=1
echo [PASS] GLFW: Exists
goto summary
:no_glfw
set /a WARN+=1
echo [WARN] GLFW: Not found

:summary
echo.
echo ============================================================================
echo Summary: PASS=%PASS% WARN=%WARN% FAIL=%FAIL%
echo ============================================================================

if not "%FAIL%"=="0" goto has_errors
echo [OK] All checks passed!
goto gen_config
:has_errors
echo [!] %FAIL% error(s) found.

:gen_config
echo.
if exist "%~dp0env_config.txt" goto done

echo Generating env_config.txt...
echo # VGLite Environment Config > "%~dp0env_config.txt"
echo # Edit this file to customize paths >> "%~dp0env_config.txt"
echo VS_INSTALL_DIR=%VS_INSTALL_DIR% >> "%~dp0env_config.txt"
echo ANDROID_SDK=%ANDROID_SDK% >> "%~dp0env_config.txt"
echo JAVA_HOME=%JAVA_HOME% >> "%~dp0env_config.txt"
echo ANDROID_NDK_VERSION=%NDK_VERSION% >> "%~dp0env_config.txt"
echo ANDROID_CMAKE_VERSION=%CMAKE_VERSION% >> "%~dp0env_config.txt"
echo ANDROID_BUILD_TOOLS_VERSION=%BUILD_TOOLS_VERSION% >> "%~dp0env_config.txt"
echo ANDROID_PLATFORM_VERSION=%PLATFORM_VERSION% >> "%~dp0env_config.txt"
echo Generated: %~dp0env_config.txt

:done
if "%MODE%"=="check" pause
exit /b 0

:parse_config
set PC_KEY=%~1
set PC_VAL=%~2
if "%PC_KEY%"=="VS_INSTALL_DIR" if not "%PC_VAL%"=="" set VS_INSTALL_DIR=%PC_VAL%
if "%PC_KEY%"=="ANDROID_SDK" if not "%PC_VAL%"=="" set ANDROID_SDK=%PC_VAL%
if "%PC_KEY%"=="JAVA_HOME" if not "%PC_VAL%"=="" set JAVA_HOME=%PC_VAL%
if "%PC_KEY%"=="ANDROID_NDK_VERSION" if not "%PC_VAL%"=="" set NDK_VERSION=%PC_VAL%
if "%PC_KEY%"=="ANDROID_CMAKE_VERSION" if not "%PC_VAL%"=="" set CMAKE_VERSION=%PC_VAL%
if "%PC_KEY%"=="ANDROID_BUILD_TOOLS_VERSION" if not "%PC_VAL%"=="" set BUILD_TOOLS_VERSION=%PC_VAL%
if "%PC_KEY%"=="ANDROID_PLATFORM_VERSION" if not "%PC_VAL%"=="" set PLATFORM_VERSION=%PC_VAL%
goto :EOF