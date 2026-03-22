# VGLite ThorVG CTS - GoogleTest 测试框架

基于 VSI_CTS 测试用例的 1:1 迁移，使用 GoogleTest 框架进行 VGLite ThorVG 后端测试。

## 目录结构

```
tests/
├── CMakeLists.txt              # CMake 配置文件
├── README.md                   # 本文件
├── include/                    # 测试头文件
│   ├── vglite_test.h          # VGLiteTest 基类
│   ├── image_compare.h/cpp    # 图像比较工具
│   └── test_helpers.h         # 测试辅助函数
├── src/
│   ├── main.cpp               # GoogleTest 入口
│   ├── build_verification_test.cpp  # 构建验证测试
│   ├── samples/               # 样本测试 (32个文件)
│   │   ├── clear_test.cpp
│   │   ├── vector_test.cpp
│   │   ├── rotate_test.cpp
│   │   ├── scale_test.cpp
│   │   ├── linear_grad_test.cpp
│   │   ├── radial_grad_test.cpp
│   │   └── ... (更多)
│   ├── unit_test/             # 单元测试
│   │   ├── clear_unit_test.cpp
│   │   ├── combination_test.cpp
│   │   ├── draw_image_test.cpp
│   │   └── tessellation_test.cpp
│   └── unit_test1/            # API 单元测试
│       ├── api_test.cpp
│       ├── gradient_test.cpp
│       ├── matrix_test.cpp
│       ├── memory_test.cpp
│       └── pattern_test.cpp
├── resources/                 # 测试资源文件
│   ├── rotate/landscape.raw
│   ├── scale/circle.raw
│   └── colorkey/landscape.raw
├── thorvg/                    # ThorVG 库编译配置
│   └── CMakeLists.txt
├── utils/                     # 工具库
│   ├── vg_lite_util.c/h
│   ├── zlib/
│   ├── libpng/
│   └── stb_image*.h
├── stubs/                     # LVGL 存根头文件
└── third_party/               # 第三方库
    └── googletest/            # GoogleTest 框架
```

## 构建要求

- **编译器**: Visual Studio 18 2022 或更高版本
- **CMake**: 3.16+
- **平台**: Windows x64

## 构建步骤

```bash
# 1. 进入测试目录
cd vglite_thorvg/tests

# 2. 生成构建文件
cmake -B build -G "Visual Studio 18 2022" -A x64

# 3. 编译
cmake --build build --config Release

# 4. 运行测试
./build/Release/vglite_tests.exe
```

## 运行测试

```bash
# 运行所有测试
./build/Release/vglite_tests.exe

# 运行特定测试套件
./build/Release/vglite_tests.exe --gtest_filter=ClearTest.*

# 运行特定测试
./build/Release/vglite_tests.exe --gtest_filter=ClearTest.Clear

# 输出详细日志
./build/Release/vglite_tests.exe --gtest_print_time=0

# 生成 XML 报告
./build/Release/vglite_tests.exe --gtest_output=xml:test_results.xml
```

## 测试状态

