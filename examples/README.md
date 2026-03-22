# VGLite ThorVG Examples

This directory contains demo applications for the VGLite API using ThorVG as the rendering backend.

## Directory Structure

```
examples/
├── cmake/
│   └── DemoTemplate.cmake    # CMake template for building demos
├── demos/
│   └── clear_demo/           # Clear demo (vg_lite_clear example)
│       ├── CMakeLists.txt    # Demo-specific CMake configuration
│       ├── clear_demo.cpp    # Desktop version source
│       ├── android/          # Android build
│       └── README.md         # Demo documentation
├── build.bat                 # Build script (Windows)
├── CMakeLists.txt            # Top-level CMake configuration
└── README.md                 # This file
```

## Features

- **Multiple demos**: Each demo in its own directory under `demos/`
- **Dual backend support**: Every demo builds both GL and SW versions
- **Auto-discovery**: New demos are automatically picked up by the build system
- **Cross-platform**: Desktop (Windows) and Android support

## Prerequisites

### Desktop (Windows)
- Visual Studio 17 2022 with C++ tools
- CMake 3.14 or later
- Git (for GLFW FetchContent)

### Android
- Android Studio or command-line tools
- Android NDK
- Gradle

## Building

### Build All Demos

```cmd
build.bat
```

This builds all discovered demos with both GL and SW backends.

### Build Specific Demo

```cmd
build.bat clear_demo          # Build clear_demo (GL + SW)
build.bat clear_demo gl       # Build clear_demo (GL only)
build.bat clear_demo sw       # Build clear_demo (SW only)
```

### Clean Build

```cmd
build.bat clean
```

### Run Demo

```cmd
build.bat run clear_demo
```

### List Available Demos

```cmd
build.bat list
```

## Build Outputs

After building, executables are located in:

```
build/
└── Release/
    ├── clear_demo_gl.exe     # GL backend version
    ├── clear_demo_sw.exe     # SW backend version
    └── ...                   # Other demos
```

## Adding a New Demo

1. Create a new directory under `demos/`:
   ```cmd
   mkdir demos/my_demo
   ```

2. Create `demos/my_demo/CMakeLists.txt`:
   ```cmake
   add_demo(my_demo my_demo.cpp)
   ```

3. Add your source file(s) to the demo directory.

4. Rebuild:
   ```cmd
   build.bat
   ```

The new demo will be automatically discovered and built with both GL and SW versions.

## Android Demos

Each demo can optionally include an Android version:

```cmd
cd demos/clear_demo/android
./gradlew assembleDebug
```

APK will be generated at: `app/build/outputs/apk/debug/app-debug.apk`

## Demos

### clear_demo

Demonstrates `vg_lite_clear()` API usage.

**Features**:
- Clears buffer with 5 cycling colors (red, green, blue, yellow, cyan)
- Runs at 1 FPS color transitions
- Supports both GL and SW backends

**Files**:
- Desktop: `demos/clear_demo/clear_demo.cpp`
- Android: `demos/clear_demo/android/app/src/main/cpp/main.cpp`

## Backend Differences

### GL Backend (`_gl`)
- Uses OpenGL Framebuffer Object for rendering
- Faster for GPU-accelerated operations
- Requires OpenGL 2.0+ support

### SW Backend (`_sw`)
- Uses software rendering
- Renders to memory buffer, then uploads to texture
- Works on systems without GPU acceleration

## Troubleshooting

### Build Fails: "Could not find Git"
Make sure Git is installed and available in PATH. GLFW is downloaded via CMake's FetchContent which requires Git.

### Build Fails: "No CMAKE_C_COMPILER"
Ensure Visual Studio 17 2022 is installed with C++ development tools.

### Demo Not Found
Check that the demo directory exists under `demos/` and contains a valid `CMakeLists.txt` file.

## See Also

- [VGLite API Reference](../vg_lite.h)
- [ThorVG Documentation](../libs/thorvg-1.0.2/README.md)
- [GLFW Documentation](https://www.glfw.org/docs/latest/)
