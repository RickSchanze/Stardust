---
name: SetupDependency
description: >-
  Guides adding and configuring Stardust ThirdParty git submodule dependencies
  (especially slang and SDL3). Use when adding a third-party library, setting up
  slang or SDL3, fixing submodule init/recursive clone, or editing
  ThirdParty/CMakeLists.txt.
---

# SetupDependency

Stardust 第三方**只许** `git submodule` + `ThirdParty/CMakeLists.txt` 的 `add_subdirectory`，并链到 `StardustThirdParty`。**禁止** `FetchContent` / 配置期下载。

网络失败时优先试代理：`http://127.0.0.1:7890`（`HTTP_PROXY` / `HTTPS_PROXY`）。

详细步骤：

- slang → [slang.md](slang.md)
- SDL3 → [sdl3.md](sdl3.md)

## 通用流程

1. 在仓库根 `Stardust/` 添加 submodule（URL 优先用 `RickSchanze` fork，若有）：
   ```bash
   git submodule add https://github.com/RickSchanze/<name>.git ThirdParty/<name>
   git submodule update --init --recursive ThirdParty/<name>
   ```
2. 在 `ThirdParty/CMakeLists.txt`：关测试/示例等，`add_subdirectory(<name> EXCLUDE_FROM_ALL)`，再 `target_link_libraries(StardustThirdParty INTERFACE ...)`。
3. 重新 configure / build：`cmake --preset debug` 后 `cmake --build Build/Debug`。
4. 确认 `.gitmodules` 已登记；嵌套 submodule 必须 `--recursive`。

## 检查清单

- [ ] `.gitmodules` 有对应 `path` / `url`
- [ ] 目录 `ThirdParty/<name>` 非空，嵌套依赖已 checkout
- [ ] CMake 未启用会触发网络拉取的选项
- [ ] 已链入 `StardustThirdParty`
- [ ] Debug 配置能编过
