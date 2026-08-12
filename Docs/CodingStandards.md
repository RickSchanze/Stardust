# Stardust 编码规范

本文档与 `.cursor/rules/stardust-conventions.mdc`、`.clang-format`、`.clang-tidy` 保持一致。

## 1. 语言与工具链

- **标准**：C++26（`CMAKE_CXX_STANDARD 26`）
- **编译器**：GCC 16+（MSYS2 / CLion Toolchain）
- **特性优先**：能用语言/标准库解决的，不用宏或过时惯用法；静态反射（`^^`、`std::meta`、splicer `[: :]`）按 GCC 实现使用
- **IDE**：CLion；构建目录为 CMake 生成目录（如 `cmake-build-debug`）

## 2. 仓库布局

```
Stardust/
├── CMakeLists.txt          # 根：标准、子目录
├── Source/
│   ├── CMakeLists.txt      # 引擎目标与源文件
│   └── ...
├── ThirdParty/
│   ├── CMakeLists.txt      # add_subdirectory（git submodule）
│   ├── spdlog/             # submodule: RickSchanze/spdlog
│   ├── glm/                # submodule: g-truc/glm
│   └── mimalloc/           # submodule: RickSchanze/mimalloc
├── Docs/
├── .clang-format
├── .clang-tidy
└── .cursor/rules/
```

克隆后：

```bash
git submodule update --init --recursive
# 若 GitHub 拉不下：
git -c http.proxy=http://127.0.0.1:7890 -c https.proxy=http://127.0.0.1:7890 submodule update --init --recursive
```

## 3. 命名风格（UE 风格）

| 类别 | 规则 | 正确 | 错误 |
|------|------|------|------|
| 命名空间 | 大驼峰 | `Stardust::Core` | `stardust::core` |
| 类 / 结构体 / 枚举 | 大驼峰 | `class RenderPass` | `class render_pass` |
| 函数 / 方法 | 大驼峰 | `void DrawFrame()` | `void draw_frame()` |
| 局部变量 / 参数 | 大驼峰 | `int BufferCount` | `int bufferCount` |
| public 成员 | 大驼峰 | `int Width` | `int width` / `int mWidth` |
| protected / private | `m` + 大驼峰 | `int mWidth` | `int Width` / `int m_Width` |
| 全局变量 | `g` + 大驼峰 | `Engine* gEngine` | `Engine* engine` |
| 静态变量（任意） | `s` + 大驼峰 | `static int sCount` | `static int Count` |

### 示例

```cpp
namespace Stardust::Render
{

class RenderDevice
{
public:
    void CreateBuffer(int BufferSize);

    int DeviceIndex = 0;

protected:
    void* mNativeHandle = nullptr;

private:
    static RenderDevice* sInstance;
    int mBufferCount = 0;
};

RenderDevice* gPrimaryDevice = nullptr;

} // namespace Stardust::Render
```

### 例外

1. `main` / `WinMain` / `wWinMain` 保持语言要求的名字；业务入口可另写 `Main()`。
2. 第三方符号不重命名、不包一层仅为改名。

## 4. 尽量不用宏

引擎代码**默认禁止**新增 `#define` 业务宏。

| 旧宏写法 | C++26 替代 |
|----------|------------|
| `#define K 42` | `constexpr int K = 42;` |
| `#define Max(a,b) ...` | `constexpr` 函数 / `std::max` |
| `#define TYPE int` | `using Type = int;` |
| `#ifdef FEATURE` 业务分支 | `if constexpr`、特性检测、配置对象 |
| 手工枚举字符串 | 静态反射 |

仅允许与预处理器强绑定的最小集合（如 `#pragma once`、包含第三方、无法替代的平台头探测），不得渗入游戏/引擎逻辑。

## 5. 格式化与静态检查

- **clang-format**：Allman 大括号、缩进 4 空格、列宽 120、指针/引用贴类型左侧（`Type*`）
- **clang-tidy**：`readability-identifier-naming` 按第 3 节规则检查
- CLion：启用 ClangFormat、Clang-Tidy，与仓库配置文件对齐

## 6. 第三方库

**禁止 FetchContent。** 一律 git submodule + `add_subdirectory`。

| 库 | 用途 | 仓库 | CMake 目标 |
|----|------|------|------------|
| spdlog | 日志 | https://github.com/RickSchanze/spdlog.git | `spdlog::spdlog` |
| glm | 数学 | https://github.com/g-truc/glm.git | `glm::glm` |
| mimalloc | 分配器 | https://github.com/RickSchanze/mimalloc | `mimalloc-static` |

引擎目标通过 `StardustThirdParty` 统一链接。`Main.cpp` 包含 `mimalloc-new-delete.h` 覆盖全局 `new`/`delete`。

拉取失败时使用代理：`http://127.0.0.1:7890`。