| 测试套件 | 测试数量 | 状态 | 说明 |
|---------|---------|------|------|
| BuildVerification | 2 | ✅ PASSED (1), ⏭ SKIPPED (1) | 构建验证 |
| ClearTest | 1 | ✅ PASSED | 清除缓冲区测试 |
| ClearUnitTest | 4 | ✅ PASSED | 清除单元测试 |
| LinearGradTest | 1 | ✅ PASSED | 线性渐变测试 |
| RadialGradTest | 1 | ✅ PASSED | 径向渐变测试 |
| RotateTest | 1 | ✅ PASSED | 旋转测试 |
| ScaleTest | 1 | ✅ PASSED | 缩放测试 |
| VectorTest | 1 | ✅ PASSED | 矢量绘图测试 |
| StencilTest | 1 | ✅ PASSED | 模板测试 |
| PixelsAlignTest | 1 | ✅ PASSED | 像素对齐测试 |
| MultiDrawTest | 1 | ✅ PASSED | 多次绘制测试 |
| DoubleImageTest | 1 | ✅ PASSED | 双图像测试 |
| PixelMatrixTest | 1 | ✅ PASSED | 像素矩阵测试 |
| GradientTest | 1 | ✅ PASSED | 渐变测试 |
| MemoryTest | 1 | ✅ PASSED | 内存测试 |
| MatrixTest | 4 | ✅ PASSED (2), ⏭ SKIPPED (2) | 矩阵测试 |
| CombinationTest | 4 | ⏭ SKIPPED | SEH 异常 - 路径格式不支持 |
| DrawImageTest | 2 | ⏭ SKIPPED | SEH 异常 - 格式组合不支持 |
| TessellationTest | 6 | ⏭ SKIPPED | SEH 异常 - VG_LITE_S32 不支持 |
| ArcPathTest | 1 | ⏭ SKIPPED | vg_lite_append_path 未实现 |
| BlendModeTest | 1 | ⏭ SKIPPED | vg_lite_append_path 未实现 |
| ColorKeyTest | 1 | ⏭ SKIPPED | colorkey 功能未实现 |
| StrokeTest | 1 | ⏭ SKIPPED | VG_LITE_S32 路径格式不支持 |
| PatternFillTest | 1 | ⏭ SKIPPED | vg_lite_draw_pattern 不完全支持 |
| MaskTest | 2 | ⏭ SKIPPED | mask 功能未实现 |
| ScissorTest | 2 | ⏭ SKIPPED | scissor 功能未实现 |
| GlobalAlphaTest | 1 | ⏭ SKIPPED | global_alpha 功能未实现 |
| GammaTest | 1 | ⏭ SKIPPED | gamma 功能未实现 |
| MirrorTest | 1 | ⏭ SKIPPED | mirror 功能未实现 |
| ColorTransformationTest | 1 | ⏭ SKIPPED | color_transformation 功能未实现 |
| DitherTest | 1 | ⏭ SKIPPED | dither 功能未实现 |
| GaussianBlurTest | 1 | ⏭ SKIPPED | gaussian_blur 功能未实现 |
| ImgIndexTest | 1 | ⏭ SKIPPED | 索引图像格式未实现 |
| ImgA4Test | 1 | ⏭ SKIPPED | A4 格式未实现 |
| ImgA8Test | 1 | ⏭ SKIPPED | A8 格式未完全支持 |
| BlendLvglTest | 1 | ⏭ SKIPPED | LVGL blend 未完全支持 |
| BlendPremultiplyTest | 1 | ⏭ SKIPPED | 预乘 blend 未完全支持 |
| RecolorTest | 1 | ⏭ SKIPPED | recolor 功能未实现 |
| Format24bitTest | 1 | ⏭ SKIPPED | 24位格式未实现 |
| NewBlendModeTest | 1 | ⏭ SKIPPED | 新 blend 模式未实现 |
| ApiTest | 5 | ⏭ SKIPPED | 资源文件缺失 |
| PatternTest | 1 | ⏭ SKIPPED | pattern 不完全支持 |

**总计**: 64 个测试, 22 通过, 42 跳过, 0 失败

## ThorVG 后端未实现的功能

以下 VGLite API 在 ThorVG 后端返回 `VG_LITE_NOT_SUPPORT` 或未实现：

| API 函数 | 状态 |
|---------|------|
| `vg_lite_append_path()` | 返回 VG_LITE_NOT_SUPPORT |
| `vg_lite_get_path_length()` | 返回 0 (存根) |
| `vg_lite_enable_scissor()` | 未实现 |
| `vg_lite_disable_scissor()` | 未实现 |
| `vg_lite_source_global_alpha()` | 未实现 |
| `vg_lite_dest_global_alpha()` | 未实现 |
| `vg_lite_set_gamma()` | 未实现 |
| `vg_lite_set_mirror()` | 未实现 |
| `vg_lite_set_color_transformation()` | 未实现 |
| `vg_lite_set_dither()` | 未实现 |
| `vg_lite_enable_masklayer()` | 返回 VG_LITE_NOT_SUPPORT |
| `vg_lite_disable_masklayer()` | 返回 VG_LITE_NOT_SUPPORT |
| `vg_lite_draw_pattern()` | 可能不完全支持 |

## 路径格式支持

