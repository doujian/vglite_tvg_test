# VGLite ThorVG Test

VGLite ThorVG backend implementation and test suite, independent of LVGL dependencies.

## Features

- ThorVG 1.0.2 integration for vector graphics rendering
- Standalone implementation without LVGL dependencies
- GoogleTest-based test framework
- Support for various drawing operations: clear, gradient, blit, path rendering
- **Windows Desktop builds** (Visual Studio 2022)
- **Android APK builds** (native library)

## Quick Start

### 1. Check Environment

Run the environment check script to verify your setup:

```cmd
check_env.bat
```

This will:
- Auto-detect Visual Studio, Android SDK, JDK installations
- Generate `env_config.txt` configuration file
- Show which dependencies are missing

### 2. Build

**Windows Tests:**
```cmd
cd tests
build_tests.bat
```

**Windows Examples:**
```cmd
cd examples
build.bat              # Build all demos
build.bat clear_demo   # Build specific demo
```

**Android APK:**
```cmd
cd examples
build_android.bat              # Build all APKs
build_android.bat clear_demo   # Build specific demo APK
```

## Requirements

### Windows Desktop Build

| Requirement | Version | Notes |
|-------------|---------|-------|
| Visual Studio | 2022 | With C++ development tools |
| CMake | 3.14+ | System PATH |
| Git | Any | For FetchContent (optional) |

### Android Build

| Requirement | Version | Notes |
|-------------|---------|-------|
| Android SDK | Any recent | Via Android Studio or standalone |
| Android NDK | 29.0.14206865 | Via SDK Manager |
| Build Tools | 34.0.0 | Via SDK Manager |
| JDK | 17 or 11 | Set JAVA_HOME |

## Environment Configuration

### Auto-Detection

`check_env.bat` automatically detects:
- Visual Studio: `D:\Program Files\Microsoft Visual Studio\*\Community`
- Android SDK: `D:\Android\Sdk`, `E:\Android\Sdk`, `C:\Users\...\AppData\Local\Android\Sdk`
- JDK: `D:\Programs\jdk-*`, `C:\Program Files\Java\jdk-*`

### Manual Configuration

Edit `env_config.txt` to customize paths:

```ini
# Visual Studio installation directory
VS_INSTALL_DIR=D:\Program Files\Microsoft Visual Studio\18\Community

# Android SDK directory
ANDROID_SDK=D:\Android\Sdk

# JDK directory (do NOT include \bin)
JAVA_HOME=D:\Programs\jdk-17.0.147

# Android versions
ANDROID_NDK_VERSION=29.0.14206865
ANDROID_CMAKE_VERSION=3.22.1
ANDROID_BUILD_TOOLS_VERSION=34.0.0
ANDROID_PLATFORM_VERSION=34
```

### Common Issues

| Issue | Solution |
|-------|----------|
| `JAVA_HOME` wrong | Don't include `\bin` in the path |
| `Visual Studio not found` | Edit `env_config.txt` or set `VS_INSTALL_DIR` |
| `apksigner failed` | Check `JAVA_HOME` is set correctly |
| `aapt2 failed` | Install Build Tools via SDK Manager |

## Directory Structure

```
vglite_tvg_test/
├── check_env.bat           # Environment check and setup
├── env_config.txt          # User configuration (auto-generated)
├── debug.keystore          # Android debug keystore (auto-generated)
├── libs/
│   └── thorvg-1.0.2/       # ThorVG 1.0.2 library
├── tests/
│   ├── build_tests.bat     # Test build script
│   ├── CMakeLists.txt      # Test CMake config
│   └── src/                # Test source files
├── examples/
│   ├── build.bat           # Windows build script
│   ├── build_android.bat   # Android APK build script
│   ├── demos/              # Demo applications
│   └── CMakeLists.txt      # Examples CMake config
├── vg_lite.h               # VGLite API header
├── vg_lite_matrix.c        # Matrix operations
├── vg_lite_tvg.cpp         # ThorVG backend implementation
└── vglite_config.h         # Standalone configuration
```

## Build Scripts

| Script | Purpose |
|--------|---------|
| `check_env.bat` | Check environment, generate config |
| `check_env.bat check` | Check only (pause at end) |
| `check_env.bat set` | Set variables (silent, for scripts) |
| `tests/build_tests.bat` | Build and run tests |
| `examples/build.bat` | Build Windows demos |
| `examples/build_android.bat` | Build Android APKs |

## Test Results

- 20 tests passing
- 44 tests skipped (features not yet implemented)
- 0 tests failing

## License

See individual component licenses.

- ThorVG: MIT License
- GoogleTest: BSD-3-Clause License
- VGLite: See vg_lite.h