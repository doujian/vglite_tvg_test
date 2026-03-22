# vg_lite_clear Demo

这是一个使用 vg_lite_clear 函数的演示程序，展示了如何在 GLFW 窗口中循环显示五种颜色。

## 功能特性

- 使用 `vg_lite_clear` 函数清除缓冲区
- 使用 GLFW 创建窗口
- 使用 OpenGL 显示渲染结果
- 五种颜色循环显示：红、绿、蓝、黄、青
- 无限循环，按 ESC 键或关闭窗口退出

## 前置要求

### 必需软件

- **Visual Studio 18 2022** - C++ 编译器
- **CMake 3.14+** - 构建系统
- **Git** - 用于 GLFW FetchContent

### 依赖库

以下依赖库会自动下载或从项目引用：

- **GLFW 3.3.8** - 窗口管理（通过 FetchContent 自动下载）
- **ThorVG 1.0.2** - 矢量图形渲染引擎（项目内置）
- **vg_lite** - VGLite API 实现（项目内置）

## 目录结构

```
examples/
├── CMakeLists.txt        # CMake 构建配置
├── build.bat             # Windows 构建脚本
├── README.md             # 本文件
└── clear_demo/
    └── clear_demo.cpp    # 演示程序源码
```

## 构建步骤

### 方法一：使用构建脚本（推荐）

```cmd
cd examples
build.bat
```

### 方法二：手动构建

```cmd
cd examples
cmake -B build -G "Visual Studio 18 2022" -A x64
cmake --build build --config Release
```

## 运行

```cmd
cd examples
build\Release\clear_demo.exe
```

或者使用构建脚本直接运行：

```cmd
build.bat run
```

## 预期行为

1. 程序启动后，会创建一个 640x480 的 GLFW 窗口
2. 窗口中央显示一个 256x256 的渲染区域
3. 渲染区域会每秒切换一次颜色，循环显示五种颜色：
   - 红色 (0xFFFF0000)
   - 绿色 (0xFF00FF00)
   - 蓝色 (0xFF0000FF)
   - 黄色 (0xFFFFFF00)
   - 青色 (0xFF00FFFF)
4. 控制台会输出当前显示的颜色信息
5. 按 ESC 键或点击窗口关闭按钮退出程序

## 退出方式

- 按 **ESC** 键
- 点击窗口的 **关闭按钮**

## 代码结构说明

### clear_demo.cpp

主要包含以下部分：

1. **配置常量** - 窗口大小、缓冲区大小、颜色定义
2. **全局状态** - vg_lite 缓冲区和 OpenGL 纹理
3. **OpenGL 设置** - 纹理创建和更新
4. **渲染函数** - 将纹理绘制到窗口
5. **GLFW 回调** - 错误处理和键盘输入
6. **主函数** - 初始化、主循环、清理

### 主循环流程

```
while (!glfwWindowShouldClose(window)) {
    1. 获取当前颜色
    2. vg_lite_clear() - 清除缓冲区
    3. vg_lite_finish() - 完成渲染
    4. update_texture() - 更新 OpenGL 纹理
    5. render_texture() - 渲染到窗口
    6. glfwSwapBuffers() - 交换缓冲区
    7. 等待 1 秒（同时处理事件）
    8. 切换到下一个颜色
}
```

## 故障排除

### 构建失败

1. 确保已安装 Visual Studio 18 2022
2. 确保已安装 CMake 3.14 或更高版本
3. 确保网络连接正常（GLFW 需要从 GitHub 下载）

### 运行失败

1. 确保显卡支持 OpenGL 2.0 或更高版本
2. 检查控制台输出的错误信息

## 相关文档

- [VGLite API 参考](../vg_lite.h)
- [ThorVG 文档](../libs/thorvg/README.md)
- [GLFW 文档](https://www.glfw.org/docs/latest/)
