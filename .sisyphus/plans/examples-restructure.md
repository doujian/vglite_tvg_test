# Examples 目录结构重构计划

## TL;DR

> **目标**: 重构 examples/ 目录结构，支持多 demo 扩展，同时构建 GL/SW 双版本，并支持每个 demo 内置 Android 构建

**Deliverables**:
- demos/clear_demo/ 目录
- cmake/DemoTemplate.cmake
- 重构的 CMakeLists.txt 文件
- 重构的 build.bat

**Estimated Effort**: Large
**Parallel Execution**: YES - 3 waves

---

## Context

### Interview Summary
- 目录结构: 独立 demo 目录，顶层自动发现
- 构建模式: 同时构建 GL 和 SW 双版本
- 构建产物: 统一 examples/build/
- Android: 每个 demo 内置 android/ 目录

---

## Work Objectives

### Must Have
- 每个 demo 独立目录结构
- 同时构建 GL/SW 双版本
- 统一的 build/ 目录
- 可扩展的 add_demo() 函数

### Must NOT Have
- 不要破坏 tests/ 目录现有功能
- 不要在 demos 目录外创建新 build 目录

---

## Execution Strategy

### Wave 1 (基础设施)
1. 创建 cmake/DemoTemplate.cmake - 定义 add_demo() 函数
2. 重构 tests/thorvg/CMakeLists.txt - 支持 thorvg_gl 和 thorvg_sw
3. 重构 examples/CMakeLists.txt - 自动发现 demos/*/

### Wave 2 (Demo 迁移)
4. 创建 demos/clear_demo/ 目录结构
5. 迁移 clear_demo 桌面源码和 CMake
6. 迁移 clear_demo Android 构建

### Wave 3 (清理)
7. 删除旧的 build_* 目录
8. 重构 build.bat
9. 更新 README.md

### Final Verification
F1. 完整构建验证
F2. 功能验证
F3. Android 构建验证

---

## TODOs

- [ ] 1. 创建 cmake/DemoTemplate.cmake
  定义 add_demo(DEMO_NAME SOURCES...) 函数，自动创建 _gl 和 _sw 两个目标

- [ ] 2. 重构 tests/thorvg/CMakeLists.txt
  创建 thorvg_gl (含 GL engine) 和 thorvg_sw (仅 SW engine) 两个库

- [ ] 3. 重构 examples/CMakeLists.txt
  使用 file(GLOB) 自动发现 demos/*/，设置 VGLITE_BUILD_DUAL_THORVG=ON

- [ ] 4. 创建 demos/clear_demo/ 目录结构
  包含 CMakeLists.txt, clear_demo.cpp, android/, README.md

- [ ] 5. 迁移 clear_demo 桌面源码
  复制 .cpp 文件，创建 CMakeLists.txt 调用 add_demo()

- [ ] 6. 迁移 clear_demo Android 构建
  复制 android/ 目录，更新 VGLITE_ROOT 路径 (6级 → 8级)

- [ ] 7. 删除旧目录
  rm -rf build build_gl build_sw build_test build_test_gl build_makefiles clear_demo

- [ ] 8. 重构 build.bat
  支持: build.bat [demo] [gl|sw], build.bat clean, build.bat run <demo>

- [ ] 9. 更新 README.md
  更新目录结构说明，添加新 demo 指南，Android 构建说明

---

## Final Verification

- [ ] F1. 完整构建验证
  cmake -B build && cmake --build build --config Release
  验证 build/Release/clear_demo_gl.exe 和 clear_demo_sw.exe 存在

- [ ] F2. 功能验证
  运行 clear_demo_gl.exe 和 clear_demo_sw.exe，验证窗口显示

- [ ] F3. Android 构建验证
  cd demos/clear_demo/android && ./gradlew assembleDebug
  验证 APK 生成成功

---

## Commit Strategy

Commit 1: Wave 1 (DemoTemplate + ThorVG + 顶层 CMake)
Commit 2: Wave 2 (demos/clear_demo/ 迁移)
Commit 3: Wave 3 (build.bat + README.md)

---

## Success Criteria

build.bat                    # 构建所有 demo
build.bat clear_demo gl      # 只构建 clear_demo GL
build.bat run clear_demo     # 运行 demo
ls build/Release/*_gl.exe    # 验证 GL 版本
ls build/Release/*_sw.exe    # 验证 SW 版本
