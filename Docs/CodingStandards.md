# Stardust 编码规范

本文档与 `.cursor/rules/stardust-conventions.mdc`、`.cursor/rules/clang-format-and-tidy.mdc`、`.clang-format`、`.clang-tidy` 保持一致。

## 1. 语言与工具链

- **标准**：C++26（`CMAKE_CXX_STANDARD 26`）
- **编译器**：GCC 16+（MSYS2 / CLion Toolchain）
- **特性优先**：能用语言/标准库解决的，不用宏或过时惯用法；静态反射（`^^`、`std::meta`、splicer `[: :]`）按 GCC 实现使用
- **IDE**：CLion；构建目录为 `Build/Debug`、`Build/Release`、`Build/RelWithDebugInfo`

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
| public 成员（含 static） | 大驼峰，无前缀 | `int Width` / `static Device* Instance` | `int mWidth` |
| protected / private（含 static） | `m` + 大驼峰 | `int mWidth` / `static Device* mInstance` | `int Width` / `static sInstance` |
| 文件作用域全局 / 命名空间 `inline constexpr` | `g` + 大驼峰 | `gCommandAlignment` | `CommandAlignment` |
| 静态成员 | 无单独前缀，按访问属性 | 同上 | `sInstance` / `gMInstance` |

命名空间内的 `inline constexpr` 常量同样视为全局常量，必须加 `g` 前缀（clang-tidy `GlobalConstantPrefix`）。

### 示例

```cpp
namespace Stardust::Render
{

class RenderDevice
{
public:
    void CreateBuffer(int BufferSize);

    int DeviceIndex = 0;
    static RenderDevice* Instance;

protected:
    void* mNativeHandle = nullptr;

private:
    static RenderDevice* mInstance;
    int mBufferCount = 0;
};

RenderDevice* gPrimaryDevice = nullptr;

} // namespace Stardust::Render
```

### 例外

1. `main` / `WinMain` / `wWinMain` 保持语言要求的名字；业务入口可另写 `Main()`。
2. 第三方符号不重命名、不包一层仅为改名。

## 4. 宏

**默认**：宏只用于条件编译（`#if` / `#ifdef` 等开关）。

**特例**：日志宏（`LogDebug` / `LogInfo` / `LogWarn` / `LogError` / `LogCritical`）——预处理拼接 `"[Category] " Format`，展开为单次 spdlog format；`Category` 为 `Logcat` 枚举项名，枚举保留以列出所有分类。

| 用途 | 做法 |
|------|------|
| 功能开关 | `#define STARDUST_LOG_WITH_FILE_INFO 1` + `#if` |
| 日志 | `LogDebug(Render, "{}", 12);` |
| 数值 / 配置常量 | `inline constexpr int gMinimumLogLevel = 0;` |
| 其它业务 | **禁止**宏，用语言特性 |

## 5. 格式化与静态检查

- **clang-format**：Allman 大括号、缩进 4 空格、列宽 120、指针/引用贴类型左侧（`Type*`）；构造初始化列表与继承列表逗号在行尾（`BreakConstructorInitializers/BreakInheritanceList: AfterColon`），禁止 Leading comma
- **clang-tidy**：`readability-identifier-naming` 按第 3 节规则检查；`HeaderFilterRegex` 仅覆盖 `Source/`。容器/分配器/STL 兼容接口（`begin`/`end`/`value_type`/`allocate`/`push_back` 等）见 `.clang-tidy` 的 `*IgnoredRegexp`，保持标准库命名。
- **强制**：每次改完 `Source/` 下 C/C++ 文件后必须立刻跑 `clang-format` **与** `clang-tidy`（见 `.cursor/rules/clang-format-and-tidy.mdc`），两者都干净才能结束；禁止只改内容或只跑其中一个
- CLion：启用 ClangFormat、Clang-Tidy，与仓库配置文件对齐

## 6. C++ 惯用法（CLion 检查）

### 指定初始化（designated initializers）

聚合类型 / POD struct 初始化时，**优先使用指定初始化**，尤其嵌套成员（如 `RHI::Command`、各类 `*Command`、Desc）。带成员函数的 class（如 `CommandBuffer`）不能整体 designated-init，应对其 struct 成员字段使用指定初始化。

```cpp
// 正确
Command Header{.Type = TypeValue};
DrawCommand{
    .Header = {.Type = DrawCommand::TypeValue},
    .VertexCount = 3,
};

// 错误
Command Header{TypeValue};
DrawCommand Command{};
Command.VertexCount = 3;
```

CLion 检查：`CppUseDesignatedInitializers`（Common Practices and Code Improvements）。

### `auto`

局部变量、以及 `static constexpr` 成员在右侧类型可明确推断时使用 `auto`：

```cpp
const auto Offset = AlignCommandOffset(mCommands.Num());
static constexpr auto TypeValue = CommandType::Draw;
```

以下情况保留显式类型：接口边界、窄化风险、`auto` 降低可读性。

CLion 检查：`CppUseAuto`。

### 值类型形参 `const`

函数/方法的**值传递**形参（基础数值类型、枚举、按值传递的 Handle 等）若在函数体内不修改，必须声明为 `const`：

```cpp
// 正确
void Destroy(const BufferHandle Handle);
void Draw(const UInt32 VertexCount, const UInt32 InstanceCount = 1);
bool IsValid(const GPUHandle Handle) const noexcept;

// 错误
void Destroy(BufferHandle Handle);
void Draw(UInt32 VertexCount);
```

引用形参与指针形参遵循各自的 const-correctness 规则，本条仅针对值传递形参。

CLion 检查：`CppParameterMayBeConst`（Common Practices and Code Improvements）。

项目配置：`.idea/inspectionProfiles/Project_Default.xml`。

## 7. 第三方库

**禁止 FetchContent。** 一律 git submodule + `add_subdirectory`。

| 库 | 用途 | 仓库 | CMake 目标 |
|----|------|------|------------|
| spdlog | 日志 | https://github.com/RickSchanze/spdlog.git | `spdlog::spdlog` |
| glm | 数学 | https://github.com/g-truc/glm.git | `glm::glm` |
| mimalloc | 分配器 | https://github.com/RickSchanze/mimalloc | `mimalloc-static` |

引擎目标通过 `StardustThirdParty` 统一链接。`Main.cpp` 包含 `mimalloc-new-delete.h` 覆盖全局 `new`/`delete`。

拉取失败时使用代理：`http://127.0.0.1:7890`。
