# VGLite ThorVG Test

VGLite ThorVG backend implementation and test suite, independent of LVGL dependencies.

## Features

- ThorVG 1.0.2 integration for vector graphics rendering
- Standalone implementation without LVGL dependencies
- GoogleTest-based test framework
- Support for various drawing operations: clear, gradient, blit, path rendering

## Build Requirements

- CMake 3.14+
- Visual Studio 2019+ (Windows) or GCC/Clang (Linux/macOS)
- C++17 support

## Build Instructions

```bash
cd tests
mkdir build && cd build
cmake .. -G "Visual Studio 18 2026" -A x64
cmake --build . --config Release
```

## Run Tests

```bash
./Release/vglite_tests.exe
```

## Directory Structure

```
vglite_thorvg/
├── libs/thorvg-1.0.2/    # ThorVG 1.0.2 library
├── tests/                # Test framework
│   ├── src/              # Test source files
│   ├── utils/            # Utility libraries (zlib, libpng)
│   └── third_party/      # GoogleTest
├── vg_lite.h             # VGLite API header
├── vg_lite_matrix.c      # Matrix operations
├── vg_lite_tvg.cpp       # ThorVG backend implementation
└── vglite_config.h       # Standalone configuration
```

## Test Results

- 20 tests passing
- 44 tests skipped (features not yet implemented)
- 0 tests failing

## License

See individual component licenses.
