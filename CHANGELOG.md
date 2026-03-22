# Changelog

All notable changes to the VGLite ThorVG project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Unified build system with root-level `build.bat` script
- Build configuration file `build_config.bat` for centralized path management
- Android APK build support for `clear_demo`
- `.clang-format` configuration for consistent code formatting
- Comprehensive `.gitignore` patterns for build artifacts

### Changed
- Optimized build scripts with interactive menu and command-line modes
- Improved project structure documentation

### Fixed
- Fixed `clear_demo_gl.exe` and `clear_demo_sw.exe` crashes on Windows
  - Added `THORVG_GL_TARGET_GL` definition for Windows WGL context management
  - Fixed `VGLITE_USE_GL_BACKEND` and `TVG_ENABLE_GL_ENGINE` macro checks
  - Corrected `THORVG_GL_RASTER_SUPPORT` value-based checking
- Fixed Android APK build issues
  - Fixed `CMakeLists.txt` path resolution (9 levels up instead of 8)
  - Added automatic `classes.dex` generation for NativeActivity
  - Fixed APK output path to match Windows executable location

## [1.0.0] - 2024-XX-XX

### Added
- Initial VGLite ThorVG integration
- Windows OpenGL (GLFW) backend support
- Windows Software rendering backend support
- ThorVG 1.0.2 integration
- CMake build system
- Example demo: `clear_demo`
  - OpenGL version
  - Software rendering version
  - Android APK version
- Test suite framework
- GLFW integration for window management

### Features
- VGLite API compatibility layer
- OpenGL ES 3.0 support
- Multi-platform support (Windows, Android)
- Hardware-accelerated rendering via GPU
- Software fallback rendering

## Project Structure

```
vglite_thorvg/
├── build.bat              # Unified build entry point
├── build_config.bat       # Build configuration
├── .clang-format          # Code formatting rules
├── CMakeLists.txt         # Main CMake configuration
├── vg_lite.h              # VGLite API header
├── vg_lite_matrix.c       # Matrix operations
├── vg_lite_tvg.cpp        # ThorVG integration
├── demos/                 # Demo applications
│   └── clear_demo/
│       ├── clear_demo.cpp
│       ├── CMakeLists.txt
│       └── android/       # Android-specific files
├── tests/                 # Test suite
├── inc/                   # Public headers
├── libs/                  # Third-party libraries
│   ├── glfw/              # GLFW window library
│   └── thorvg-1.0.2/      # ThorVG rendering library
└── examples/              # Build scripts
    ├── build.bat          # Windows build script
    └── build_android.bat  # Android build script
```

## Known Issues

### Windows
- Visual Studio 2022 or later required for WGL function loading
- OpenGL context creation requires proper graphics drivers

### Android
- Only arm64-v8a architecture supported currently
- Minimum Android API level: 21
- Requires OpenGL ES 3.0 capable device

## Migration Guide

### From Previous Versions

If upgrading from an earlier version:

1. Run `build.bat clean` to clean old build artifacts
2. Update `build_config.bat` with your paths
3. Use new unified build commands:
   - `build.bat windows clear_demo` (instead of `cd examples && build.bat clear_demo`)
   - `build.bat android clear_demo` (instead of `cd examples && build_android.bat clear_demo`)

## Contributing

When contributing to this project:

1. Run `clang-format` on your changes before submitting
2. Test both Windows and Android builds if applicable
3. Update this CHANGELOG with your changes
4. Follow existing code style and patterns

## Notes

- Dates are in YYYY-MM-DD format
- Version numbers follow Semantic Versioning
- Breaking changes are marked with ⚠️
- Security fixes are marked with 🔒
