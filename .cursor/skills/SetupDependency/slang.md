# slang 依赖搭建指引

仓库：[`RickSchanze/slang`](https://github.com/RickSchanze/slang)（fork）  
路径：`ThirdParty/slang`  
CMake 目标：`slang`（产物名可能是 `slang-compiler`）

## 前置

- CMake ≥ 3.22（本工程已 3.28+）
- C++ 编译器（本工程 GCC 16+ / Ninja）
- **Python 3**（slang 构建 `spirv-tools` 需要）
- Git（支持 submodule）

## 1. 添加 / 更新 submodule

在仓库根 `Stardust/`：

```bash
# 首次添加
git submodule add https://github.com/RickSchanze/slang.git ThirdParty/slang

# 拉取 slang 自身的 external/* 嵌套 submodule（必做）
git submodule update --init --recursive ThirdParty/slang
```

已存在时只更新：

```bash
git submodule update --init --recursive ThirdParty/slang
```

网络失败时：

```bash
export HTTP_PROXY=http://127.0.0.1:7890
export HTTPS_PROXY=http://127.0.0.1:7890
# Windows PowerShell:
# $env:HTTP_PROXY="http://127.0.0.1:7890"; $env:HTTPS_PROXY="http://127.0.0.1:7890"
```

## 2. 拉取 git tags（强烈建议）

版本号依赖 tags。fork 可能缺 tag，从上游补：

```bash
cd ThirdParty/slang
git fetch https://github.com/shader-slang/slang.git "refs/tags/*:refs/tags/*"
cd ../..
```

未拉 tag 时 `git describe` 只有 hash，slang 会用 `0.0.0` 一类占位版本，一般仍能编，但不理想。

## 3. CMake 接入（已写在工程里）

`ThirdParty/CMakeLists.txt` 中关键选项（**关掉会触发下载的功能**）：

| 选项 | 值 | 原因 |
|------|-----|------|
| `SLANG_SLANG_LLVM_FLAVOR` | `DISABLE` | 禁止下载 slang-llvm |
| `SLANG_ENABLE_DXIL` | `OFF` | 禁止 FetchDXC |
| `SLANG_EXCLUDE_DAWN` / `SLANG_EXCLUDE_TINT` | `ON` | 禁止 dawn/tint 二进制下载 |
| `SLANG_ENABLE_SLANG_RHI` / `GFX` / `TESTS` / `EXAMPLES` 等 | `OFF` | 减依赖、加快编译 |
| `SLANG_ENABLE_MIMALLOC` / `SLANG_ENABLE_SPIRV_TOOLS_MIMALLOC` | `OFF` | 避免与工程已有 `mimalloc-static` 目标冲突 |
| `SLANG_ENABLE_SPLIT_DEBUG_INFO` | `OFF` | 嵌套进 Stardust 时 split debug 的 generators 路径易坏 |
| `SLANG_LIB_TYPE` | `STATIC` | 静态链入引擎，免拷 DLL |

然后：

```cmake
add_subdirectory(slang EXCLUDE_FROM_ALL)
target_link_libraries(StardustThirdParty INTERFACE slang)
```

头文件：`#include <slang.h>`（通过 slang 的 `include/`）。

## 4. Configure & Build

```bash
cmake --preset debug
cmake --build --preset debug --target Stardust -j
```

首次编 slang + spirv-tools / glslang 会较久，属正常。

## 5. 常见问题

| 现象 | 处理 |
|------|------|
| `external/*` 空目录 | `git submodule update --init --recursive ThirdParty/slang` |
| Python 找不到 | 安装 Python 3 并保证在 `PATH` |
| 仍尝试下载 | 确认上表选项被 CACHE FORCE；清 `Build/*/CMakeCache.txt` 后重配 |
| 与工程 `mimalloc`/`glm` 目标名冲突 | 当前关掉 RHI/GFX/Examples 后通常不碰 slang 的 glm；若冲突再改 slang 的 `SLANG_OVERRIDE_*` 或关对应选项 |
| `undefined reference to WinMain`（MinGW） | slang 用 `wmain`：确认 `ThirdParty/CMakeLists.txt` 对 `slang-bootstrap`/`slangc` 加了 `-municode` |

## 6. 新机器克隆本仓库

```bash
git clone --recursive <stardust-url>
# 或克隆后：
git submodule update --init --recursive
```

尤其不要漏 `ThirdParty/slang` 的嵌套 submodule。
