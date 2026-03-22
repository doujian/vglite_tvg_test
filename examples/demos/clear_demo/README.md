# Clear Demo

This demo shows how to use `vg_lite_clear()` API to clear a buffer with different colors.

## Desktop Build

Build using the top-level build system:
```bash
cd ../../..
build.bat clear_demo    # Build both GL and SW versions
build.bat clear_demo gl # Build only GL version
build.bat clear_demo sw # Build only SW version
```

Run:
```bash
build\Release\clear_demo_gl.exe
build\Release\clear_demo_sw.exe
```

## Android Build

```bash
cd android
./gradlew assembleDebug
```

Output: `app/build/outputs/apk/debug/app-debug.apk`

## Features

- Demonstrates vg_lite_clear() usage
- Cycles through 5 colors (red, green, blue, yellow, cyan)
- Supports both GL and SW backends
- Runs on desktop (GLFW) and Android

## Source Files

- `clear_demo.cpp` - Desktop version using GLFW
- `android/app/src/main/cpp/main.cpp` - Android version using NativeActivity
