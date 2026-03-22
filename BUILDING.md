# Building VGLite ThorVG

This guide covers how to build VGLite ThorVG for Windows and Android platforms.

## Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Windows Build](#windows-build)
- [Android Build](#android-build)
- [Configuration](#configuration)
- [Troubleshooting](#troubleshooting)
- [Advanced Topics](#advanced-topics)

---

## Prerequisites

### Common Requirements

- **Git** - For cloning the repository and fetching dependencies
- **CMake** (3.14 or later) - Build system generator
  - Download: https://cmake.org/download/
  - Make sure `cmake` is in your PATH

### Windows Requirements

- **Visual Studio 2022** (or later) with C++ development tools
  - Community edition is sufficient
  - Required workloads: "Desktop development with C++"
- **Windows SDK** (usually installed with Visual Studio)

### Android Requirements

- **Android Studio** - IDE and SDK manager
  - Download: https://developer.android.com/studio
- **Android SDK** (API 34 recommended)
- **Android NDK** (r29 or later)
- **Java JDK** (17 or later)
- **CMake** (via Android SDK Manager)
- **Ninja** (via Android SDK Manager)

---

## Quick Start

### 1. Clone the Repository

```bash
git clone <repository-url>
cd vglite_thorvg
```

### 2. Configure Build Environment

Edit `build_config.bat` and set your paths:

```batch
REM Visual Studio
set "VS_INSTALL_DIR=C:\Program Files\Microsoft Visual Studio\2022\Community"

REM Android SDK
set "ANDROID_SDK=C:\Users\%USERNAME%\AppData\Local\Android\Sdk"
set "ANDROID_NDK=%ANDROID_SDK%\ndk\29.0.14206865"
```

### 3. Build

#### Interactive Mode

```cmd
build.bat
```

Then select your target from the menu.

#### Command Line Mode

```cmd
REM Build for Windows
build.bat windows clear_demo

REM Build Android APK
build.bat android clear_demo

REM Build for both platforms
build.bat both clear_demo
```

---

## Windows Build

### Detailed Steps

1. **Setup Visual Studio Environment**

   The build script will automatically initialize the VS environment using `vcvarsall.bat`. Ensure your VS path is correct in `build_config.bat`.

2. **Build Specific Demo**

   ```cmd
   build.bat windows clear_demo
   ```

3. **Build All Demos**

   ```cmd
   build.bat windows
   ```
   
   Or simply:
   ```cmd
   build.bat
   ```
   Then select option 1 → 2

4. **Run the Demo**

   ```cmd
   cd examples
   build.bat run clear_demo
   ```
   
   Or manually:
   ```cmd
   build\clear_demo_gl.exe
   build\clear_demo_sw.exe
   ```

### Build Options

| Option | Description |
|--------|-------------|
| `gl` | Build OpenGL backend only |
| `sw` | Build Software backend only |
| (none) | Build both GL and SW backends |

Example:
```cmd
build.bat windows clear_demo gl
```

### Output Location

Build outputs are placed in:
- `build\demos\<demo_name>\<demo_name>_gl.exe`
- `build\demos\<demo_name>\<demo_name>_sw.exe`

---

## Android Build

### Prerequisites Setup

1. **Install Android Studio**

   Download from: https://developer.android.com/studio

2. **Install SDK Components**

   Open Android Studio → SDK Manager and install:
   - Android SDK Platform 34
   - Android SDK Build-Tools 34.0.0
   - NDK (Side by side) - 29.0.14206865
   - CMake - 3.22.1
   - Android SDK Platform-Tools

3. **Configure Environment**

   Set environment variables or edit `build_config.bat`:
   
   ```batch
   set "ANDROID_SDK=C:\Users\%USERNAME%\AppData\Local\Android\Sdk"
   set "ANDROID_NDK=%ANDROID_SDK%\ndk\29.0.14206865"
   ```

### Build Steps

1. **Build APK**

   ```cmd
   build.bat android clear_demo
   ```

2. **Install on Device**

   Connect your Android device via USB with debugging enabled:
   
   ```cmd
   adb install -r build\demos\clear_demo\clear_demo.apk
   ```

3. **Run on Device**

   The app should appear in your app launcher as "VGLite clear_demo".

### Build Process

The Android build performs these steps:

1. **Configure** - CMake generates Ninja build files
2. **Compile** - Builds native library (`libvglite_demo.so`)
3. **Package** - Creates APK structure with manifest
4. **Assemble** - Adds native library and DEX to APK
5. **Sign** - Signs APK with debug keystore

### Output Location

The final APK is located at:
- `build\demos\clear_demo\clear_demo.apk`

---

## Configuration

### build_config.bat Options

| Variable | Description | Default |
|----------|-------------|---------|
| `VS_INSTALL_DIR` | Visual Studio installation path | `D:\Program Files\Microsoft Visual Studio\18\Community` |
| `ANDROID_SDK` | Android SDK root path | `E:\Android\Sdk` |
| `ANDROID_NDK` | Android NDK path | `%ANDROID_SDK%\ndk\29.0.14206865` |
| `DEFAULT_GENERATOR` | CMake generator | `Ninja` |
| `DEFAULT_CONFIG` | Build configuration | `Release` |
| `DEFAULT_ARCH` | Target architecture | `x64` |

### Custom Configuration

Create `build_config.local.bat` to override settings without modifying the main file:

```batch
@echo off
REM My local settings
set "VS_INSTALL_DIR=C:\Program Files\Microsoft Visual Studio\2022\Community"
set "ANDROID_SDK=C:\Users\John\AppData\Local\Android\Sdk"
```

---

## Troubleshooting

### Windows Build Issues

#### "Visual Studio not found"

**Problem:** Build script cannot find Visual Studio.

**Solution:**
1. Verify VS is installed
2. Update `VS_INSTALL_DIR` in `build_config.bat`
3. Common paths:
   - VS 2022 Community: `C:\Program Files\Microsoft Visual Studio\2022\Community`
   - VS 2019 Community: `C:\Program Files (x86)\Microsoft Visual Studio\2019\Community`

#### "CMake not found"

**Problem:** CMake is not in PATH.

**Solution:**
1. Download CMake from https://cmake.org/download/
2. During install, select "Add CMake to system PATH"
3. Or manually add `C:\Program Files\CMake\bin` to PATH

#### "GLFW/glfw3.h file not found"

**Problem:** Dependencies not fetched.

**Solution:**
```cmd
build.bat clean
build.bat windows clear_demo
```

CMake will automatically fetch GLFW via FetchContent.

### Android Build Issues

#### "Android SDK not found"

**Problem:** SDK path is incorrect.

**Solution:**
1. Open Android Studio
2. File → Settings → Appearance & Behavior → System Settings → Android SDK
3. Copy the "Android SDK Location"
4. Update `ANDROID_SDK` in `build_config.bat`

#### "NDK not found"

**Problem:** NDK not installed or path incorrect.

**Solution:**
1. Open Android Studio SDK Manager
2. Go to "SDK Tools" tab
3. Check "NDK (Side by side)"
4. Click Apply to install
5. Note the version number and update `ANDROID_NDK` path

#### "Java not found"

**Problem:** Java JDK not installed or not in PATH.

**Solution:**
1. Download JDK 17 or later from Oracle or OpenJDK
2. Set `JAVA_HOME` environment variable
3. Add `%JAVA_HOME%\bin` to PATH

#### APK Installation Fails

**Problem:** Device not connected or debugging not enabled.

**Solution:**
1. Enable USB debugging on your Android device:
   - Settings → About Phone → Tap "Build Number" 7 times
   - Settings → Developer Options → Enable "USB Debugging"
2. Connect device and accept debugging prompt
3. Verify connection: `adb devices`

### Runtime Issues

#### "clear_demo_gl.exe crashes immediately"

**Problem:** OpenGL context creation failed.

**Solution:**
1. Update graphics drivers
2. Verify OpenGL 3.3+ support: Use OpenGL Extensions Viewer
3. Try software backend: `clear_demo_sw.exe`

#### "APK crashes on launch"

**Problem:** OpenGL ES 3.0 not supported or native library missing.

**Solution:**
1. Check device supports OpenGL ES 3.0
2. Verify APK contains native library:
   ```cmd
   unzip -l clear_demo.apk | findstr libvglite
   ```
3. Check logcat for errors:
   ```cmd
   adb logcat -c && adb logcat | findstr vglite
   ```

---

## Advanced Topics

### Adding a New Demo

1. Create demo directory:
   ```
   demos/my_demo/
   ├── my_demo.cpp
   └── CMakeLists.txt
   ```

2. Add to `demos/CMakeLists.txt`:
   ```cmake
   add_subdirectory(my_demo)
   ```

3. Build:
   ```cmd
   build.bat windows my_demo
   ```

### Custom Build Flags

For custom CMake flags, edit the build scripts or use environment variables:

```cmd
set CMAKE_FLAGS=-DENABLE_SANITIZER=ON
build.bat windows clear_demo
```

### Cross-Compilation

Currently supported:
- Windows x64 → Windows x64 (native)
- Windows x64 → Android arm64-v8a (cross)

Future support planned for:
- Linux builds
- WebAssembly (Emscripten)

### Code Formatting

Use clang-format to maintain consistent style:

```cmd
REM Format single file
clang-format -i src/myfile.cpp

REM Format all source files
for /r %f in (*.cpp *.c *.h) do clang-format -i "%f"
```

---

## Getting Help

If you encounter issues not covered here:

1. Check [CHANGELOG.md](CHANGELOG.md) for known issues
2. Review build logs in the build directory
3. Run with verbose output:
   ```cmd
   set VERBOSE=1
   build.bat windows clear_demo
   ```
4. Open an issue with:
   - Build command used
   - Full error message
   - System information (OS version, VS version, etc.)

---

## See Also

- [CHANGELOG.md](CHANGELOG.md) - Version history and changes
- [README.md](README.md) - Project overview
- [CMake Documentation](https://cmake.org/documentation/)
- [Android NDK Documentation](https://developer.android.com/ndk/guides)
