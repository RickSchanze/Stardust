# SDL3 依赖搭建指引

仓库：[`RickSchanze/SDL`](https://github.com/RickSchanze/SDL)（fork，上游 [`libsdl-org/SDL`](https://github.com/libsdl-org/SDL)）  
路径：`ThirdParty/SDL`  
CMake 目标：`SDL3::SDL3`（静态时对应 `SDL3::SDL3-static`）

## 前置

- CMake ≥ 3.16（本工程已 3.28+）
- C/C++ 编译器（本工程 GCC 16+ / Ninja；Windows 上 MinGW 亦可）
- Git（支持 submodule）
- Windows：一般无额外系统包；Linux 若要完整窗口/输入能力需桌面相关开发包（见上游 `docs/README-linux.md`）

## 1. 添加 / 更新 submodule

在仓库根 `Stardust/`：

```bash
# 首次添加
git submodule add https://github.com/RickSchanze/SDL.git ThirdParty/SDL

# 拉取（SDL 通常无深层嵌套 submodule，仍建议 --recursive）
git submodule update --init --recursive ThirdParty/SDL
```

已存在时只更新：

```bash
git submodule update --init --recursive ThirdParty/SDL
```

网络失败时：

```bash
export HTTP_PROXY=http://127.0.0.1:7890
export HTTPS_PROXY=http://127.0.0.1:7890
# Windows PowerShell:
# $env:HTTP_PROXY="http://127.0.0.1:7890"; $env:HTTPS_PROXY="http://127.0.0.1:7890"
```

## 2. CMake 接入（已写在工程里）

`ThirdParty/CMakeLists.txt` 中关键选项：

| 选项 | 值 | 原因 |
|------|-----|------|
| `SDL_SHARED` | `OFF` | 不编 DLL，免运行时拷贝 `SDL3.dll` |
| `SDL_STATIC` | `ON` | 静态链入引擎（与 slang 等一致） |
| `SDL_TEST_LIBRARY` | `OFF` | 不编测试库 |
| `SDL_TESTS` | `OFF` | 不编测试程序 |
| `SDL_EXAMPLES` | `OFF` | 不编示例 |
| `SDL_INSTALL` | `OFF` | 嵌套工程不需要 install 规则 |

然后：

```cmake
add_subdirectory(SDL EXCLUDE_FROM_ALL)
target_link_libraries(StardustThirdParty INTERFACE SDL3::SDL3)
```

头文件：`#include <SDL3/SDL.h>`（通过 `SDL3::SDL3` / `SDL3::Headers`）。

## 3. Configure & Build

```bash
cmake --preset debug
cmake --build Build/Debug --target Stardust -j
```

若只想先验证 SDL 目标：

```bash
cmake --build Build/Debug --target SDL3-static -j
```

## 4. 常见问题

| 现象 | 处理 |
|------|------|
| `ThirdParty/SDL` 空目录 | `git submodule update --init --recursive ThirdParty/SDL` |
| 找不到 `SDL3::SDL3` | 确认已 `add_subdirectory(SDL)` 且 `SDL_STATIC=ON`（或 shared）；清 `Build/*/CMakeCache.txt` 后重配 |
| 仍想编 shared | 改 `SDL_SHARED=ON` 并处理 Windows 下把 `SDL3.dll` 拷到可执行文件旁（见上游 `docs/README-cmake.md`） |
| Linux 无窗口 / 缺 X11·Wayland | 装系统开发包，或确认本机是否真需要 GUI；上游文档列依赖 |
| 与其它目标名冲突 | SDL3 目标带 `SDL3::` 命名空间，一般不冲突 |

## 5. 新机器克隆本仓库

```bash
git clone --recursive <stardust-url>
# 或克隆后：
git submodule update --init --recursive
```

确保 `ThirdParty/SDL` 已 checkout。
