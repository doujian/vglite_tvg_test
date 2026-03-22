# VGLite ThorVG 项目优化计划

## 概述

对 vglite_thorvg 项目进行全面优化，包括构建系统统一、代码结构整理、文档完善等方面。

## 当前问题分析

### 1. 文件清理
- [x] `nul` 文件 - Windows 重定向错误创建的垃圾文件（已清理）
- [x] `examplesclear_demo` 空目录 - 命名错误（已清理）

### 2. 构建脚本分散
- `examples/build.bat` - Windows 桌面构建
- `examples/build_android.bat` - Android APK 构建
- `tests/build_vs.bat` - Visual Studio 测试构建
- `libs/thorvg-1.0.2/build_tvg.bat` - ThorVG 构建

### 3. 目录结构
- 根目录源文件过多（6个核心文件）
- 缺少 `src/` 目录组织

---

## TODOs

### Wave 1: 构建系统统一（高优先级）

- [ ] 1. **创建统一构建入口脚本**
  
  **What to do**: 创建 `build.bat` 作为根目录统一入口，整合 Windows 和 Android 构建
  
  **Must Have**:
  - 支持交互式菜单选择构建目标
  - 支持命令行参数直接指定构建目标
  - 统一配置管理（VS路径、Android SDK路径）
  
  **Command**: `build.bat windows` | `build.bat android clear_demo` | `build.bat`

- [ ] 2. **添加环境检测**
  
  **What to do**: 在构建开始前自动检测必要工具
  
  **检测项**:
  - Visual Studio 安装路径
  - Android SDK/NDK 路径
  - CMake 版本
  - Java 版本

- [ ] 3. **创建配置文件**
  
  **What to do**: 将路径配置提取到独立配置文件
  
  **文件**: `build_config.bat` 或 `build.env`
  
  **内容**: VS路径、Android SDK路径、签名密钥配置

### Wave 2: 代码结构优化（中优先级）

- [ ] 4. **整理源文件目录**
  
  **What to do**: 可选 - 将根目录源文件移动到 `src/` 目录
  
  **注意**: 此项需要更新所有 CMakeLists.txt 中的路径引用
  
  **建议**: 保持现状，除非需要大规模重构

- [ ] 5. **添加 .clang-format**
  
  **What to do**: 创建统一的代码格式化配置
  
  **文件**: `.clang-format`
  
  **风格**: LLVM 或 Google 风格

- [ ] 6. **完善 .gitignore**
  
  **What to do**: 添加更多需要忽略的文件模式
  
  **添加**:
  ```
  # Build outputs
  *.apk
  *.idsig
  
  # Generated files
  nul
  *.dex
  
  # Debug keystore
  debug.keystore
  ```

### Wave 3: 文档完善（中优先级）

- [ ] 7. **创建 CHANGELOG.md**
  
  **What to do**: 记录版本变更历史
  
  **内容**:
  - 功能新增
  - Bug 修复
  - API 变更

- [ ] 8. **完善 README.md**
  
  **What to do**: 更新项目文档
  
  **添加**:
  - 构建指南
  - Android 构建说明
  - 已知问题

- [ ] 9. **创建 BUILDING.md**
  
  **What to do**: 详细的构建指南
  
  **内容**:
  - Windows 构建步骤
  - Android 构建步骤
  - 环境配置
  - 常见问题

### Wave 4: CI/CD（低优先级）

- [ ] 10. **添加 GitHub Actions**
  
  **What to do**: 创建自动化构建流程
  
  **文件**: `.github/workflows/build.yml`
  
  **功能**:
  - Windows 构建
  - Android 构建
  - 自动化测试

---

## 执行建议

### 立即执行（推荐）
1. 清理垃圾文件（已完成）
2. 创建统一构建入口（Wave 1）
3. 完善 .gitignore

### 后续优化
1. 代码格式化配置
2. 文档完善
3. CI/CD 自动化

---

## 风险评估

| 优化项 | 风险 | 建议 |
|--------|------|------|
| 统一构建脚本 | 低 | 推荐执行 |
| 目录重构 | 高 | 保持现状 |
| CI/CD | 中 | 可选执行 |

---

## 下一步

运行 `/start-work` 开始执行优化计划。