| 路径格式 | 状态 |
|---------|------|
| VG_LITE_S8 | ✅ 支持 |
| VG_LITE_S16 | ⚠️ 部分支持 |
| VG_LITE_S32 | ❌ 导致 SEH 异常 |
| VG_LITE_FP32 | ✅ 支持 |

## 缓冲区格式支持

ThorVG 后端对以下格式的支持有限：

- ✅ **完全支持**: VG_LITE_RGBA8888, VG_LITE_BGRA8888
- ⚠️ **部分支持**: VG_LITE_RGB565, VG_LITE_BGR565, VG_LITE_A8
- ❌ **不支持**: VG_LITE_A4, VG_LITE_L8, VG_LITE_BGRA2222, VG_LITE_INDEX_*, VG_LITE_BGR888, VG_LITE_RGB888

## 添加新测试

1. 在 `src/samples/` 创建新的测试文件:

```cpp
// my_test.cpp
#include <gtest/gtest.h>
#include "vg_lite.h"

namespace {

class MyTest : public ::testing::Test {
protected:
    void SetUp() override {
        // 初始化代码
    }
    
    void TearDown() override {
        // 清理代码
    }
};

TEST_F(MyTest, MyTestCase) {
    // 测试代码
    EXPECT_EQ(vg_lite_init(256, 256), VG_LITE_SUCCESS);
}

} // anonymous namespace
```

2. 在 `CMakeLists.txt` 中添加测试文件:

```cmake
add_executable(vglite_tests
    ...
    src/samples/my_test.cpp
)
```

3. 重新构建并运行测试。

## 测试编写规范

1. **1:1 迁移**: 保持与原始 VSI_CTS 测试用例完全一致的逻辑
2. **不简化**: 不要删除或简化测试代码
3. **跳过标记**: 如果功能未实现，使用 `GTEST_SKIP()` 并注释原始代码
4. **资源目录**: 为每个需要资源文件的测试创建单独的目录

```cpp
// 跳过测试的示例
TEST_F(MyTest, UnsupportedFeature) {
    GTEST_SKIP() << "SKIPPED: feature_x() not implemented";
    
    /* ORIGINAL TEST BODY - COMMENTED OUT DUE TO MISSING IMPLEMENTATION
    // ... 原始测试代码 ...
    */
}
```

## 故障排除

### SEH 异常 (0xc0000005)
- 检查是否使用了不支持的路径格式 (VG_LITE_S32)
- 检查是否使用了不支持的缓冲区格式

### 链接错误
- 确保 ThorVG 库已正确编译
- 检查 `tests/thorvg/CMakeLists.txt` 配置

### 资源文件缺失
- 将 `.raw` 文件放入 `tests/resources/<testname>/` 目录
- 更新测试代码中的资源路径

## 相关文档

- [VGLite API 参考](../vg_lite.h)
- [ThorVG 文档](../libs/thorvg/README.md)
- [GoogleTest 文档](https://google.github.io/googletest/)

## VSI_CTS 测试迁移

### 迁移来源

测试用例从 `VGLite_Tests/VSI_CTS/samples/` 迁移，保持 1:1 代码逻辑。

### 迁移统计

| 优先级 | 迁移数 | 通过 | DISABLED | 跳过 |
|--------|--------|------|----------|------|
| P1 | 3 | 1 | 1 | 1 |
| P2 | 6 | 0 | 5 | 1 |
| P3 | 11 | 1 | 9 | 0 |
| **总计** | **20** | **2** | **15** | **2** |

### 已迁移测试列表

#### P1 优先级

| 测试 | 文件 | 状态 | 说明 |
|------|------|------|------|
| tiger | `tiger_test.cpp` | ✅ PASS | 复杂矢量图形渲染 |
| Elementary | - | ⏭️ 跳过 | 框架库，非单个测试 |
| tiled | `tiled_test.cpp` | ⚠️ DISABLED | ThorVG 不支持 tiled buffer |

#### P2 优先级

| 测试 | 文件 | 状态 | 说明 |
|------|------|------|------|
| vector_upload | `vector_upload_test.cpp` | ⚠️ DISABLED | vg_lite_upload_path 不支持 |
| wrap_user_memory | `wrap_user_memory_test.cpp` | ⚠️ DISABLED | vg_lite_map 不支持 |
| gfx | - | ⏭️ 跳过 | 大型测试套件 (100+ 文件) |
| glyphs2 | `glyphs2_test.cpp` | ⚠️ DISABLED | 需要字体数据文件 |
| imgIndex_endian | `imgIndex_endian_test.cpp` | ⚠️ DISABLED | 索引字节序功能不支持 |
| 24bit_planar | `planar_24bit_test.cpp` | ⚠️ DISABLED | 24位平面格式不支持 |

#### P3 优先级

| 测试 | 文件 | 状态 | 说明 |
|------|------|------|------|
| ARGB2222 | `ARGB2222_test.cpp` | ⚠️ DISABLED | RGBA2222 格式不支持 |
| clock | `clock_test.cpp` | ✅ PASS | 时钟动画演示 |
| linearGradient | `linearGradient_ext_test.cpp` | ⚠️ DISABLED | 扩展渐变 API 不支持 |
| decnano_compress | `decnano_compress_test.cpp` | ⚠️ DISABLED | 需要 landscape.raw + 压缩 API |
| lossy | `lossy_test.cpp` | ⚠️ DISABLED | 需要 folder.raw + YUV tiled 格式 |
| parking | `parking_test.cpp` | ⚠️ DISABLED | 需要 Resource_6/*.img.h 资源文件 |
| premultiply | `premultiply_test.cpp` | ⚠️ DISABLED | 需要 landscape.raw |
| rgba8_etc2 | `rgba8_etc2_test.cpp` | ⚠️ DISABLED | 需要 landscape.pkm |
| testEvo | `testEvo_test.cpp` | ⚠️ DISABLED | 需要 Elm 框架 + EVO 文件 |
| ui | `ui_test.cpp` | ⚠️ DISABLED | 需要 icons/*.raw 文件 |
| yuv_input | `yuv_input_test.cpp` | ⚠️ DISABLED | 需要 YUV raw 文件 |

### 跳过的大型测试

#### Elementary - 框架库

**路径:** `VSI_CTS/samples/Elementary/`

**跳过原因:** 这是一个完整的图形框架库 (Elm Framework)，不是单个测试用例。

```
Elementary/
├── Elm.h              # 框架头文件
├── ElmBuffer.cpp      # 缓冲区管理
├── ElmDraw.cpp        # 绘制引擎
├── ElmObject.cpp      # 对象系统
├── ElmPath.cpp        # 路径处理
├── ElmImage.cpp       # 图像处理
├── ElmFill.cpp        # 填充
├── ElmGradient.cpp    # 渐变
├── ElmStroke.cpp      # 描边
└── ... (20+ 文件)
```

**依赖关系:**
- 需要整个 Elm 框架编译
- testEvo 测试依赖此框架
- 迁移需要移植整个框架，而非单个文件

#### gfx - 大型测试套件

**路径:** `VSI_CTS/samples/gfx/`

**跳过原因:** 这是一个包含 100+ 个独立测试用例的大型套件。

```
gfx/
├── Test_BasicFill.cpp
├── Test_BasicStroke.cpp
├── Test_PathCopy.cpp
├── Test_PathLength.cpp
├── Test_Transform.cpp
├── Test_ClipRect.cpp
├── Test_GradientLinear.cpp
├── Test_GradientRadial.cpp
├── Test_Image.cpp
├── Test_Blend.cpp
└── ... (100+ 文件)
```

**迁移策略:** 按需从中挑选特定测试用例单独迁移。

### ThorVG 后端限制

以下功能在 ThorVG 后端不被支持，导致测试被 DISABLED：

| API/功能 | 状态 |
|----------|------|
| `vg_lite_upload_path()` | 不支持 |
| `vg_lite_map()` | 不支持 |
| Tiled buffer | 不支持 |
| VG_LITE_RGBA2222 格式 | 不支持 |
| 24位平面格式 | 不支持 |
| 索引字节序 | 不支持 |
| 扩展线性渐变 | 不支持 |
| DecNano 压缩 | 不支持 |
| ETC2 纹理压缩 | 不支持 |
| YUV tiled 格式 | 不支持

## 许可证

与主项目相同。
